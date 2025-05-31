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
 * @brief JSON形式フォーマッタ
 * @details 構造化ログとしてJSON形式で出力
 */
class JsonFormatter : public IFormatter {
   public:
    /**
     * @brief ログエントリをJSON形式でフォーマット
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

        // JSONではカラータグを除去してクリーンなメッセージにする
        char clean_message[256];
        Utils::ColorHelper::strip_color_tags(entry.message, clean_message,
                                             sizeof(clean_message));

        // JSON形式でフォーマット（カラータグなし）
        snprintf(output, max_len,
                 R"({"level":"%s","file":"%s","line":%d,"message":"%s"})",
                 level_str, filename, entry.line, clean_message);
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

/**
 * @brief CSVフォーマッタ
 * @details CSV形式でログを出力（分析用途）
 */
class CsvFormatter : public IFormatter {
   public:
    /**
     * @brief ログエントリをCSV形式でフォーマット
     * @param entry ログエントリ
     * @param output 出力バッファ
     * @param max_len 最大長
     */
    void format(const LogEntry& entry, char* output, int max_len) override {
        const char* level_str =
            Utils::StringUtils::get_level_string(entry.level);
        const char* filename =
            Utils::StringUtils::extract_filename(entry.filename);

        // CSVではカラータグを除去し、ダブルクォートをエスケープ
        char csv_message[256];
        Utils::ColorHelper::strip_color_tags(entry.message, csv_message,
                                             sizeof(csv_message));

        // ダブルクォートのエスケープ処理（簡易版）
        char escaped_message[256];
        int in_pos = 0, out_pos = 0;
        while (csv_message[in_pos] && out_pos < sizeof(escaped_message) - 2) {
            if (csv_message[in_pos] == '"') {
                escaped_message[out_pos++] = '"';
                escaped_message[out_pos++] = '"';  // ダブルクォートをエスケープ
            } else {
                escaped_message[out_pos++] = csv_message[in_pos];
            }
            in_pos++;
        }
        escaped_message[out_pos] = '\0';

        // CSV形式: level,file,line,message
        snprintf(output, max_len, R"("%s","%s",%d,"%s")", level_str, filename,
                 entry.line, escaped_message);
    }
};

/**
 * @brief XMLフォーマッタ
 * @details XML形式でログを出力
 */
class XmlFormatter : public IFormatter {
   public:
    /**
     * @brief ログエントリをXML形式でフォーマット
     * @param entry ログエントリ
     * @param output 出力バッファ
     * @param max_len 最大長
     */
    void format(const LogEntry& entry, char* output, int max_len) override {
        const char* level_str =
            Utils::StringUtils::get_level_string(entry.level);
        const char* filename =
            Utils::StringUtils::extract_filename(entry.filename);

        // XMLではカラータグを除去し、XMLエスケープ処理
        char xml_message[256];
        Utils::ColorHelper::strip_color_tags(entry.message, xml_message,
                                             sizeof(xml_message));

        // XMLエスケープ処理（基本的な文字のみ）
        char escaped_message[256];
        int in_pos = 0, out_pos = 0;
        while (xml_message[in_pos] && out_pos < sizeof(escaped_message) - 6) {
            switch (xml_message[in_pos]) {
                case '<':
                    strcpy(escaped_message + out_pos, "&lt;");
                    out_pos += 4;
                    break;
                case '>':
                    strcpy(escaped_message + out_pos, "&gt;");
                    out_pos += 4;
                    break;
                case '&':
                    strcpy(escaped_message + out_pos, "&amp;");
                    out_pos += 5;
                    break;
                case '"':
                    strcpy(escaped_message + out_pos, "&quot;");
                    out_pos += 6;
                    break;
                default:
                    escaped_message[out_pos++] = xml_message[in_pos];
                    break;
            }
            in_pos++;
        }
        escaped_message[out_pos] = '\0';

        // XML形式
        snprintf(output, max_len,
                 R"(<log level="%s" file="%s" line="%d">%s</log>)", level_str,
                 filename, entry.line, escaped_message);
    }
};

}  // namespace Formatters
}  // namespace logger
#endif  // LOG_FORMATTERS_HPP