/**
 * @file hightlow.c
 * @author 24-326 Ren Toda
 * @brief loop等の練習、数あてゲーム。
 * `mkdir o; gcc hightlow.c -o o/hightlow && ./o/hightlow`
 * でコンパイル->実行をできます。
 * @details
 * @copyright Copyright (c) 2025
 * @date 2025-07-1
 */

#include <stdio.h>   // 標準入出力ライブラリ
#include <stdlib.h>  //乱数のため
#include <time.h>    //UNIX時間取得のため

/**
 * @brief main funciton
 * @return 0 正常終了
 * @return 1 異常終了
 */
int main(void) {
    srand((unsigned int)time(NULL));  // UNIX時間で初期化

    int answer = rand() % 100;  // 答え
    int guess;                  // userの予測
    int iteration = 0;          // 正当な質問数

    printf("乱数ゲームです 0~99で予想してください。\n");

    while(1) {
        guess = -1;
        printf("整数を入力してください(%d回目): ", iteration);
        scanf("%d", &guess);

        if(guess == -1) {
            // 不正な入力（%d以外）
            printf("整数を入力してください（やり直し）\n");
            // 入力バッファをクリア
            int c;
            while((c = getchar()) != '\n' && c != EOF) {
                // 何もしない：バッファを読み飛ばすだけ
            }
            continue;
        }

        if(guess < 0 || guess > 99) {
            printf("0~99で入力してください\n");
            continue;
        }

        if(guess == answer) {
            printf("あたり! 答えは%dでした!", answer);
            break;
        }

        printf("外れ、");
        if(guess < answer) {
            printf("答えはそれより大きいです\n");
        } else if(guess > answer) {
            printf("答えはそれより小さいです\n");
        }

        if(abs(guess - answer) < 10) {
            printf("だけど、答えは近いです!\n");
        }

        iteration++;
    }

    return 0;  // 正常終了
}