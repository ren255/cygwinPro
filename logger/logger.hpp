/**
 * @file logger.hpp
 * @brief C++用高機能ログライブラリ - パブリックインターフェース
 * @details ユーザー向けのメインAPIを提供
 * get_logger().set_level(LogLevel::DEBUG);  // DEBUGレベル以上を表示
 * LOG_DEBUG("デバッグメッセージy|黄色|: %s", "詳細情報");
 * @author ren255
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <map>

class Logger;
class LoggerConfig;

/**
 * @brief グローバルLoggerインスタンスを取得
 * @details シングルトンパターンでLoggerインスタンスを提供
 * @return Loggerインスタンスへの参照
 */
Logger& get_logger();

/**
 * @brief ロガー設定を取得
 * @return LoggerConfigインスタンスへの参照
 */
LoggerConfig& get_logger_config();

/**
 * @brief ログレベル列挙型
 * @details ログメッセージの重要度を定義
 */
enum class LogLevel {
    DEBUG,    ///< デバッグレベル - 詳細な開発情報
    INFO,     ///< 情報レベル - 一般的な情報
    WARNING,  ///< 警告レベル - 注意が必要な状況
    ERROR     ///< エラーレベル - エラー情報
};

/**
 * @brief ログエントリ構造体
 * @details 単一のログメッセージに関する全情報を格納
 */
struct LogEntry {
    LogLevel level;        ///< ログレベル
    const char* filename;  ///< ソースファイル名
    int line;              ///< 行番号
    const char* function;  ///< 関数名（将来用）
    const char* message;   ///< ログメッセージ
    // timestamp_t timestamp; ///< タイムスタンプ（将来実装）
};

/**
 * @brief カラーコードマップ
 * @details カラータグとANSIコードの対応表（一元管理）
 */
namespace ColorMap {
/**
 * @brief カラータグ文字とANSIコードの対応マップ
 */
static const std::map<char, const char*> COLORS = {
    {'r', "\033[31m"},  // Red
    {'g', "\033[32m"},  // Green
    {'y', "\033[33m"},  // Yellow
    {'b', "\033[34m"},  // Blue
    {'d', "\033[0m"}    // Default/Reset
};

/**
 * @brief ログレベル用カラーマップ
 */
static const std::map<LogLevel, const char*> LEVEL_COLORS = {
    {LogLevel::DEBUG, "\033[34m"},    // Blue
    {LogLevel::INFO, "\033[32m"},     // Green
    {LogLevel::WARNING, "\033[33m"},  // Yellow
    {LogLevel::ERROR, "\033[31m"}     // Red
};

/**
 * @brief リセットコード
 */
static const char* RESET = "\033[0m";
}  // namespace ColorMap

/**
 * @brief ロガー設定クラス
 * @details ログ出力の動作を制御する設定を管理
 */
class LoggerConfig {
   public:
    LogLevel min_level = LogLevel::INFO;  ///< 最小出力レベル
    bool color_enabled = true;            ///< カラー出力の有効/無効
    bool show_timestamp = false;          ///< タイムスタンプ表示（将来実装）

    /**
     * @brief 最小ログレベルを設定
     * @param level 設定するログレベル
     */
    void set_min_level(LogLevel level) { min_level = level; }

    /**
     * @brief カラー出力の有効/無効を設定
     * @param enabled カラー出力を有効にするかどうか
     */
    void set_color_enabled(bool enabled) { color_enabled = enabled; }
};



#include "utils.hpp"
#include "writers.hpp"
#include "formatters.hpp"
#include "core.hpp"

/**
 * @brief DEBUGログ出力マクロ（カラータグ検証付き）
 * @param fmt フォーマット文字列
 * @param ... 可変引数
 */
#define LOG_DEBUG(fmt, ...)                                                \
    do {                                                                   \
        static_assert(                                                     \
            Utils::ValidationUtils::validate_color_tags_compile_time(fmt), \
            "Invalid color tags: check | pairing");                        \
        get_logger().debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__);        \
    } while (0)

/**
 * @brief INFOログ出力マクロ（カラータグ検証付き）
 * @param fmt フォーマット文字列
 * @param ... 可変引数
 */
#define LOG_INFO(fmt, ...)                                                 \
    do {                                                                   \
        static_assert(                                                     \
            Utils::ValidationUtils::validate_color_tags_compile_time(fmt), \
            "Invalid color tags: check | pairing");                        \
        get_logger().info(__FILE__, __LINE__, fmt, ##__VA_ARGS__);         \
    } while (0)

/**
 * @brief WARNINGログ出力マクロ（カラータグ検証付き）
 * @param fmt フォーマット文字列
 * @param ... 可変引数
 */
#define LOG_WARNING(fmt, ...)                                              \
    do {                                                                   \
        static_assert(                                                     \
            Utils::ValidationUtils::validate_color_tags_compile_time(fmt), \
            "Invalid color tags: check | pairing");                        \
        get_logger().warning(__FILE__, __LINE__, fmt, ##__VA_ARGS__);      \
    } while (0)

/**
 * @brief ERRORログ出力マクロ（カラータグ検証付き）
 * @param fmt フォーマット文字列
 * @param ... 可変引数
 */
#define LOG_ERROR(fmt, ...)                                                \
    do {                                                                   \
        static_assert(                                                     \
            Utils::ValidationUtils::validate_color_tags_compile_time(fmt), \
            "Invalid color tags: check | pairing");                        \
        get_logger().error(__FILE__, __LINE__, fmt, ##__VA_ARGS__);        \
    } while (0)

#endif  // LOGGER_HPP