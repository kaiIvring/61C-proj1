#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asserts.h"
// Necessary due to static functions in game.c
#include "game.c"

/* Look at asserts.c for some helpful assert functions */

int greater_than_forty_two(int x) { return x > 42; }

bool is_vowel(char c) {
  char *vowels = "aeiouAEIOU";
  for (int i = 0; i < strlen(vowels); i++) {
    if (c == vowels[i]) {
      return true;
    }
  }
  return false;
}

/*
  Example 1: Returns true if all test cases pass. False otherwise.
    The function greater_than_forty_two(int x) will return true if x > 42. False otherwise.
    Note: This test is NOT comprehensive
*/
bool test_greater_than_forty_two() {
  int testcase_1 = 42;
  bool output_1 = greater_than_forty_two(testcase_1);
  if (!assert_false("output_1", output_1)) {
    return false;
  }

  int testcase_2 = -42;
  bool output_2 = greater_than_forty_two(testcase_2);
  if (!assert_false("output_2", output_2)) {
    return false;
  }

  int testcase_3 = 4242;
  bool output_3 = greater_than_forty_two(testcase_3);
  if (!assert_true("output_3", output_3)) {
    return false;
  }

  return true;
}

/*
  Example 2: Returns true if all test cases pass. False otherwise.
    The function is_vowel(char c) will return true if c is a vowel (i.e. c is a,e,i,o,u)
    and returns false otherwise
    Note: This test is NOT comprehensive
*/
bool test_is_vowel() {
  char testcase_1 = 'a';
  bool output_1 = is_vowel(testcase_1);
  if (!assert_true("output_1", output_1)) {
    return false;
  }

  char testcase_2 = 'e';
  bool output_2 = is_vowel(testcase_2);
  if (!assert_true("output_2", output_2)) {
    return false;
  }

  char testcase_3 = 'i';
  bool output_3 = is_vowel(testcase_3);
  if (!assert_true("output_3", output_3)) {
    return false;
  }

  char testcase_4 = 'o';
  bool output_4 = is_vowel(testcase_4);
  if (!assert_true("output_4", output_4)) {
    return false;
  }

  char testcase_5 = 'u';
  bool output_5 = is_vowel(testcase_5);
  if (!assert_true("output_5", output_5)) {
    return false;
  }

  char testcase_6 = 'k';
  bool output_6 = is_vowel(testcase_6);
  if (!assert_false("output_6", output_6)) {
    return false;
  }

  return true;
}

/* Task 4.1 */

bool test_is_tail() {
  char valid_tails[] = {'a', 'w', 's', 'd'};
  for (int i = 0; i < 4; i++) {
      if (!assert_true(&valid_tails[i], is_tail(valid_tails[i]))) {
          return false;
      }
  }

  char invalid_chars[] = {'r', 'A', 'W', 'S', 'D', ' ', '*', '#', '\n'};
  for (int i = 0; i < sizeof(invalid_chars) / sizeof(char); i++) {
      if (!assert_false(&invalid_chars[i], is_tail(invalid_chars[i]))) {
          return false;
      }
  }

  return true;
}


bool test_is_head() {
  char valid_heads[] = {'W', 'A', 'S', 'D','x'};
  for (int i = 0; i < 5; i++)
  {
    if (!assert_true(&valid_heads[i], is_head(valid_heads[i])))
    {
      return false;
    }
  }

  char invalid_chars[] = {'w', 'a', 's', 'd', ' ', '*', '\n'};
  for (int i = 0; i < sizeof(invalid_chars) / sizeof(char); i++)
  {
    if (!assert_false(&invalid_chars[i], is_head(invalid_chars[i])))
    {
      return false;
    }
  }
  return true;
}

bool test_is_snake() {
  char valid_body[] = {'a', 'w', 's', 'd', 'A', 'W', 'S', 'D', 'x', '<', '>', '^', 'v'};
  for (int i = 0; i < 13; i++)
  {
    if (!assert_true(&valid_body[i], is_snake(valid_body[i])))
    {
      return false;
    }
  }

  char invalid_chars[] = {'q', 'i', ' ', '\n', '\0', 'm'};
  for (int i = 0; i < sizeof(invalid_chars) / sizeof(char); i++)
  {
    if (!assert_false(&invalid_chars[i], is_snake(invalid_chars[i])))
    {
      return false;
    }
  }
  return true;
}

bool test_body_to_tail() {
  char test_case[] = {'<', '>', 'v', '^'};
  char expected_results[] = {'a', 'd', 's', 'w'};

  for (int i = 0; i < 4; i++)
  {
    char result = body_to_tail(test_case[i]);
    if (!assert_equals_char(&test_case[i], expected_results[i], result))
    {
      return false;
    }
  }

  return true;
}

bool test_head_to_body() {
  char test_case[] = {'A', 'S', 'D', 'W'};
  char expected_results[] = {'<', 'v', '>', '^'};

  for (int i = 0; i < 4; i++)
  {
    char result = head_to_body(test_case[i]);
    if (!assert_equals_char(&test_case[i], expected_results[i], result))
    {
      return false;
    }
  }
  return true;
}

bool test_get_next_row() {
  unsigned int currRow = 5;
  char add_case[] = {'s', 'S', 'v'};
  for (int i = 0; i < 3; i++) {
      unsigned int result = get_next_row(currRow, add_case[i]);
      if (!assert_equals_int("Row should increase", currRow + 1, result)) {
          return false;
      }
  }

  char minus_case[] = {'w', 'W', '^'};
  for (int i = 0; i < 3; i++) {
      unsigned int result = get_next_row(currRow, minus_case[i]);
      if (!assert_equals_int("Row should decrease", currRow - 1, result)) {
          return false;
      }
  }

  char other_case[] = {'a', 'A', 'd', 'D', '>', '<'};
  for (int i = 0; i < 6; i++) {
      unsigned int result = get_next_row(currRow, other_case[i]);
      if (!assert_equals_int("Row should remain unchanged", currRow, result)) {
          return false;
      }
  }

  return true;
}

bool test_get_next_col() {
  unsigned int currCol = 5;

  char add_case[] = {'d', 'D', '>'};
  for (int i = 0; i < 3; i++) 
  {
      unsigned int result = get_next_col(currCol, add_case[i]);
      if (!assert_equals_int("col should increase", currCol + 1, result)) 
      {
          return false;
      }
  }

  char minus_case[] = {'a', 'A', '<'};
  for (int i = 0; i < 3; i++) 
  {
      unsigned int result = get_next_col(currCol, minus_case[i]);
      if (!assert_equals_int("col should decrease", currCol - 1, result)) 
      {
          return false;
      }
  }

  char other_case[] = {'w', 'W', 's', 'S', '^', 'v'};
  for (int i = 0; i < 6; i++) 
  {
      unsigned int result = get_next_col(currCol, other_case[i]);
      if (!assert_equals_int("col should remain", currCol, result)) 
      {
          return false;
      }
  }

  return true;
}

bool test_customs() {
  if (!test_greater_than_forty_two()) {
    printf("%s\n", "test_greater_than_forty_two failed.");
    return false;
  }
  if (!test_is_vowel()) {
    printf("%s\n", "test_is_vowel failed.");
    return false;
  }
  if (!test_is_tail()) {
    printf("%s\n", "test_is_tail failed");
    return false;
  }
  if (!test_is_head()) {
    printf("%s\n", "test_is_head failed");
    return false;
  }
  if (!test_is_snake()) {
    printf("%s\n", "test_is_snake failed");
    return false;
  }
  if (!test_body_to_tail()) {
    printf("%s\n", "test_body_to_tail failed");
    return false;
  }
  if (!test_head_to_body()) {
    printf("%s\n", "test_head_to_body failed");
    return false;
  }
  if (!test_get_next_row()) {
    printf("%s\n", "test_get_next_row failed");
    return false;
  }
  if (!test_get_next_col()) {
    printf("%s\n", "test_get_next_col failed");
    return false;
  }
  return true;
}

int main(int argc, char *argv[]) {
  init_colors();

  if (!test_and_print("custom", test_customs)) {
    return 0;
  }

  return 0;
}
