#!/usr/bin/python3
import os
print("Content-Type: text/html\r")
print("\r")
print("<html><body><h1>Environment Variables</h1><ul>")
for key, value in os.environ.items():
    if key.startswith('HTTP_') or key in ['REQUEST_METHOD', 'QUERY_STRING', 'CONTENT_TYPE']:
        print(f"<li><b>{key}</b>: {value}</li>")
print("</ul></body></html>")