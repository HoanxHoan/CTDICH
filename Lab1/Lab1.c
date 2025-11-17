#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_WORD 9000
#define MAX_LEN  90000
#define MAX_LINE 90000

typedef struct {
    char word[100];          // từ
    int count;              // số lần xuất hiện
    int lines[10000];         // danh sách dòng
    int lineCount;          // số dòng xuat hien
} IndexItem;

IndexItem table[MAX_WORD];  //mảng các mục từ.
int tableSize = 0;          //số từ hiện có trong bảng chỉ mục.

char stopwords[500][100];   //danh sách từ cần bỏ qua.
int stopCount = 0;          //số lượng stopword.



//xác định một từ đứng sau dấu kết câu
int isSentenceEnd(char ch) {
    return (ch == '.' || ch == '!' || ch == '?');
}
//kiểm tra stopwords
int isStopWord(char *w) {
    for (int i = 0; i < stopCount; i++) {
        if (strcmp(stopwords[i], w) == 0)
            return 1;
    }
    return 0;
}
//Tìm vị trí từ trong bảng
int findWord(char *w) {
    for (int i = 0; i < tableSize; i++) {
        if (strcmp(table[i].word, w) == 0)
            return i;
    }
    return -1;
}
//Thêm từ vào bảng chỉ mục
void addIndex(char *w, int line) {
    int idx = findWord(w);
    
    if (idx == -1) {// từ chưa có
        strcpy(table[tableSize].word, w);
        table[tableSize].count = 1;
        table[tableSize].lineCount = 1;
        table[tableSize].lines[0] = line;
        tableSize++;
    } else {// từ đã tồn tại
        table[idx].count++;

        // thêm dòng xuất hiện nếu dòng hiện tại không trùng dòng cuối trong danh sách
        if (table[idx].lines[table[idx].lineCount - 1] != line) {
            table[idx].lines[table[idx].lineCount++] = line;
        }
    }
}
//hàm dùng cho Qsort
int cmp(const void *a, const void *b) {
    IndexItem *x = (IndexItem *)a;
    IndexItem *y = (IndexItem *)b;
    return strcmp(x->word, y->word);
}

int main(int argc, char *argv[]) {
    char *textFile = argv[1];
    char *stopFile = argv[2];

    //Đọc stop words 
    FILE *fs = fopen(stopFile, "r");
    if (!fs) {
        printf("Khong mo duoc file %s \n",stopFile);
        return 1;
    }

    while (fscanf(fs, "%s", stopwords[stopCount]) == 1) {
        for (int i = 0; stopwords[stopCount][i]; i++)
            stopwords[stopCount][i] = tolower(stopwords[stopCount][i]);
        stopCount++;
    }
    fclose(fs);


    //Đọc văn bản 
    FILE *fv = fopen(textFile, "r");
    if (!fv) {
        printf("Khong mo duoc %s\n",textFile);
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
            if (isalpha(lineStr[i])) {//Tách từ trong một dòng
                word[pos++] = lineStr[i];
            } else {
                if (pos > 0) {
                    word[pos] = '\0';

// kiểm tra đứng sau dấu kết câu
                    int afterEnd = 0;
                    int k = i - pos - 1;

                    while (k >= 0) {
                        if (isspace(lineStr[k])) {
                            k--; continue;
                        }
                        if (isSentenceEnd(lineStr[k]))
                            afterEnd = 1;
                        break;
                    }

                   

                    if (isupper(word[0])) {
                        if (!afterEnd) {
                            pos = 0;
                            i++;
                            continue;   // loại danh từ riêng
                        }
                    }

                    // lowercase
                    for (int t = 0; word[t]; t++)
                        word[t] = tolower(word[t]);

                    if (!isStopWord(word)) {
                        addIndex(word, lineNumber);
                    }
                    

                }
                pos = 0;
            }
            i++;
        }
    }

    fclose(fv);

    //Sắp xếp
    qsort(table, tableSize, sizeof(IndexItem), cmp);

    //In kết quả 
    for (int i = 0; i < tableSize; i++) {
        printf("%-12s %d  ", table[i].word, table[i].count);
        for (int j = 0; j < table[i].lineCount; j++) {
            printf("%d", table[i].lines[j]);
            if (j < table[i].lineCount - 1)
                printf(", ");
        }
        printf("\n");
    }

    return 0;
}
