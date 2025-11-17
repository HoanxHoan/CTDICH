#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_WORD 9000
#define MAX_LEN  9000

typedef struct {
    char word[100];
    int count;
    int lines[500];
    int lineCount;
} IndexItem;

IndexItem table[MAX_WORD];
int tableSize = 0;

// stopwords
char stopwords[500][100];
int stopCount = 0;

// danh sách tên riêng thực sự
char properNames[500][100];
int properCount = 0;

int isSentenceEnd(char ch) {
    return (ch == '.' || ch == '!' || ch == '?');
}

int isStopWord(char *w) {
    for (int i = 0; i < stopCount; i++)
        if (strcmp(stopwords[i], w) == 0)
            return 1;
    return 0;
}

int isProperName(char *w) {
    for (int i = 0; i < properCount; i++)
        if (strcmp(properNames[i], w) == 0)
            return 1;
    return 0;
}

int findWord(char *w) {
    for (int i = 0; i < tableSize; i++) {
        if (strcmp(table[i].word, w) == 0)
            return i;
    }
    return -1;
}

// xoá 1 mục khỏi table
void removeWordFromTable(char *w) {
    int pos = findWord(w);
    if (pos == -1) return;

    for (int i = pos; i < tableSize - 1; i++)
        table[i] = table[i + 1];

    tableSize--;
}

// thêm tên riêng thật
void addProperName(char *w) {
    // nếu đã có thì thôi
    if (isProperName(w)) return;

    strcpy(properNames[properCount++], w);

    // xóa từ đó trong table
    removeWordFromTable(w);
}

void addIndex(char *w, int line) {
    if (isProperName(w)) return; // nếu là tên riêng thật → bỏ

    int idx = findWord(w);
    if (idx == -1) {
        strcpy(table[tableSize].word, w);
        table[tableSize].count = 1;
        table[tableSize].lineCount = 1;
        table[tableSize].lines[0] = line;
        tableSize++;
    } else {
        table[idx].count++;
        if (table[idx].lines[table[idx].lineCount - 1] != line)
            table[idx].lines[table[idx].lineCount++] = line;
    }
}

int cmp(const void *a, const void *b) {
    return strcmp(((IndexItem*)a)->word, ((IndexItem*)b)->word);
}

int main(int argc, char *argv[]) {
    char *filename1 = argv[1];   // stopword
    char *filename2 = argv[2];   // text file

    // đọc stopword
    FILE *fs = fopen(filename2, "r");
    if (!fs) {
        printf("Khong mo duoc %s\n",filename2);
        return 1;
    }

    while (fscanf(fs, "%s", stopwords[stopCount]) == 1) {
        for (int i = 0; stopwords[stopCount][i]; i++)
            stopwords[stopCount][i] = tolower(stopwords[stopCount][i]);
        stopCount++;
    }
    fclose(fs);

    // đọc văn bản
    FILE *fv = fopen(filename1, "r");
    if (!fv) {
        printf("Khong mo duoc %s\n",filename1);
        return 1;
    }

    char lineStr[MAX_LEN];
    int lineNumber = 0;

    while (fgets(lineStr, sizeof(lineStr), fv)) {
        lineNumber++;
        int len = strlen(lineStr);
        int i = 0;

        char word[100];
        int pos = 0;

        while (i <= len) {
            if (isalpha(lineStr[i])) {
                word[pos++] = lineStr[i];
            } else {
                if (pos > 0) {
                    word[pos] = '\0';

                    // kiểm tra xem có phải ở đầu câu?
                    int afterEnd = 0;
                    int k = i - pos - 1;

                    while (k >= 0 && isspace(lineStr[k])) k--;
                    if (k >= 0 && isSentenceEnd(lineStr[k]))
                        afterEnd = 1;

                    // kiểm tra viết hoa
                    if (isupper(word[0])) {
                        if (!afterEnd) {
                            // Đây là tên riêng thực sự → thêm vào danh sách
                            char lower[100];
                            for (int t = 0; word[t]; t++)
                                lower[t] = tolower(word[t]);
                            lower[strlen(word)] = '\0';

                            addProperName(lower);
                        }
                    }

                    // chuẩn hóa lowercase
                    for (int t = 0; word[t]; t++)
                        word[t] = tolower(word[t]);

                    if (!isStopWord(word) && !isProperName(word))
                        addIndex(word, lineNumber);
                }
                pos = 0;
            }
            i++;
        }
    }

    fclose(fv);

    qsort(table, tableSize, sizeof(IndexItem), cmp);

    for (int i = 0; i < tableSize; i++) {
        printf("%-12s %d  ", table[i].word, table[i].count);
        for (int j = 0; j < table[i].lineCount; j++) {
            printf("%d", table[i].lines[j]);
            if (j < table[i].lineCount - 1) printf(", ");
        }
        printf("\n");
    }

    return 0;
}
