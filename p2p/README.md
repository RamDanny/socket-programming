# Project 1
### Ramdhanush Venkatakrishnan - Himanshu Singh

The project consists of separate client and server files.
Running each program creates an instance of the respective client/server.

To run the program as intended:

1. Run the server (`python server.py`). This creates an instance of the server where all the clients connect to.
2. Run the clients (`python client.py hostname`). Each client should be run in a new terminal with a different hostname and upload port.
3. Navigate the client menu to interact with the server, and perform actions like adding, lookup, or download of RFC files.
4. The actions invoke the protocol, whose messages are exchanged between the client peers and the server.
