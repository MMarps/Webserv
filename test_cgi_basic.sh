#!/bin/bash
# Script de test basique pour les CGI

echo "=================================="
echo "Test basique des CGI"
echo "=================================="
echo ""

BASE_URL="http://localhost:8080"

# Couleurs
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Fonction de test
test_request() {
    local name=$1
    local url=$2
    local method=${3:-GET}
    local data=$4
    
    echo -n "Test: $name ... "
    
    if [ "$method" = "POST" ]; then
        status=$(curl -s -o /tmp/cgi_test_output.txt -w "%{http_code}" -X POST -d "$data" "$url" --max-time 10)
    else
        status=$(curl -s -o /tmp/cgi_test_output.txt -w "%{http_code}" "$url" --max-time 10)
    fi
    
    if [ "$status" = "200" ]; then
        echo -e "${GREEN}✓ OK${NC} (HTTP $status)"
        return 0
    else
        echo -e "${RED}✗ FAILED${NC} (HTTP $status)"
        return 1
    fi
}

# Test 1: CGI simple
echo "1. Test CGI simple"
test_request "GET /cgi/test1.py" "$BASE_URL/cgi/test1.py"
echo ""

# Test 2: CGI avec paramètres
echo "2. Test CGI avec paramètres"
test_request "GET /cgi/test2.py?sleep=1" "$BASE_URL/cgi/test2.py?sleep=1"
echo ""

# Test 3: CGI POST
echo "3. Test CGI POST"
test_request "POST /cgi/post.py" "$BASE_URL/cgi/post.py" "POST" "test_data=hello_world"
echo ""

# Test 4: Multiple requests en parallèle
echo "4. Test de requêtes parallèles"
echo "   Lancement de 5 requêtes CGI en parallèle..."

start_time=$(date +%s)

curl -s -o /dev/null "$BASE_URL/cgi/test2.py?sleep=2" &
curl -s -o /dev/null "$BASE_URL/cgi/test2.py?sleep=2" &
curl -s -o /dev/null "$BASE_URL/cgi/test2.py?sleep=2" &
curl -s -o /dev/null "$BASE_URL/cgi/test2.py?sleep=2" &
curl -s -o /dev/null "$BASE_URL/cgi/test2.py?sleep=2" &

wait

end_time=$(date +%s)
duration=$((end_time - start_time))

echo "   Durée totale: ${duration}s"

if [ $duration -lt 5 ]; then
    echo -e "   ${GREEN}✓ SUCCÈS${NC}: Requêtes traitées en parallèle (non-bloquant)"
else
    echo -e "   ${RED}✗ ÉCHEC${NC}: Requêtes probablement traitées séquentiellement (bloquant)"
fi

echo ""
echo "=================================="
echo "Tests terminés"
echo "=================================="
