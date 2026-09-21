# Recherche arborescente Monte-Carlo (MCTS)

Projet C++ consacré à l'implémentation d'un joueur utilisant la **recherche arborescente Monte-Carlo** (*Monte-Carlo Tree Search*, ou **MCTS**) pour affronter différents joueurs dans un jeu décrit comme une **boîte noire**.

Dans ce contexte, le programme ne dispose pas d'une stratégie explicite du jeu : il connaît uniquement les coups possibles, l'évolution de l'état de la partie et le résultat final — victoire ou partie nulle. L'objectif est d'évaluer les coups par simulation afin de sélectionner celui qui offre les meilleures chances de victoire.

## Fonctionnalités

- Organisation de challenges composés de plusieurs parties.
- Alternance du joueur qui commence entre les parties.
- Comparaison de trois stratégies :
  - **MCTS** : recherche arborescente Monte-Carlo ;
  - **Brutal** : joueur de référence qui choisit généralement le dernier coup possible et peut produire un coup invalide ;
  - **Random** : choix aléatoire parmi les coups possibles.
- Limitation du temps de réflexion d'un joueur à **10 millisecondes par coup**.
- Détection des coups invalides, des coups illicites et des joueurs qui ne rendent pas la main dans le temps imparti.
- Exécution des joueurs dans des threads séparés, avec synchronisation par mutex.
- Sauvegarde et rechargement de l'arbre MCTS dans le fichier `mcts_tree.dat`.
- Affichage du déroulement des parties et du bilan final du challenge.

## Principe du jeu

Le jeu est représenté par un état entier. Chaque coup est ajouté à la représentation de l'état en décalant sa valeur d'un chiffre vers la gauche puis en ajoutant le coup joué.

La classe `Jeu` fournit notamment les opérations suivantes :

- déterminer le nombre de coups disponibles ;
- vérifier si un état est terminal ;
- vérifier si un coup est licite ;
- jouer un coup ;
- déterminer s'il y a victoire ou partie nulle ;
- réinitialiser la partie.

Les états terminaux et les états gagnants sont définis directement dans `jeu.cpp`, ce qui permet de conserver le jeu sous la forme d'une boîte noire pour les joueurs.

## Joueurs disponibles

### Joueur aléatoire

`Joueur_Random` sélectionne un coup au hasard parmi les coups disponibles. Il sert de référence simple pour évaluer le comportement du joueur MCTS.

### Joueur brutal

`Joueur_Brutal` choisit généralement le dernier coup disponible. Il simule également occasionnellement un coup invalide, ce qui permet de tester la gestion des erreurs de l'arbitre.

### Joueur MCTS

`Joueur_MCTS` construit et explore un arbre de recherche composé de nœuds contenant notamment :

- le coup ayant mené au nœud ;
- le nombre de visites ;
- le nombre de victoires ;
- les coups qui n'ont pas encore été essayés ;
- les nœuds enfants.

La sélection des nœuds repose sur une formule combinant exploitation et exploration, proche de la formule UCT. Le joueur effectue ensuite des simulations aléatoires jusqu'à atteindre la limite de temps, puis choisit le coup associé au meilleur résultat observé.

L'arbre de recherche peut être sauvegardé dans `mcts_tree.dat` puis rechargé lors d'une nouvelle exécution.

## Arbitre

La classe `Arbitre` gère les confrontations entre deux joueurs :

1. initialisation du jeu et des joueurs ;
2. lancement des parties ;
3. exécution de chaque coup dans un thread dédié ;
4. contrôle du temps de réflexion ;
5. validation du coup joué ;
6. attribution de la victoire en cas de coup invalide ou de dépassement de temps ;
7. comptabilisation des victoires et des parties nulles.

Dans la configuration actuelle de `main.cpp`, un challenge de **150 parties** oppose un joueur aléatoire à un joueur MCTS :

```cpp
Arbitre a(player::RAND, player::MCTS, 150);
a.challenge();
```

## Structure du projet

```text
.
├── CMakeLists.txt          # Configuration principale de compilation
├── main.cpp                # Point d'entrée et configuration du challenge
├── arbitre.h/.cpp          # Gestion des parties et des confrontations
├── jeu.h/.cpp              # Modèle du jeu et gestion des états
├── joueurs/
│   ├── CMakeLists.txt      # Configuration de la bibliothèque des joueurs
│   ├── joueur.h/.cpp       # Classe abstraite commune aux joueurs
│   ├── joueur_random.h/.cpp
│   ├── joueur_brutal.h/.cpp
│   └── joueur_MCTS.h/.cpp  # Implémentation de la stratégie MCTS
├── man.txt                 # Instructions de compilation d'origine
└── mcts_tree.dat           # Fichier de persistance généré par le joueur MCTS
```

## Prérequis

- Un compilateur compatible avec **C++14** ;
- **CMake 3.1** ou une version supérieure ;
- `make` ou un générateur de compilation compatible ;
- le support des threads C++ (`pthread` sous Linux et les systèmes compatibles).

## Compilation

Depuis la racine du dépôt :

```bash
cmake .
make
```

L'exécutable produit s'appelle `Arene_light`.

Pour utiliser un répertoire de compilation séparé :

```bash
cmake -S . -B build
cmake --build build
```

## Exécution

Après compilation depuis la racine du projet :

```bash
./Arene_light
```

Avec une compilation dans le répertoire `build` :

```bash
./build/Arene_light
```

Le programme initialise le générateur aléatoire, lance le challenge configuré dans `main.cpp`, affiche les parties jouées, puis présente le nombre de victoires obtenu par chaque joueur.

## Modifier les joueurs du challenge

Les joueurs utilisés sont définis dans `main.cpp` avec l'énumération `player` :

```cpp
enum class player { MCTS, BRUTAL, RAND };
```

Par exemple, pour comparer MCTS et le joueur brutal :

```cpp
Arbitre a(player::MCTS, player::BRUTAL, 150);
a.challenge();
```

Le nombre de parties peut également être modifié en changeant la constante `NB_PARTIES` dans `main.cpp`.

## Technologies utilisées

- C++14 ;
- CMake ;
- bibliothèque standard C++ ;
- threads et mutex ;
- algorithme de recherche arborescente Monte-Carlo ;
- persistance de l'arbre de recherche dans un fichier texte.

## Limites connues

- Les règles du jeu sont codées directement dans `jeu.cpp` et ne sont pas configurables depuis un fichier externe.
- Le temps de réflexion est défini par une constante dans `arbitre.h`.
- Le joueur MCTS utilise actuellement un nombre limité d'itérations et une fenêtre de recherche très courte afin de respecter la contrainte de temps.
- Le fichier `mcts_tree.dat` est généré automatiquement et peut contenir l'état persistant de l'arbre entre deux exécutions.
- Le projet ne contient pas encore de suite de tests automatisés.

## Objectif pédagogique

Ce projet permet d'étudier :

- l'implémentation d'un algorithme MCTS ;
- la comparaison de stratégies de jeu ;
- la conception d'une interface commune pour plusieurs joueurs ;
- la programmation concurrente avec threads, mutex et atomiques ;
- la gestion de contraintes de temps dans un environnement compétitif.
