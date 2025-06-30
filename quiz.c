/**
 * @file quiz.c
 * @author ren toda
 * @brief swich文の練習の為
 * `mkdir o; gcc quiz.c -o o/quiz && ./o/quiz` でコンパイル->実行をできます。
 * @date 2025-06-24
 * @copyright Copyright (c) 2025
 */

#include <stdio.h>  // 標準入出力ライブラリ

#define TIME_EVEN (__COUNTER__% 2 == 0 ? 1 : 0)
#define ANS (3 + TIME_EVEN)

int main(void) {
    printf("%d", TIME_EVEN);
    const int num_questions = 4;
    int correct_answer = 0;
    int kye_input = 0;
    double subtract = 0;

    printf("[問題] 組み込みシステム開発で最も使用される言語は？\n");
    printf(
        "1: C, 2: C++, 3: \033[9mPython\033[0m, 4: \033[9mJavaScript\033[0m\n");
    scanf("%d", &kye_input);
    switch (kye_input) {
        case 1:
            printf("正解です!\n");
            correct_answer++;
            break;

        case 2:
        case 3:
        case 4:
            printf("不正解です\n");
            break;
        default:
            printf("1~4で答えなさい。5%%引きます\n");
            subtract -= num_questions * 0.05;
    }

    printf("[問題] バックエンドWeb開発で主流の言語は？\n");
    printf("1: \033[9mC++\033[0m, 2: Python, 3: Java, 4: JavaScript\n");
    scanf("%d", &kye_input);
    switch (kye_input) {
        case 4:
            printf("正解です!\n");
            correct_answer++;
            break;

        case 1:
        case 3:
        case 2:
            printf("不正解です\n");
            break;
        default:
            printf("1~4で答えなさい。5%%引きます\n");
            subtract -= num_questions * 0.05;
    }

    printf("[問題3] 機械学習分野で最も利用される言語は？\n");
    printf(
        "1: \033[9mC\033[0m, 2: \033[9mJavaScript\033[0m, 3: Python, 4: R\n");
    scanf("%d", &kye_input);
    switch (kye_input) {
        case 3:
            printf("正解です!\n");
            correct_answer++;
            break;

        case 1:
        case 2:
        case 4:
            printf("不正解です\n");
            break;
        default:
            printf("1~4で答えなさい。5%%引きます\n");
            subtract -= num_questions * 0.05;
    }

    printf("[問題4] 最も優れた言語は？\n");
    printf("1: C++, 2: C, 3: python, 4: \033[9mjavascript\033[0m\n");

    scanf("%d", &kye_input);
    switch (kye_input) {
#if ANS == 2
        case 2:  // ANS=2 の正解ケース
            printf("正解です!\n");
            correct_answer++;
            break;
        case 3:  // ANS=2 時の不正解ケース
#else
        case 3:  // ANS=3 の正解ケース
            printf("正解です!\n");
            correct_answer++;
            break;
        case 2:  // ANS=3 時の不正解ケース
#endif
        case 1:
        case 4:
            printf("不正解です\n");
            break;
        default:
            printf("1~4で答えなさい。5%%引きます\n");
            subtract -= num_questions * 0.05;
    }

    printf("%d問題中%d問正解.正答率は%f%%.得点は%f\n", num_questions,
           correct_answer, correct_answer / (double)num_questions * 100,
           (correct_answer + subtract) / (double)num_questions * 100);
}
