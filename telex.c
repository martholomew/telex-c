#include <stdio.h>
#include <regex.h>
#include <string.h>

#define REGEX "^(ngh|d\\^|ch|gh|kh|nh|ng|ph|th|tr|qu|[bcdghklmnpqrstvx]?)([aeiouy\\^)`\\/>~.]*)(ch|nh|ng|[cmnpt]?)$"

void cpy_range(char *str1, char *str2, int x1, int x2) {
    int i, j = 0;
    for(i = x1; i < x2; i++, ++j) {
        str1[j] = str2[i];
    }
    str1[j] = 0;
}

int is_ntd(char c) {
    int ntd = 0;
    switch(c) {
        case 'a':
        case 'w':
        case 'e':
        case 'o':
        case 'u':
            ntd = 1;
            break;
    }
    return ntd;
}

int telex(char *word, char c) {
    char regex_word[150] = REGEX;
    regex_t regex_compiled;
    regmatch_t group_array[4];
    regcomp(&regex_compiled, regex_word, REG_EXTENDED|REG_ICASE);
    if(regexec(&regex_compiled, word, 4, group_array, 0)) return 1;
    regfree(&regex_compiled);
    char beg_cons[3];
    char vowels[6];
    char end_cons[2];
    for(int i = 0; i < 4; i++) {
        if(group_array[i].rm_so == (size_t) - 1) break;
        char word_copy[strlen(word) + 1];
        cpy_range(word_copy, word, group_array[i].rm_so, group_array[i].rm_eo);
        if(i == 1) strcpy(beg_cons, word_copy);
        else if(i == 2) strcpy(vowels, word_copy);
        else if(i == 3) strcpy(end_cons, word_copy);
        //printf("\n%d - %s (%d - %d)\n", i, word_copy, group_array[i].rm_so, group_array[i].rm_eo);
        memset(word_copy, 0, sizeof(word_copy));
    }
    char word_test[11] = "";
    strcat(word_test, beg_cons);
    strcat(word_test, vowels);
    strcat(word_test, end_cons);
    if(strcmp(word_test, word)) return 1;
    int b = beg_cons[0] != 0;
    int v = vowels[0] != 0;
    int e = end_cons[0] != 0;
    char l_vowel = vowels[strlen(vowels) - 1];
    char sl_vowel = vowels[strlen(vowels) - 2];
    if(b && !v && !e && strlen(beg_cons) == 1 && beg_cons[0] == c && c == 'd') {
        strcat(word, "^");
        return 0;
    } else if(!e && is_ntd(c) && is_ntd(l_vowel)) {
        if(c == l_vowel && (c == 'a' || c == 'e' || c == 'o')) {
            strcat(word, "^");
            return 0;
        } else if(c == 'w' && (l_vowel == 'a' || l_vowel == 'o' || l_vowel == 'u')) {
            strcat(word, ")");
            return 0;
        }
    } else if(!e && is_ntd(c) && (l_vowel == '^' || l_vowel == ')')) {
        if(l_vowel == '^' && c == sl_vowel && (c == 'a' || c == 'e' || c == 'o')) {
            word[strlen(word) - 1] = c;
            return 0;
        } else if(l_vowel == ')' && c == 'w' && (sl_vowel == 'a' || sl_vowel == 'o' || sl_vowel == 'u')) {
            word[strlen(word) - 1] = c;
            return 0;
        }
    } else if(v && e) {
        char regex_ntd[20] = "[aeou][\\^)`\\/>~.]+";
        regmatch_t matches[2];
        regcomp(&regex_compiled, regex_ntd, REG_EXTENDED|REG_ICASE);
        char slider[strlen(vowels) + 1];
        strcpy(slider, vowels);
        for(int i = 0; i < 2; i++) {
            printf("\nslider - %s\n", slider);
            if(regexec(&regex_compiled, slider, 1, matches, 0)) break;
            if(matches[i].rm_so == (size_t) - 1) break;
            char vowels_copy[strlen(slider) + 1];
            cpy_range(vowels_copy, slider, matches[i].rm_so, matches[i].rm_eo);
            printf("%d - %s\n", i, vowels_copy);
            if(matches[i].rm_eo == strlen(slider)) break;
            // needs fixes here
            memset(vowels_copy, 0, sizeof(vowels_copy));
        }
        regfree(&regex_compiled);
    }
    return 0;
}

int main() {
    int i;
    char test[][15] = {"huye^`n", "d", "xo", "xo^", "mo", "mo)", "mu)o)t"};
    char c[] = {'0', 'd', 'o', 'o', 'w', 'w', 's'};
    char expect[][15] = {"huye^`n", "d^", "xo^", "xoo", "mo)", "mow", "mu)o)/t"};
    for(i = 0; i < 7; i++) {
        printf("Input: %s\n", test[i]);
        printf("Input Char: %c\n", c[i]);
        if(telex(test[i], c[i])) printf("Not valid Vietnamese.\n");
        printf("Output: %s\n", test[i]);
        printf("Expected: %s\n\n", expect[i]);
    }
}
