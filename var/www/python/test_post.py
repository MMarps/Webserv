cat > var/www/python/test_post.py << 'EOF'
#!/usr/bin/python3
import sys
import os

print("Content-Type: text/html\r")
print("\r")
print("<html><body>")
print("<h1>PATH_INFO Test</h1>")

path_info = os.environ.get('PATH_INFO', '')
print(f"<p><b>PATH_INFO:</b> {path_info}</p>")

content_length = os.environ.get('CONTENT_LENGTH', '0')
print(f"<p><b>Content-Length:</b> {content_length}</p>")

print(f"<p><b>stdin isatty:</b> {sys.stdin.isatty()}</p>")

if content_length != '0':
    body = sys.stdin.read(int(content_length))
    print(f"<p><b>Body:</b> {body}</p>")

print("</body></html>")
EOF

chmod +x var/www/python/test_post.py