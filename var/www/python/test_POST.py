#!/usr/bin/python3
import sys
import os

print("Content-Type: text/html\r")
print("\r")
print("<html><body>")
print("<h1>POST Test</h1>")

content_length = os.environ.get('CONTENT_LENGTH', '0')
print(f"<p>Content-Length: {content_length}</p>")

if content_length != '0':
	body = sys.stdin.read(int(content_length))
	print(f"<p>Body received: {body}</p>")
else:
	print("<p>No body received</p>")

print("</body></html>")