#!/usr/bin/env python3
"""
Test de stress pour les CGI - envoie beaucoup de requêtes concurrentes
"""

import requests
import threading
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
import sys

BASE_URL = "http://localhost:8080"

def stress_test(num_requests=50, workers=20):
    """Envoie un grand nombre de requêtes concurrentes"""
    
    print(f"\n{'='*60}")
    print(f"TEST DE STRESS CGI")
    print(f"{'='*60}")
    print(f"Nombre de requêtes: {num_requests}")
    print(f"Workers concurrents: {workers}")
    print(f"URL: {BASE_URL}/cgi/test1.py")
    
    def make_request(req_id):
        try:
            start = time.time()
            response = requests.get(f"{BASE_URL}/cgi/test1.py", timeout=30)
            duration = time.time() - start
            return {
                'id': req_id,
                'status': response.status_code,
                'duration': duration,
                'success': response.status_code == 200
            }
        except Exception as e:
            return {
                'id': req_id,
                'error': str(e),
                'success': False
            }
    
    print("\n🚀 Lancement du stress test...")
    start_total = time.time()
    
    with ThreadPoolExecutor(max_workers=workers) as executor:
        futures = [executor.submit(make_request, i) for i in range(num_requests)]
        
        # Afficher la progression
        completed = 0
        results = []
        for future in as_completed(futures):
            result = future.result()
            results.append(result)
            completed += 1
            if completed % 10 == 0:
                print(f"  Progression: {completed}/{num_requests} ({completed*100//num_requests}%)")
    
    total_duration = time.time() - start_total
    
    # Analyse des résultats
    success_count = sum(1 for r in results if r.get('success', False))
    failed_count = num_requests - success_count
    
    durations = [r['duration'] for r in results if 'duration' in r]
    
    print(f"\n{'='*60}")
    print("RÉSULTATS")
    print(f"{'='*60}")
    print(f"Durée totale: {total_duration:.2f}s")
    print(f"Requêtes/seconde: {num_requests/total_duration:.2f}")
    print(f"Succès: {success_count}/{num_requests} ({success_count*100//num_requests}%)")
    print(f"Échecs: {failed_count}/{num_requests}")
    
    if durations:
        print(f"\nLatences:")
        print(f"  Min: {min(durations):.3f}s")
        print(f"  Max: {max(durations):.3f}s")
        print(f"  Moyenne: {sum(durations)/len(durations):.3f}s")
        print(f"  Médiane: {sorted(durations)[len(durations)//2]:.3f}s")
    
    # Verdict
    print(f"\n{'='*60}")
    if success_count == num_requests:
        print("✓ TEST RÉUSSI: Toutes les requêtes ont abouti!")
    elif success_count > num_requests * 0.9:
        print("⚠️  TEST PARTIEL: Plus de 90% de réussite")
    else:
        print("✗ TEST ÉCHOUÉ: Trop de requêtes ont échoué")
    print(f"{'='*60}\n")
    
    return success_count == num_requests

def test_burst():
    """Test avec des bursts de requêtes"""
    print(f"\n{'='*60}")
    print("TEST DE BURST")
    print(f"{'='*60}")
    print("Envoie 3 bursts de 20 requêtes avec 2s d'intervalle")
    
    for burst in range(1, 4):
        print(f"\n🔥 Burst {burst}/3 - 20 requêtes...")
        
        def make_request(req_id):
            try:
                response = requests.get(f"{BASE_URL}/cgi/test1.py", timeout=10)
                return response.status_code == 200
            except:
                return False
        
        start = time.time()
        with ThreadPoolExecutor(max_workers=20) as executor:
            futures = [executor.submit(make_request, i) for i in range(20)]
            results = [f.result() for f in as_completed(futures)]
        
        duration = time.time() - start
        success = sum(1 for r in results if r)
        
        print(f"   Résultats: {success}/20 succès en {duration:.2f}s")
        
        if burst < 3:
            time.sleep(2)
    
    print(f"\n✓ Test de burst terminé")

def main():
    if len(sys.argv) > 1:
        num_requests = int(sys.argv[1])
    else:
        num_requests = 50
    
    if len(sys.argv) > 2:
        workers = int(sys.argv[2])
    else:
        workers = 20
    
    try:
        # Test de connexion
        print("\n📡 Test de connexion au serveur...")
        response = requests.get(BASE_URL, timeout=5)
        print(f"✓ Serveur accessible")
    except Exception as e:
        print(f"✗ Impossible de se connecter: {e}")
        return
    
    # Stress test principal
    stress_test(num_requests, workers)
    
    # Test de burst
    test_burst()

if __name__ == "__main__":
    main()
