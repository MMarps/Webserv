# Tests CGI avec Epoll

Ce dossier contient des scripts de test pour vérifier que votre implémentation CGI avec epoll fonctionne correctement.

## Prérequis

1. **Python 3** avec le module `requests`:
   ```bash
   pip3 install requests
   ```

2. **Le serveur doit être lancé**:
   ```bash
   ./webserv configs/default.conf
   ```

3. **Les scripts CGI doivent être exécutables**:
   ```bash
   chmod +x var/www/cgi/*.py
   ```

## Scripts de test disponibles

### 1. Test basique (Bash)
```bash
./test_cgi_basic.sh
```

**Ce qu'il teste:**
- Requêtes GET simples
- Requêtes avec paramètres
- Requêtes POST
- 5 requêtes parallèles avec sleep(2) pour vérifier le non-blocage

**Résultat attendu:** 
- Durée du test parallèle: ~2-3s (si non-bloquant)
- Durée du test parallèle: ~10s (si bloquant - PROBLÈME)

### 2. Test concurrent (Python)
```bash
./test_cgi_concurrent.py
```

**Ce qu'il teste:**
- 10 requêtes CGI simples concurrentes
- 5 requêtes avec sleep(2) en parallèle (test de non-blocage)
- 5 requêtes POST concurrentes
- Mix de GET/POST concurrents

**Résultat attendu:**
- Tous les tests passent (✓ PASS)
- Le test de non-blocage doit prendre ~2s, pas 10s

### 3. Test de stress (Python)
```bash
./test_cgi_stress.py [num_requests] [workers]
```

Par défaut: 50 requêtes avec 20 workers

**Exemples:**
```bash
./test_cgi_stress.py              # 50 requêtes, 20 workers
./test_cgi_stress.py 100 30       # 100 requêtes, 30 workers
./test_cgi_stress.py 200 50       # 200 requêtes, 50 workers
```

**Ce qu'il teste:**
- Grand nombre de requêtes concurrentes
- Stabilité sous charge
- Latence moyenne/min/max
- Taux de succès

## Interprétation des résultats

### ✓ Implémentation correcte (non-bloquante)
```
Test de requêtes parallèles avec sleep(2):
Durée totale: ~2-3s
✓ SUCCÈS: Requêtes traitées en parallèle
```

Les 5 requêtes qui font sleep(2) s'exécutent en parallèle, donc ça prend ~2s au total.

### ✗ Implémentation bloquante (problème)
```
Test de requêtes parallèles avec sleep(2):
Durée totale: ~10s
✗ ÉCHEC: Requêtes probablement traitées séquentiellement
```

Les 5 requêtes s'exécutent l'une après l'autre: 2+2+2+2+2 = 10s

## Vérifications importantes

### 1. Vérifier que les CGI sont bien gérés par epoll

Dans les logs du serveur, vous devriez voir:
```
[INFO] CGI process started, pipes added to epoll
[INFO] CGI completed, response ready
```

### 2. Vérifier qu'aucun processus zombie ne reste

Pendant que le serveur tourne:
```bash
ps aux | grep defunct
```

Ne devrait rien retourner (ou très peu temporairement).

### 3. Vérifier les file descriptors

```bash
lsof -p $(pgrep webserv) | wc -l
```

Le nombre ne devrait pas augmenter indéfiniment après les tests.

## Dépannage

### Le serveur ne répond pas
```bash
# Vérifier que le serveur écoute
netstat -tuln | grep 8080

# Tester avec curl
curl -v http://localhost:8080/
```

### Erreur: Connection refused
Le serveur n'est pas lancé ou écoute sur un autre port.

### Erreur: Timeout
Le CGI prend trop de temps ou le serveur bloque. Vérifiez les logs.

### Les CGI retournent 502
- Vérifier que les scripts sont exécutables: `chmod +x var/www/cgi/*.py`
- Vérifier que l'interpréteur Python est configuré dans la config
- Vérifier les logs pour voir l'erreur exacte

## Exemple d'exécution complète

```bash
# Terminal 1: Lancer le serveur
./webserv configs/default.conf

# Terminal 2: Lancer les tests
./test_cgi_basic.sh
./test_cgi_concurrent.py
./test_cgi_stress.py 100 30
```

## Métriques de performance attendues

Avec une implémentation correcte (epoll non-bloquant):

- **Latence moyenne** (requête simple): < 50ms
- **Requêtes/seconde**: > 100 req/s
- **Test de non-blocage**: ~2s pour 5×sleep(2)
- **Taux de succès sous stress**: > 95%

## Structure des tests

```
test_cgi_basic.sh          # Tests simples avec bash/curl
test_cgi_concurrent.py     # Tests de concurrence
test_cgi_stress.py         # Tests de charge
var/www/cgi/
  ├── test1.py            # CGI simple
  ├── test2.py            # CGI avec sleep (test non-blocage)
  └── post.py             # CGI POST (existant)
```
