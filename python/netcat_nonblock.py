#!/usr/bin/env python
#-*- coding: utf-8 -*-
#######################################################################
# File Name: netcat_nonblock.py
# Author: Stewie
# E-mail: 793377164@qq.com
# Created Time: 2018-08-20
#######################################################################

import errno
import fcntl
import os 
import select
import socket
import sys 

def set_nonblocking(fd):
    flags = fcntl.fcntl(fd, fcntl.F_GETFL)
    fcntl.fcntl(fd, fcntl.F_SETFL, flags | os.O_NONBLOCK)

def nonblocking_write(fd, data):
    try:
        nw = os.write(fd, data)
        return nw 
    except OSError as e:
        if e.errno == errno.EWOULDBLOCK:
            return -1

def relay(sock):
    poll = select.poll()
    sock_events = select.POLLIN 
    poll.register(sock, sock_events)
    poll.register(sys.stdin, select.POLLIN)

    set_nonblocking(sock)

    done = False 
    sock_buffer = ''
    while not done:
        events = poll.poll(100000)
        for fileno, event in events:
            if event & select.POLLIN:
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
                        assert len(sock_buffer) == 0
                        nw = nonblocking_write(sock.fileno(), data)
                        if nw < len(data):
                            if nw < 0:
                                nw = 0
                            sock_buffer = data[nw:]
                            sock_events |= select.POLLOUT 
                            poll.register(sock, sock_events)
                            poll.unregister(sys.stdin)
                    else:
                        sock.shutdown(socket.SHUT_WR)
                        poll.unregister(sys.stdin)
            if event & select.POLLOUT:
                if fileno == sock.fileno():
                    assert len(sock_buffer) > 0
                    nw = nonblocking_write(sock.fileno(), sock_buffer)
                    if nw < len(sock_buffer):
                        assert nw > 0
                        sock_buffer = sock_buffer[nw:]
                    else:
                        sock_buffer = ''
                        # 停止观察POLLOUT, 否则CPU 100%
                        sock_events &= ~select.POLLOUT 
                        poll.register(sock, sock_events)
                        poll.register(sys.stdin, select.POLLIN)

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
