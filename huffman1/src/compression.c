#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "encode.h"
#include "compression.h"
#define NSYMBOLS 256

extern int huffman_code[NSYMBOLS];

// 何種類の文字が含まれているのかを数える
static int count_symbol_variety(void);
static int convert_binary(int n);

static int count_symbol_variety(void) {
    int c = 0;
    for (int i = 0; i < NSYMBOLS; i++) {
        if (huffman_code[i] != -1) {
            c += 1;
        }
    }
    return c;
}

static int convert_binary(int n) {
    int binary = 0;
    int remainder;
    int i = 1;

    while (n > 0) {
        remainder = n % 2;
        n = n / 2;
        binary += remainder * i;
        i *= 10;
    }
    return binary;
}

// 出力ファイルに圧縮
void compress(const char* inputfile, const char* outputfile) {
    // 入力ファイル読み込み
    FILE *fp1 = fopen(inputfile, "rb");
    if (fp1 == NULL) {
        fprintf(stderr, "error: cannot open %s\n", inputfile);
        exit(1);
    }

    // 出力ファイル読み込み
    FILE *fp2 = fopen(outputfile, "wb");
    if (fp2 == NULL) {
        fprintf(stderr, "error: cannot open %s\n", outputfile);
        exit(1);
    }

    // 
    int num = count_symbol_variety();
    fprintf(fp2, "%d\n", num);

    for (int i = 0; i < NSYMBOLS; i++) {
        if (huffman_code[i] != -1) {
            if (i != 10) {
                fprintf(fp2, "%c %d\n", i, convert_binary(huffman_code[i]));
            } else {
                fprintf(fp2, "LF %d\n", convert_binary(huffman_code[i]));
            }
        }
    }
    
    char c;
    while(fread(&c, sizeof(char), 1, fp1) != 0) {
        fprintf(fp2, "%d", convert_binary(huffman_code[(int)c]));
    }
    fclose(fp1);
    fclose(fp2);
}