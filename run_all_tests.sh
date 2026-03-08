#!/bin/bash
# Script pour lancer tous les tests CGI automatiquement

echo "╔════════════════════════════════════════════════════════════╗"
echo "║         TEST SUITE COMPLET - CGI AVEC EPOLL               ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Couleurs
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Vérifier que le serveur est accessible
echo -e "${BLUE}[1/5]${NC} Vérification de la connexion au serveur..."
if curl -s -o /dev/null -w "%{http_code}" http://localhost:8080 --max-time 2 | grep -q "200\|404"; then
    echo -e "      ${GREEN}✓${NC} Serveur accessible"
else
    echo -e "      ${RED}✗${NC} Serveur inaccessible"
    echo ""
    echo "      Assurez-vous que le serveur est lancé:"
    echo "      $ ./webserv configs/default.conf"
    echo ""
    exit 1
fi

# Vérifier Python et requests
echo -e "${BLUE}[2/5]${NC} Vérification des dépendances..."
if command -v python3 &> /dev/null; then
    echo -e "      ${GREEN}✓${NC} Python3 installé"
    if python3 -c "import requests" 2>/dev/null; then
        echo -e "      ${GREEN}✓${NC} Module 'requests' installé"
    else
        echo -e "      ${YELLOW}⚠${NC}  Module 'requests' manquant"
        echo "      Installation: pip3 install requests"
        USE_PYTHON=false
    fi
else
    echo -e "      ${YELLOW}⚠${NC}  Python3 non disponible"
    USE_PYTHON=false
fi

# Vérifier les permissions des CGI
echo -e "${BLUE}[3/5]${NC} Vérification des scripts CGI..."
if [ -x "var/www/cgi/test1.py" ] && [ -x "var/www/cgi/test2.py" ]; then
    echo -e "      ${GREEN}✓${NC} Scripts CGI exécutables"
else
    echo -e "      ${YELLOW}⚠${NC}  Scripts CGI non exécutables"
    echo "      Correction automatique..."
    chmod +x var/www/cgi/*.py 2>/dev/null
    if [ -x "var/www/cgi/test1.py" ]; then
        echo -e "      ${GREEN}✓${NC} Permissions corrigées"
    else
        echo -e "      ${RED}✗${NC} Impossible de corriger les permissions"
    fi
fi

echo ""
echo "════════════════════════════════════════════════════════════"
echo ""

# Test 1: Tests basiques
echo -e "${BLUE}[4/5] TEST BASIQUE (bash/curl)${NC}"
echo "────────────────────────────────────────────────────────────"
if [ -x "./test_cgi_basic.sh" ]; then
    ./test_cgi_basic.sh
    BASIC_RESULT=$?
else
    echo -e "${RED}✗ Script non trouvé ou non exécutable${NC}"
    BASIC_RESULT=1
fi

echo ""
echo "════════════════════════════════════════════════════════════"
echo ""

# Test 2: Tests de concurrence
if [ "$USE_PYTHON" != "false" ]; then
    echo -e "${BLUE}[5/5] TESTS AVANCÉS (Python)${NC}"
    echo "────────────────────────────────────────────────────────────"
    
    echo -e "\n${YELLOW}→ Test de concurrence${NC}"
    if [ -x "./test_cgi_concurrent.py" ]; then
        ./test_cgi_concurrent.py
        CONCURRENT_RESULT=$?
    else
        echo -e "${RED}✗ Script non trouvé ou non exécutable${NC}"
        CONCURRENT_RESULT=1
    fi
    
    echo ""
    echo -e "${YELLOW}→ Test de stress (50 requêtes)${NC}"
    if [ -x "./test_cgi_stress.py" ]; then
        ./test_cgi_stress.py 50 20
        STRESS_RESULT=$?
    else
        echo -e "${RED}✗ Script non trouvé ou non exécutable${NC}"
        STRESS_RESULT=1
    fi
else
    echo -e "${BLUE}[5/5] TESTS AVANCÉS${NC}"
    echo "────────────────────────────────────────────────────────────"
    echo -e "${YELLOW}⚠  Tests Python sautés (dépendances manquantes)${NC}"
    CONCURRENT_RESULT=0
    STRESS_RESULT=0
fi

# Résumé final
echo ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║                    RÉSUMÉ DES TESTS                        ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

TOTAL_TESTS=0
PASSED_TESTS=0

if [ $BASIC_RESULT -eq 0 ]; then
    echo -e "  ${GREEN}✓ PASS${NC} - Tests basiques"
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo -e "  ${RED}✗ FAIL${NC} - Tests basiques"
fi
TOTAL_TESTS=$((TOTAL_TESTS + 1))

if [ "$USE_PYTHON" != "false" ]; then
    if [ $CONCURRENT_RESULT -eq 0 ]; then
        echo -e "  ${GREEN}✓ PASS${NC} - Tests de concurrence"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "  ${RED}✗ FAIL${NC} - Tests de concurrence"
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    if [ $STRESS_RESULT -eq 0 ]; then
        echo -e "  ${GREEN}✓ PASS${NC} - Tests de stress"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "  ${RED}✗ FAIL${NC} - Tests de stress"
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
fi

echo ""
echo "────────────────────────────────────────────────────────────"

if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo -e "${GREEN}🎉 TOUS LES TESTS SONT PASSÉS! ($PASSED_TESTS/$TOTAL_TESTS)${NC}"
    echo ""
    echo "Votre implémentation CGI avec epoll fonctionne correctement!"
    EXIT_CODE=0
else
    echo -e "${YELLOW}⚠️  $PASSED_TESTS/$TOTAL_TESTS tests réussis${NC}"
    echo ""
    echo "Consultez les logs ci-dessus pour plus de détails."
    EXIT_CODE=1
fi

echo "════════════════════════════════════════════════════════════"
echo ""

exit $EXIT_CODE
