# This example code is in the Public Domain (or CC0 licensed, at your option.)

# Unless required by applicable law or agreed to in writing, this
# software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied.

# -*- coding: utf-8 -*-

from builtins import input
import socket
import threading
import sys
import struct
import time
from time import sleep

# -----------  Config  ----------
PORT = 5555
IP_VERSION = 'IPv4'
host = '192.168.4.1'
#IPV4 = '127.0.0.1'
# -------------------------------
INTERVAL = 0.05  # update rate for state information
TIMEOUT = 30 # timeout for socket when receiving

addr = (host, PORT)
family_addr = socket.AF_INET

# receive message from Huzzah32
def rcvlog():
    print('Started rcvlog thread')
    index = 0
    while not rcvStop.is_set():
        try:
            reply, addr = sock.recvfrom(128)  # bufsize 128 bytes
            if not reply:
                break
            print('Reply[' + addr[0] + ':' + str(addr[1]) + '] - ' + str(reply))
        except socket.timeout:
            print("Socket timeout-recvfrom")
            rcvStop.set() # terminate thread, but shutdown sockets in main loop
        except socket.error as msg:
            print('Error Code : ' + str(msg[0]) + ' Message: ' + msg[1])
            rcvStop.set() # terminate thread, but shutdown sockets in main loop
        # sleep(INTERVAL)
        index +=1
    print('finished rcvlog thread with %d messages' % (index))


try:
    sock = socket.socket(family_addr, socket.SOCK_DGRAM)
    sock.settimeout(TIMEOUT)  # timeout in seconds
except socket.error:
    print('Failed to create socket')
    sys.exit()
print('Using addr:')
print(addr)

rcvThread = threading.Thread(target=rcvlog)
rcvThread.daemon = False  # want clean file close
rcvStop = threading.Event()
rcvStop.clear()
print('Starting rcvlog thread')
rcvThread.start()
sendcnt = 0  # count of message sent
while not rcvStop.is_set():
    time.sleep(0.5)
    try:
        msg = input('Enter string "command value": ')
        temp= msg.split() # get separate command and value elements
        msg_bytes = struct.pack('8si',str.encode(temp[0]),int(temp[1]))
# use structure so there is no need for risky sscanf on Huzzah32
        sock.sendto(msg_bytes, addr)  # send cmd + value over UDP
#       text_msg= 'sent# ' + str(sendcnt) +' ' + msg +'\r\n'
#       sock.sendto(str.encode(text_msg),addr)
    except socket.timeout:
        print("Socket timeout-sendto")
        rcvStop.set() 
    except socket.error as msg:
        print('Error Code : ' + str(msg[0]) + ' Message: ' + msg[1])
        rcvStop.set() #
    except KeyboardInterrupt:
        print('Closed by keyboard interrupt')
        rcvStop.set() #
    sendcnt = sendcnt+1
print('shutting down')
rcvStop.set()  # set stop variable
rcvThread.join(timeout=3.0)   # wait for termination of state thread before c
sock.shutdown(socket.SHUT_RDWR)
sock.close()
sys.exit()
