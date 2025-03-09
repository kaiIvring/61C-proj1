#include <stdio.h>
#include <string.h>

#include "snake_utils.h"
#include "game.h"

void print_board_stdout(game_t *game) {
    if (!game || !game->board) {
        fprintf(stderr, "Error: game or board is NULL\n");
        return;
    }

    for (int i = 0; i < game->num_rows; i++) {
        printf("%s\n", game->board[i]);  // 直接输出每一行
    }
}

int main(int argc, char *argv[]) {
  bool io_stdin = false;
  char *in_filename = NULL;
  char *out_filename = NULL;
  game_t *game = NULL;

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0 && i < argc - 1) {
      if (io_stdin) {
        fprintf(stderr, "Usage: %s [-i filename | --stdin] [-o filename]\n", argv[0]);
        return 1;
      }
      in_filename = argv[i + 1];
      i++;
      continue;
    } else if (strcmp(argv[i], "--stdin") == 0) {
      if (in_filename != NULL) {
        fprintf(stderr, "Usage: %s [-i filename | --stdin] [-o filename]\n", argv[0]);
        return 1;
      }
      io_stdin = true;
      continue;
    }
    if (strcmp(argv[i], "-o") == 0 && i < argc - 1) {
      out_filename = argv[i + 1];
      i++;
      continue;
    }
    fprintf(stderr, "Usage: %s [-i filename | --stdin] [-o filename]\n", argv[0]);
    return 1;
  }

  // Do not modify anything above this line.

  /* Task 7 */

  // Read board from file, or create default board
  if (in_filename != NULL) {
    FILE *file = fopen(in_filename, "r");
    if (!file)
    {
      perror("Error opening input file");
      return -1;
    }
    game = load_board(file);
    if (!game)
    {
      printf("Error: failed to initialize game\n");
      return -1;
    }
    game = initialize_snakes(game);
    if (!game)
    {
      printf("Error: failed to initialize snakes\n");
      return -1;
    }
    fclose(file);
  } else if (io_stdin) {
    // load game
    game = load_board(stdin);
    if (!game)
    {
      printf("Error: failed to initialize game\n");
      return -1;
    }
    // load snakes
    game = initialize_snakes(game);
    if (!game)
    {
      printf("Error: failed to initialize snakes\n");
      return -1;
    }
  } else {
    game = create_default_game();
    if (!game)
    {
      printf("Error: failed to initialize game\n");
      return -1;
    }
  }


  // update game
  update_game(game, &deterministic_food);
  
  // Write updated board to file or stdout
  if (out_filename != NULL) {
    save_board(game, out_filename);
  } else {
    save_board(game, NULL);
  }

  free_game(game);

  return 0;
}
