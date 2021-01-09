#!/usr/bin/env python3

import socket
import struct

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 5555  # The port used by the server
#PORT = 3333
#HOST = '192.168.4.1'
#address = ('192.168.4.1', 5555)

#with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
    s.settimeout(5)  # timeout in seconds
    s.connect((HOST, PORT))
 #   s.sendto(struct.pack('i', 1234), address)
    s.sendall(b"Hello, world")
    data = s.recv(1024)
    s.shutdown(socket.SHUT_RDWR)
    s.close()
print("Received", repr(data))
