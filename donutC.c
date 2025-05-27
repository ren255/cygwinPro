#include <stdio.h>
#include <math.h>
#include <string.h>

// 定数定義
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 22
#define BUFFER_SIZE (SCREEN_WIDTH * 22)

// メイン関数
int main() {
    // 回転角度
    float angleA = 0;
    float angleB = 0;

    // バッファと奥行き情報
    float zBuffer[BUFFER_SIZE];
    char outputBuffer[BUFFER_SIZE];

    // コンソールクリア（ANSIエスケープシーケンス）
    printf("\x1b[2J");

    // 無限ループ（描画更新ループ）
    for (;;) {
        // バッファ初期化
        memset(outputBuffer, ' ', BUFFER_SIZE);
        memset(zBuffer, 0, sizeof(zBuffer));

        // トーラスのパラメータをループ
        for (float theta = 0; theta < 2 * M_PI; theta += 0.03f) {         // トーラスの回転角
            for (float phi = 0; phi < 2 * M_PI; phi += 0.01f) {           // トーラスの周囲角
                // 三角関数事前計算
                float cosTheta = cos(theta), sinTheta = sin(theta);
                float cosPhi = cos(phi), sinPhi = sin(phi);
                float cosA = cos(angleA), sinA = sin(angleA);
                float cosB = cos(angleB), sinB = sin(angleB);

                // トーラス座標計算
                float circleX = cosTheta + 2;  // 円のX位置（オフセット2で中心を外側に）
                float circleY = sinTheta;

                // 3D空間への投影
                float x = circleX * (cosB * cosPhi + sinA * sinB * sinPhi) - circleY * cosA * sinB;
                float y = circleX * (sinB * cosPhi - sinA * cosB * sinPhi) + circleY * cosA * cosB;
                float z = cosA * circleX * sinPhi + circleY * sinA + 5;  // 視点からの奥行き補正
                float ooz = 1 / z;  // 奥行きの逆数（遠近感用）

                // スクリーン座標に変換
                int xp = (int)(SCREEN_WIDTH / 2 + 30 * ooz * x);
                int yp = (int)(SCREEN_HEIGHT / 2 + 15 * ooz * y);
                int index = xp + SCREEN_WIDTH * yp;

                // 輝度計算（ライティング）
                float luminance = (sinPhi * cosTheta * sinB - cosA * cosTheta * sinPhi - sinA * sinTheta + cosB * (cosA * sinTheta - cosTheta * sinA * sinPhi));
                int luminanceIndex = (int)(luminance * 8);
                if (luminanceIndex < 0) luminanceIndex = 0;

                // 表示可能範囲内なら更新
                if (yp >= 0 && yp < SCREEN_HEIGHT && xp >= 0 && xp < SCREEN_WIDTH && ooz > zBuffer[index]) {
                    zBuffer[index] = ooz;
                    outputBuffer[index] = ".,-~:;=!*#$@"[luminanceIndex > 0 ? luminanceIndex : 0];
                }
            }
        }

        // カーソルを左上に戻す（ANSI）
        printf("\x1b[H");

        // バッファ出力
        for (int i = 0; i < BUFFER_SIZE; i++) {
            putchar(i % SCREEN_WIDTH ? outputBuffer[i] : '\n');
        }

        // 回転角更新
        angleA += 0.04f;
        angleB += 0.02f;
    }

    return 0;
}
