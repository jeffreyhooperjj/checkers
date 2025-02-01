gcc -Wall -Werror -std=c99 \
  -I/Users/macbookx/Coding/checkers/third-party/raylib/build/raylib/include \
  -o main main.c \
  -L/Users/macbookx/Coding/checkers/third-party/raylib/build/raylib \
  -lraylib \
  -framework CoreVideo \
  -framework IOKit \
  -framework Cocoa &&
  ./main
