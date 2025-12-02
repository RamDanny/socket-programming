#!/usr/bin/env python3
import sys
import socket
import threading
import os
from datetime import datetime, timezone

SERVER_HOST = '127.0.0.1'
SERVER_PORT = 7734
RFC_FOLDER = './rfcs/'
HOSTNAME_MAP = {
    'host1': '127.0.0.1',
    'host2': '127.0.0.1',
    'host3': '127.0.0.1'
}


def handlepeerreq(conn, addr):
    try:
        data = b''
        while True:
            # receive req
            peerreq = conn.recv(1024)
            if not peerreq:
                return
            data += peerreq
            if b'\r\n\r\n' in data:
                break
        # extract header
        lines = data.decode().strip().split('\r\n')
        reqline = lines[0]
        headers = {}
        for l in lines[1:]:
            if ': ' in l:
                headers[l.split(': ')[0]] = l.split(': ')[1]
        parts = reqline.split()
        # handle bad req
        if len(parts) != 4 or parts[0].upper() != 'GET' or parts[1].upper() != 'RFC' or parts[3] != 'P2P-CI/1.0':
            conn.send(b'P2P-CI/1.0 400 Bad Request\r\n\r\n')
            return
        # identify rfc
        rfc_num = parts[2]
        filename = f'rfc{rfc_num}.txt'
        filepath = os.path.join(RFC_FOLDER, filename)
        # handle file not found
        if not os.path.isfile(filepath):
            conn.send(b'P2P-CI/1.0 404 Not Found\r\n\r\n')
            return
        # send rfc file
        size = os.path.getsize(filepath)
        date = datetime.now(timezone.utc).strftime('%a, %d %b %Y %H:%M:%S GMT')
        last_modified = datetime.fromtimestamp(os.path.getmtime(filepath), tz=timezone.utc).strftime('%a, %d %b %Y %H:%M:%S GMT')
        header = (
            f'P2P-CI/1.0 200 OK\r\n'
            f'Date: {date}\r\n'
            f'OS: {os.name}\r\n'
            f'Last-Modified: {last_modified}\r\n'
            f'Content-Length: {size}\r\n'
            f'Content-Type: text/plain\r\n\r\n'
        )
        conn.send(header.encode())
        with open(filepath, 'rb') as f:
            conn.sendall(f.read())
    finally:
            conn.close()

def uplserver(uploadport):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('', uploadport))
    s.listen()
    print(f'[UPLOAD SERVER] Listening on port {uploadport}')
    while True:
        conn, addr = s.accept()
        threading.Thread(target=handlepeerreq, args=(conn, addr), daemon=True).start()


def sendreq(sock, msg):
    sock.send(msg.encode())
    data = b''
    while True:
        chunk = sock.recv(4096)
        if not chunk:
            break
        data += chunk
        if b'\r\n\r\n' in data:
            break
    print(data.decode())


def addrfc(sock, hostname, port):
    rfc = input('Enter RFC number: ').strip()
    title = input('Enter RFC title: ').strip()
    req = f'ADD RFC {rfc} P2P-CI/1.0\r\nHost: {hostname}\r\nPort: {port}\r\nTitle: {title}\r\n\r\n'
    sendreq(sock, req)


def lookuprfc(sock, hostname, port):
    rfc = input('Enter RFC number to lookup: ').strip()
    title = input('Enter RFC title: ').strip()
    req = f'LOOKUP RFC {rfc} P2P-CI/1.0\r\nHost: {hostname}\r\nPort: {port}\r\nTitle: {title}\r\n\r\n'
    sendreq(sock, req)


def listrfc(sock, hostname, port):
    req = f'LIST ALL P2P-CI/1.0\r\nHost: {hostname}\r\nPort: {port}\r\n\r\n'
    sendreq(sock, req)


def downloadrfc(sock, hostname, port):
    rfc = input('Enter RFC number to download: ').strip()
    # Lookup peers from server
    req = f'LOOKUP RFC {rfc} P2P-CI/1.0\r\nHost: {hostname}\r\nPort: {port}\r\nTitle: \r\n\r\n'
    sock.send(req.encode())
    data = b''
    while True:
        chunk = sock.recv(4096)
        if not chunk:
            break
        data += chunk
        if b'\r\n\r\n' in data:
            break
    resp = data.decode()
    if '404 Not Found' in resp:
        print(f'No peers have RFC {rfc}')
        return
    # Parse peers listed
    lines = resp.strip().splitlines()[2:]
    peers = []
    for l in lines:
        parts = l.split()
        if len(parts) >= 4:
            peers.append({'host': parts[2], 'port': int(parts[3]), 'title': parts[1]})
    print('Peers with RFC', rfc)
    for i, p in enumerate(peers):
        print(f'{i+1}. {p['host']}:{p['port']} ({p['title']})')
    choice = int(input('Choose peer number to download from: ')) - 1
    if choice < 0 or choice >= len(peers):
        print('Invalid choice')
        return
    peerhost = HOSTNAME_MAP.get(peers[choice]['host'], peers[choice]['host'])
    peerport = peers[choice]['port']
    # Download
    try:
        s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s2.connect((peerhost, peerport))
        req = f'GET RFC {rfc} P2P-CI/1.0\r\nHost: {peerhost}\r\nOS: {os.name}\r\n\r\n'
        s2.send(req.encode())
        data = b''
        while True:
            chunk = s2.recv(4096)
            if not chunk:
                break
            data += chunk
        header, _, body = data.partition(b'\r\n\r\n')
        status = header.decode().splitlines()[0]
        if '200 OK' in status:
            filepath = os.path.join(RFC_FOLDER, f'rfc{rfc}.txt')
            with open(filepath, 'wb') as f:
                f.write(body)
            print(f'RFC {rfc} downloaded to {filepath}')
            # Add downloaded RFC
            title = peers[choice]['title']
            addreq = f'ADD RFC {rfc} P2P-CI/1.0\r\nHost: {hostname}\r\nPort: {port}\r\nTitle: {title}\r\n\r\n'
            sock.send(addreq.encode())
            resp2 = sock.recv(4096)
            print(resp2.decode())
        else:
            print('Download failed:', status)
    except Exception as e:
        print('Error downloading RFC:', e)
    finally:
        s2.close()


def main():
    if len(sys.argv) < 2:
        print('Usage: python3 client.py <hostname>')
        return
    hostname = sys.argv[1]
    uploadport = int(input('Enter upload port for this peer: '))
    threading.Thread(target=uplserver, args=(uploadport,), daemon=True).start()
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((SERVER_HOST, SERVER_PORT))
        while True:
            print('\nMenu')
            print('1. Add RFC')
            print('2. Lookup RFC')
            print('3. List RFCs')
            print('4. Download RFC from Peer')
            print('5. Exit')
            choice = input('Enter choice: ').strip()
            if choice == '1':
                addrfc(sock, hostname, uploadport)
            elif choice == '2':
                lookuprfc(sock, hostname, uploadport)
            elif choice == '3':
                listrfc(sock, hostname, uploadport)
            elif choice == '4':
                downloadrfc(sock, hostname, uploadport)
            elif choice == '5':
                print('Exiting...')
                break
            else:
                print('Invalid choice')


if __name__ == '__main__':
    main()
