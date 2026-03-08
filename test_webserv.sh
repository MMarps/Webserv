#!/bin/bash

# Script de tests complet pour Webserv
# Tests GET, POST, DELETE - simples et complexes

# Couleurs
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
HOST="http://localhost:8080"
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Fonction d'affichage de bannière
print_banner() {
    echo -e "${CYAN}"
    echo "╔════════════════════════════════════════════════════════════════╗"
    echo "║                   WEBSERV TEST SUITE                          ║"
    echo "║                  Tests GET, POST, DELETE                      ║"
    echo "╚════════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

# Fonction de test avec code de retour attendu
test_request() {
    local test_name="$1"
    local expected_code="$2"
    local url="$3"
    shift 3
    local curl_opts="$@"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    printf "%-60s " "$test_name"
    
    http_code=$(curl -s -o /dev/null -w '%{http_code}' $curl_opts "$url" 2>/dev/null)
    
    if [ "$http_code" = "$expected_code" ]; then
        echo -e "${GREEN}✓ PASS${NC} [${http_code}]"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}✗ FAIL${NC} [Expected: ${expected_code}, Got: ${http_code}]"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

# Fonction de test sans vérification de code (juste pour exécuter)
run_test() {
    local test_name="$1"
    shift
    local cmd="$@"
    
    printf "%-60s " "$test_name"
    eval "$cmd" > /dev/null 2>&1
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ EXECUTED${NC}"
    else
        echo -e "${YELLOW}⚠ WARNING${NC}"
    fi
}

# Fonction de préparation des fichiers de test
prepare_test_files() {
    echo -e "${BLUE}📁 Préparation des fichiers de test...${NC}"
    
    # Créer répertoires de test
    mkdir -p /tmp/webserv_test
    mkdir -p var/www/uploads
    
    # Créer fichiers de test
    echo "Hello World" > /tmp/webserv_test/test_post.txt
    echo "File 1 content" > /tmp/webserv_test/file1.txt
    echo "File 2 content" > /tmp/webserv_test/file2.txt
    echo "Delete me" > var/www/uploads/test_delete.txt
    
    # Créer dossiers de test pour DELETE
    mkdir -p var/www/uploads/delete_dir_empty
    mkdir -p var/www/uploads/delete_dir_full
    echo "content" > var/www/uploads/delete_dir_full/file.txt
    
    # Créer fichiers pour tests multiples
    for i in {1..5}; do
        echo "Test file $i" > /tmp/webserv_test/upload_$i.txt
    done
    
    # Créer gros fichier (5MB)
    dd if=/dev/zero of=/tmp/webserv_test/big_file.bin bs=1M count=5 2>/dev/null
    
    # Créer fichier trop gros (15MB - dépasse la limite de 10MB)
    dd if=/dev/zero of=/tmp/webserv_test/too_big.bin bs=1M count=15 2>/dev/null
    
    echo -e "${GREEN}✓ Fichiers de test préparés${NC}\n"
}

# Fonction de nettoyage
cleanup() {
    echo -e "\n${BLUE}🧹 Nettoyage des fichiers de test...${NC}"
    rm -rf /tmp/webserv_test
    rm -f var/www/uploads/test_delete.txt 2>/dev/null
    rm -rf var/www/uploads/delete_dir_* 2>/dev/null
    rm -f var/www/readonly.txt 2>/dev/null
    rm -rf var/www/readonly_dir 2>/dev/null
    echo -e "${GREEN}✓ Nettoyage terminé${NC}\n"
}

# ============================================================================
# TESTS GET - SIMPLES
# ============================================================================
test_get_simple() {
    echo -e "\n${PURPLE}╔════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}║                    TESTS GET - SIMPLES                        ║${NC}"
    echo -e "${PURPLE}╚════════════════════════════════════════════════════════════════╝${NC}\n"
    
    test_request "GET - Page d'accueil" "200" "${HOST}/"
    test_request "GET - Fichier HTML spécifique" "200" "${HOST}/index.html"
    test_request "GET - Fichier test.html" "200" "${HOST}/test.html"
    test_request "GET - Dossier avec index" "200" "${HOST}/withIndex/"
    test_request "GET - Headers seulement (HEAD)" "200" "${HOST}/index.html" "-I"
    test_request "GET - Fichier CSS" "200" "${HOST}/style.css"
}

# ============================================================================
# TESTS GET - COMPLEXES
# ============================================================================
test_get_complex() {
    echo -e "\n${PURPLE}╔════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}║                   TESTS GET - COMPLEXES                        ║${NC}"
    echo -e "${PURPLE}╚════════════════════════════════════════════════════════════════╝${NC}\n"
    
    test_request "GET - Autoindex (liste dossier)" "200" "${HOST}/location/autoindex/"
    test_request "GET - Redirection 301" "301" "${HOST}/old"
    test_request "GET - Erreur 404 (inexistant)" "404" "${HOST}/nonexistent.html"
    test_request "GET - Query string simple" "200" "${HOST}/cgi/test1.py?name=John"
    test_request "GET - Multiples query params" "200" "${HOST}/python/env1.py?p1=v1&p2=v2&p3=v3"
    test_request "GET - Dossier avec index" "200" "${HOST}/withIndex/"
    test_request "GET - Dossier autoindex avec sous-dossier" "200" "${HOST}/location/autoindex/test/"
}

# ============================================================================
# TESTS POST - SIMPLES
# ============================================================================
test_post_simple() {
    echo -e "\n${PURPLE}╔════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}║                   TESTS POST - SIMPLES                         ║${NC}"
    echo -e "${PURPLE}╚════════════════════════════════════════════════════════════════╝${NC}\n"
    
    test_request "POST - Données simples" "201" "${HOST}/upload/" "-X POST -d 'name=John&age=30'"
    test_request "POST - Fichier texte" "201" "${HOST}/upload/" "-X POST -F 'file=@/tmp/webserv_test/test_post.txt'"
    test_request "POST - Données JSON" "201" "${HOST}/upload/" "-X POST -H 'Content-Type: application/json' -d '{\"name\":\"John\",\"age\":30}'"
    test_request "POST - Form data" "201" "${HOST}/upload/" "-X POST -d 'username=test&password=pass123'"
}

# ============================================================================
# TESTS POST - COMPLEXES
# ============================================================================
test_post_complex() {
    echo -e "\n${PURPLE}╔════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}║                  TESTS POST - COMPLEXES                        ║${NC}"
    echo -e "${PURPLE}╚════════════════════════════════════════════════════════════════╝${NC}\n"
    
    test_request "POST - Multiple files" "201" "${HOST}/upload/" \
        "-X POST -F 'file1=@/tmp/webserv_test/file1.txt' -F 'file2=@/tmp/webserv_test/file2.txt'"
    
    test_request "POST - Gros fichier (5MB)" "201" "${HOST}/upload/" \
        "-X POST -F 'file=@/tmp/webserv_test/big_file.bin'"
    
    test_request "POST - Fichier trop gros (413)" "413" "${HOST}/upload/" \
        "-X POST -F 'file=@/tmp/webserv_test/too_big.bin'"
    
    test_request "POST - CGI avec données" "200" "${HOST}/cgi/post.py" \
        "-X POST -d 'name=Alice&message=Hello'"
    
    test_request "POST - Multipart form" "201" "${HOST}/upload/" \
        "-X POST -F 'name=John' -F 'email=john@example.com' -F 'file=@/tmp/webserv_test/test_post.txt'"
    
    test_request "POST - Method Not Allowed (405)" "405" "${HOST}/" \
        "-X POST -d 'test=data'"
    
    test_request "POST - Headers custom" "201" "${HOST}/upload/" \
        "-X POST -H 'X-Custom-Header: test' -H 'User-Agent: MyClient/1.0' -d 'data=test'"
}

# ============================================================================
# TESTS DELETE - SIMPLES
# ============================================================================
test_delete_simple() {
    echo -e "\n${PURPLE}╔════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}║                  TESTS DELETE - SIMPLES                        ║${NC}"
    echo -e "${PURPLE}╚════════════════════════════════════════════════════════════════╝${NC}\n"
    
    # Préparer les fichiers
    echo "delete me" > var/www/uploads/test_delete_1.txt
    mkdir -p var/www/uploads/empty_test_dir
    
    test_request "DELETE - Supprimer un fichier" "204" "${HOST}/uploads/test_delete_1.txt" "-X DELETE"
    test_request "DELETE - Supprimer dossier vide" "204" "${HOST}/uploads/empty_test_dir/" "-X DELETE"
    test_request "DELETE - Dossier non vide (409)" "409" "${HOST}/uploads/delete_dir_full/" "-X DELETE"
    test_request "DELETE - Fichier inexistant (404)" "404" "${HOST}/uploads/nonexistent.txt" "-X DELETE"
}

# ============================================================================
# TESTS DELETE - COMPLEXES
# ============================================================================
test_delete_complex() {
    echo -e "\n${PURPLE}╔════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}║                 TESTS DELETE - COMPLEXES                       ║${NC}"
    echo -e "${PURPLE}╚════════════════════════════════════════════════════════════════╝${NC}\n"
    
    # Préparer fichier sans permissions en écriture
    echo "readonly" > var/www/readonly.txt
    chmod 444 var/www/readonly.txt
    
    test_request "DELETE - Fichier readonly (403)" "403" "${HOST}/readonly.txt" "-X DELETE"
    
    # Restaurer permissions et nettoyer
    chmod 644 var/www/readonly.txt 2>/dev/null
    rm -f var/www/readonly.txt 2>/dev/null
    
    # Test de suppressions multiples
    echo -e "\n${CYAN}Test de suppressions multiples...${NC}"
    for i in {1..5}; do
        echo "file $i" > var/www/uploads/delete_multi_$i.txt
    done
    
    for i in {1..5}; do
        test_request "DELETE - Multiple $i/5" "204" "${HOST}/uploads/delete_multi_$i.txt" "-X DELETE"
    done
    
    # Test double delete
    echo "once" > var/www/uploads/delete_once.txt
    test_request "DELETE - Premier delete" "204" "${HOST}/uploads/delete_once.txt" "-X DELETE"
    test_request "DELETE - Second delete (404)" "404" "${HOST}/uploads/delete_once.txt" "-X DELETE"
    
    # Test avec query string (devrait être ignoré)
    echo "test" > var/www/uploads/with_query.txt
    test_request "DELETE - Avec query string" "204" "${HOST}/uploads/with_query.txt?param=value" "-X DELETE"
}

# ============================================================================
# TESTS MIXTES ET EDGE CASES
# ============================================================================
test_edge_cases() {
    echo -e "\n${PURPLE}╔════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}║              TESTS MIXTES ET EDGE CASES                        ║${NC}"
    echo -e "${PURPLE}╚════════════════════════════════════════════════════════════════╝${NC}\n"
    
    # Séquence POST puis DELETE
    echo "temp" > /tmp/webserv_test/temp_upload.txt
    test_request "POST puis DELETE - Upload" "201" "${HOST}/upload/" \
        "-X POST -F 'file=@/tmp/webserv_test/temp_upload.txt'"
    
    # Attendre un peu que le fichier soit écrit
    sleep 1
    
    test_request "POST puis DELETE - Delete" "204" "${HOST}/uploads/temp_upload.txt" "-X DELETE"
    
    # Méthode non supportée
    test_request "Method PUT non supportée" "405" "${HOST}/index.html" "-X PUT -d 'data=test'"
    
    # Headers custom
    test_request "GET avec headers custom" "200" "${HOST}/index.html" \
        "-H 'Accept: text/html' -H 'Accept-Language: fr-FR'"
    
    # URL encoding
    test_request "GET avec URL encodée" "200" "${HOST}/cgi/test1.py?name=John%20Doe"
}

# ============================================================================
# TEST DE PERFORMANCE
# ============================================================================
test_performance() {
    echo -e "\n${PURPLE}╔════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}║                   TEST DE PERFORMANCE                          ║${NC}"
    echo -e "${PURPLE}╚════════════════════════════════════════════════════════════════╝${NC}\n"
    
    echo -e "${CYAN}Envoi de 20 requêtes GET simultanées...${NC}"
    
    start_time=$(date +%s.%N)
    
    for i in {1..20}; do
        curl -s "${HOST}/index.html" > /dev/null &
    done
    
    wait
    
    end_time=$(date +%s.%N)
    duration=$(echo "$end_time - $start_time" | bc)
    
    echo -e "${GREEN}✓ 20 requêtes terminées en ${duration}s${NC}"
}

# ============================================================================
# AFFICHAGE DES RÉSULTATS
# ============================================================================
show_results() {
    echo -e "\n${CYAN}╔════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║                       RÉSULTATS FINAUX                         ║${NC}"
    echo -e "${CYAN}╚════════════════════════════════════════════════════════════════╝${NC}\n"
    
    echo -e "Total de tests exécutés : ${YELLOW}${TOTAL_TESTS}${NC}"
    echo -e "Tests réussis           : ${GREEN}${PASSED_TESTS}${NC}"
    echo -e "Tests échoués           : ${RED}${FAILED_TESTS}${NC}"
    
    success_rate=0
    if [ $TOTAL_TESTS -gt 0 ]; then
        success_rate=$(echo "scale=2; ($PASSED_TESTS * 100) / $TOTAL_TESTS" | bc)
    fi
    
    echo -e "Taux de réussite        : ${YELLOW}${success_rate}%${NC}\n"
    
    if [ $FAILED_TESTS -eq 0 ]; then
        echo -e "${GREEN}╔════════════════════════════════════════════════════════════════╗${NC}"
        echo -e "${GREEN}║          ✓✓✓ TOUS LES TESTS ONT RÉUSSI ! ✓✓✓                 ║${NC}"
        echo -e "${GREEN}╚════════════════════════════════════════════════════════════════╝${NC}\n"
        exit 0
    else
        echo -e "${RED}╔════════════════════════════════════════════════════════════════╗${NC}"
        echo -e "${RED}║          ✗✗✗ CERTAINS TESTS ONT ÉCHOUÉ ✗✗✗                   ║${NC}"
        echo -e "${RED}╚════════════════════════════════════════════════════════════════╝${NC}\n"
        exit 1
    fi
}

# ============================================================================
# MAIN
# ============================================================================
main() {
    print_banner
    
    # Vérifier que le serveur est lancé
    echo -e "${BLUE}🔍 Vérification de la connexion au serveur...${NC}"
    if ! curl -s --connect-timeout 2 "${HOST}/" > /dev/null 2>&1; then
        echo -e "${RED}✗ Erreur: Le serveur n'est pas accessible sur ${HOST}${NC}"
        echo -e "${YELLOW}Assurez-vous que ./webserv est lancé${NC}\n"
        exit 1
    fi
    echo -e "${GREEN}✓ Serveur accessible${NC}\n"
    
    # Préparation
    prepare_test_files
    
    # Lancer les tests
    test_get_simple
    test_get_complex
    test_post_simple
    test_post_complex
    test_delete_simple
    test_delete_complex
    test_edge_cases
    test_performance
    
    # Nettoyage
    cleanup
    
    # Afficher les résultats
    show_results
}

# Trap pour nettoyer en cas d'interruption
trap cleanup EXIT INT TERM

# Lancer le script
main
