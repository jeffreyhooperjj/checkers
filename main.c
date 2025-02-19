#include <stdio.h>
#include <stdbool.h>
#include <raylib.h>

#define PLAYER_CHECKER_COUNT 12
#define PLAYER_COUNT 2
#define BACKGROUND_COLOR (Color) {175, 128, 79, 255}
#define START_PLAYER_IDX 1
#define PLAYER_ONE 0
#define PLAYER_TWO 1
#define BOARD_SIZE 8
#define MAX_JUMP_COUNT 10

typedef struct Position {
  int x;
  int y;
}Position;


typedef struct PositionPair {
  Position enemy;
  Position land_on;
}PositionPair;

typedef enum PieceType {
  PAWN,
  KING,
} PieceType;

typedef enum Direction {
  UP,
  DOWN
} Direction;

typedef struct Checker {
  Position pos;
  // TODO: could remove this and just set pos to -1 -1 or something
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
  Player *current_player;
} GameState;

void player_init(Player *p, Color c, int start_row) {
  p->c = c;
  p->selected_piece = -1;
  int placed_piece_count = 0;
  for (int r = start_row; r < start_row + 3; r++) {
    for (int c = 0; c < 8; c++) {
      if ((r + c) % 2 == 1) {
        p->cs[placed_piece_count] = (Checker) {
          .pos = {c, r},
          .is_alive = true,
        };
        placed_piece_count++;
      }
    }
  }
}

void game_init(GameState *game) {
  for (int i = 0; i < PLAYER_COUNT; i++) {
    Color c;
    int start_row;
    if (i == 0) {
      c = RED;
      start_row = 0;
    } else {
      c = BLACK;
      start_row = 5;
    }
    player_init(&game->players[i], c, start_row);
  }
  // start with black colored checkers
  game->current_player = &game->players[START_PLAYER_IDX];
}

void draw_checkers(Player p, int grid_size, Position board_start, int checker_radius) {
  for (int i = 0; i < PLAYER_CHECKER_COUNT; i++) {
    Checker c = p.cs[i];
    if (c.is_alive) {
      int x_offset = p.cs[i].pos.x * grid_size + board_start.x;
      int y_offset = p.cs[i].pos.y * grid_size + board_start.y;
      DrawCircle(x_offset + grid_size/2, y_offset + grid_size/2, checker_radius, p.c);
    }
  }
}

void display_board(GameState game, int grid_count, int grid_size, Position board_start) {
  // draw grid
  for (int x = 0; x < grid_count; x++) {
    for (int y = 0; y < grid_count; y++) {
      Color c;
      if ((x + y) % 2 == 0) {
        //c = (Color){0x18, 0x18, 0x18, 0xFF};
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
  // perform move
  
  // change current_player_turn
  // this is only needed for local play
  // otherwise only server should determine current player
  *current_player = *current_player + 1;
  *current_player %= 1;
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

bool contains_enemy(GameState *game, int enemy_idx, Position pos) {
  Player enemy = game->players[enemy_idx];
  for (int c = 0; c < PLAYER_CHECKER_COUNT; c++) {
    Position enemy_pos = enemy.cs[c].pos;
    if (enemy_pos.x == pos.x && enemy_pos.y == pos.y) {
      printf("Contains enemy piece!\n");
      return true;
    }
  }
  return false;
}

void simulate_jump(GameState *game, int enemy_idx,
                   Position start, Position end, 
                   Direction dir, 
                   bool visited[BOARD_SIZE][BOARD_SIZE],
                   PositionPair path[MAX_JUMP_COUNT],
                   int *jump_count,
                   int *successful_jump_count) {
  // if we are not on a dark square we can automatically ignore
  if ((end.x + end.y) % 2 != 1) {
    return;
  }
  if (*jump_count >= MAX_JUMP_COUNT) {
    return;
  }
  if (visited[start.x][start.y]) {
    return;
  }
  if (start.x == end.x && start.y == end.y) {
    printf("Made it to the end!\n");
    printf("jump count: %d\n", *jump_count);
    *successful_jump_count = *jump_count;
    return;
  }
  visited[start.x][start.y] = true;
  // now we need to move from the start position to the end position
  Position current = start;
  int dy = (dir == UP) ? 1 : -1;
  for (int dx = -1; dx <= 1; dx+=2) {
    Position next = {current.x + dx, current.y + dy};
    Position jump = {next.x + dx, next.y + dy};
    if (contains_enemy(game, enemy_idx, next) &&
        is_empty_space(*game, jump)) {
      printf("Simulating jump!");
      path[*jump_count] = (PositionPair){next, jump};
      (*jump_count)++;
      simulate_jump(game, enemy_idx, jump, end, dir, visited, path, jump_count, successful_jump_count);
      (*jump_count)--;
    }
  }
  visited[start.x][start.y] = false;
}

int get_player_checker_idx_from_position(GameState *game, Position checker_pos, int player_idx) {
  for (int i = 0; i < PLAYER_CHECKER_COUNT; i++) {
    Checker c = game->players[player_idx].cs[i];
    if (c.pos.x == checker_pos.x && c.pos.y == checker_pos.y) {
      return i;
    }
  }
  return -1;
}

bool is_valid_move(GameState *game, Position selected_board_pos) {
  // TODO: determine what a valid move is in checkers
  // need to check for jump first
  // if (can_jump())
  // simple move
  Player *current = game->current_player;
  Position selected_piece_pos = current->cs[current->selected_piece].pos;
  // TODO: turn into macros
  int curr_player_idx = (current == &game->players[PLAYER_ONE]) ? PLAYER_ONE : PLAYER_TWO;
  int enemy_player_idx = (curr_player_idx == PLAYER_ONE) ? PLAYER_TWO : PLAYER_ONE;
  Direction piece_dir = (curr_player_idx == PLAYER_ONE) ? UP : DOWN;
  bool can_move_to_position = false;
  int grid_x_diff = selected_piece_pos.x - selected_board_pos.x;
  int grid_y_diff = selected_piece_pos.y - selected_board_pos.y;
    // check for simple move
  if ((grid_x_diff == -1 || grid_x_diff == 1) && (grid_y_diff == -1 || grid_y_diff == 1)) {
    if (piece_dir == DOWN && selected_piece_pos.y > selected_board_pos.y) {
      can_move_to_position = true;
    } else if (piece_dir == UP && selected_piece_pos.y < selected_board_pos.y) {
      can_move_to_position = true;
    }
  } else {
    // simulate jump
    // construct a sequence of jumps to get to selected position
    bool visited[BOARD_SIZE][BOARD_SIZE] = {0};
    PositionPair jump_path[MAX_JUMP_COUNT];
    int jump_count = 0;
    int successful_jump_count = 0;
    simulate_jump(game, enemy_player_idx, 
                  selected_piece_pos, selected_board_pos, 
                  piece_dir, visited, jump_path, &jump_count, &successful_jump_count);
    printf("jump count outside func: %d\n", successful_jump_count);
    if (successful_jump_count) {
      can_move_to_position = true;
    }
    for (int c = 0; c < successful_jump_count; c++) {
      int checker_idx = get_player_checker_idx_from_position(game, jump_path[c].enemy, enemy_player_idx);
      if (checker_idx != -1) {
        game->players[enemy_player_idx].cs[checker_idx].is_alive = false;
        game->players[enemy_player_idx].cs[checker_idx].pos = (Position){-1, -1};
      }
      printf("Checker idx %d\n", checker_idx);
      printf("Enemy point: %d, %d\n", jump_path[c].enemy.x, jump_path[c].enemy.y);
      printf("Land on spot: %d, %d\n", jump_path[c].land_on.x, jump_path[c].land_on.y);
    }
  }
  return is_empty_space(*game, selected_board_pos) && can_move_to_position;
}

void player_select_piece(Player *curr_player, Vector2 mouse_pos, int grid_size, Position board_start) {
  for (int i = 0; i < PLAYER_CHECKER_COUNT; i++) {
    Checker checker = curr_player->cs[i];
    Rectangle checker_rect = {
      checker.pos.x * grid_size + board_start.x,
      checker.pos.y * grid_size + board_start.y,
      grid_size,
      grid_size};
    // draw rectangle around selected piece
    if (CheckCollisionPointRec(mouse_pos, checker_rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      curr_player->selected_piece = i;
    } 
  }
}

void draw_selected_checker_board(Player *curr_player, int grid_size, Position board_start) {
  int selected_piece = curr_player->selected_piece;
  if (selected_piece != -1) {
    Checker checker = curr_player->cs[selected_piece];
    Rectangle checker_rect = {
      checker.pos.x * grid_size + board_start.x,
      checker.pos.y * grid_size + board_start.y,
      grid_size,
      grid_size};
      DrawRectangleLinesEx(checker_rect, 5.f, BLACK);
  }
}

void player_attempt_move(GameState *game, Vector2 mouse_pos, int grid_size, int grid_count, Position board_start) {
  Player *curr_player = game->current_player;
  int selected_piece = curr_player->selected_piece;
  if (selected_piece == -1) {
    return;
  }
  // get rect at current mouse position
  Position current_pos = get_current_xy_coords_hovering(
    mouse_pos, grid_count, grid_size, board_start 
  );
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && is_valid_move(game, current_pos)) {
    // make move
    curr_player->cs[selected_piece].pos = current_pos;
    curr_player->selected_piece = -1;
    
    // change player_turn
    int curr_player_idx = (curr_player == &game->players[PLAYER_ONE]) ? PLAYER_ONE : PLAYER_TWO;
    curr_player_idx += 1;
    curr_player_idx %= PLAYER_COUNT;
    game->current_player = &game->players[curr_player_idx];
  }
}

int main() {
  GameState game = {0};
  // TODO: learn how to use camera/rotate rectangles
  // look into rlTranslatef
  Camera2D camera = {0};
  printf("%f, %f", camera.rotation, camera.offset.y);
  camera.rotation = 180.f;
  camera.offset.x = 400;
  camera.offset.y = 300;

  game_init(&game);
  InitWindow(800, 600, "Checkers");
  int board_size = 500;
  int grid_count = 8;
  // checkers board is an 8x8 grid
  // we want alternating pieces of dark and light colors
  int grid_size = board_size/grid_count;
  int board_start_x = 150;
  int board_start_y = 50;
  Position board_start = (Position) {board_start_x, board_start_y};
  //int current_player_turn = 0;
  while (!WindowShouldClose()) {
    BeginDrawing();
      ClearBackground((Color) {
        .r=200, .g=200, .b=200, .a=255
      });
      display_board(game, grid_count, grid_size, board_start);
      // general approach to moving a piece
      // check if user is hovering over a piece
      // then if a user clicks on a piece
      // that piece will be selected to be moved
      Vector2 mouse_pos = GetMousePosition();
      Player *curr_player = game.current_player;
      player_select_piece(curr_player, mouse_pos, grid_size, board_start);
      draw_selected_checker_board(curr_player, grid_size, board_start);
      player_attempt_move(&game, mouse_pos, grid_size, grid_count, board_start);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
