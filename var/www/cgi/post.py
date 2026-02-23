#!/usr/bin/python3
import sys
print("Content-Type: text/html\r")
print("\r")
body = sys.stdin.read()
print(f"<html><body><h1>POST Data Received</h1><pre>{body}</pre></body></html>")