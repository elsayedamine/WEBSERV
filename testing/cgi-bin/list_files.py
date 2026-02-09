#!/usr/bin/python3
import json
import os
import mimetypes
import sys

def list_directory(directory="."):
    files = []
    for filename in os.listdir(directory):
        # Skip hidden files and . / ..
        if filename.startswith('.') or filename in {'.', '..'}:
            continue
        full_path = os.path.join(directory, filename)
        # Only include actual files (not directories)
        if not os.path.isfile(full_path):
            continue
        mime_type, _ = mimetypes.guess_type(full_path)
        files.append({
            "name": filename,
            "path": filename,
            "type": mime_type or "application/octet-stream"
        })
    return files

if __name__ == "__main__":
    directory = "../uploads"
    files = list_directory(directory)
    sys.stdout.write("Status: 200\nContent-Type: application/json\n\n")
    json.dump(files, sys.stdout, indent=2)
    sys.stdout.write("\n")
