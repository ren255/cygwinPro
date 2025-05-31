#include "utils.h"
#include "logger/logger.hpp"

// サンプル関数群
void test_basic_logging() {
    LOG_INFO("システム初期化開始");
    LOG_DEBUG("デバッグ情報: メモリ確認中...");
    LOG_WARNING("警告: 温度が上昇しています");
    LOG_ERROR("エラー: センサー接続失敗");
}

void test_formatted_logging() {
    int sensor_value = 42;
    float temperature = 25.7;
    const char* status = "正常";

    LOG_INFO("センサー値: %d, 温度: %.1f°C, ステータス: %s", sensor_value,
             temperature, status);

    LOG_DEBUG("メモリ使用量: %d bytes", 1024);
    LOG_WARNING("バッテリー残量: %d%%", 15);
    LOG_ERROR("エラーコード: 0x%04X", 0xDEAD);
}

void test_color_logging() {
    LOG_INFO("g|接続成功| - デバイスが正常に接続されました");
    LOG_WARNING("y|注意:| メモリ使用量が r|80%%| を超えました");
    LOG_ERROR("r|致命的エラー:| システムの再起動が必要です");
    LOG_DEBUG("b|デバッグ:| プロセス g|開始| から r|終了| まで");
}

void test_level_filtering() {
    LOG_INFO("レベルフィルタリングテスト開始");

    // DEBUGレベルに設定
    get_logger().set_level(LogLevel::DEBUG);
    LOG_INFO("現在のレベル: DEBUG - 全てのログが表示されます");
    LOG_DEBUG("DEBUGメッセージ");
    LOG_INFO("INFOメッセージ");
    LOG_WARNING("WARNINGメッセージ");
    LOG_ERROR("ERRORメッセージ");

    // WARNINGレベルに設定
    get_logger().set_level(LogLevel::WARNING);
    LOG_INFO("現在のレベル: WARNING - WARNING以上のみ表示");
    LOG_DEBUG("このDEBUGメッセージは表示されません");
    LOG_INFO("このINFOメッセージは表示されません");
    LOG_WARNING("このWARNINGメッセージは表示されます");
    LOG_ERROR("このERRORメッセージは表示されます");

    // レベルをINFOに戻す
    get_logger().set_level(LogLevel::INFO);
}

void simulate_sensor_reading() {
    static int reading_count = 0;
    static float last_temp = 25.0;

    reading_count++;

    // 温度値をシミュレート
    last_temp += (reading_count % 3 == 0) ? 0.5 : -0.2;

    if (last_temp > 30.0) {
        LOG_WARNING("y|高温警告:| 温度 %.1f°C が閾値を超えています", last_temp);
    } else if (last_temp < 20.0) {
        LOG_WARNING("y|低温警告:| 温度 %.1f°C が閾値を下回っています",
                    last_temp);
    } else {
        LOG_INFO("センサー読み取り #%d: g|%.1f°C| (正常範囲)", reading_count,
                 last_temp);
    }

    if (reading_count % 10 == 0) {
        LOG_INFO("g|統計:| %d回の測定を完了しました", reading_count);
    }

    // エラーシミュレート
    if (reading_count % 15 == 0) {
        LOG_ERROR("r|センサーエラー:| 読み取り失敗 (試行 #%d)", reading_count);
    }
}

void color2test() {
    int senser = 0;
    int temp = 0;
    LOG_INFO("センサー読み取りテスト開始");
    for (int i = 0; i < 30; i++) {
        senser = i % 5;        // 0から4の値をシミュレート
        temp = 20 + (i % 10);  // 20から29の温度をシミュレート

        if (temp > 25) {
            LOG_WARNING("センサー g|#%d|: r|温度 %.1f°C| (非正常範囲)", senser,
                        (float)temp);
        } else {
            LOG_INFO("センサー g|#%d|: g|温度 %.1f°C| (正常範囲)", senser,
                     (float)temp);
        }
    }
}

void mytest() {
    int reading_count = 42;

    printf("=== Color Formatter Edge Case Tests ===\n\n");

    // 1. 正常なケース
    LOG_INFO("g|統計:| %d回の測定を完了しました", reading_count);
    LOG_INFO("y|yellow| 正常な黄色メッセージ");

    // 2. パイプ文字"|"を含む問題のあるケース
    printf("\n--- パイプ文字関連のテスト ---\n");
    LOG_INFO("g|統計|追加のパイプ|複数| %d回", reading_count);
    LOG_INFO("b|情報|パイプ|が|多すぎる|場合| テスト");

    // 3. 不正なカラーコード
    printf("\n--- 不正なカラーコードテスト ---\n");
    LOG_INFO("x|不明な色| このカラーコードは存在しない");
    LOG_INFO("gg|重複| 重複したG");
    LOG_INFO("123|数字| 数字のカラーコード");

    // 4. 空の要素
    printf("\n--- 空要素テスト ---\n");
    LOG_INFO("|空のカラー| メッセージ");

    // 5. 特殊文字を含むケース
    printf("\n--- 特殊文字テスト ---\n");
    LOG_INFO("r|エラー\n改行| 改行文字を含む");
    LOG_INFO("g|統計\t| タブ文字を含む %d", reading_count);
    LOG_INFO("y|警告\"引用符\"| 引用符を含む");
    LOG_INFO("b|情報%%| パーセント文字を含む");

    // 6. 極端に長いケース
    printf("\n--- 長さテスト ---\n");
    LOG_INFO("g|非常に長いタグ名前前前前前前前前前前前前| %d", reading_count);
    char long_msg[1000];
    strcpy(long_msg, "非常に長いメッセージ");
    for (int i = 0; i < 5; i++) {
        strcat(long_msg, "追加追加追加追加追加追加追加追加");
    }
    LOG_INFO("y|長文| %s", long_msg);

    // 7. フォーマット文字列の問題
    printf("\n--- フォーマット文字列テスト ---\n");
    LOG_INFO("r|危険| %s %d %f", "文字列なし");  // 引数不足
    LOG_INFO("g|統計| %%d 回完了");              // パーセントエスケープ

    // 8. ネストしたパイプ構造
    printf("\n--- ネスト構造テスト ---\n");
    LOG_INFO("g|外側|内側|更に内側| ネストしたパイプ");
    LOG_INFO("d|e| 多層ネスト");

    // 9. Unicode文字
    printf("\n--- Unicode文字テスト ---\n");
    LOG_INFO("b|情報🔵| 絵文字を含む %d", reading_count);
    LOG_INFO("g|統計🔢| Unicode文字 %d回", reading_count);

    // 10. バッファオーバーフロー潜在性
    printf("\n--- バッファテスト ---\n");
    char huge_tag[500];
    memset(huge_tag, 'A', 499);
    huge_tag[499] = '\0';
    LOG_INFO("%c|%s| 巨大なタグ", 'r', huge_tag);

    printf("\n=== テスト完了 ===\n");
}

int main() {
    printf("STM32/\\Arduino R4 Logger Library Demo\n");
    printf("=====================================\n");
    // ロガー初期化
    get_logger().set_level(LogLevel::DEBUG);

    printf("=== STM32/Arduino R4 Logger Library Demo ===\n\n");

    LOG_INFO("g|Logger Library| デモ開始");

    // 基本ログテスト
    printf("\n--- 基本ログテスト ---\n");
    test_basic_logging();

    // フォーマット付きログテスト
    printf("\n--- フォーマット付きログテスト ---\n");
    test_formatted_logging();

    // カラーログテスト
    printf("\n--- カラーログテスト ---\n");
    test_color_logging();

    // レベルフィルタリングテスト
    printf("\n--- レベルフィルタリングテスト ---\n");
    test_level_filtering();

    // センサーシミュレーション
    printf("\n--- センサー読み取りシミュレーション ---\n");
    for (int i = 0; i < 20; i++) {
        simulate_sensor_reading();
    }

    LOG_INFO("g|デモ完了|");
    LOG_INFO("");

    color2test();

    LOG_INFO("g|全てのテストが完了しました|");
    mytest();
    LOG_INFO("g|プログラム終了|");

    return 0;
}