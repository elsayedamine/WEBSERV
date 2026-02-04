#!/usr/bin/python3
from sys import stdin, stdout
import os
import socket

if __name__ == "__main__":
    request = stdin.buffer.read()
    resp = b""
    with socket.create_connection(("127.0.0.1", 8080)) as sock:
        sock.sendall(request)
        while True:
            chunk = sock.recv(1024)
            if not chunk:
                break
            resp += chunk
    stdout.buffer.write(resp)
    stdout.flush()