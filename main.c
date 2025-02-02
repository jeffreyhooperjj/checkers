#include <stdio.h>
#include <stdbool.h>
#include <raylib.h>

#define PLAYER_CHECKER_COUNT 16
#define PLAYER_COUNT 2
#define BACKGROUND_COLOR (Color) {175, 128, 79, 255}

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
  int selected_piece;
}Player;

typedef struct GameState {
  Player players[PLAYER_COUNT];
  bool is_game_over;
} GameState;

void player_init(Player *p, Color c, int offset) {
  p->c = c;
  p->selected_piece = -1;
  for (int i = 0; i < PLAYER_CHECKER_COUNT; i++) {
    p->cs[i] = (Checker){
      .pos = {
        .x=i%8,
        .y=i/8 + offset
      },
      .is_alive = true,
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
        c = WHITE;
      } else {
        c = BACKGROUND_COLOR;
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

void player_move(Player *p, int *current_player) {

}

Position get_current_xy_coords_hovering(Vector2 mouse_pos, int grid_count, int grid_size, Position board_start) {
  for (int x = 0; x < grid_count; x++) {
    for (int y = 0; y < grid_count; y++) {
      Rectangle board_rect = {
        x * grid_size + board_start.x,
        y * grid_size + board_start.y,
        grid_size,
        grid_size
      };
      if (CheckCollisionPointRec(mouse_pos, board_rect)) {
        return (Position) {x, y};
      }
    }
  }
  // mouse is not hovering above board
  return (Position) {-1, -1};
}


bool is_empty_space(GameState game, Position board_idx) {
  for (int i = 0; i < PLAYER_COUNT; i++) {
    Player p = game.players[i];
    for (int c = 0; c < PLAYER_CHECKER_COUNT; c++) {
      Position checker_pos = p.cs[c].pos;
      if (checker_pos.x == board_idx.x && checker_pos.y == board_idx.y) {
        return false;
      }
    }
  }
  return true;
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
  int current_player_turn = 0;
  while (!WindowShouldClose()) {
    BeginDrawing();
      ClearBackground((Color) {
        .r=200, .g=200, .b=200, .a=255
      });
      display_board(game, grid_count, grid_size, (Position) {board_start_x, board_start_y});
      // general approach to moving a piece
      // check if user is hovering over a piece
      // then if a user clicks on a piece
      // that piece will be selected to be moved
      Vector2 mouse_pos = GetMousePosition();
      for (int i = 0; i < PLAYER_CHECKER_COUNT; i++) {
        Checker checker = game.players[current_player_turn].cs[i];
        Rectangle checker_rect = {
          checker.pos.x * grid_size + board_start_x,
          checker.pos.y * grid_size + board_start_y,
          grid_size,
          grid_size};
        if (CheckCollisionPointRec(mouse_pos, checker_rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          game.players[current_player_turn].selected_piece = i;
          DrawRectangleLinesEx(checker_rect, 5.f, BLACK);
        } 
      }
      int selected_piece = game.players[current_player_turn].selected_piece;
      if (selected_piece != -1) {
        Checker checker = game.players[current_player_turn].cs[selected_piece];
        Rectangle checker_rect = {
          checker.pos.x * grid_size + board_start_x,
          checker.pos.y * grid_size + board_start_y,
          grid_size,
          grid_size};
          DrawRectangleLinesEx(checker_rect, 5.f, BLACK);
      }
      if (selected_piece != -1) {
        // get rect at current mouse position
        Position current_pos = get_current_xy_coords_hovering(mouse_pos, grid_count, grid_size, (Position) {board_start_x, board_start_y});
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && is_empty_space(game, current_pos)) {
          game.players[current_player_turn].cs[selected_piece].pos = current_pos;
          game.players[current_player_turn].selected_piece = -1;
          
          // change player_turn
        }
      }
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
