#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "encode.h"
#include "compression.h"
#define NSYMBOLS 256

struct bitwriter {
    unsigned char buffer;
    int bit_count;
};

extern unsigned int huffman_code[NSYMBOLS];
extern unsigned int huffman_code_length[NSYMBOLS];

// 何種類の文字が含まれているのかを数える
static unsigned char count_symbol_variety(void);

// 初期化
static void init_bitwriter(BitWriter *bw);

// ビットを書き込む
static void write_bit(BitWriter *bw, int bit, FILE* fp);

// ビット列を書き込む
static void write_bits(BitWriter* bw, unsigned int bits, int length, FILE* fp);

// バッファに残ったビットを出力
static void flush(BitWriter* bw, FILE* fp);

static unsigned char count_symbol_variety(void) {
    unsigned c = 0;
    for (int i = 0; i < NSYMBOLS; i++) {
        if (huffman_code_length[i] > 0) {
            c += 1;
        }
    }
    return c;
}

static void init_bitwriter(BitWriter *bw) {
    bw->buffer = 0;
    bw->bit_count = 0;
}

static void write_bit(BitWriter *bw, int bit, FILE* fp) {
    bw->buffer = (bw->buffer << 1) | (bit & 1);
    bw->bit_count++;

    if (bw->bit_count == 8) {// 8bitに到達したらファイルに書き込む
        fwrite(&(bw->buffer), sizeof(unsigned char), 1, fp);
        bw->buffer = 0;
        bw->bit_count = 0;
    }
}

static void write_bits(BitWriter* bw, unsigned int bits, int length, FILE* fp) {
    // bitsを2進数と見て、左から見ていく
    for (int i = length - 1; i >= 0; i--) {
        int bit = (bits >> i) & 1;
        write_bit(bw, bit, fp);
    }
}

static void flush(BitWriter* bw, FILE* fp) {
    if (bw->bit_count > 0) {
        bw->buffer <<= (8 - bw->bit_count);
        fwrite(&(bw->buffer), sizeof(unsigned char), 1, fp);
        bw->buffer = 0;
        bw->bit_count = 0;
    }
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

    // 出力ファイルに書き込む

    unsigned char num = count_symbol_variety();
    fwrite(&num, sizeof(unsigned char), 1, fp2);// 文字数を書き込み

    // 変換の対応を書き込む
    for (int i = 0; i < NSYMBOLS; i++) {
        if (huffman_code_length[i] > 0) {
            fwrite(&i, sizeof(unsigned char), 1, fp2);
            fwrite(&huffman_code_length[i], sizeof(unsigned int), 1, fp2); // コード長を書き込む
            fwrite(&huffman_code[i], sizeof(unsigned int), 1, fp2); // コードを書き込む
        }
    }

    BitWriter bw;
    init_bitwriter(&bw);
    
    // 変換後のコード列を書き込む
    unsigned char c;
    while(fread(&c, sizeof(unsigned char), 1, fp1) != 0) {
        unsigned int code = huffman_code[c];
        int length = huffman_code_length[c];
        write_bits(&bw, code, length, fp2);
    }

    flush(&bw, fp2);

    fclose(fp1);
    fclose(fp2);
}