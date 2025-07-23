#include <stdio.h>
#include <string.h>
#include "ustr.h"

int main() {
    char input[256];
    while(fgets(input, sizeof(input), stdin)) {
        // Remove newline character if present
        size_t l = strlen(input);
        if (l > 0 && input[l - 1] == '\n') {
            input[l - 1] = '\0';
        }

        UStr s = new_ustr(input);
        printf("len(%s) = %d\n", s.contents, len(s));
        free_ustr(s);
    }
}
