#!/usr/bin/env python
#-*- coding: utf-8 -*-
#######################################################################
# File Name: netcat.py
# Author: Stewie
# E-mail: 793377164@qq.com
# Created Time: 2018-08-20
#######################################################################

import os 
import select
import socket
import sys 

def relay(sock):
    poll = select.poll()
    poll.register(sock, select.POLLIN)
    poll.register(sys.stdin, select.POLLIN)

    done = False 
    while not done:
        events = poll.poll(100000)
        for fileno, event in events:
            if fileno == sock.fileno():
                data = sock.recv(8192)
                if data:
                    sys.stdout.write(data)
                else:
                    done = True 
            else:
                assert fileno == sys.stdin.fileno()
                data = os.read(fileno, 8192)
                if data:
                    sock.sendall(data)
                else:
                    sock.shutdown(socket.SHUT_WR)
                    poll.unregister(sys.stdin)

def run_server(port):
    server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_sock.bind(('', port))
    server_sock.listen(5)
    (client_sock, client_addr) = server_sock.accept()
    server_sock.close()
    relay(client_sock)

def run_client(hostname, port):
    sock = socket.create_connection((hostname, port))
    relay(sock)

def main(argv):
    if len(argv) < 3:
        print "Usage:\n  %s -l port\n  %s host port" % (argv[0], argv[0])
        return 
    port = int(argv[2])
    if argv[1] == "-l":
        run_server(port)
    else:
        run_client(argv[1], port)

if __name__ == "__main__":
    main(sys.argv)
