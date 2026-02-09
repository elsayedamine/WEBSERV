#!/usr/bin/python3
from sys import stdin, stdout, stderr
from urllib.parse import parse_qs
import os
import socket

if __name__ == "__main__":
    query = parse_qs(os.environ.get("QUERY_STRING"), keep_blank_values=True)
    request = query.get("request", [""])[0]
    if not request:
        stdout.write("Status: 400 Bad Request\r\n")
        stdout.write("Content-Type: text/plain\r\n\r\n")
        stdout.write("Missing 'request' query parameter.\n")
        stdout.flush()
        raise SystemExit(0)

    resp = b""
    with socket.create_connection(("127.0.0.1", 8080)) as sock:
        sock.sendall(request.encode("utf-8"))
        while True:
            chunk = sock.recv(1024)
            if not chunk:
                break
            resp += chunk

    sep = b"\r\n\r\n"
    split_at = resp.find(sep)
    if split_at == -1:
        sep = b"\n\n"
        split_at = resp.find(sep)

    if split_at == -1:
        headers_bytes = b""
        body = resp
    else:
        headers_bytes = resp[:split_at]
        body = resp[split_at + len(sep):]

    header_lines = headers_bytes.decode("iso-8859-1").splitlines() if headers_bytes else []
    status_line = header_lines[0] if header_lines else ""
    code = "502"
    reason = "Bad Gateway"

    if status_line.startswith("HTTP/"):
        parts = status_line.split(" ", 2)
        if len(parts) >= 2:
            code = parts[1]
        if len(parts) == 3:
            reason = parts[2]

    stdout.buffer.write(f"Status: {code} {reason}\r\n".encode("utf-8"))
    start_idx = 1 if status_line.startswith("HTTP/") else 0
    for line in header_lines[start_idx:]:
        if not line.strip():
            continue
        if ":" not in line:
            continue
        stdout.buffer.write((line + "\r\n").encode("iso-8859-1"))
    stdout.buffer.write(b"\r\n")
    stdout.buffer.write(body)
    stdout.flush()