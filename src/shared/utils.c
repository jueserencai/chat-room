
#include "utils.h"

#include <stdio.h>
#include <string.h>

void parse_header_line(char* line, char* type, char* content) {
    int equal_pos = 0;
    while (line[equal_pos] != '=') {
        equal_pos++;
    }
    strncpy(type, line, equal_pos);
    strcpy(content, line + equal_pos + 1);
}
