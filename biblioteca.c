#include "biblioteca.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

bool inverted_mode = false;
int current_score = 0;
int high_scores[5] = {0};

//Cria um novo nó (que é um segmento da cobra) com as coordenadas especificadas.
Node *create_node(int x, int y) {
  Node *new_node = (Node *)malloc(sizeof(Node));
  new_node->point.x = x;
  new_node->point.y = y;
  new_node->next = NULL;
  return new_node;
}

//Adiciona um novo segmento ao final da cobra.
void append_node(Snake *snake, int x, int y) {
  Node *new_node = create_node(x, y);
  if (snake->tail) {
    snake->tail->next = new_node;
  } else {
    snake->head = new_node;
  }
  snake->tail = new_node;
  snake->length++;
}

//Remove o nó da cabeça da cobra, “movendo” todo o corpo para frente.
void delete_node(Snake *snake) {
  if (snake->head) {
    Node *temp = snake->head;
    snake->head = snake->head->next;
    if (!snake->head) {
      snake->tail = NULL;
    }
    free(temp);
    snake->length--;
  }
}

//“Move” a cobra atualizando as posições dos nós para a direção na qual ela está se movendo.
void move_snake(Snake *snake) {
  Node *current = snake->tail;
  Node *prev = NULL;

  while (current != snake->head) {
    prev = snake->head;
    while (prev->next != current) {
      prev = prev->next;
    }
    current->point.x = prev->point.x;
    current->point.y = prev->point.y;
    current = prev;
  }

  snake->head->point.x += snake->dx;
  snake->head->point.y += snake->dy;
}

//Desenha os segmentos da cobra na tela.
void draw_snake(Snake *snake) {
  Node *current = snake->head;
  ALLEGRO_COLOR snake_color =
      inverted_mode ? al_map_rgb(220, 0, 0) : al_map_rgb(0, 220, 0);

  while (current) {
    al_draw_filled_rounded_rectangle(
        current->point.x, current->point.y, current->point.x + SNAKE_SIZE,
        current->point.y + SNAKE_SIZE, 5, 5, snake_color);
    current = current->next;
  }
}

//Desenha a comida que aumenta um segmento da cobra.
void draw_food(Food *food) {
  ALLEGRO_COLOR food_color =
      inverted_mode ? al_map_rgb(0, 220, 0) : al_map_rgb(220, 0, 0);

  al_draw_filled_circle(food->position.x + SNAKE_SIZE / 2,
                        food->position.y + SNAKE_SIZE / 2, SNAKE_SIZE / 2,
                        food_color);
}

//Verifica se a cobra colidiu com o próprio corpo. Pois, em caso afirmativo, o jogador perde.
bool check_collision_with_self(Snake *snake) {
  Node *current = snake->head->next;
  while (current) {
    if (snake->head->point.x == current->point.x &&
        snake->head->point.y == current->point.y) {
      return true;
    }
    current = current->next;
  }
  return false;
}

//Define uma posição aleatória para a comida na tela.
void randomize_food_position(Food *food) {
  food->position.x = (rand() % (SCREEN_WIDTH / SNAKE_SIZE)) * SNAKE_SIZE;
  food->position.y = (rand() % (SCREEN_HEIGHT / SNAKE_SIZE)) * SNAKE_SIZE;
}

//Exibe a tela do menu inicial.
void draw_menu(ALLEGRO_FONT *font) {
  al_clear_to_color(al_map_rgb(105, 105, 105));

  const char *title = "SNAKE GAME";
  const char *instructions = "Press SPACE to Start";
  const char *inverted_mode_text =
      inverted_mode ? "Inverted Mode: ON" : "Inverted Mode: OFF";
  const char *instructions_inverted = "(Press i to change)";
  const char *exit_instructions = "Press ESC to Exit";
  const char *high_scores_text = "Press H to see High Scores";

  int title_width = al_get_text_width(font, title);
  int instructions_width = al_get_text_width(font, instructions);
  int inverted_mode_width = al_get_text_width(font, inverted_mode_text);
  int instructions_inverted_width =
      al_get_text_width(font, instructions_inverted);
  int exit_instructions_width = al_get_text_width(font, exit_instructions);
  int high_scores_text_width = al_get_text_width(font, high_scores_text);

  al_draw_text(font, al_map_rgb(0, 220, 0), (SCREEN_WIDTH - title_width) / 2,
               SCREEN_HEIGHT / 2 - 50, 0, title);
  al_draw_text(font, al_map_rgb(0, 220, 0),
               (SCREEN_WIDTH - instructions_width) / 2, SCREEN_HEIGHT / 2 - 30,
               0, instructions);
  al_draw_text(font, al_map_rgb(0, 220, 0),
               (SCREEN_WIDTH - inverted_mode_width) / 2, SCREEN_HEIGHT / 2 + 30,
               0, inverted_mode_text);
  al_draw_text(font, al_map_rgb(0, 220, 0),
               (SCREEN_WIDTH - instructions_inverted_width) / 2,
               SCREEN_HEIGHT / 2 + 50, 0, instructions_inverted);
  al_draw_text(font, al_map_rgb(220, 0, 0),
               (SCREEN_WIDTH - exit_instructions_width) / 2,
               SCREEN_HEIGHT / 2 + 110, 0, exit_instructions);
  al_draw_text(font, al_map_rgb(0, 220, 0),
               (SCREEN_WIDTH - high_scores_text_width) / 2,
               SCREEN_HEIGHT / 2 + 90, 0, high_scores_text);

  al_flip_display();
}

//Exibe a tela de Game Over.
void draw_game_over(ALLEGRO_FONT *font) {
  al_clear_to_color(al_map_rgb(0, 0, 0));
  const char *game_over_message = "Game Over! Press SPACE to return to menu";

  int text_width = al_get_text_width(font, game_over_message);
  al_draw_text(font, al_map_rgb(255, 0, 0), (SCREEN_WIDTH - text_width) / 2,
               SCREEN_HEIGHT / 2, 0, game_over_message);
  al_flip_display();
}

//Exibe a tela das pontuações mais altas registradas.
void draw_high_scores(ALLEGRO_FONT *font) {
  al_clear_to_color(al_map_rgb(105, 105, 105));

  const char *title = "High Scores";
  int title_width = al_get_text_width(font, title);
  al_draw_text(font, al_map_rgb(0, 220, 0), (SCREEN_WIDTH - title_width) / 2,
               50, 0, title);

  for (int i = 0; i < 5; i++) {
    char score_text[30];
    sprintf(score_text, "%d. %d", i + 1, high_scores[i]);
    al_draw_text(font, al_map_rgb(0, 220, 0),
                 (SCREEN_WIDTH - al_get_text_width(font, score_text)) / 2,
                 100 + i * 30, 0, score_text);
  }

  const char *back_text = "Press SPACE to return to menu";
  int back_text_width = al_get_text_width(font, back_text);
  al_draw_text(font, al_map_rgb(220, 0, 0),
               (SCREEN_WIDTH - back_text_width) / 2, SCREEN_HEIGHT - 50, 0,
               back_text);

  al_flip_display();
}

//Atualiza o vetor de pontuações altas com a pontuação atual caso ela seja maior do que uma das 5 mais altas.
void update_high_scores() {
  for (int i = 0; i < 5; i++) {
    if (current_score > high_scores[i]) {
      for (int j = 4; j > i; j--) {
        high_scores[j] = high_scores[j - 1];
      }
      high_scores[i] = current_score;
      break;
    }
  }
}

//Salva as maiores pontuações em um arquivo de texto.
void save_high_scores() {
  FILE *file = fopen("high_scores.txt", "w");
  if (file == NULL) {
    printf("Erro ao abrir o arquivo para salvar as pontuações.\n");
    return;
  }

  for (int i = 0; i < 5; i++) {
    fprintf(file, "%d\n", high_scores[i]);
  }

  fclose(file);
}

//Lê e carrega as pontuações altas de um arquivo de texto.
void load_high_scores() {
  FILE *file = fopen("high_scores.txt", "r");
  if (file == NULL) {
    for (int i = 0; i < 5; i++) {
      high_scores[i] = 0;
    }
    return;
  }

  for (int i = 0; i < 5; i++) {
    fscanf(file, "%d", &high_scores[i]);
  }

  fclose(file);
}
