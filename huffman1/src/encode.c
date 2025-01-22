#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "encode.h"
#define NSYMBOLS 256

// 構造体定義
struct node{
    int symbol;// 元の文字のASCIIコード
    int huffman;// ハフマン符号
    int count;
    Node *left;
    Node *right;
};

// 各シンボルの出現回数
static int symbol_count[NSYMBOLS];

// ハフマン符号を保存するグローバル変数
int huffman_code[NSYMBOLS];

// 以下このソースで有効なstatic関数のプロトタイプ宣言

// ファイルを読み込み、static配列の値を更新する関数
static void count_symbols(const char *filename);

// symbol_count をリセットする関数
static void reset_count(void);

// huffman_code をリセットする関数
static void reset_huffman_code(void);

// 与えられた引数でNode構造体を作成し、そのアドレスを返す関数
static Node *create_node(int symbol, int count, Node *left, Node *right);

// Node構造体へのポインタが並んだ配列から、最小カウントを持つ構造体をポップしてくる関数
// n は 配列の実効的な長さを格納する変数を指している（popするたびに更新される）
static Node *pop_min(int *n, Node *nodep[]);

// ハフマン木を構成する関数
static Node *build_tree(void);

static void assign_codes(Node *np, int code, int depth);

static void print_binary(int n);

// 以下 static関数の実装
static void count_symbols(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "error: cannot open %s\n", filename);
        exit(1);
    }

    // 1Byteずつ読み込み、カウントする
    char c;   
    while (fread(&c, sizeof(char), 1, fp) != 0) {
        symbol_count[(int)c] += 1;
    }

    fclose(fp);
}
static void reset_count(void)
{
    for (int i = 0 ; i < NSYMBOLS ; i++) symbol_count[i] = 0;
}

static void reset_huffman_code(void) {
    for (int i = 0 ; i < NSYMBOLS ; i++) huffman_code[i] = -1;// まだ符号化されていない文字は-1を入れておく
}

// nodeを作る時は、ハフマン符号は０にしておく
static Node *create_node(int symbol ,int count, Node *left, Node *right)
{
    Node *ret = (Node *)malloc(sizeof(Node));
    *ret = (Node){ .symbol = symbol, .huffman = 0, .count = count, .left = left, .right = right};
    return ret;
}

static Node *pop_min(int *n, Node *nodep[])
{
    // Find the node with the smallest count
    // カウントが最小のノードを見つけてくる
    int argmin = 0;
    for (int i = 0; i < *n; i++) {
        if (nodep[i]->count < nodep[argmin]->count) {
            argmin = i;
        }
    }
    
    Node *node_min = nodep[argmin];
    
    // Remove the node pointer from nodep[]
    // 見つかったノード以降の配列を前につめていく
    for (int i = argmin; i < (*n) - 1; i++) {
        nodep[i] = nodep[i + 1];
    }
    // 合計ノード数を一つ減らす
    (*n)--;
    
    return node_min;
}

static Node *build_tree(void)
{
    int n = 0;
    Node *nodep[NSYMBOLS];
    
    for (int i = 0; i < NSYMBOLS; i++) {
        // カウントの存在しなかったシンボルには何もしない
        if (symbol_count[i] == 0) continue;
        nodep[n++] = create_node(i, symbol_count[i], NULL, NULL);
    }

    const int dummy = -1; // ダミー用のsymbol を用意しておく
    while (n >= 2) {
        Node *node1 = pop_min(&n, nodep);
        Node *node2 = pop_min(&n, nodep);
        
        // Create a new node
        // 選ばれた2つのノードを元に統合ノードを新規作成
        // 作成したノードはnodep にどうすればよいか?

        Node *dnode = create_node(dummy, node1->count + node2->count, node1, node2);
        nodep[n] = dnode;// 末尾に追加
        n++;// 配列の要素数をプラスする
    }

    // なぜ以下のコードで木を返したことになるか少し考えてみよう
    return (n==0)?NULL:nodep[0];
}

static void assign_codes(Node *np, int code, int depth) {
    // leaf nodeだったら、終了する
    if (np->left == NULL && np->right == NULL) {
        huffman_code[np->symbol] = code;// huffman_code配列にコード語を追加
        np->huffman = code;
        return;
    }
    
    unsigned int temp = code;
    // 左の枝に進む
    code = temp * 2;
    assign_codes(np->left, code, depth + 1);

    // 右の枝に進む
    code = temp * 2 + 1;
    assign_codes(np->right, code, depth + 1);
}

static void print_binary(int n) {
    for (int i = 0; i < 8; i++) {
        if (n & 1 << (7 - i)) {
            printf("1");
        } else {
            printf("0");
        }
    }
}

/*
行きがけ順 (preorder)
親 -> 左の子 -> 右の子 の再帰
通りがけ順 (inorder)
左の子 -> 親 -> 右の子 の再帰
帰りがけ順 (postorder)
左の子 -> 右の子 -> 親 の再帰
*/

// Perform depth-first traversal of the tree
// 深さ優先で木を走査する
// 木の構造が見やすくなるように表示を工夫
void traverse_tree(const Node* np, const int depth, const char *prefix, int is_left) {
    if (np == NULL) return;

    printf("%s", prefix);// インデントを表示
    if (depth > 0) {
        printf("%s── ", is_left ? "├": "└");// ├── (左のノード)または └── (右のノード)を表示
    }

    // ノードの情報を表示
    if (np->symbol == -1) {
        printf("[*] (count: %d)\n", np->count);
    } else if (np->symbol == 10) {// 改行文字だったら、LFと表示
        printf("[LF] (symbol: %d, count: %d, huffman code: ", np->symbol, np->count);
        print_binary(np->huffman);
        printf(")\n");
    } else {
        printf("[%c] (symbol: %d, count: %d, huffman code: ", np->symbol, np->symbol, np->count);
        print_binary(np->huffman);
        printf(")\n");
    }

    // 次の段のインデントを更新
    char new_prefix[NSYMBOLS];
    sprintf(new_prefix, "%s%s   ", prefix, is_left ? "│" : " ");

    traverse_tree(np->left, depth + 1, new_prefix, 1);
    traverse_tree(np->right, depth + 1, new_prefix, 0);
}

// この関数は外部 (main) で使用される (staticがついていない)
Node *encode(const char *filename)
{
    reset_count();
    reset_huffman_code();
    count_symbols(filename);
    Node *root = build_tree();
    
    if (root == NULL){
        fprintf(stderr,"A tree has not been constructed.\n");
    }

    assign_codes(root, 0, 0);

    return root;
}
