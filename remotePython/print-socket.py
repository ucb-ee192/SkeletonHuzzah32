# basic code to listen to a socket and print

import socket
from time import sleep
import sys
# import curses

INTERVAL = 0.2



def report(str):
    print(str)
    
if __name__ == "__main__":
 # set up receive socket
    # local_ip = '192.168.10.1'
    local_ip = '127.0.0.1'
    local_port = 4445
    try:
        socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # socket for sending cmd
        socket.bind((local_ip, local_port))
    except OSError: 
        print('Socket open error')
        sys.exit()
    print('socket.bind done') 
    print(local_ip,local_port)
    try:
        index = 0
        while True:
            index += 1
            response, ip = socket.recvfrom(1024)
            out = 'Huzzah32:\n' + str(response)
            report(out)
            sleep(INTERVAL)
    except KeyboardInterrupt as e:
 #       curses.echo()
 #       curses.nocbreak()
 #       curses.endwin()
        print('Closed by keyboard interrupt')
        socket.shutdown(socket.SHUT_RDWR)
        socket.close()


