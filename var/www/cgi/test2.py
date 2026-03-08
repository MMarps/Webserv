#!/usr/bin/env python3

import os
import sys
import time
from urllib.parse import parse_qs

# Parse query string for sleep parameter
query_string = os.environ.get("QUERY_STRING", "")
params = parse_qs(query_string)
sleep_time = int(params.get("sleep", ["0"])[0])

# Sleep if requested
if sleep_time > 0:
    time.sleep(sleep_time)

# Header HTTP obligatoire
print("Content-Type: text/plain")
print()

# Body
print("Hello from CGI")
print("Method:", os.environ.get("REQUEST_METHOD"))
print("Query:", os.environ.get("QUERY_STRING"))
print("Sleep time:", sleep_time)