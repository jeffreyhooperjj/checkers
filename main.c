#include <stdio.h>
#include <raylib.h>

int main() {
  printf("Hello, world\n");
  InitWindow(800, 600, "First window");
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
      DrawRectangle(board_start_x, board_start_y, board_size, board_size, RED);
      // draw grid
      for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
          Color c;
          if ((x + y) % 2 == 0) {
            c = BLUE;
          } else {
            c = GREEN;
          }
          DrawRectangle((x*grid_size) + board_start_x, (y*grid_size) + board_start_y, grid_size, grid_size, c);
        }
      }
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
