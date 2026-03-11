#!/bin/bash

echo "Content-Type: text/html"
echo ""

echo "<!DOCTYPE html>"
echo "<html>"
echo "<head>"
echo "<title>CGI Bash</title>"
echo "</head>"
echo "<body>"

echo "<h1>Bonjour depuis une CGI Bash</h1>"

echo "<p>Méthode : $REQUEST_METHOD</p>"
echo "<p>Query string : $QUERY_STRING</p>"

echo "</body>"
echo "</html>"