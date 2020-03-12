#include <stdio.h>
#include <regex.h>
#include <string.h>

#define REGEX "^(ngh|d\\^|ch|gh|kh|nh|ng|ph|th|tr|qu|[bcdghklmnpqrstvx]?)([aeiouy\\^)`\\/>~.]*)(ch|nh|ng|[cmnpt]?)$"

void ins_char(char *str1, char c, int pos) {
    int i, len;
    pos += 2;
    for(len = 0; str1[len] != 0; len++);
    for(i = len; i >= pos; i--)
        str1[i] = str1[i - 1];
    str1[pos - 1] = c;
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
    char tl_vowel = vowels[strlen(vowels) - 3];
    if(b && !v && !e && strlen(beg_cons) == 1 && beg_cons[0] == c && c == 'd') {
        strcat(word, "^");
        return 0;
    } else if(!e && is_ntd(c) && is_ntd(l_vowel)) {
        if(c == l_vowel && (c == 'a' || c == 'e' || c == 'o')) {
            strcat(word, "^");
        } else if(c == 'w' && (l_vowel == 'a' || l_vowel == 'o' || l_vowel == 'u')) {
            strcat(word, ")");
        }
        return 0;
    } else if(!e && is_ntd(c) && (l_vowel == '^' || l_vowel == ')')) {
        if(l_vowel == '^' && c == sl_vowel && (c == 'a' || c == 'e' || c == 'o')) {
            word[strlen(word) - 1] = c;
        } else if(l_vowel == ')' && c == 'w' && (sl_vowel == 'a' || sl_vowel == 'o' || sl_vowel == 'u')) {
            word[strlen(word) - 1] = c;
        }
        return 0;
    } else if(v && e) {
        char regex_ntd[20] = "[aeou][\\^)`\\/>~.]+";
        regmatch_t matches[1];
        regcomp(&regex_compiled, regex_ntd, REG_EXTENDED|REG_ICASE);
        char slider[strlen(word) + 1];
        strcpy(slider, word);
        int pos = 0;
        int offset = 0;
        for(int i = 0; i < 2; i++) {
            //printf("\nslider - %s\n", slider);
            if(regexec(&regex_compiled, slider, 1, matches, 0)) break;
            if(matches[0].rm_so == (size_t) - 1) break;
            char vowels_copy[strlen(slider) + 1];
            cpy_range(vowels_copy, slider, matches[0].rm_so, matches[0].rm_eo);
            //printf("%d - %s (%d - %d)\n", i, vowels_copy, matches[0].rm_so, matches[0].rm_eo);
            pos = matches[0].rm_eo + offset - 1;
            if(matches[0].rm_eo == strlen(slider)) break;
            offset = matches[0].rm_eo;
            cpy_range(slider, word, matches[0].rm_eo, strlen(slider));
            memset(vowels_copy, 0, sizeof(vowels_copy));
            //printf("\nChar - %c (pos: %d)\n", word[pos], pos);
        }
        regfree(&regex_compiled);
        if(is_tone(c) && pos) {
            char tone = get_tone(c);
            if(!tone && is_tone(word[pos])) {
                memmove(&word[pos], &word[pos + 1], strlen(word) - pos);
            } else if(tone && is_tone(word[pos])) {
                word[pos] = tone;
            } else if(tone && !is_tone(word[pos])) {
                //printf("\n Word %s, Tone %c, Pos %d", word, tone, pos);
                ins_char(word, tone, pos);
            }
        } else if(is_tone(c) && !pos) {
            char tone = get_tone(c);
            ins_char(word, tone, strlen(beg_cons) + strlen(vowels) - 1);
        }
        return 0;
    } else if(v && !e && is_tone(c)) {
        char tone = get_tone(c);
        if(strlen(vowels) > 1) {
            ins_char(word, tone, strlen(beg_cons) + strlen(vowels) - 2);
        } else if(strlen(vowels) == 1) {
            ins_char(word, tone, strlen(beg_cons) + strlen(vowels) - 1);
        }
        return 0;
    } else if(v && !e && is_end_cons(c)) {
        //printf("\nChar - %c\n", word[2]);
        if(is_vowel(l_vowel) && is_tone(sl_vowel) && is_vowel(tl_vowel)) {
            int del_pos = strlen(word) - 2;
            memmove(&word[del_pos], &word[del_pos + 1], strlen(word) - del_pos);
            ins_char(word, sl_vowel, strlen(word) - 1);
            ins_char(word, c, strlen(word) - 1);
        }
        return 0;
    } else return 1;
}
int main() {
    int i;
    char test[][15] = {"d", "xo", "xo^", "mo", "mo)", "mu)o)t", "huye^`n", "huye^`n", "huye^n", "huyen", "hoi", "hi", "hi>e"};
    char c[] = {'d', 'o', 'o', 'w', 'w', 's', 'z', 's', 'r', 'r', 's', 'r', 'n'};
    char expect[][15] = {"d^", "xo^", "xoo", "mo)", "mow", "mu)o)/t", "huye^n", "huye^/n", "huye^>n", "huye>n", "ho/i", "hi>", "hie>n"};
    for(i = 0; i < 13; i++) {
        //printf("Input: %s\n", test[i]);
        //printf("Input Char: %c\n", c[i]);
        if(telex(test[i], c[i])) printf("Not valid Vietnamese.\n");
        //printf("Output: %s\n", test[i]);
        //printf("Expected: %s\n\n", expect[i]);
        if(!strcmp(test[i], expect[i])) printf("Success!\n");
    }
}
