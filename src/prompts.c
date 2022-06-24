#include "prompts.h"

int prompt_which_fd(t_list *list) {
  char buf[256];
  int selection = 0;

  while (selection <= 0 || selection > list->count) {
    puts("\n\t\033[34;1m--| Which FD do you want to edit? |--\033[0m\n");
    dump_list(list);
    printf("\n\033[32;1m Your Selection >\033[0m ");
    scanf("%255s", &buf);
    selection = atoi(buf);
  }
  puts("");
  return selection-1;
}

int prompt_confirm_overwrite() {
  char ans;

  while (ans != 'n' && ans != 'N' && ans != 'y' && ans != 'Y') {
    printf(" [\033[33;1m?\033[0m] Confirm Overwrite? [y/n] > ");
    ans = getchar();
    if (ans == '\n') ans = getchar();
  }
  if (ans == 'y' || ans == 'Y')
    return 1;
  return 0;
}