/**
 * @file sphere_density.c
 * @author 24-326 Ren Toda
 * @brief 球の密度を計算する
 * @details 質量と半径を入力し、密度を求める。pow, scanf, printf の練習。
 * @copyright Copyright (c) 2025
 * @date 2025-05-20
 */

#include <math.h>   // 数学関数ライブラリ（pow関数使用のため）
#include <stdio.h>  // 標準入出力ライブラリ

#define PI 3.14159265358979323846  // 円周率の定義

/**
 * @brief main function
 * @return 0 正常終了
 * @return 1 異常終了
 */
int main(void) {
    // 変数宣言
    double mass, radius, volume, density;  // 質量,半径,体積,密度

    // キーボードから質量と半径を入力
    printf("球の半径[m]入力してください: ");
    scanf("%lf", &radius);
    printf("球の質量[kg]入力してください: ");
    scanf("%lf", &mass);

    // 体積を計算 (V = 4/3 * π * r^3)
    volume = (4.0 / 3.0) * PI * pow(radius, 3);

    // 密度を計算 (density = mass / volume)
    density = mass / volume;

    // 結果を表示
    printf(
        "質量%.2g[kg]、半径%.2f[m]の球の体積は%.2g[m^3]、密度は%.2g[kg/"
        "m^3]です。",
        mass, radius, volume, density);

    return 0;  // 正常終了
}
