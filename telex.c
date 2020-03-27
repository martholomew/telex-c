#include <stdio.h>
#include <regex.h>
#include <string.h>

#define REGEX "^(ngh|d\\^|ch|gh|kh|nh|ng|ph|th|tr|qu|[bcdghklmnpqrstvx]?)([aeiouy\\^)`\\/>~.]*)(ch|nh|ng|[cmnpt]?)$"

void ins_char(char *str1, char c, int pos) {
    int len = strlen(str1);
    int i;
    char buff[20];
    for(i = 0; i < len + 1; ++i) {
        if(i == pos) buff[i] = c;
        else if(i > pos) buff[i] = str1[i - 1];
        else buff[i] = str1[i];
    }
    buff[i] = 0;
    strcpy(str1, buff);
}

void cat_char(char *str1, char c) {
    int len;
    for(len = 0; str1[len] != 0; len++);
    str1[len] = c;
    str1[len + 1] = 0;
}

void cpy_range(char *str1, char *str2, int x1, int x2) {
    int i, j = 0;
    for(i = x1; i < x2; i++, ++j) {
        str1[j] = str2[i];
    }
    str1[j] = 0;
}

int is_vowel(char c) {
    int vowel = 0;
    switch(c) {
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':
        case 'y':
            vowel = 1;
            break;
    }
    return vowel;
}

char get_tone(char c) {
    char tone;
    switch(c) {
        case 'z':
            tone = 0;
            break;
        case 'f':
            tone = '`';
            break;
        case 's':
            tone = '/';
            break;
        case 'r':
            tone = '>';
            break;
        case 'x':
            tone = '~';
            break;
        case 'j':
            tone = '.';
            break;
    }
    return tone;
}

int is_tone(char c) {
    int tone = 0;
    switch(c) {
        case '`':
        case '/':
        case '>':
        case '~':
        case '.':
        case 'z':
        case 'f':
        case 's':
        case 'r':
        case 'x':
        case 'j':
            tone = 1;
            break;
    }
    return tone;
}

int is_end_cons(char c) {
    int end_cons = 0;
    switch(c) {
        case 'c':
        case 'm':
        case 'n':
        case 'p':
        case 't':
            end_cons = 1;
            break;
    }
    return end_cons;
}

int telex(char *word, char c) {
    char regex_word[150] = REGEX;
    regex_t regex_compiled;
    regmatch_t group_array[4];
    regcomp(&regex_compiled, regex_word, REG_EXTENDED|REG_ICASE);
    if(regexec(&regex_compiled, word, 4, group_array, 0)) return 1;
    regfree(&regex_compiled);
    char beg_cons[4] = {};
    char vowels[10] = {};
    char end_cons[3] = {};
    int i;
    for(i = 0; i < 4; i++) {
        // Divide the word into sections.
        char temp[strlen(word) + 1];
        cpy_range(temp, word, group_array[i].rm_so, group_array[i].rm_eo);
        if(i == 1) strcpy(beg_cons, temp);
        else if(i == 2) strcpy(vowels, temp);
        else if(i == 3) strcpy(end_cons, temp);
        memset(temp, 0, sizeof(temp));
    }
    char word_test[strlen(word) + 1];
    strcat(word_test, beg_cons);
    strcat(word_test, vowels);
    strcat(word_test, end_cons);
    // Make sure the sections added together are the same as the original word.
    if(strcmp(word_test, word)) return 1;
    int b = strlen(beg_cons);
    int v = strlen(vowels);
    int e = strlen(end_cons);
    int vowel_count = 0, spec_pos = 0, tone_pos = 0;
    for(i = 0; i < v; i++) {
        if(is_vowel(vowels[i])) vowel_count++;
        else if(vowels[i] == ')' || vowels[i] == '^') spec_pos = i;
        else if(is_tone(vowels[i])) tone_pos = i;
    }
    char tone = get_tone(c);
    if(b && !v && !e && b == 1 && beg_cons[0] == c && c == 'd') {
        // Bar'd D only occurs at the beginning of a word.
        cat_char(beg_cons, '^');
    } else if(v && c == vowels[v - 1] && (c == 'a' || c == 'e' || c == 'o')) {
        // Two of either 'a', 'e', or 'o' puts a hat on em.
        cat_char(vowels, '^');
    } else if(v && c == 'w' && (vowels[v - 1] == 'a' || vowels[v - 1] == 'o' || vowels[v - 1] == 'u')) {
        // Any of these vowels with a 'w' after puts a tail thing on em.
        cat_char(vowels, ')');
    } else if((!e && (c == vowels[v - 2] && vowels[v - 1] == '^') || (c == 'w' && vowels[v - 1] == ')')) || (!v && !e && !strcmp(beg_cons, "d^"))) {
        // If you hit 'w', 'd' or 'a', 'e' or 'o' thrice, no hat, just the letter
        if(v) vowels[v - 1] = c;
        else if(b) beg_cons[b - 1] = c;
        else return 1;
    } else if(v && e && is_tone(c)) {
        if(tone_pos) {
            // If a tone character is found.
            if(!tone && is_tone(vowels[tone_pos])) {
                // If tone is 'z', we delete the tone marking.
                memmove(&vowels[tone_pos], &vowels[tone_pos + 1], strlen(vowels) - tone_pos);
            } else if(tone && is_tone(vowels[tone_pos])) {
                // Else, we change the tone marking at the position found.
                vowels[tone_pos] = tone;
            } else return 1;
        } else if(tone && !tone_pos && !spec_pos) {
            // If a tone character isn't found, put it right after the vowels.
            if(vowel_count < 3) {
                cat_char(vowels, tone);
            } else if(vowel_count == 3) {
                ins_char(vowels, tone, 2);
            } else return 1;
        } else if(tone && spec_pos) {
            // If there is a ')' or '^' tone always goes there.
            ins_char(vowels, tone, spec_pos + 1);
        } else return 1;
    } else if(v && !e && tone_pos != strlen(vowels) - 1 && is_end_cons(c)) {
        // If a tone char is found and there are 2 vowels, move it to after the last vowel.
        if (vowel_count == 2) {
            char tone_char = vowels[tone_pos];
            memmove(&vowels[tone_pos], &vowels[tone_pos + 1], strlen(vowels) - tone_pos);
            cat_char(vowels, tone_char);
            cat_char(vowels, c);
        } else return 1;
    } else if(v && !e && is_tone(c)) {
        // If no ending, move tone to right after or after the first vowel depending on number of.
        if(spec_pos && vowel_count < 4) {
            ins_char(vowels, tone, spec_pos + 1);
        } else if(tone_pos && vowel_count < 4) {
            if(tone) vowels[tone_pos] = tone;
            else memmove(&vowels[tone_pos], &vowels[tone_pos + 1], strlen(vowels) - tone_pos);
        } else if(vowel_count == 3) {
            ins_char(vowels, tone, 2);
        } else if(vowel_count < 3) {
            ins_char(vowels, tone, 1);
        } else return 1;
    } else return 1;
    // Cat all the goodness together.
    char temp2[b + v + e];
    strcpy(temp2, beg_cons);
    strcat(temp2, vowels);
    strcat(temp2, end_cons);
    strcpy(word, temp2);
    return 0;
}
int main() {
    printf("Tone &c tests.\n\n");
    int i;
    char test[][20] = {"d", "d^", "xo", "xo^", "mo", "mo)", "mu)o)t", "huye^`n", "huye^`n", "huye^n", "huyen", "hoi", "hi", "hi>e", "hi^", "huoy", "on", "huyon", "hon", "ho^yun", "huon", "hon", "ho/i", "ho/", "ho>"};
    char c[] = {'d', 'd', 'o', 'o', 'w', 'w', 's', 'z', 's', 'r', 'r', 's', 'r', 'n', 'r', 'r', 'r', 'r', 'o', 'r', 'r', 'o', 'r', 'r', 'z'};
    char expect[][15] = {"d^", "dd", "xo^", "xoo", "mo)", "mow", "mu)o)/t", "huye^n", "huye^/n", "huye^>n", "huy>en", "ho/i", "hi>", "hie>n", "hi^>", "huo>y", "o>n", "huy>on", "ho^n", "ho^>yun", "huo>n", "ho^n", "ho>i", "ho>", "ho"};
    for(i = 0; i < 25; i++) {
        printf("\nInput: %s\n", test[i]);
        printf("Input Char: %c\n", c[i]);
        if(telex(test[i], c[i])) printf("||Not valid Vietnamese.||\n");
        if(strcmp(test[i], expect[i])) {
            printf("||Failure!||\n");
            printf("Output: %s\n", test[i]);
            printf("Expected: %s\n\n", expect[i]);
        }
    }
    printf("Completed.\n\n");
    printf("Normal usage tests.\n\n");
    char norm_test[][15] = {"", "ba", "hu>oy", "xo^"};
    char norm_c[] = {'a', 'y', 'n', 'w'};
    for(i = 0; i < 3; i++) {
        if(!telex(norm_test[i], norm_c[i])) {
            printf("||Failure!||\n");
            printf("\nInput: %s\n", norm_test[i]);
            printf("Input Char: %c\n", norm_c[i]);
        }
    }
    printf("Completed.\n");
}
