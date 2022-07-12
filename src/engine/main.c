#include <cm.h>
#include <engine.h>
#include <stdio.h>

int main() {
  node* tokens = tokenize("test a+=b 1==2 final");

  for (int i = 0; i < *tokens->size; i++) {
    puts(linked_list_get(tokens, i));
  }
  return 0;
}
