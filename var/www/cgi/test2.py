#!/usr/bin/env python3

import os
import sys

# Header HTTP obligatoire
print("Content-Type: text/plain")
print()

# Body
print("Hello from CGI")
print("Method:", os.environ.get("REQUEST_METHOD"))
print("Query:", os.environ.get("QUERY_STRING"))