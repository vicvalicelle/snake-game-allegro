#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>

//Essas constantes definem as dimensões da tela, o tamanho de cada segmento da cobra e a velocidade dela.
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SNAKE_SIZE 20
#define SPEED 20

typedef struct Point {
  int x, y;
} Point;

typedef struct Node {
  Point point;
  struct Node *next;
} Node;

typedef struct {
  Node *head;
  Node *tail;
  int length;
  int dx, dy;
} Snake;

typedef struct {
  Point position;
} Food;

//Para fins de organização e estruturação, o jogo é dividido em diferentes estados pelos quais o jogador pode passar. Dessa forma, existem 4 estados possíveis para o jogador:
/*
MENU → Em que o jogo exibe o menu principal
PLAYING → Que é o momento em que ocorre a jogatina
GAME_OVER → Quando o jogador perde, finalizando o jogo
HIGH_SCORES → Que é quando é exibido as pontuações mais altas
*/

typedef enum { MENU, PLAYING, GAME_OVER, HIGH_SCORES } GameState;

//Variáveis globais que armazenam se o jogador ativou o “modo invertido”, a pontuação do jogador no momento de execução do jogo e o recorde de pontuação de 5 jogadores.
extern bool inverted_mode;
extern int current_score;
extern int high_scores[5];

Node *create_node(int x, int y);
void append_node(Snake *snake, int x, int y);
void delete_node(Snake *snake);
void move_snake(Snake *snake);
void draw_snake(Snake *snake);
void draw_food(Food *food);
bool check_collision_with_self(Snake *snake);
void randomize_food_position(Food *food);
void draw_menu(ALLEGRO_FONT *font);
void draw_game_over(ALLEGRO_FONT *font);
void draw_high_scores(ALLEGRO_FONT *font);
void update_high_scores();
void save_high_scores();
void load_high_scores();

#endif // BIBLIOTECA_H
