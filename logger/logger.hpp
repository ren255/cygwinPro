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

#include "log_type.hpp"
#include "log_utils.hpp"
#include "log_writers.hpp"
#include "log_formatters.hpp"
#include "log_core.hpp"

// グローバル関数の実装

/**
 * @brief グローバルLoggerインスタンスを取得
 * @return Loggerインスタンスへの参照
 */
Logger& get_logger() {
    static Logger instance;
    return instance;
}

/**
 * @brief グローバルロガー設定を取得
 * @return LoggerConfigインスタンスへの参照
 */
LoggerConfig& get_logger_config() {
    static LoggerConfig config;
    return config;
}

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