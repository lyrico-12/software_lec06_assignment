#pragma once

typedef struct node Node;
   
// ファイルをエンコードし木のrootへのポインタを返す
Node *encode(const char *filename);
// Treeを走査して表示する
void traverse_tree(const Node* np, const int depth, const char *prefix, int is_left);
// ハフマン木をフリーする
void free_tree(Node *np);