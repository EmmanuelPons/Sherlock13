Description : Ce projet implémente le jeu Sherlock 13 en architecture client‑serveur. Le serveur distribue les cartes, gère les tours et les interactions (investigations, interrogations, accusations). Le client SDL2 affiche la main du joueur, sa fiche de symboles et permet d’envoyer les actions au serveur.

Prérequis : – GCC ou équivalent
– SDL2, SDL2_image, SDL2_ttf et leurs paquets de développement
– pthread
– make (optionnel)

Fichiers principaux : server.c code du serveur
sh13.c code du client SDL2
Makefile script de compilation

Compilation avec le Makefile : Dans le répertoire du projet, lancer la commande : make Cela produit deux exécutables : server et sh13_pons
Pour nettoyer les fichiers générés : make clean

Compilation sans Makefile : Pour le serveur : gcc -Wall -Wextra -g server.c -o server -pthread
Pour le client : gcc -Wall -Wextra -g sdl2-config --cflags --libs sh13.c -o sh13_pons -lSDL2_image -lSDL2_ttf -pthread

Lancement :

    Démarrer le serveur (par exemple sur le port 12345) : ./server 12345

    Dans quatre terminaux différents, démarrer les clients :
    ./sh13_pons 127.0.0.1 12345 127.0.0.1 5000 Joueur1
    ./sh13_pons 127.0.0.1 12345 127.0.0.1 5001 Joueur2
    ./sh13_pons 127.0.0.1 12345 127.0.0.1 5002 Joueur3
    ./sh13_pons 127.0.0.1 12345 127.0.0.1 5003 Joueur4
    Les arguments sont : adresse du serveur, port du serveur, adresse IP du client, port d’écoute du client, nom du joueur.

    Dans chaque fenêtre client, cliquer sur CONNECT. Le serveur envoie alors l’ID, la liste des joueurs, la main et la fiche de symboles. Le bouton GO s’active au tour de chaque joueur.

Utilisation du Makefile : make compile serveur et client
make clean supprime les binaires

Structure du Makefile : Les variables CC et CFLAGS définissent le compilateur et ses options
SDL_CFLAGS et SDL_LIBS sont récupérés via sdl2-config pour le client
La cible all construit server et sh13_pons
La cible clean efface les exécutables
