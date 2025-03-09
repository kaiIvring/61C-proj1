#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_t *game, unsigned int snum);
static char next_square(game_t *game, unsigned int snum);
static void update_tail(game_t *game, unsigned int snum);
static void update_head(game_t *game, unsigned int snum);

// get the cols of the board
unsigned int get_board_cols(game_t *game, unsigned int which_row)
{
  if (!game || !game->board || !game->board[which_row])
  {
    return -1;
  }

  unsigned int num_cols = 0;
  char* board_ptr = game->board[which_row];

  while(*board_ptr != '\n')
  {
    num_cols++;
    // load snakes
    board_ptr++;
  }

  return num_cols;
  
}
/* Task 1 */
game_t *create_default_game() {
  // alloc memory for game
  game_t* game = (game_t*)malloc(sizeof(game_t));
  if (!game)
  {
    return NULL;
  }

  game->num_rows = 18;
  game->num_snakes = 1;

  // initialize board
  game->board = (char**)malloc(sizeof(char*) * game->num_rows);
  if(!game->board)
  {
    free(game);
    return NULL;
  }

  for (int i = 0; i < game->num_rows; i++)
  {
    game->board[i] = (char*)malloc(sizeof(char) * 22); // 20columns + '\n' + '\0'
    if (!game->board[i])
    {
      for (int j = 0; j < i; j++)
      {
        free(game->board[j]);
      }
      free(game->board);
      free(game);
      return NULL;
    }
  }
  
  // fill in the board
  for (int i = 0; i < game->num_rows; i++)
  {
    for (int j = 0; j < 20; j++)
    {
      if (i == 0 || i == 17 || j ==0 || j == 19)
      {
        game->board[i][j] = '#';
      }
      else
      {
        game->board[i][j] = ' ';
      }
    }
    game->board[i][20] = '\n';
    game->board[i][21] = '\0';
  }

  // snake body
  game->board[2][2] = 'd';
  game->board[2][3] = '>';
  game->board[2][4] = 'D';

  // the fruit
  game->board[2][9] = '*';

  // alloc memory for snake
  game->snakes = (snake_t*)malloc(sizeof(snake_t));
  if (!game->snakes)
  {
    for (int i = 0; i < game->num_rows; i++)
    {
      free(game->board[i]);
    }
    free(game->board);
    free(game);
    return NULL;
  }

  // initialize snakes, 0 represents the "0st" snake.
  game->snakes[0].tail_col = 2;
  game->snakes[0].tail_row = 2;
  game->snakes[0].head_col = 4;
  game->snakes[0].head_row = 2;
  game->snakes[0].live = true;

  return game;
}

/* Task 2 */
void free_game(game_t *game) {

  for (int i = 0; i < game->num_rows; i++)
  {
    free(game->board[i]);
  }

  free(game->board);
  free(game->snakes);
  free(game);

  return;
}

/* Task 3 */
void print_board(game_t *game, FILE *fp) {
  if (!game || !fp || !game->board)
  {
    return;
  }

  for (int row = 0; row < game->num_rows; row++)
  {
    fprintf(fp, "%s", game->board[row]);
  }
  return;
}

/*
  Saves the current game into filename. Does not modify the game object.
  (already implemented for you).
*/
void save_board(game_t *game, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(game, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_t *game, unsigned int row, unsigned int col) { return game->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch) {
  game->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  return (c == 'a' || c == 'w' || c == 's' || c == 'd');
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  return (c == 'A' || c == 'W' || c == 'S' || c == 'D' || c == 'x');
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  return (c == 'A' || c == 'W' || c == 'S' || c == 'D' || c == 'x' 
                    || c == 'a' || c == 's' || c == 'd' || c == 'w'
                     || c == '<' || c == '>' || c == '^' || c == 'v');
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  switch (c)
  {
  case '<':
    return 'a';
  case '>':
    return 'd'; 
  case 'v':
    return 's';
  case '^':
    return 'w';
  default:
    break;
  }
  return '\0';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  switch (c)
  {
    case 'A':
      return '<';
    case 'W':
      return '^';
    case 'S':
      return 'v';
    case 'D':
      return '>';
    default:
      break;
  }
  return '\0';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
    switch (c) {
        case 'v': case 's': case 'S':
            return cur_row + 1;
        case '^': case 'w': case 'W':
            return cur_row - 1;
        default:
            return cur_row;
    }
}


/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
    switch (c) {
        case 'd': case 'D': case '>':
            return cur_col + 1;
        case 'a': case 'A': case '<':
            return cur_col - 1;
        default:
            return cur_col;
    }
}


/*
  Task 4.2

  Helper function for update_game. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_t *game, unsigned int snum) {
  // deal with invalid input.
  if (!game || snum >= game->num_snakes)
  {
    return '#';
  }
  

  snake_t *snakes = game->snakes;
  unsigned int s_row = snakes[snum].head_row;
  unsigned int s_col = snakes[snum].head_col;

  // get the char of the snake's head
  char head_char = get_board_at(game, s_row, s_col);
  // next row and col
  unsigned int n_row = get_next_row(s_row, head_char);
  unsigned int n_col = get_next_col(s_col,head_char);

  unsigned int num_cols = get_board_cols(game, n_row);

  // boder check!
  if (n_row >= game->num_rows || n_col >= num_cols)
  {
    return '#';
  }

  return game->board[n_row][n_col]; 
}

/*
  Task 4.3

  Helper function for update_game. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_t *game, unsigned int snum) {
  if (!game || snum >= game->num_snakes)
  {
    return;
  }


  snake_t* snakes = game->snakes;
  unsigned int s_row = snakes[snum].head_row;
  unsigned int s_col = snakes[snum].head_col;

  char head_char = get_board_at(game, s_row, s_col);
  if (!is_head(head_char))
  {
    return;
  }

  unsigned int n_row = get_next_row(s_row, head_char);
  unsigned int n_col = get_next_col(s_col, head_char);

  unsigned int num_cols = get_board_cols(game, n_row);

  // update board
  if (n_row >= game->num_rows || n_col >= num_cols)
  {
    return;
  }
  game->board[n_row][n_col] = head_char;
  char htb = head_to_body(head_char);
  game->board[s_row][s_col] = htb;

  // update snake_t
  snakes[snum].head_col = n_col;
  snakes[snum].head_row = n_row;

  return;
}

/*
  Task 4.4

  Helper function for update_game. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_t *game, unsigned int snum) {
  if (!game || snum >= game->num_snakes)
  {
    return;
  }
  snake_t* snakes = game->snakes;
  unsigned int s_row = snakes[snum].tail_row;
  unsigned int s_col = snakes[snum].tail_col;

  char tail_char = get_board_at(game, s_row, s_col);
  if (!is_tail(tail_char))
  {
    return;
  }

  unsigned int n_row = get_next_row(s_row, tail_char);
  unsigned int n_col = get_next_col(s_col, tail_char);
  char new_tail_char = body_to_tail(get_board_at(game, n_row, n_col));

  // update board
  game->board[n_row][n_col] = new_tail_char;
  game->board[s_row][s_col] = ' ';

  // update snake
  snakes[snum].tail_col = n_col;
  snakes[snum].tail_row = n_row;

  return;
}


/* Task 4.5 */
void update_game(game_t *game, int (*add_food)(game_t *game)) {
  if (!game)
  {
    return;
  }

  for (unsigned int i = 0; i < game->num_snakes; i++)
  {
    snake_t* snake = &(game->snakes[i]);

    // skip dead snakes
    if (!snake->live)
    {
      continue;
    }

    char next = next_square(game, i);

    if (next == '#' || is_snake(next))
    {
      game->board[snake->head_row][snake->head_col] = 'x';
      snake->live = false; // snake died
    }
    else if (next == '*')
    {
      update_head(game, i);
      add_food(game); // snake eats the fruit
    }
    else
    {
      update_head(game, i);
      update_tail(game, i); // everything is fine, update tail
    }
  }
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
    if (!fp) return NULL;

    int length = 0;
    int c;
    long start_pos = ftell(fp);  // 记录当前文件指针位置

    // 计算当前行的长度
    while ((c = getc(fp)) != '\n' && c != EOF) {
        length++;
    }

    if (length == 0 && c == EOF) { // 处理 EOF
        return NULL;
    }

    // 回退文件指针，使其回到行首
    fseek(fp, start_pos, SEEK_SET);

    // 分配内存
    char *buffer = (char *)malloc(length + 2);  // +1 预留给 '\n'，+1 预留给 '\0'
    if (!buffer) return NULL;

    // 读取完整行
    if (fread(buffer, 1, length, fp) != (size_t)length) { // 读取指定长度
        free(buffer);
        return NULL;
    }
    
    buffer[length] = '\0';  // 确保字符串以 NULL 结尾

    // 读取并检查换行符
    c = getc(fp);
    if (c == '\n') {
        buffer[length] = '\n';
        buffer[length + 1] = '\0';
    }

    return buffer;
}



/* Task 5.2 */
game_t *load_board(FILE *fp) {
  
  if (!fp)
  {
    return NULL;
  }

  char** board = NULL;
  unsigned int num_rows = 0;

  char* line;
  while ((line = read_line(fp)) != NULL)
  {
    // check if it is a blank line
    if(strlen(line) == 0 || strcmp(line, "\n") == 0)
    {
      free(line);
      continue;
    }
    char **new_board = realloc(board, sizeof(char*) * (num_rows + 1));
    if (!new_board)
    {
      free(line);
      for (int i = 0; i < num_rows; i++)
      {
        free(board[i]);
      }
      free(board);
      return NULL;
    }
    board = new_board;
    board[num_rows] = line;
    num_rows++;
  }

  if (num_rows == 0)
  {
    return NULL;
  }

  game_t* game = (game_t*) malloc(sizeof(game_t));
  if (!game)
  {
    for (int i = 0; i < num_rows; i++)
    {
      free(board[i]);
    }
    free(board);
    return NULL;
  }

  game->num_rows = num_rows;
  game->board = board;
  game->num_snakes = 0;
  game->snakes = NULL;

  return game;
  
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_t *game, unsigned int snum) {
  if (!game || !game->snakes)
  {
    printf("Error: game or game->snakes is NULL\n");
    return;
  }
  snake_t* snake = &(game->snakes[snum]);
  unsigned int tail_row = snake->tail_row;
  unsigned int tail_col = snake->tail_col;

  while(1)
  {
    if (tail_row >= game->num_rows || tail_col >= strlen(game->board[tail_row]))
    {
      return;
    }
    char curr_char = get_board_at(game, tail_row, tail_col);

    if (!is_snake(curr_char))
    {
      return;
    }

    if (is_head(curr_char))
    {
      break;
    }

    tail_col = get_next_col(tail_col, curr_char);
    tail_row = get_next_row(tail_row, curr_char);
  }

  snake->head_col = tail_col;
  snake->head_row = tail_row;
}

/* Task 6.2 */
game_t *initialize_snakes(game_t *game) {

  // get the number of snakes
  for (unsigned int row = 0; row < game->num_rows; row++)
  {
    unsigned int num_cols = get_board_cols(game, row);
    for (unsigned int col = 0; col < num_cols; col++)
    {
      if (is_tail(get_board_at(game, row, col)))
      {
        game->num_snakes++;
      }
    }
  }


  // alloc memory for snakes and fill it in
  game->snakes = (snake_t*)malloc(sizeof(snake_t) * game->num_snakes);
  if (!game->snakes)
  {
    return NULL;
  }
  unsigned int snake_index = 0;
  for (unsigned int row = 0; row < game->num_rows; row++)
  {
    unsigned int num_cols = get_board_cols(game, row);
    for (unsigned int col = 0; col < num_cols; col++)
    {
      if (is_tail(get_board_at(game, row, col)))
      {
        game->snakes[snake_index].tail_col = col;
        game->snakes[snake_index].tail_row = row;
        game->snakes[snake_index].live = true;

        find_head(game, snake_index);
        snake_index++;
      }
    }
  }

  return game;
}
