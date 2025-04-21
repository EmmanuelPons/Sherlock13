Sherlock 13
Jeu de déduction collaboratif en client-serveur

Prérequis :
- Linux (tests réalisés sous Ubuntu)
- GCC (version 9 ou supérieure)
- Bibliothèques SDL2, SDL2_image, SDL2_ttf et leurs paquets de développement
- Pthread

Pour installer les dépendances sous Ubuntu :
sudo apt update
sudo apt install build-essential libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev

Compilation :
Serveur :
gcc -pthread -o server server.c

Client SDL :
gcc -pthread `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf -o sh13_pons client_sdl.c

Lancement :
1. Démarrer le serveur sur un port libre, par exemple 12345 :
./server 12345

2. Dans quatre terminaux différents, lancer chacun un client :
./sh13_pons 127.0.0.1 12345 127.0.0.1 <port_client> <nom_joueur>
Exemple :
./sh13_pons 127.0.0.1 12345 127.0.0.1 5000 Joueur1
./sh13_pons 127.0.0.1 12345 127.0.0.1 5001 Joueur2
./sh13_pons 127.0.0.1 12345 127.0.0.1 5002 Joueur3
./sh13_pons 127.0.0.1 12345 127.0.0.1 5003 Joueur4

Jeu :
1. Dans chaque fenêtre client, cliquer sur CONNECT.
2. Chaque client reçoit son identifiant, la liste des joueurs, sa main (3 cartes) et sa grille de symboles.
3. Quand c’est votre tour (le bouton GO apparaît) :
   - Investiguer : sélectionner un symbole, puis GO.
   - Interroger : choisir un joueur et un symbole, puis GO.
   - Accuser : sélectionner un suspect dans la liste, puis GO.
4. Les réponses s’affichent dans la grille. Les éliminations et la victoire sont annoncées automatiquement.

Nettoyage :
Pour supprimer les binaires :
rm server sh13_pons

