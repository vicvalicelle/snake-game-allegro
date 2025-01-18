#include "biblioteca.h"

int main() {
  srand(time(NULL));

//Inicializa a biblioteca Allegro. Caso ocorra alguma falha, o jogo não poderá continuar
  if (!al_init()) {
    printf("Failed to initialize Allegro.\n");
    return -1;
  }

  al_init_primitives_addon();
  al_install_keyboard();
  al_init_font_addon();
  al_init_ttf_addon();

//Cria uma janela com as dimensões já definidas em constantes, mas, caso ocorra alguma falha, o jogo irá encerrar.
  ALLEGRO_DISPLAY *display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
  if (!display) {
    printf("Failed to create display.\n");
    return -1;
  }

  ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue(); //Cria uma fila para armazenar eventos do jogo. Os eventos são ocorrências que o programa precisa captar e processar para que o jogo funcione corretamente.
  ALLEGRO_TIMER *timer = al_create_timer(0.1); //Cria um timer que gera eventos a cada 0,1 segundo usado para controlar a taxa de atualização do jogo.
  ALLEGRO_FONT *font = al_create_builtin_font(); //Cria uma fonte simples que pode ser usada para desenhar texto na tela.

//Caso qualquer uma dessas criações falhe, os recursos já alocadas são liberados e o jogo irá terminar.
  if (!event_queue || !timer || !font) {
    printf("Failed to create event queue, timer or font.\n");
    al_destroy_display(display);
    return -1;
  }

//Registra as fontes de evento do teclado, timer e display na fila de eventos.
  al_register_event_source(event_queue, al_get_keyboard_event_source());
  al_register_event_source(event_queue, al_get_timer_event_source(timer));
  al_register_event_source(event_queue, al_get_display_event_source(display));
  al_start_timer(timer);

//Inicialização das estruturas necessárias para o funcionamento do jogo
  Snake snake = {NULL, NULL, 0, SPEED, 0};
  Food food;

  GameState state = MENU; //Inicializa o jogo no menu principal
  bool running = true; //Variável necessária para o funcionamento contínuo do jogo, controlando o "ciclo" de vida do programa

  load_high_scores(); //Carrega as pontuações mais altas do arquivo

  while (running) { //Loop principal do jogo
    ALLEGRO_EVENT event;
    al_wait_for_event(event_queue, &event);

    //A partir daqui, o código processa diferentes tipos de eventos para que o programa funcione adequadamente
    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
      running = false;
    } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) { // Verifica se uma tecla foi pressionada
      if (state == MENU) {
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { // Fecha o jogo se a tecla ESC for pressionada (se estiver no menu)
          running = false; 
        } else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
          //Inicia o jogo quando a barra de espaço for pressionada
          state = PLAYING;
          snake.head = snake.tail = NULL;
          snake.length = 0;
          snake.dx = SPEED;
          snake.dy = 0;
          append_node(&snake, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
          randomize_food_position(&food);
          current_score = 0;
        } else if (event.keyboard.keycode == ALLEGRO_KEY_I) { //Ativa ou desativa o modo invertido
          inverted_mode = !inverted_mode;
        } else if (event.keyboard.keycode == ALLEGRO_KEY_H) { //Muda para a tela das pontuações mais altas
          state = HIGH_SCORES;
        }
        //Se o estado for GAME_OVER e a barra de espaço for pressionada, o jogo retorna ao menu principal
      } else if (state == GAME_OVER) {
        if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
          state = MENU;
        }
      } else if (state == PLAYING) {
        //Permite que o jogador controle a direção da cobra usando as setas do teclado
        switch (event.keyboard.keycode) {
        case ALLEGRO_KEY_UP:
          if (snake.dy == 0) {
            snake.dx = 0;
            snake.dy = inverted_mode ? SPEED : -SPEED; // Se o modo invertido estiver ativo, a direção é invertida
          }
          break;
        case ALLEGRO_KEY_DOWN:
          if (snake.dy == 0) {
            snake.dx = 0;
            snake.dy = inverted_mode ? -SPEED : SPEED;
          }
          break;
        case ALLEGRO_KEY_LEFT:
          if (snake.dx == 0) {
            snake.dx = inverted_mode ? SPEED : -SPEED;
            snake.dy = 0;
          }
          break;
        case ALLEGRO_KEY_RIGHT:
          if (snake.dx == 0) {
            snake.dx = inverted_mode ? -SPEED : SPEED;
            snake.dy = 0;
          }
          break;
        }
      //Se o jogador está na tela de pontuações altas, ele pode retornar ao menu principal pressionando a barra de espaço.
      } else if (state == HIGH_SCORES) {
        if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
          state = MENU;
        }
      }
    }

    //Processa eventos de timer que controlam o movimento da cobra e outras atualizações periódicas
    if (state == PLAYING && event.type == ALLEGRO_EVENT_TIMER) {
      move_snake(&snake);

      //Verifica se a cobra colidiu com a borda da tela ou com ela mesma
      if (snake.head->point.x < 0 || snake.head->point.x >= SCREEN_WIDTH ||
          snake.head->point.y < 0 || snake.head->point.y >= SCREEN_HEIGHT ||
          check_collision_with_self(&snake)) {
        state = GAME_OVER;
        update_high_scores();
        current_score = 0;
      }

      //Verifica se a cobra comeu a comida
      if (snake.head->point.x == food.position.x &&
          snake.head->point.y == food.position.y) {
        append_node(&snake, snake.tail->point.x, snake.tail->point.y);
        randomize_food_position(&food);
        current_score++;
      }

      //Atualiza a tela com as mudanças
      al_clear_to_color(al_map_rgb(211, 211, 211));
      draw_snake(&snake);
      draw_food(&food);

      //Exibe a pontuação atual na tela
      char score_text[20];
      sprintf(score_text, "Score: %d", current_score);
      al_draw_text(font, al_map_rgb(0, 0, 0), 10, 10, 0, score_text);

      //Atualiza a tela para mostrar as mudanças
      al_flip_display();
    }

    //"Desenha" as diferentes telas (menu, game over, pontuações altas) com base no estado atual do jogo
    if (state == MENU) {
      draw_menu(font);
    } else if (state == GAME_OVER) {
      draw_game_over(font);
    } else if (state == HIGH_SCORES) {
      draw_high_scores(font);
    }
  }

  //Salva as pontuações antes de sair
  save_high_scores(); 

  while (snake.head) {
    delete_node(&snake);
  }

  //Libera memória ocupada pelo recursos alocados para o correto funcionamento do jogo.
  al_destroy_display(display);
  al_destroy_event_queue(event_queue);
  al_destroy_timer(timer);
  al_destroy_font(font);

  return 0;
}
