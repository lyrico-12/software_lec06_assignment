# software_lec06_assignment
## 課題1
### 疑問点解消
aaaabbbccdという文字列を
```
       (10)
      /    \
   a(4)   bcd(6)
         /      \
      b(3)      cd(3)
              /      \
           c(2)      d(1)
```
みたいな木に変換したかったけど、
```
       (10)
      /    \
    (6)    a(4)
   /    \
 (3)     b(3)
 /   \
c(2)  d(1)
```
こうなってしまっていた。
build_tree関数の
```c
Node *dnode = create_node(dummy, node1->count + node2->count, node2, node1);
```
を
```c
Node *dnode = create_node(dummy, node1->count + node2->count, node1, node2);
```
に変えるとうまく行く！(cとdはどうしても逆になっちゃう)
```
       (10)
      /    \
   a(4)   (6)
         /    \
      b(3)   (3)
           /   \
        d(1)  c(2)
```
### ハフマン符号化の実装
```c
// 構造体定義
struct node{
    int symbol;// 元の文字のASCIIコード
    char huffman[NSYMBOLS];// コード語
    int count;
    Node *left;
    Node *right;
};
```
構造体定義にハフマン符号化した後のコード語を追加した。
```c
static void assign_codes(Node *np, char* code, int depth) {
    if (np == NULL) return;

    // leaf nodeだったら、終了する
    if (np->left == NULL && np->right == NULL) {
        code[depth] = '\0';
        strcpy(np->huffman, code);
        // printf("Symbol: %c, Code: %s\n", np->symbol, code);
        return;
    }

    // 左の枝に進む
    code[depth] = '0';
    assign_codes(np->left, code, depth + 1);

    // 右の枝に進む
    code[depth] = '1';
    assign_codes(np->right, code, depth + 1);
}
```
上のように、preorderで走査しながら、葉ノードに到達したら、np->huffmanにコード語をコピーするという方法で、符号化を行なった。
### 木の構造を見やすく表示
```c
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
        printf("[LF] (symbol: %d, count: %d, huffman code: %s)\n", np->symbol, np->count, np->huffman);
    } else {
        printf("[%c] (symbol: %d, count: %d, huffman code: %s)\n", np->symbol, np->symbol, np->count, np->huffman);
    }

    // 次の段のインデントを更新
    char new_prefix[NSYMBOLS];
    sprintf(new_prefix, "%s%s   ", prefix, is_left ? "│" : " ");

    traverse_tree(np->left, depth + 1, new_prefix, 1);
    traverse_tree(np->right, depth + 1, new_prefix, 0);
}
```
上のように、インデント情報をprefixに保存しながら、再帰を用いて表示した。左のノードなのか、右のノードなのかが重要で、左のノードだったら、縦線"│"を保存するようにしている。