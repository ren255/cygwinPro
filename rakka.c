/**
 * @file rakka.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-05-20
 *
 * @copyright Copyright (c) 2025
 *
 */

#define g 9.81      // 重力加速度
#include <stdio.h>  // 標準入出力ライブラリ

int main(void) {
    double time, hight, velocity;

    // キーボードから時間を入力
    printf("落下時間[s]を入力してください: ");
    scanf("%lf", &time);

    velocity = -g * time;            // 速度を計算
    hight = -(g * time * time) / 2;  // 高さを計算
    // 結果を表示
    printf("落下時間%.2f[s]のとき、速度は%.2f[m/s]、高さは%.2f[m]です。\n",
           time, velocity, hight);
}