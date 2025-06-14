#include <stdio.h>
#include <string.h>

// 構造体の定義
typedef struct {
    char name[32];
    int age;
} Person;

// 関数の宣言
void print_person(Person p);
int add(int a, int b);

int main(void) {
    // 変数宣言
    Person alice = {"Alice", 30};
    Person bob = {"Bob", 25};

    // 配列
    Person people[2] = {alice, bob};

    // ループ
    for (int i = 0; i < 2; i++) {
        print_person(people[i]);
    }

    // 関数呼び出し
    int sum = add(10, 20);
    printf("Sum: %d\n", sum);

    // 文字列操作
    char msg[64];
    strcpy(msg, "Hello, ");
    strcat(msg, alice.name);
    printf("%s\n", msg);

    return 0;
}

// 関数の定義
void print_person(Person p) { printf("Name: %s, Age: %d\n", p.name, p.age); }

int add(int a, int b) { return a + b; }

int subtract(int a, int b) { add(a, b); }
