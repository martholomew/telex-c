#include <stdio.h>
#include <regex.h>
#include <string.h>

#define REGEX "^(ngh|d\\^|ch|gh|kh|nh|ng|ph|th|tr|qu|[bcdghklmnpqrstvx]?)([aeiouy\\^\\)`\\/>~\\.]*)(ch|nh|ng|[cmnpt]?)$"

void telex(char *word, char c) {
    char regex_string[100] = REGEX;
    regex_t regex_compiled;
    regmatch_t group_array[4];
    regcomp(&regex_compiled, regex_string, REG_EXTENDED|REG_ICASE);
    regexec(&regex_compiled, word, 4, group_array, 0);
    char beg_cons[3];
    char vowels[6];
    char end_cons[2];
    for(int i = 0; i < 4; i++) {
        if(group_array[i].rm_so == (size_t)-1)
            break;
        char word_copy[strlen(word) + 1];
        strcpy(word_copy, word);
        word_copy[group_array[i].rm_eo] = 0;
        strcpy(word_copy, word_copy + group_array[i].rm_so);
        printf("\n%d - %s\n", i, word_copy);
        if(i == 1) strcpy(beg_cons, word_copy);
        else if(i == 2) strcpy(vowels, word_copy);
        else if(i == 3) strcpy(end_cons, word_copy);
        memset(word_copy, 0, sizeof(word_copy));
    }
    printf("%s %s %s\n", beg_cons, vowels, end_cons);
}

int main() {
    int i;
    char test[][5] = {"ba", "viep", "a/", "a/", "ba", "poi", "viet"};
    char c[] = {'w', 'z', 'z', 'j', 'j', 's', 's'};
    char expect[][5] = {"ba)", "viep", "a", "a.", "ba.", "po/i", "vie/t"};
    for(i = 0; i < 7; i++) {
        printf("Input: %s\n", test[i]);
        printf("Input Char: %c\n", c[i]);
        telex(test[i], c[i]);
        printf("Output: %s\n", test[i]);
        printf("Expected: %s\n\n", expect[i]);
    }
}
