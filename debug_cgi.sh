#!/bin/bash
# Script pour déboguer le CGI

echo "=== Débogage CGI ==="
echo ""

# Tester si le script CGI fonctionne directement
echo "1. Test direct du script CGI:"
python3 var/www/cgi/test1.py 2>&1 | head -10
echo ""

# Vérifier le chemin complet
echo "2. Chemin complet:"
realpath var/www/cgi/test1.py
echo ""

# Vérifier que le serveur tourne
echo "3. Serveur en écoute:"
netstat -tuln | grep 8080 || echo "Serveur non trouvé"
echo ""

# Tester une requête CGI avec timeout court
echo "4. Test requête CGI:"
timeout 3 curl -v http://localhost:8080/cgi/test1.py 2>&1 | grep -E "HTTP/|Connected|Empty"
echo ""

echOutput: "5. Vérifier les logs du serveur dans le terminal où il tourne"
echo ""
