#!/bin/bash
# Test rapide pour vérifier que le serveur répond

echo "Test de connexion au serveur..."

# Test 1: Page normale
echo -n "1. GET / ... "
STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/ --max-time 3)
if [ "$STATUS" = "200" ] || [ "$STATUS" = "404" ]; then
    echo "✓ OK (HTTP $STATUS)"
else
    echo "✗ FAILED (HTTP $STATUS)"
fi

# Test 2: CGI simple  
echo -n "2. GET /cgi/test1.py ... "
STATUS=$(curl -s -o /tmp/test_output.txt -w "%{http_code}" http://localhost:8080/cgi/test1.py --max-time 5)
if [ "$STATUS" = "200" ]; then
    echo "✓ OK (HTTP $STATUS)"
    echo "   Output: $(head -c 50 /tmp/test_output.txt)"
else
    echo "✗ FAILED (HTTP $STATUS)"
fi

# Test 3: CGI avec délai
echo -n "3. GET /cgi/test2.py?sleep=1 ... "
START=$(date +%s)
STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/cgi/test2.py?sleep=1 --max-time 5)
END=$(date +%s)
DURATION=$((END - START))
if [ "$STATUS" = "200" ]; then
    echo "✓ OK (HTTP $STATUS, ${DURATION}s)"
else
    echo "✗ FAILED (HTTP $STATUS)"
fi

echo ""
echo "Tests terminés"
