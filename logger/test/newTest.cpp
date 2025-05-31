/**
 * @file logger_level_examples.cpp
 * @brief ログレベル設定の各種パターン
 */

#include "utils.h"
#include "logger.hpp"

void test_basic_level_setting() {
    LOG_INFO("=== Basic Level Setting Test ===");
    
    // 方法1: Loggerインスタンス経由で設定
    get_logger().set_level(LogLevel::DEBUG);
    LOG_DEBUG("DEBUG level enabled - this should be visible");
    LOG_INFO("INFO level message");
    
    // レベルを上げる
    get_logger().set_level(LogLevel::WARNING);
    LOG_DEBUG("DEBUG message - should NOT be visible");
    LOG_INFO("INFO message - should NOT be visible");
    LOG_WARNING("WARNING message - should be visible");
    LOG_ERROR("ERROR message - should be visible");
}

void test_config_based_setting() {
    LOG_INFO("=== Config-based Level Setting Test ===");
    
    // 方法2: LoggerConfig経由で設定
    LoggerConfig& config = get_logger_config();
    config.set_min_level(LogLevel::INFO);
    
    // 注意: ConfigとLoggerは現在の実装では独立している
    // Loggerに設定を反映させる必要がある
    get_logger().set_level(config.min_level);
    
    LOG_DEBUG("DEBUG - should NOT be visible (level=INFO)");
    LOG_INFO("INFO - should be visible");
    LOG_WARNING("WARNING - should be visible");
}

void test_all_levels() {
    LOG_INFO("=== All Log Levels Test ===");
    
    // 全レベル表示
    get_logger().set_level(LogLevel::DEBUG);
    LOG_DEBUG("Level 0: DEBUG - 開発時の詳細情報");
    LOG_INFO("Level 1: INFO - 一般的な情報");
    LOG_WARNING("Level 2: WARNING - 注意が必要");
    LOG_ERROR("Level 3: ERROR - エラー情報");
}

void test_level_filtering() {
    LOG_INFO("=== Level Filtering Test ===");
    
    // 各レベルでのフィルタリング動作を確認
    LogLevel levels[] = {LogLevel::DEBUG, LogLevel::INFO, LogLevel::WARNING, LogLevel::ERROR};
    const char* level_names[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    
    for (int i = 0; i < 4; i++) {
        LOG_INFO("--- Setting level to %s ---", level_names[i]);
        get_logger().set_level(levels[i]);
        
        LOG_DEBUG("  DEBUG message");
        LOG_INFO("  INFO message");
        LOG_WARNING("  WARNING message");
        LOG_ERROR("  ERROR message");
    }
}

void test_runtime_level_change() {
    LOG_INFO("=== Runtime Level Change Test ===");
    
    // 実行時にレベルを動的に変更
    get_logger().set_level(LogLevel::ERROR);
    LOG_INFO("This INFO should NOT appear");
    LOG_ERROR("This ERROR should appear");
    
    // レベルを下げる
    get_logger().set_level(LogLevel::DEBUG);
    LOG_INFO("Now INFO should appear");
    LOG_DEBUG("And DEBUG should also appear");
}

void demonstrate_level_hierarchy() {
    LOG_INFO("=== Log Level Hierarchy ===");
    LOG_INFO("DEBUG(0) < INFO(1) < WARNING(2) < ERROR(3)");
    LOG_INFO("Setting level to X means only messages with level >= X are shown");
    
    // 階層の実演
    get_logger().set_level(LogLevel::WARNING);
    LOG_INFO("Level set to WARNING");
    LOG_DEBUG("DEBUG: filtered out");
    LOG_INFO("INFO: filtered out");
    LOG_WARNING("WARNING: shown");
    LOG_ERROR("ERROR: shown");
}

// ユーティリティ関数: 現在のレベルを表示
void show_current_level() {
    LogLevel current = get_logger().get_level();
    const char* level_name = "";
    
    switch (current) {
        case LogLevel::DEBUG:   level_name = "DEBUG"; break;
        case LogLevel::INFO:    level_name = "INFO"; break;
        case LogLevel::WARNING: level_name = "WARNING"; break;
        case LogLevel::ERROR:   level_name = "ERROR"; break;
    }
    
    LOG_INFO("Current log level: %s", level_name);
}

int main() {
    LOG_INFO("Logger Level Configuration Examples");
    LOG_INFO("=====================================");
    
    // 現在のレベルを表示
    show_current_level();
    
    // 各テストを実行
    test_basic_level_setting();
    test_config_based_setting();
    test_all_levels();
    test_level_filtering();
    test_runtime_level_change();
    demonstrate_level_hierarchy();
    
    // 最後に標準レベルに戻す
    get_logger().set_level(LogLevel::INFO);
    LOG_INFO("All tests completed. Level reset to INFO.");
    
    return 0;
}