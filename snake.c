//
// Created by zoblaigg on 21/09/23.
//
#include "graphics.h"

#define BLOCK_SIZE 16
#define BODY_WIDTH 16    //le rayon du cercle qui sera la dimension du corps du serpent
#define PIXEL 32        // pour definir chaque case de la map
#define COLDIM 32      //1024 en pixels
#define ROWDIM 16     //512 en pixels
#define WALL '*'
#define EMPTY 'e'
#define APPLE '#' // Pomme
#define MAGIC_APPLE '@' // Pomme qui fait grandir le serpent de 2 unités

typedef struct Snake
{
    POINT body;
    struct Snake *next;

} Snake ;
/* Ensemble de tous les prototypes */
Snake * initialize_snake();                        // ---> Fonction d'initialisation du snake
char **initialize_food_and_map(char level_map[]);                  // ---> initialise la 1ere pomme et la map contituer de murs
void display_snake(Snake *snake);                 // ---> Fonction  d'affichage
void remove_tail(Snake *snake);                 // ---> Fonciton qui efface la queue
void upgrade_position_snake(POINT *direction); // Mise à jour de la postion
POINT move_snake (Snake **snake,POINT direction,char **map);// ---> Foncton qui fais deplacer le snake
void add_new_cell_before(Snake **snake,POINT new_head);              // ---> Foncton permettant d'aggrandire le snake
void remove_last_cell(Snake **snake);
void reload_apple(Snake *snake, char **map);                               // ---> Recharge aléatoirement la postion  de la pomme
int verif_snake_mouve(Snake *snake,char **map,POINT next_coor); // ---> Verifie si le snake est dans un mur ou a manger un pomme
void display_decor_map(char **map,char *player_name);
void teleportation(char ** map,POINT *point);
char* get_player_name(void);
void display_player_score(int score);
char* intToChar(int x);
void remove_old_apple(char ** map,POINT old_apple);
void fill_map_level(char ** map,char level_map[]);
char* recup_level_map(char map_level[]);
void welcome_to_snake(void);


int main (void)
{
    Snake *snake; char **map = NULL; int quit,stop =0,score =0;
    char *player_name, *level_map = NULL;
    POINT direction,snake_head;
    direction.x=1;direction.y=0;
    srand(time(NULL));

    player_name= get_player_name();
    welcome_to_snake();
    level_map =  recup_level_map("map1.txt");

    do {
        if(score >= 20){

            level_map =recup_level_map("map2.txt");
        } stop ++;

        init_graphics((COLDIM +5)*PIXEL ,ROWDIM*PIXEL);
        map = initialize_food_and_map(level_map);
        snake = initialize_snake();
        display_decor_map(map,player_name);
        display_snake(snake);

        do
        {
            attendre (150);
            upgrade_position_snake(&direction);
            remove_tail(snake);
            snake_head = move_snake (&snake,direction,map);
            quit = verif_snake_mouve(snake,map,snake_head); // Pour vérifier si la pomme à été bouffée par le snake

            if(quit == 1 || quit ==2){ // Ajoute une seul cellule si la POMME a été mangée

                add_new_cell_before(&snake,snake_head);
                remove_old_apple(map,snake_head);
                if(quit == 2){
                    snake_head = move_snake (&snake,direction,map);
                    add_new_cell_before(&snake,snake_head);  score ++;
                }
                reload_apple(snake,map); score ++;
            }
            else {
                remove_last_cell(&snake);
                add_new_cell_before(&snake, snake_head);
            }
            display_snake(snake);
            display_player_score(score);
        }while (quit != 0);

        init_graphics(COLDIM*PIXEL,ROWDIM*PIXEL); POINT p;p.x= 25;p.y=300;
        if(score >=20 && stop ==1){
            aff_pol("Bravo ! Vous avez au moins 2O points. Let's go to next level !",30,p,white);
        }else{
            aff_pol("  Dommage ! Allez ! tu peux mieux faire ! ;)",40,p,white);
        }
        attendre(5000);

    }while( score >= 20 && stop == 1);
    wait_escape();
    return 0;
}
char **initialize_food_and_map(char level_map[])   // !!! Attention cette fonction ne renvoie que la map déjà remplie de murs
{
    int i;
    char **map = NULL;
    POINT apple;

    map = (char**) malloc(sizeof(char*) * ROWDIM);

    for(i = 0;i <ROWDIM; i++){
        map[i]= (char*) malloc(sizeof(char) * COLDIM);
    }

    fill_map_level(map,level_map); //initialisaion des caractères qui indiqueront la possibilité du serpent à se telporter du coté symetrique

        // Recupère la première pomme
    apple.x = PIXEL * 10; apple.y = PIXEL * 10;
    map[apple.x/PIXEL][apple.x/PIXEL] = APPLE;
    draw_fill_circle(apple,BODY_WIDTH,vert);

    return map;
}

Snake * initialize_snake()
{
    //initialisation du snake avec deux disques
    Snake *snake;
    snake = (Snake*)malloc(sizeof(Snake));
    snake->body.x = 192; snake->body.y = 192;
    snake->next = (Snake *)malloc(sizeof(Snake));
    snake->next->body.x = 160; snake->next->body.y= 192;
    snake->next->next = NULL;

    return snake;
}
void display_snake(Snake *snake)
{
    Snake *snake_tmp;
    snake_tmp = snake->next ;

    draw_fill_circle(snake->body, BODY_WIDTH, rouge);
    while(snake_tmp != NULL){

        draw_fill_circle(snake_tmp->body, BODY_WIDTH, orange);
        snake_tmp = snake_tmp->next;
    }
    return;
}

void remove_tail(Snake *snake)
{
    Snake *Snake_tmp;
    Snake_tmp = snake;

    while(Snake_tmp->next !=NULL) {
        Snake_tmp = Snake_tmp->next;
    }
    draw_fill_circle(Snake_tmp->body, BODY_WIDTH, black);
    return;
}

void upgrade_position_snake(POINT* direction)
{
    POINT direction_tmp;
    POINT old_direction;
    old_direction.x = direction->x;
    old_direction.y = direction->y;

    direction_tmp = get_arrow();
    if (direction_tmp.x != 0 || direction_tmp.y != 0)
    {
        direction->x = direction_tmp.x;
        direction->y = direction_tmp.y;
    }
    if(old_direction.x == - direction->x){  // Ce if permet d'éviter le retour brusque en arrière
        direction->x = - direction->x;
    }
    if(old_direction.y == - direction->y){
        direction->y = - direction->y;
    }
    return;
}

POINT move_snake (Snake **snake, POINT direction,char **map) // lui faire envooyer la rirection afin de pouvoir tester
{
    POINT  head_tmp;
    head_tmp.x =  (*snake)->body.x;
    head_tmp.y =  (*snake)->body.y;

    if (direction.x != 0)     // ces if permettent de faire évoluer la direction  du serpent
    {
        head_tmp.x  = (*snake)->body.x + (direction.x * PIXEL);
        head_tmp.y = (*snake)->body.y;
    }
    else if (direction.y != 0)
    {
        head_tmp.x  = (*snake)->body.x;
        head_tmp.y = (*snake)->body.y + (direction.y * PIXEL);
    }

    //TELEPORTATION
    teleportation(map,&head_tmp);

    return head_tmp;
}

void add_new_cell_before(Snake **snake,POINT new_head)
{
    //printf("je suis dans grow up\n");
    Snake * new_link = NULL;
    new_link = (Snake*)malloc(sizeof(Snake));
    if(new_link == NULL) {
        fprintf(stderr, "Erreur d'allocation !");
        exit(EXIT_FAILURE);
    }
    new_link->body = new_head;
    new_link->next = *snake;
    *snake = new_link;
    return;
}


void reload_apple(Snake *snake,char **map)
{
    POINT apple;
    apple.x = ((rand()%(COLDIM-4))+2) *PIXEL;   // COLDIM-2 permet d'éviter que la pomme apparaisse sur sur les murs
    apple.y = ((rand()%(ROWDIM-4))+2)*PIXEL;  // (nombre aléatoire choisi entre [2,ROWDIM-3] et [2,COLDIM-3)                                                                */

    while (snake != NULL){  // Cette boucle s'assure que la pomme ne se recharge pas sur le corps du snake
        if((snake->body.x == apple.x && snake->body.y == apple.y) || map[apple.y/PIXEL][apple.x/PIXEL] == WALL ){
            apple.x = ((rand()%(COLDIM-4))+2) *PIXEL;
            apple.y = ((rand()%(ROWDIM-4))+2)*PIXEL;
        }
        snake = snake->next;
    }

    map[apple.y/PIXEL][apple.x/PIXEL] = APPLE;
    draw_fill_circle(apple,BODY_WIDTH,green);
    if(apple.y/PIXEL %2 == 0){
        map[apple.y/PIXEL][apple.x/PIXEL] = MAGIC_APPLE;
        draw_fill_circle(apple,BODY_WIDTH,red);
    }
    return ;
}

int verif_snake_mouve(Snake *snake,char **map,POINT next_coor)
{
    POINT snake_head,apple;
    snake_head = snake->body;

    if( map[(next_coor.y/PIXEL)][(next_coor.x/PIXEL)] == WALL){
        printf(" Vous avez perdu ! le snake à touché un mur\n");
        return 0;
    }
    else if( map[next_coor.y/PIXEL][next_coor.x/PIXEL] == APPLE || map[next_coor.y/PIXEL][next_coor.x/PIXEL] == MAGIC_APPLE){
        apple.x=next_coor.x;apple.y =next_coor.y;
        draw_fill_circle(apple,BODY_WIDTH,black);
        if(map[next_coor.y/PIXEL][next_coor.x/PIXEL] == MAGIC_APPLE) return 2 ;
        return 1;
    }
    else{
        snake = snake->next;
        while (snake != NULL){  // Verifi si le serpent se mort la queue

            if(snake->body.x == snake_head.x && snake->body.y == snake_head.y) return 0;
            snake = snake->next;
        }
    }

    return 3;
}

void display_decor_map(char **map,char *player_name)
{
    POINT point1,point2;
    int i,j ;                     // Affichage des murs
    for ( i = 0; i < ROWDIM; ++i) {
        for ( j = 0; j < COLDIM; ++j) {
            if(map[i][j] == WALL){
                point1.x= (j*PIXEL) -BODY_WIDTH ; point1.y = (i*PIXEL) - BODY_WIDTH;
                point2.x = point1.x + PIXEL; point2.y = point1.y + PIXEL;
                draw_fill_rectangle(point1,point2,magenta);
            }
        }
    }
    //  Cadrage de jeu
    point1.x= (COLDIM-1)*PIXEL +BLOCK_SIZE; point1.y = BLOCK_SIZE; point2.x = (COLDIM-1)*PIXEL +BLOCK_SIZE; point2.y = (ROWDIM-1)*PIXEL +BLOCK_SIZE ;
    draw_fill_rectangle(point1,point2,jaune); //cadre de droite
    point1.x= point1.y = BLOCK_SIZE; point2.x = BLOCK_SIZE; point2.y = (ROWDIM-1)*PIXEL +BLOCK_SIZE;
    draw_fill_rectangle(point1,point2,jaune);  //cadre de gauche
    point1.x= BLOCK_SIZE; point1.y = BLOCK_SIZE; point2.x = (COLDIM + 4)*PIXEL+BLOCK_SIZE; point2.y = BLOCK_SIZE;
    draw_fill_rectangle(point1,point2,jaune); //cadre du bas
    point1.x= BLOCK_SIZE; point1.y = (ROWDIM-1)*PIXEL + BLOCK_SIZE; point2.x = (COLDIM +4)*PIXEL+ BLOCK_SIZE; point2.y = (ROWDIM-1)*PIXEL + BLOCK_SIZE;
    draw_fill_rectangle(point1,point2,jaune); //cadre du haut
    // cadre du score
    point1.x= (COLDIM+4)*PIXEL +BLOCK_SIZE; point1.y = BLOCK_SIZE; point2.x = (COLDIM + 4)*PIXEL +BLOCK_SIZE; point2.y = (ROWDIM-1)*PIXEL +BLOCK_SIZE ;
    draw_fill_rectangle(point1,point2,jaune); //cadre de droite
    point1.x= (COLDIM-1)*PIXEL +BLOCK_SIZE; point1.y = ((ROWDIM+2)*PIXEL + BLOCK_SIZE)/2; point2.x = (COLDIM +4)*PIXEL+ BLOCK_SIZE; point2.y = ((ROWDIM+2)*PIXEL + BLOCK_SIZE)/2;
    draw_fill_rectangle(point1,point2,jaune); //cadre du haut
    point1.x = (COLDIM)*PIXEL; point1.y = (ROWDIM-2)*PIXEL  ;
    aff_pol("Pseudo :",30,point1,magenta);
    point1.x = (COLDIM)*PIXEL; point1.y = (ROWDIM-4)*PIXEL  ;
    aff_pol(player_name,30,point1,yellow);
    point1.x = (COLDIM)*PIXEL; point1.y = (ROWDIM-8)*PIXEL  ;
    aff_pol(" Level 1 ",30,point1,white);
    point1.x = (COLDIM)*PIXEL; point1.y = (ROWDIM-10)*PIXEL  ;
    aff_pol("\tScore ",30,point1,magenta);

    
    return;
}


void teleportation(char ** map,POINT *point)
{
    if (map[point->y/PIXEL ][point->x/PIXEL] == EMPTY) {

        if(point->x == 0){
            point->x = (COLDIM-1)*PIXEL;
        }
        else if(point->y == 0){
            point->y = (ROWDIM-1)*PIXEL;
        }
        else if(point->x == (COLDIM-1)*PIXEL){
            point->x = 1*PIXEL;
        }
        else if(point->y == (ROWDIM-1)*PIXEL){
            point->y = 1*PIXEL;
        }
        return;
    }

    return;
}

void remove_last_cell(Snake **snake)
{
    Snake *link_tmp;
    if(snake != NULL){

        if((*snake)->next == NULL){
            free(*snake);
            *snake = NULL;
        }else{
            link_tmp = *snake;
            while (link_tmp->next->next != NULL){
                link_tmp = link_tmp->next;
            }
            free(link_tmp->next);
            link_tmp->next =NULL;
        }
    }
    return;
}


char* get_player_name(void)
{
    char *player_name;
    player_name = (char*)malloc(20*sizeof(char));
    printf(" Welcome !  What is your name please ?\n");
    scanf(" %s",player_name);
    printf("Thank you !");

    return player_name;
}

void display_player_score(int score)
{
    POINT point1,point2;
    char *c_score;
    point1.x = (COLDIM)*PIXEL; point1.y = (ROWDIM-12)*PIXEL +BLOCK_SIZE;
    point2.x = (COLDIM + 3)*PIXEL+2*BLOCK_SIZE; point2.y = 2*BLOCK_SIZE;
    draw_fill_rectangle(point1,point2,black); //cadre de droite

    point1.x = (COLDIM+1)*PIXEL -BLOCK_SIZE; point1.y = (ROWDIM-12)*PIXEL ;
    c_score =  intToChar(score);
    aff_pol(c_score,70, point1, green);

    return;
}

char* intToChar(int x) {
    char int_to_char[20]; // Pour stocker la chaîne de caractères

    // Convertir l'entier en une chaîne de caractères
    sprintf(int_to_char, "%d", x);

    // Allouer de la mémoire pour une copie de la chaîne de caractères
    char* result = (char*)malloc(strlen(int_to_char) + 1);

    // Copier la chaîne de caractères dans le résultat
    strcpy(result, int_to_char);

    return result;
}

void remove_old_apple(char ** map,POINT old_apple)
{
    map[old_apple.y/PIXEL][old_apple.x/PIXEL] = 'h';

    return;
}


void fill_map_level(char ** map, char level_map[]){

    FILE *fichier = NULL;
    fichier = fopen(level_map, "r+");
    if (fichier == NULL) {
        printf("Erreur sur fichier\n");
        exit(1);
    }
    char t;
    rewind(fichier);

    for (int i = 0; i < ROWDIM; ++i) {
        for (int j = 0; j < COLDIM; ++j) {
            fscanf(fichier, "%c ",&t);
            map[ROWDIM-1-i][j] = t;
        }
    }
    fclose(fichier);
    return;
}

char* recup_level_map(char map_level[])
{
    char * level;
    level = (char*)malloc(10*sizeof(char));
    level = map_level;

    return level;
}


void welcome_to_snake(void)
{
    POINT point1;
    point1.x = 150; point1.y = 300;

    init_graphics(COLDIM * PIXEL, ROWDIM * PIXEL);
    aff_pol("WELCOME to Gg SNAKE ",50,point1,yellow);
    attendre(1000);
    return;
}