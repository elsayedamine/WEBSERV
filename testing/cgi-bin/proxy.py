#!/usr/bin/python3
from sys import stdin, stdout
import os
import socket

r1 = b"GET /cgi-bin/list_files.py?a=b&z=y HTTP/1.1\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Encoding: gzip, deflate, br, zstd\r\nConnection: keep-alive\r\nAccept-Language: en-US,en;q=0.9\r\nHost: 127.0.0.1:8080\r\nPriority: u=0, i\r\nSec-Fetch-Dest: document\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-Site: none\r\nSec-Fetch-User: ?1\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:147.0) Gecko/20100101 Firefox/147.0\r\n\r\n"

if __name__ == "__main__":
    # request = stdin.buffer.read()
    request = r1
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