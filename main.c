#include <stdio.h>
#include <stdbool.h>
#include <raylib.h>

#define PLAYER_CHECKER_COUNT 16
#define PLAYER_COUNT 2

typedef struct Position {
  int x;
  int y;
}Position;

typedef struct Checker {
  Position pos;
  bool is_alive;
}Checker;

typedef struct Player {
  Checker cs[PLAYER_CHECKER_COUNT];
  Color c;
}Player;

typedef struct GameState {
  Player players[PLAYER_COUNT];
  bool is_game_over;
} GameState;

void player_init(Player *p, Color c, int offset) {
  p->c = c;
  for (int i = 0; i < PLAYER_CHECKER_COUNT; i++) {
    p->cs[i] = (Checker){
      .pos = {
        .x=i%8,
        .y=i/8 + offset
      },
      .is_alive = true
    };
  }
}

void game_init(GameState *game) {
  for (int i = 0; i < PLAYER_COUNT; i++) {
    Color c;
    int offset;
    if (i == 0) {
      c = RED;
      offset = 0;
    } else {
      c = BLACK;
      offset = 6;
    }
    player_init(&game->players[i], c, offset);
  }
}

void draw_checkers(Player p, int grid_size, Position board_start, int checker_radius) {
  for (int i = 0; i < PLAYER_CHECKER_COUNT; i++) {
    int x_offset = p.cs[i].pos.x * grid_size + board_start.x;
    int y_offset = p.cs[i].pos.y * grid_size + board_start.y;
    DrawCircle(x_offset + grid_size/2, y_offset + grid_size/2, checker_radius, p.c);
  }
}

void display_board(GameState game, int grid_count, int grid_size, Position board_start) {
  // draw grid
  for (int x = 0; x < grid_count; x++) {
    for (int y = 0; y < grid_count; y++) {
      Color c;
      if ((x + y) % 2 == 0) {
        c = BLUE;
      } else {
        c = GREEN;
      }
      // top left corner of each square
      int x_offset = (x*grid_size) + board_start.x;
      int y_offset = (y*grid_size) + board_start.y;
      DrawRectangle(x_offset, y_offset, grid_size, grid_size, c);
    }
  }
  for (int i = 0; i < PLAYER_COUNT; i++) {
    draw_checkers(game.players[i], grid_size, board_start, 2.f*(float)grid_size/5.f);
  }
}

int main() {
  GameState game = {0};
  game_init(&game);
  InitWindow(800, 600, "Checkers");
  int board_size = 500;
  int grid_count = 8;
  // checkers board is an 8x8 grid
  // we want alternating pieces of dark and light colors
  int grid_size = board_size/grid_count;
  int board_start_x = 150;
  int board_start_y = 50;
  while (!WindowShouldClose()) {
    BeginDrawing();
      ClearBackground((Color) {
        .r=200, .g=200, .b=200, .a=255
      });
      display_board(game, grid_count, grid_size, (Position) {board_start_x, board_start_y});
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
