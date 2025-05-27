/**
 * @file sosuu.c
 * @author 24-326 Ren Toda
 * @brief if文を使って素数判定を行うプログラム
 * @details if文の練習
 * @date 2025-05-27
 * @copyright Copyright (c) 2025
 */

#include "stdio.h"

int checkPrime(int num);
int roop(void);

int main(void) {
    int num;  // 受け取る数

    printf("素数判定 数を入力してください(1~120): ");
    scanf("%d", &num);

    if (num <= 1) {
        printf("素数でない\n");
        return -1;
    }

    if (num == 2 || num == 3 || num == 5 || num == 7) {
        printf("素数!!!!!!!\n");
        return 0;
    }

    if ((num % 2) == 0) {
        printf("2の倍数なので素数でない\n");
        return -1;
    }
    if ((num % 3) == 0) {
        printf("3の倍数なので素数でない\n");
        return -1;
    }
    if ((num % 5) == 0) {
        printf("5の倍数なので素数でない\n");
        return -1;
    }
    if ((num % 7) == 0) {
        printf("7の倍数なので素数でない\n");
        return -1;
    }

    if (num > 120) {
        printf("120以上は分からない\n");
        return -1;
    }

    printf("素数!!!!!!!\n");
    return 0;
}

int roop(void) {
    for (int i = -10; i <= 150; i++) {
        printf("素数判定: %d     ", i);
        checkPrime(i);
    }
}

int checkPrime(int num) {
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            printf("%dは素数ではない\n", num);
            return -1;
        }
    }
}