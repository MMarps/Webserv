#!/usr/bin/env python3
import os
import sys

content_length = os.environ.get("CONTENT_LENGTH")

body = ""
if content_length:
    body = sys.stdin.read(int(content_length))

sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write("\r\n")

sys.stdout.write("<html><body>")
sys.stdout.write("<h1>POST DATA</h1>")
sys.stdout.write("<pre> body : " + body + "</pre>")
sys.stdout.write("<pre> content length :" + content_length + "</pre>")
sys.stdout.write("</body></html>")

