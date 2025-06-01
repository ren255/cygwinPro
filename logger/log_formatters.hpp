/**
 * @file formatters.hpp
 * @brief ログフォーマット専用ライブラリ
 * @details 様々な出力形式に対応するフォーマッタクラス群
 * @author ren255
 */

#ifndef LOG_FORMATTERS_HPP
#define LOG_FORMATTERS_HPP

#include "logger.hpp"
#include <cstring>
#include <cstdio>

namespace logger {
/**
 * @brief フォーマット機能を提供する名前空間
 */
namespace Formatters {

/**
 * @brief フォーマッタインターフェース
 * @details 全てのフォーマッタが実装すべき基底クラス
 */
class IFormatter {
   public:
    virtual ~IFormatter() = default;

    /**
     * @brief ログエントリをフォーマット
     * @param entry ログエントリ
     * @param output 出力バッファ
     * @param max_len 出力バッファの最大長
     */
    virtual void format(const LogEntry& entry, char* output, int max_len) = 0;
};

/**
 * @brief コンソール用フォーマッタ
 * @details カラー付きでコンソールに適した形式でフォーマット
 */
class ConsoleFormatter : public IFormatter {
   private:
    bool color_enabled;

   public:
    /**
     * @brief コンストラクタ
     * @param enable_color カラー出力を有効にするか
     */
    explicit ConsoleFormatter(bool enable_color = true)
        : color_enabled(enable_color) {}

    /**
     * @brief ログエントリをコンソール形式でフォーマット
     * @param entry ログエントリ
     * @param output 出力バッファ
     * @param max_len 最大長
     */
    void format(const LogEntry& entry, char* output, int max_len) override {
        // Utils::ColorHelperとUtils::StringUtilsを使用して統一処理
        const char* level_str =
            Utils::StringUtils::get_level_string(entry.level);
        const char* filename =
            Utils::StringUtils::extract_filename(entry.filename);
        const char* color =
            Utils::ColorHelper::get_level_color(entry.level, color_enabled);
        const char* reset = Utils::ColorHelper::get_reset_color(color_enabled);

        // カラーメッセージを解析（統一処理を使用）
        char colored_msg[256];
        Utils::ColorHelper::parse_color_tags(
            entry.message, colored_msg, sizeof(colored_msg), color_enabled);

        // レベル部分をパディング（8文字固定）
        char level_padded[16];
        snprintf(level_padded, sizeof(level_padded), "[%s]%-*s", level_str,
                 (int)(8 - strlen(level_str) - 2), "");

        // 最終フォーマット: [LEVEL]   filename:line        : message
        snprintf(output, max_len, "%s%s%s %s:%d%*s : %s", color, level_padded,
                 reset, filename, entry.line,
                 (int)(13 - strlen(filename) -
                       snprintf(nullptr, 0, "%d", entry.line)),
                 "", colored_msg);
    }
};

/**
 * @brief プレーンテキストフォーマッタ
 * @details シンプルなテキスト形式（カラーなし）
 */
class PlainFormatter : public IFormatter {
   public:
    /**
     * @brief ログエントリをプレーンテキスト形式でフォーマット
     * @param entry ログエントリ
     * @param output 出力バッファ
     * @param max_len 最大長
     */
    void format(const LogEntry& entry, char* output, int max_len) override {
        // 統一処理を使用
        const char* level_str =
            Utils::StringUtils::get_level_string(entry.level);
        const char* filename =
            Utils::StringUtils::extract_filename(entry.filename);

        // プレーンテキストではカラータグを除去
        char plain_message[256];
        Utils::ColorHelper::strip_color_tags(entry.message, plain_message,
                                             sizeof(plain_message));

        // シンプルなフォーマット（カラーなし）
        snprintf(output, max_len, "[%s] %s:%d : %s", level_str, filename,
                 entry.line, plain_message);
    }
};


}  // namespace Formatters
}  // namespace logger
#endif  // LOG_FORMATTERS_HPP