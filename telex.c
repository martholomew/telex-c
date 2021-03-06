#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <ctype.h>


#define REGEX "^(ngh|d\\^|ch|gh|kh|nh|ng|ph|th|tr|qu|[bcdghklmnpqrstvx]?)([aeiouy\\^)`\\/>~.]*)(ch|nh|ng|[cmnpt]?)$"

enum comp {HAT, TAIL, UNDO};

int char_comp(char char1, char char2, char char3, enum comp flag) {
    int comp = 0;
    char1 = tolower(char1);
    char2 = tolower(char2);
    char3 = tolower(char3);
    switch(flag) {
        case HAT:
            switch(char1) {
                case 'a':
                case 'e':
                case 'o':
                case 'd':
                    if(char1 == char2) comp = 1;
                    break;
            }
            break;
        case TAIL:
            switch(char2) {
                case 'a':
                case 'o':
                case 'u':
                    if(char1 == 'w') comp = 1;
                    break;
            }
            break;
        case UNDO:
            if(char1 == char3 && char2 == '^') comp = 1;
            else if(char1 == 'w' && char2 == ')') comp = 1;
            break;
    }
    return comp;
}

void rem_char(char *str1, int pos) {
    int i, len = strlen(str1);
    char buff[20];
    for(i = 0; i < len; i++) {
        if(i > pos) buff[i - 1] = str1[i];
        else if(i < pos) buff[i] = str1[i];
    }
    buff[i - 1] = 0;
    strcpy(str1, buff);
}

void ins_char(char *str1, char c, int pos) {
    int i, len = strlen(str1);
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
    int len = strlen(str1);
    str1[len] = c;
    str1[len + 1] = 0;
}

void cpy_range(char *str1, char *str2, int x1, int x2) {
    int i, j = 0;
    for(i = x1; i < x2; i++, ++j) str1[j] = str2[i];
    str1[j] = 0;
}

char get_tone(char c) {
    char d = tolower(c);
         if(d == 'z') return 'z';
    else if(d == 'f') return '`';
    else if(d == 's') return '/';
    else if(d == 'r') return '>';
    else if(d == 'x') return '~';
    else if(d == 'j') return '.';
    else return 0;
}

int telex(char *word, char c) {
    // Make sure c is alphabetic and not 'i' or 'y'.
    if(!isalpha(c)) return 1;
    if(tolower(c) == 'i' || tolower(c) == 'y') return 1;
    char regex_word[150] = REGEX;
    regex_t regex_compiled;
    regmatch_t group_array[4];
    regcomp(&regex_compiled, regex_word, REG_EXTENDED|REG_ICASE);
    if(regexec(&regex_compiled, word, 4, group_array, 0)) return 1;
    regfree(&regex_compiled);
    char beg_cons[4], vowels[10], end_cons[3];
    int i;
    for(i = 0; i < 4; i++) {
        // Divide the word into sections with magic regex.
        char temp[strlen(word) + 1];
        cpy_range(temp, word, group_array[i].rm_so, group_array[i].rm_eo);
             if(i == 1) strcpy(beg_cons, temp);
        else if(i == 2) strcpy(vowels, temp);
        else if(i == 3) strcpy(end_cons, temp);
        temp[0] = 0;
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
        char d = vowels[i];
        if(d == ')' || d == '^') spec_pos = i;
        else if(d == '`' || d == '/' || d == '>'
                || d == '~' || d == '.') tone_pos = i;
        else vowel_count++;
    }
    char tone = get_tone(c);
    if(b && !v && !e) {
        if(b == 1 && char_comp(c, beg_cons[0], 0, HAT)) {
            // Bar'd D only occurs at the beginning of a word.
            cat_char(beg_cons, '^');
        } else if(char_comp(c, beg_cons[b - 1], beg_cons[b - 2], UNDO)) {
            beg_cons[1] = c;
        } else return 1;
    } else if(v) {
        if(char_comp(c, vowels[v - 1], 0, HAT)) {
            // Two of either 'a', 'e', or 'o' puts a hat on em.
            cat_char(vowels, '^');
        } else if(char_comp(c, vowels[v - 1], 0, TAIL)) {
            // 'a', 'o' or 'u' with a w after gives it a tail.
            cat_char(vowels, ')');
        } else if(char_comp(c, vowels[v - 1], vowels[v - 2], UNDO) ||
                  char_comp(c, vowels[v - 1], 0, UNDO)) {
            // If mod char is pressed twice, you get just the mod char.
            vowels[v - 1] = c;
        } else if(vowels[tone_pos] == tone) {
            // If tone char is pressed twice, you get just the char.
            rem_char(vowels, tone_pos);
            if(e) cat_char(end_cons, c);
            else cat_char(vowels, c);
        } else if(e && tone) {
            if(tone_pos) {
                // If tone is 'z', we delete the tone marking.
                if(tone == 'z') rem_char(vowels, tone_pos);
                // Else, we change the tone marking at the position found.
                else vowels[tone_pos] = tone;
            } else if(spec_pos) {
                // If there is a ')' or '^' tone always goes there.
                ins_char(vowels, tone, spec_pos + 1);
            } else {
                // If a tone character isn't found, put it right
                // after the vowels.
                if(vowel_count < 3) cat_char(vowels, tone);
                else if(vowel_count == 3) ins_char(vowels, tone, 2);
                else return 1;
            }
        } else if(!e) {
            if(tone_pos && tone_pos != v - 1 && !tone) {
                // If a tone char is found and there are 2 vowels,
                // move it to after the last vowel.
                if (vowel_count == 2) {
                    char tone_char = vowels[tone_pos];
                    rem_char(vowels, tone_pos);
                    cat_char(vowels, tone_char);
                    cat_char(vowels, c);
                } else return 1;
            } else if(tone) {
                // If no ending, move tone to right after the vowels
                // or after the first vowel depending on number of.
                if(spec_pos && vowel_count < 4) {
                    ins_char(vowels, tone, spec_pos + 1);
                } else if(tone_pos && vowel_count < 4) {
                    if(tone == 'z') rem_char(vowels, tone_pos);
                    else if(tone) vowels[tone_pos] = tone;
                    else return 1;
                } else if(vowel_count == 3) {
                    ins_char(vowels, tone, 2);
                } else if(vowel_count < 3) {
                    ins_char(vowels, tone, 1);
                } else return 1;
            } else return 1;
        } else return 1;
    } else return 1;
    // Cat all the goodness together.
    char temp[strlen(word) + 1];
    strcpy(temp, beg_cons);
    strcat(temp, vowels);
    strcat(temp, end_cons);
    strcpy(word, temp);
    return 0;
}
int main() {
    printf("Tone &c tests.\n");
    int i;
    char test[][20] = {"D", "D^", "xo", "xo^", "mo", "mo)", "mu)o)t", "huye^`n", "huye^`n", "huye^n", "huyen", "hoi", "hi", "hi>e", "hi^", "huoy", "on", "huyon", "hon", "ho^yun", "huon", "hon", "ho/i", "ho/", "ho>", "ho>", "ho>n"};
    char c[] = {'d', 'd', 'o', 'o', 'w', 'w', 's', 'z', 's', 'r', 'r', 's', 'r', 'n', 'r', 'r', 'r', 'r', 'o', 'r', 'r', 'o', 'r', 'r', 'z', 'r', 'r'};
    char expect[][15] = {"D^", "Dd", "xo^", "xoo", "mo)", "mow", "mu)o)/t", "huye^n", "huye^/n", "huye^>n", "huy>en", "ho/i", "hi>", "hie>n", "hi^>", "huo>y", "o>n", "huy>on", "ho^n", "ho^>yun", "huo>n", "ho^n", "ho>i", "ho>", "ho", "hor", "honr"};
    for(i = 0; i < 27; i++) {
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
    printf("\nNormal usage tests.\n");
    char norm_test[][15] = {"", "bae", "hu>oy", "xo^"};
    char norm_c[] = {'a', 'o', 'n', 'w'};
    for(i = 0; i < 3; i++) {
        printf("\nInput: %s\n", norm_test[i]);
        printf("Input Char: %c\n", norm_c[i]);
        if(!telex(norm_test[i], norm_c[i])) {
            printf("||Failure!||\n");
        }
    }
    printf("Completed.\n");
}
