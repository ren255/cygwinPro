/**
 * @file jikosho.c
 * @author Ren Toda
 * @brief 自己紹介をprintfを使い行う
 * @details `gcc jikosho.c -o jikosho`でコンパイルし、`./jikosho`で実行する
 * @details C言語の基礎を学ぶ
 * @date 2025-04-22
 * @copyright Copyright (c) 2025
 *
 */
#include <stdio.h>  // 標準入出力ライブラリ

/**
 * @brief main funciton
 * @return 0 正常終了
 * @return 1 異常終了
 */
int main(void) {                                       // 自己紹介
    printf("私の名前はRen Todaです。\n");              // 名前
    printf("趣味: ゲーム、読書、プログラミング等\n");  // 趣味
    printf("国籍: 日本人とアメリカ人のハーフ\n");      // 国籍
    return 0;                                          // 正常終了
}