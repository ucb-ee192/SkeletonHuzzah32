# This example code is in the Public Domain (or CC0 licensed, at your option.)

# Unless required by applicable law or agreed to in writing, this
# software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied.

# -*- coding: utf-8 -*-

from builtins import input
import socket
import sys
import struct

# -----------  Config  ----------
PORT = 5555
IP_VERSION = 'IPv4'
host = '192.168.4.1'
#IPV4 = '127.0.0.1'
# -------------------------------


addr = (host, PORT)
family_addr = socket.AF_INET
try:
    sock = socket.socket(family_addr, socket.SOCK_DGRAM)
    sock.settimeout(10)  # timeout in seconds
except socket.error:
    print('Failed to create socket')
    sys.exit()
print('Using addr:')
print(addr)
while True:
#    msg = input('Enter message to send : ')
    time.sleep(2)
    try:
        sock.sendto(struct.pack('i', 6566), addr)  # send digit for testing/starting connection
 #       sock.sendto(msg.encode(), addr)
        reply, addr = sock.recvfrom(128)
        if not reply:
            break
        print('Reply[' + addr[0] + ':' + str(addr[1]) + '] - ' + str(reply))
    except socket.timeout:
        print("Socket timeout")
        sock.close()
        sys.exit()
    except socket.error as msg:
        print('Error Code : ' + str(msg[0]) + ' Message: ' + msg[1])
        sock.shutdown(socket.SHUT_RDWR)
        sock.close()
        sys.exit()
    except KeyboardInterrupt:
        print('Closed by keyboard interrupt')
        sock.shutdown(socket.SHUT_RDWR)
        sock.close()
