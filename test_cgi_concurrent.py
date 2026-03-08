#!/usr/bin/env python3
"""
Test de concurrent CGI requests pour vérifier la gestion asynchrone avec epoll
"""

import requests
import threading
import time
from concurrent.futures import ThreadPoolExecutor, as_completed

# Configuration
BASE_URL = "http://localhost:8080"
NUM_CONCURRENT_REQUESTS = 10

def test_simple_cgi(request_id):
    """Test une simple requête CGI"""
    try:
        start = time.time()
        response = requests.get(f"{BASE_URL}/cgi/test1.py", timeout=10)
        duration = time.time() - start
        
        return {
            'id': request_id,
            'status': response.status_code,
            'duration': duration,
            'success': response.status_code == 200,
            'content_length': len(response.content)
        }
    except Exception as e:
        return {
            'id': request_id,
            'error': str(e),
            'success': False
        }

def test_cgi_with_sleep(request_id, sleep_time=2):
    """Test un CGI qui prend du temps (pour vérifier la non-blocage)"""
    try:
        start = time.time()
        response = requests.get(f"{BASE_URL}/cgi/test2.py?sleep={sleep_time}", timeout=15)
        duration = time.time() - start
        
        return {
            'id': request_id,
            'status': response.status_code,
            'duration': duration,
            'expected_min_duration': sleep_time,
            'success': response.status_code == 200 and duration >= sleep_time,
            'blocked': duration < sleep_time  # Devrait être False
        }
    except Exception as e:
        return {
            'id': request_id,
            'error': str(e),
            'success': False
        }

def test_cgi_post(request_id, data):
    """Test un CGI avec POST"""
    try:
        start = time.time()
        response = requests.post(f"{BASE_URL}/cgi/post.py", data=data, timeout=10)
        duration = time.time() - start
        
        return {
            'id': request_id,
            'status': response.status_code,
            'duration': duration,
            'success': response.status_code == 200,
            'content': response.text[:100]  # Premier 100 chars
        }
    except Exception as e:
        return {
            'id': request_id,
            'error': str(e),
            'success': False
        }

def print_results(results, test_name):
    """Affiche les résultats de test"""
    print(f"\n{'='*60}")
    print(f"Test: {test_name}")
    print(f"{'='*60}")
    
    success_count = sum(1 for r in results if r.get('success', False))
    total = len(results)
    
    print(f"Succès: {success_count}/{total}")
    
    for result in results:
        if result.get('success'):
            print(f"  ✓ Request {result['id']}: OK ({result.get('duration', 0):.2f}s)")
        else:
            print(f"  ✗ Request {result['id']}: FAILED - {result.get('error', 'Unknown error')}")
    
    if 'duration' in results[0]:
        durations = [r['duration'] for r in results if 'duration' in r]
        print(f"\nDurées:")
        print(f"  Min: {min(durations):.2f}s")
        print(f"  Max: {max(durations):.2f}s")
        print(f"  Moyenne: {sum(durations)/len(durations):.2f}s")

def test_concurrent_simple():
    """Test de requêtes CGI simples concurrentes"""
    print("\n🔄 Test 1: Requêtes CGI simples concurrentes")
    
    with ThreadPoolExecutor(max_workers=NUM_CONCURRENT_REQUESTS) as executor:
        futures = [executor.submit(test_simple_cgi, i) for i in range(NUM_CONCURRENT_REQUESTS)]
        results = [f.result() for f in as_completed(futures)]
    
    print_results(results, "Requêtes CGI simples concurrentes")
    return all(r.get('success') for r in results)

def test_concurrent_with_sleep():
    """Test de requêtes CGI avec sleep pour vérifier le non-blocage"""
    print("\n🔄 Test 2: Requêtes CGI avec sleep (test non-blocage)")
    print("   Si le serveur bloque, ça prendra ~20s (5 req × 2s chacune)")
    print("   Si le serveur NE bloque PAS, ça prendra ~2s (toutes en parallèle)")
    
    start_total = time.time()
    
    with ThreadPoolExecutor(max_workers=5) as executor:
        futures = [executor.submit(test_cgi_with_sleep, i, 2) for i in range(5)]
        results = [f.result() for f in as_completed(futures)]
    
    total_duration = time.time() - start_total
    
    print_results(results, "Requêtes CGI avec sleep")
    
    # Le test est un succès si ça prend environ 2s et non 10s
    non_blocking = total_duration < 4.0  # Marge pour les latences
    
    print(f"\n{'='*60}")
    print(f"Durée totale: {total_duration:.2f}s")
    if non_blocking:
        print("✓ SUCCÈS: Le serveur gère les CGI de manière asynchrone!")
    else:
        print("✗ ÉCHEC: Le serveur semble bloquer sur les CGI")
    print(f"{'='*60}")
    
    return non_blocking

def test_concurrent_post():
    """Test de requêtes POST concurrentes"""
    print("\n🔄 Test 3: Requêtes POST CGI concurrentes")
    
    test_data = [
        f"test_data_{i}_" + "x" * 1000  # 1KB de données
        for i in range(5)
    ]
    
    with ThreadPoolExecutor(max_workers=5) as executor:
        futures = [executor.submit(test_cgi_post, i, data) for i, data in enumerate(test_data)]
        results = [f.result() for f in as_completed(futures)]
    
    print_results(results, "Requêtes POST CGI concurrentes")
    return all(r.get('success') for r in results)

def test_mixed_requests():
    """Test de mix GET/POST concurrents"""
    print("\n🔄 Test 4: Mix de requêtes GET/POST concurrentes")
    
    with ThreadPoolExecutor(max_workers=10) as executor:
        futures = []
        
        # Mix de différents types de requêtes
        for i in range(3):
            futures.append(executor.submit(test_simple_cgi, f"GET-{i}"))
        
        for i in range(3):
            futures.append(executor.submit(test_cgi_with_sleep, f"SLEEP-{i}", 1))
        
        for i in range(4):
            futures.append(executor.submit(test_cgi_post, f"POST-{i}", f"data_{i}"))
        
        results = [f.result() for f in as_completed(futures)]
    
    print_results(results, "Mix de requêtes GET/POST")
    return all(r.get('success') for r in results)

def main():
    print("="*60)
    print("TEST DE GESTION CONCURRENT DES CGI AVEC EPOLL")
    print("="*60)
    print(f"\nCible: {BASE_URL}")
    print(f"Nombre de requêtes concurrentes: {NUM_CONCURRENT_REQUESTS}")
    
    try:
        # Test de connexion
        print("\n📡 Test de connexion au serveur...")
        response = requests.get(BASE_URL, timeout=5)
        print(f"✓ Serveur accessible (status: {response.status_code})")
    except Exception as e:
        print(f"✗ Impossible de se connecter au serveur: {e}")
        print("\nAssurez-vous que le serveur est lancé avec: ./webserv configs/default.conf")
        return
    
    results = []
    
    # Test 1: Requêtes simples
    results.append(("Concurrent Simple", test_concurrent_simple()))
    
    # Test 2: Non-blocage avec sleep
    results.append(("Non-blocage (Sleep)", test_concurrent_with_sleep()))
    
    # Test 3: POST concurrent
    results.append(("Concurrent POST", test_concurrent_post()))
    
    # Test 4: Mix
    results.append(("Mix GET/POST", test_mixed_requests()))
    
    # Résumé
    print("\n" + "="*60)
    print("RÉSUMÉ DES TESTS")
    print("="*60)
    
    for test_name, success in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{status}: {test_name}")
    
    total_success = sum(1 for _, s in results if s)
    print(f"\nTotal: {total_success}/{len(results)} tests réussis")
    
    if total_success == len(results):
        print("\n🎉 TOUS LES TESTS SONT PASSÉS!")
        print("   Votre implémentation CGI avec epoll fonctionne correctement!")
    else:
        print("\n⚠️  Certains tests ont échoué")

if __name__ == "__main__":
    main()
