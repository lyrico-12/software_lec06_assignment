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