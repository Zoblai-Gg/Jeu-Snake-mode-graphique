
# Project Snake
Ceci est un jeu classique de Snake où celui-ci peut se déplacer à la recherche de fruits.

Il y a deux fichier.c :
- snake_prog_minimal.c : Ce fichier contient la version minimale du jeu
- snake.c : Ce fichier contient une version assez complète du jeu 

Dans le snake_prog_bonus, il y a deux niveaux de jeu. Lorsque le score du joueur atteint au moins 20 points même après avoir perdu, celui-ci sera rediriger vers le niveau 2.


## À connaitre sur le fonctionne ce jeu 

### Les murs

En fonction du niveau de jeu, les murs ont des dispositions différentes. Lorsque le snake touche le mur le jeu se termine.

### Concernant les fruits

Pomme verte : fait agrandir le snake d'une unité

Pomme rouge : fait agrandir celui-ci de 2 unités

### Système de score 

Si le snake mange une pomme verte, le joueur gagne un point. Et si c'est la pomme rouge qui est mangée, il gagne de deux points.

### Comment compiler et exécuter

- make snake
- ./snake

#### Dans le cas où il y a une erreur avec la bibliothèque SDL.h, il faudra l'installer avec :

sudo apt-get update

sudo apt-get install libsdl1.2-dev


## À savoir sur le code

Dans le fichier snake.c la map est géré par des appels de fichiers. Ainsi, on peut changer de map au cours du jeu.




