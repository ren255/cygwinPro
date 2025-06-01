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
 * @brief デフォルトLogger取得（コンソール出力）
 * @return デフォルト設定のLoggerインスタンス
 */
logger::Logger& get_logger() {
    static std::once_flag flag;
    static std::unique_ptr<Logger> instance;

    std::call_once(flag, []() {
        instance = std::make_unique<Logger>(
            std::make_unique<Formatters::ConsoleFormatter>(true),
            std::make_unique<Writers::ConsoleWriter>());
    });

    return *instance;
}

/**
 * @brief DEBUGログ出力マクロ（カラータグ検証付き）
 * @param fmt フォーマット文字列
 * @param ... 可変引数
 */
#define LOG_DEBUG(fmt, ...)                                                 \
    do {                                                                    \
        static_assert(logger::Utils::ValidationUtils::check_colors_ct(fmt), \
                      "Invalid color tags: check | pairing");               \
        get_logger().debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__);         \
    } while (0)

/**
 * @brief INFOログ出力マクロ（カラータグ検証付き）
 * @param fmt フォーマット文字列
 * @param ... 可変引数
 */
#define LOG_INFO(fmt, ...)                                                  \
    do {                                                                    \
        static_assert(logger::Utils::ValidationUtils::check_colors_ct(fmt), \
                      "Invalid color tags: check | pairing");               \
        get_logger().info(__FILE__, __LINE__, fmt, ##__VA_ARGS__);          \
    } while (0)

/**
 * @brief WARNINGログ出力マクロ（カラータグ検証付き）
 * @param fmt フォーマット文字列
 * @param ... 可変引数
 */
#define LOG_WARNING(fmt, ...)                                               \
    do {                                                                    \
        static_assert(logger::Utils::ValidationUtils::check_colors_ct(fmt), \
                      "Invalid color tags: check | pairing");               \
        get_logger().warning(__FILE__, __LINE__, fmt, ##__VA_ARGS__);       \
    } while (0)

/**
 * @brief ERRORログ出力マクロ（カラータグ検証付き）
 * @param fmt フォーマット文字列
 * @param ... 可変引数
 */
#define LOG_ERROR(fmt, ...)                                                 \
    do {                                                                    \
        static_assert(logger::Utils::ValidationUtils::check_colors_ct(fmt), \
                      "Invalid color tags: check | pairing");               \
        get_logger().error(__FILE__, __LINE__, fmt, ##__VA_ARGS__);         \
    } while (0)

#endif  // LOGGER_HPP