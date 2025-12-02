#!/usr/bin/env python3
import socket
import threading

SERVER_HOST = '0.0.0.0'
SERVER_PORT = 7734

peers = []
rfc_index = []
lock = threading.Lock()


def handleclient(conn, addr):
    headers = {}
    try:
        while True:
            # recv full packet
            data = b''
            while True:
                chunk = conn.recv(4096)
                if not chunk:
                    return
                data += chunk
                if b'\r\n\r\n' in data:
                    break
            packet = data.decode()
            # parse headers/data
            lines = packet.strip().split('\r\n')
            if not lines:
                continue
            req_line = lines[0].strip()
            headers = {}
            for l in lines[1:]:
                if ': ' in l:
                    k, v = l.split(': ', 1)
                    headers[k.strip()] = v.strip()
            # check method
            parts = req_line.split()
            if len(parts) < 3:
                conn.send('P2P-CI/1.0 400 Bad Request\r\n\r\n'.encode())
                continue
            method = parts[0].upper()
            version = parts[-1]
            if version != 'P2P-CI/1.0':
                conn.send('P2P-CI/1.0 505 P2P-CI Version Not Supported\r\n\r\n'.encode())
                continue
            if method == 'ADD' and parts[1].upper() == 'RFC':
                rfcnum = parts[2]
                host = headers.get('Host')
                port = int(headers.get('Port', 0))
                title = headers.get('Title', '')
                if not host or not port:
                    conn.send('P2P-CI/1.0 400 Bad Request\r\n\r\n'.encode())
                    continue
                with lock:
                    peerentry = {'hostname': host, 'port': port}
                    if peerentry not in peers:
                        peers.append(peerentry)
                    # check/add RFC entry
                    exists = False
                    for rfc in rfc_index:
                        if rfc['rfcnum'] == rfcnum and rfc['hostname'] == host and rfc['port'] == port:
                            exists = True
                            break
                    if not exists:
                        rfc_index.append({'rfcnum': rfcnum, 'title': title, 'hostname': host, 'port': port})
                resp = f'P2P-CI/1.0 200 OK\r\nRFC {rfcnum} {title} {host} {port}\r\n\r\n'
                conn.send(resp.encode())
            elif method == 'LOOKUP' and parts[1].upper() == 'RFC':
                rfcnum = parts[2]
                found = []
                with lock:
                    for rfc in rfc_index:
                        if rfc['rfcnum'] == rfcnum:
                            found.append(f"{rfc['rfcnum']} {rfc['title']} {rfc['hostname']} {rfc['port']}")
                if found:
                    resp = 'P2P-CI/1.0 200 OK\r\n\r\n' + '\r\n'.join(found) + '\r\n\r\n'
                else:
                    resp = 'P2P-CI/1.0 404 Not Found\r\n\r\n'
                conn.send(resp.encode())
            elif method == 'LIST' and parts[1].upper() == 'ALL':
                with lock:
                    lines = []
                    for rfc in rfc_index:
                        lines.append(f"{rfc['rfcnum']} {rfc['title']} {rfc['hostname']} {rfc['port']}")
                resp = 'P2P-CI/1.0 200 OK\r\n\r\n' + '\r\n'.join(lines) + '\r\n\r\n'
                conn.send(resp.encode())
            else:
                conn.send('P2P-CI/1.0 400 Bad Request\r\n\r\n'.encode())
    except Exception as e:
        print(f'[ERROR] Client {addr} exception: {e}')
    finally:
        # remove on disconnect
        with lock:
            host = headers.get('Host')
            peers[:] = [peer for peer in peers if peer.get('hostname') != host]
            rfc_index[:] = [rfc for rfc in rfc_index if rfc.get('hostname') != host]
        conn.close()


def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((SERVER_HOST, SERVER_PORT))
    s.listen()
    print(f"[SERVER] Listening on port {SERVER_PORT}")
    while True:
        conn, addr = s.accept()
        print(f"[SERVER] New connection from {addr}")
        threading.Thread(target=handleclient, args=(conn, addr), daemon=True).start()


if __name__ == "__main__":
    main()
