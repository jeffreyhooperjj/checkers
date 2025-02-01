#include <stdio.h>
#include <raylib.h>

int main() {
  printf("Hello, world\n");
  InitWindow(800, 600, "First window");
  while (!WindowShouldClose()) {
    BeginDrawing();
      ClearBackground((Color) {
        .r=200, .g=200, .b=200, .a=255
      });
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
