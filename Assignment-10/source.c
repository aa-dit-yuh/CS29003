#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const int BG_RED = 41;
const int BG_GREEN = 42;
const int BG_BLUE = 44;
const int BG_DEFAULT = 49;


inline int min2(int a, int b)
{
    return a<b?a:b;
}

inline int min3(int a, int b, int c)
{
    return a<min2(b,c)?a:min2(b,c);
}

inline int minArray(int *a, int len)
{
    int i, min_index = 0, min = a[0];
    for(i = 1; i < len; i++){
        if(a[i] < min){
            min = a[i];
            min_index = i;
        }
    }
    return min_index;
}

inline void printColour(char c, int colour)
{
    printf("\033[%dm%c\033[49m", colour, c);
}

inline void printColourStr(char c[], int colour)
{
    printf("\033[%dm%s\033[49m", colour, c);
}

inline int countLines(FILE *fp)
{
    if(fp == NULL){
        printf("Null Pointer Exception\n");
        exit(EXIT_FAILURE);
    }
    rewind(fp);
    char c;
    int lines = 1;
    while(!feof(fp)){
      c = fgetc(fp);
      if(c == '\n'){
        lines++;
      }
    }
    rewind(fp);
    return lines;
}

inline char **getLines(FILE *fp, int line_count)
{
    if(fp == NULL){
        printf("Null Pointer Exception\n");
        exit(EXIT_FAILURE);
    }
    int i;
    size_t len = 0;
    char **lines = malloc(sizeof(char *) * line_count);
    for(i = 0; i < line_count; i++){
        getline(&lines[i], &len, fp);
        if(lines[i][strlen(lines[i]) - 1] == '\n'){
            // Trim trailing line feed
            lines[i][strlen(lines[i]) - 1] = '\0';
        }
    }
    return lines;
}

inline int **calcLineDistMat(char *line_1, char *line_2)
{
    int len_1, len_2, i, j;

    len_1 = strlen(line_1);
    len_2 = strlen(line_2);

    int **distMat = malloc(sizeof(int *) * (len_1+1));
    for(i = 0; i <= len_1; i++){
        distMat[i] = malloc(sizeof(int) * (len_2+1));
    }

    for(i = 0; i <= len_1; i++){
        distMat[i][0] = i;
    }
    for(j = 0; j <= len_2; j++){
        distMat[0][j] = j;
    }

    for(i = 1; i<= len_1; i++){
        for(j = 1; j <= len_2; j++){
            if(line_1[i-1] == ' '){
                distMat[i][j] = distMat[i-1][j];
                continue;
            }
            if(line_2[j-1] == ' '){
                distMat[i][j] = distMat[i][j-1];
                continue;
            }
            if(line_1[i-1] == line_2[j-1]){
                distMat[i][j] = distMat[i-1][j-1];
                continue;
            }
            distMat[i][j] = min3(
                distMat[i-1][j-1],
                distMat[i-1][j],
                distMat[i][j-1]
            ) + 1;
        }
    }
    return distMat;
}

inline int calcLineDist(char *line_1, char *line_2)
{
    return calcLineDistMat(line_1, line_2)[strlen(line_1)][strlen(line_2)];
}

inline int printDistMat(int **distMat, int len_1, int len_2)
{
    int i, j;
    for(i = 0; i <= len_1; i++){
        for(j = 0; j <= len_2; j++){
            printf("%d\t", distMat[i][j]);
        }
        putchar('\n');
    }
}

inline void printLineDiff(char *line_1, char *line_2)
{
    int min, i = strlen(line_1), j = strlen(line_2);
    int *diffFlag_1 = malloc(sizeof(int) * i);
    int **distMat = calcLineDistMat(line_1, line_2);
    printf("Edit distance = %d\n", distMat[i][j]);
    // printDistMat(distMat, i, j);
    while(i>=1 && j>=1){
        min = min3(
            distMat[i-1][j],
            distMat[i][j-1],
            distMat[i-1][j-1]
        );
        if(min == distMat[i][j]){
            diffFlag_1[i-1] = BG_DEFAULT;
            i--;
            j--;
        }
        else{
            if(min == distMat[i-1][j-1]){
                diffFlag_1[i-1] = BG_BLUE;
                i--;
                j--;
            }
            else if(min == distMat[i-1][j]){
                diffFlag_1[i-1] = BG_RED;
                i--;
            }
            else{
                diffFlag_1[i-1] = BG_GREEN;
                j--;
            }
        }
    }
    for(i = 0; i < strlen(line_1); i++){
        printColour(line_1[i], diffFlag_1[i]);
    }
    putchar('\n');  
}

int **calcFileDistMat(FILE *file_1, FILE *file_2)
{
    int len_1, len_2, i, j, min;
    char **lines_1, **lines_2;

    len_1 = countLines(file_1);
    lines_1 = getLines(file_1, len_1);  

    len_2 = countLines(file_2);
    lines_2 = getLines(file_2, len_2);

    int **distMat = malloc(sizeof(int *) * (len_1+1));
    for(i = 0; i <= len_1; i++){
        distMat[i] = malloc(sizeof(int) * (len_2+1));
    }

    distMat[0][0] = 0;
    for(i = 1; i <= len_1; i++){
        distMat[i][0] = distMat[i-1][0] + strlen(lines_1[i-1]);
    }
    for(j = 1; j <= len_2; j++){
        distMat[0][j] = distMat[0][j-1] + strlen(lines_2[j-1]);
    }

    for(i = 1; i<= len_1; i++){
        for(j = 1; j <= len_2; j++){
            // if(strlen(lines_1[i-1]) == 0){
            //     distMat[i][j] = distMat[i-1][j];
            //     continue;
            // }
            // if(strlen(lines_2[j-1]) == 0){
            //     distMat[i][j] = distMat[i][j-1];
            //     continue;
            // }
            if(!strcmp(lines_1[i-1], lines_2[j-1])){
                distMat[i][j] = distMat[i-1][j-1];
                continue;
            }
            min = min3(
                distMat[i-1][j-1],
                distMat[i-1][j],
                distMat[i][j-1]
            );
            if(min == distMat[i-1][j-1]){
                distMat[i][j] = min + calcLineDist(lines_1[i-1], lines_2[j-1]);
            }
            else if(min == distMat[i-1][j]){
                distMat[i][j] = min + strlen(lines_1[i-1]);
            }
            else{
                distMat[i][j] = min + strlen(lines_2[j-1]);
            }
        }
    }
    return distMat;
}

void printFileDiff(FILE *file_1, FILE *file_2)
{
    int min, i = countLines(file_1), j = countLines(file_2);
    char **lines_1 = getLines(file_1, i);
    char **lines_2 = getLines(file_2, j);
    int *diffFlag_1 = malloc(sizeof(int) * i);
    int *lineMatch_1 = malloc(sizeof(int) * i);
    int *diffFlag_2 = malloc(sizeof(int) * j);
    int *lineMatch_2 = malloc(sizeof(int) * j);
    int **distMat = calcFileDistMat(file_1, file_2);
    printf("Edit distance = %d\n", distMat[i][j]);
    // printDistMat(distMat, i, j);
    while(i>=1 && j>=1){
        min = min3(
            distMat[i-1][j],
            distMat[i][j-1],
            distMat[i-1][j-1]
        );
        if(min == distMat[i][j]){
            diffFlag_1[i-1] = diffFlag_2[j-1] = BG_DEFAULT;
            i--;
            j--;
        }
        else{
            if(min == distMat[i-1][j-1]){
                diffFlag_1[i-1] = BG_BLUE;
                lineMatch_1[i-1] = j - 1;
                diffFlag_2[j-1] = BG_BLUE;
                lineMatch_2[j-1] = i - 1;
                i--;
                j--;
            }
            else if(min == distMat[i-1][j]){
                diffFlag_1[i-1] = BG_RED;
                diffFlag_2[j-1] = BG_DEFAULT;
                i--;
            }
            else{
                diffFlag_1[i-1] = BG_DEFAULT;
                diffFlag_2[j-1] = BG_GREEN;
                j--;
            }
        }
    }
    putchar('\n');
    for(i = 0; i < countLines(file_1); i++){
        if(diffFlag_1[i] != BG_BLUE){
            printColourStr(lines_1[i], diffFlag_1[i]);
        }
        else{
            printLineDiff(lines_1[i], lines_2[lineMatch_1[i]]);
        }
        putchar('\n');
    }
    putchar('\n');
    for(j = 0; j < countLines(file_2); j++){
        if(diffFlag_2[j] != BG_BLUE){
            printColourStr(lines_2[j], diffFlag_2[j]);
        }
        else{
            printLineDiff(lines_2[j], lines_1[lineMatch_2[j]]);
        }
        putchar('\n');
    }
}

void testcalcLineDistMat()
{
    printColourStr("Running tests for calcLineDistMat", BG_RED);
    putchar('\n');
    char *test_1 = "Hello universe";
    char *test_2 = "Heel World";

    printLineDiff(test_1, test_2);
    printLineDiff(test_2, test_1);
}

void testcalcFileDistMat()
{
    printColourStr("Running tests for calcFileDistMat", BG_RED);
    putchar('\n');
    FILE *f1 = fopen("test_1.c", "w");
    FILE *f2 = fopen("test_2.c", "w");

    fprintf(f1, "21\n3\n2\n4");
    fprintf(f2, "2\n3\n3\n4\n12");

    fclose(f1);
    fclose(f2);

    f1 = fopen("test_1.c", "r");
    f2 = fopen("test_2.c", "r");

    printFileDiff(f1, f2);

    remove("test_1.c");
    remove("test_2.c");
}

int main()
{
    FILE *file_1, *file_2;

    file_1 = fopen("example-1.c", "r");

    file_2 = fopen("example-2.c", "r");

    // testcalcLineDistMat();
    // testcalcFileDistMat();

    printFileDiff(file_2, file_1);

    return 0;
}