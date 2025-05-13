/**
 * @file cal_bytes.c
 * @author 23-326 Ren Toda
 * @brief 合計バイト数を計算する
 * @details sizeof演算子やprintf、scanfの練習
 * @copyright Copyright (c) 2025
 * @date 2025-05-13
 */

#include <stdio.h>  // 標準入出力ライブラリ

/**
 * @brief main funciton
 * @return 0 正常終了
 * @return 1 異常終了
 */
int main(void) {
    // 変数宣言
    int numc;  // char型変数の個数を表すint型変数
    int numi;  // int型変数の個数を表すint型変数
    int nums;  // short型変数の個数を表すint型変数
    int sum;   // 合計バイト数を表すint型変数

    // 変数の個数をkyeboardから入力
    printf("char型、int型、short型の変数の個数をそれぞれ入力してください。: ");
    scanf("%d %d %d", &numc, &numi, &nums);

    // 合計バイト数を計算
    sum = (numc * sizeof(char)) + (numi * sizeof(int)) + (nums * sizeof(short));

    // 計算結果を表示
    printf(
        "cahr型%d個、int型%d個、short型%d個の変数の合計バイト数は%"
        "dバイトです。\n",
        numc, numi, nums, sum);

    return 0;  // 正常終了
}