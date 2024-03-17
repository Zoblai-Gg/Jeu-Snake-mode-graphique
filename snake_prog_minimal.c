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

typedef struct Snake
{
    POINT body;
    struct Snake *next;

} Snake ;
/* Ensemble de tous les prototypes */
// supprimer le point de la tete en dehors du snake

Snake * initialize_snake();                        // ---> Fonction d'initialisation du snake
char **initialize_food_and_map();                  // ---> initialise la 1ere pomme et la map contituer de murs
void display_snake(Snake *snake);                 // ---> Fonction  d'affichage
void remove_tail(Snake *snake);                 // ---> Fonciton qui efface la queue
void upgrade_position_snake(POINT *direction); // Mise à jour de la postion
POINT move_snake (Snake **snake,POINT direction,char **map);// ---> Foncton qui fais deplacer le snake
void add_new_cell_before(Snake **snake,POINT new_head);              // ---> Foncton permettant d'aggrandire le snake
void remove_last_cell(Snake **snake);
void reload_apple(Snake *snake, char **map);                               // ---> Recharge aléatoirement la postion  de la pomme
int verif_snake_mouve(Snake *snake,char **map,POINT next_coor); // ---> Verifie si le snake est dans un mur ou a manger un pomme
void display_decor_map(char **map);
void teleportation(char ** map,POINT *point);
void remove_old_apple(char ** map,POINT old_apple);

int main (void)
{
    Snake *snake; char **map = NULL; int quit;
    POINT direction,snake_head;
    direction.x=1;direction.y=0;
    srand(time(NULL));

    init_graphics(COLDIM*PIXEL ,ROWDIM*PIXEL);
    map = initialize_food_and_map();
    snake = initialize_snake();
    display_decor_map(map);
    display_snake(snake);

    do
    {
        attendre (150);
        upgrade_position_snake(&direction);
        remove_tail(snake);
        snake_head = move_snake (&snake,direction,map);
        quit = verif_snake_mouve(snake,map,snake_head);

        if(quit == 1){ // Pour vérifier si la pomme a été mangée par le snake

            add_new_cell_before(&snake,snake_head);
            remove_old_apple(map, snake_head);
            reload_apple(snake,map);
        }
        else {
            remove_last_cell(&snake);
            add_new_cell_before(&snake, snake_head);
        }
        display_snake(snake);
    }while (quit != 0);

    init_graphics(COLDIM*PIXEL,ROWDIM*PIXEL); POINT p;p.x= 200;p.y=300;

    aff_pol("Dommage ! Vous avez perdu !",40,p,white);
    wait_escape();
    return 0;
}
char **initialize_food_and_map()   // !!! Attention cette fonction ne renvoie que la map déjà remplie de murs
{
    int i,x,y;
    char **map = NULL;
    POINT apple;

    map = (char**) malloc(sizeof(char*) * ROWDIM);

    for(i = 0;i <ROWDIM; i++){
        map[i]= (char*) malloc(sizeof(char) * COLDIM);
    }
    //initialisaion des caractères qui indiqueront la possibilité du serpent à se telporter du coté symetrique
    for(i = 0; i < ROWDIM; i++) {
        // murs verticaux
        map[i][0] = EMPTY;
        map[i][COLDIM -1] = EMPTY;
    }
    for(i = 0; i < COLDIM; i++) {
        // murs horizontaux
        map[0][i] = EMPTY;
        map[ROWDIM - 1][i] = EMPTY;
    }
    // Faire apparaitre 6 murs aléatoires
    for(i = 0; i < 6; i++) {
        x = ((rand()%(COLDIM-4))+2) ;
        y = ((rand()%(ROWDIM-4))+2);
        map[y][x] = WALL;
    }
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
    snake->body.x = 160; snake->body.y = 160;
    snake->next = (Snake *)malloc(sizeof(Snake));
    snake->next->body.x = 128; snake->next->body.y= 160;
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

void add_new_cell_before(Snake **snake, POINT new_head)
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
        if((snake->body.x == apple.x && snake->body.y == apple.y ) || (map[apple.y/PIXEL][apple.x/PIXEL] == WALL)){
            apple.x = ((rand()%(COLDIM-4))+2) *PIXEL;
            apple.y = ((rand()%(ROWDIM-4))+2)*PIXEL;
        }
        snake = snake->next;
    }
    map[apple.y/PIXEL][apple.x/PIXEL] = APPLE;
    draw_fill_circle(apple,BODY_WIDTH,vert);

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
    else if( map[next_coor.y/PIXEL][next_coor.x/PIXEL] == APPLE ){
        apple.x=next_coor.x;apple.y =next_coor.y;
        draw_fill_circle(apple,BODY_WIDTH,black);
        //map[next_coor.y/PIXEL][next_coor.x/PIXEL] = 'a';
        return 1;
    }else{
        snake = snake->next;
        while (snake != NULL){  // Verifi si le serpent se mort la queue

            if(snake->body.x == snake_head.x && snake->body.y == snake_head.y){
                printf(" Vous avez perdu ! le serpeent a mordu sa queue\n");
                return 0;
            }
            snake = snake->next;
        }
    }

    return 2;
}

void display_decor_map(char **map)
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
    point1.x= BLOCK_SIZE; point1.y = BLOCK_SIZE; point2.x = (COLDIM-1)*PIXEL+BLOCK_SIZE; point2.y = BLOCK_SIZE;
    draw_fill_rectangle(point1,point2,jaune); //cadre du bas
    point1.x= BLOCK_SIZE; point1.y = (ROWDIM-1)*PIXEL + BLOCK_SIZE; point2.x = (COLDIM-1)*PIXEL+ BLOCK_SIZE; point2.y = (ROWDIM-1)*PIXEL + BLOCK_SIZE;
    draw_fill_rectangle(point1,point2,jaune); //cadre du haut
    // cadre du score
    //  point1.x = (COLDIM-1)*PIXEL+ BLOCK_SIZE; point1.y = (ROWDIM-1)*PIXEL + BLOCK_SIZE; point1.x=  ; point1.y = (ROWDIM-1)*PIXEL + BLOCK_SIZE;
    // draw_fill_rectangle(point1,point2,jaune); //cadre du haut

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

void remove_old_apple(char ** map,POINT old_apple)
{
    map[old_apple.y/PIXEL][old_apple.x/PIXEL] = 'k';

    return;
}

