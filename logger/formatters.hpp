/**
 * @file formatters.hpp
 * @brief ログフォーマット専用ライブラリ
 * @details 様々な出力形式に対応するフォーマッタクラス群
 * @author ren255
 */

#ifndef FORMATTERS_HPP
#define FORMATTERS_HPP

#include "logger.hpp"
#include <cstring>
#include <cstdio>

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

    /**
     * @brief ログレベルに応じたカラーコードを取得
     * @param level ログレベル
     * @return ANSIカラーコード
     */
    const char* get_level_color(LogLevel level) {
        if (!color_enabled) return "";

        switch (level) {
            case LogLevel::DEBUG:
                return "\033[34m";  // Blue
            case LogLevel::INFO:
                return "\033[32m";  // Green
            case LogLevel::WARNING:
                return "\033[33m";  // Yellow
            case LogLevel::ERROR:
                return "\033[31m";  // Red
            default:
                return "";
        }
    }

    /**
     * @brief カラーリセットコードを取得
     * @return ANSIリセットコード
     */
    const char* get_reset_color() { return color_enabled ? "\033[0m" : ""; }

    /**
     * @brief ログレベル文字列を取得
     * @param level ログレベル
     * @return レベル文字列
     */
    const char* get_level_string(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG:
                return "DEBUG";
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARNING:
                return "WARN";
            case LogLevel::ERROR:
                return "ERROR";
            default:
                return "UNKNOWN";
        }
    }

    /**
     * @brief ファイルパスからファイル名のみを抽出
     * @param filepath フルパス
     * @return ファイル名部分
     */
    const char* extract_filename(const char* filepath) {
        const char* filename = strrchr(filepath, '/');
        if (filename == nullptr) {
            filename = strrchr(filepath, '\\');
        }
        return (filename != nullptr) ? filename + 1 : filepath;
    }

    /**
     * @brief カラータグ付きメッセージを解析してANSIコードに変換
     * @param input 入力メッセージ
     * @param output 出力バッファ
     * @param max_len 最大長
     */
    void parse_color_message(const char* input, char* output, int max_len) {
        int in_pos = 0, out_pos = 0;
        int input_len = strlen(input);

        while (in_pos < input_len && out_pos < max_len - 1) {
            // ||はエスケープされたリテラル|として処理
            if (input[in_pos] == '|' && in_pos + 1 < input_len &&
                input[in_pos + 1] == '|') {
                output[out_pos++] = '|';  // リテラル|を出力
                in_pos += 2;              // ||をスキップ
                continue;
            }

            // カラータグ開始処理 (x|形式)
            if (color_enabled && input[in_pos] != '|' &&
                in_pos + 1 < input_len && input[in_pos + 1] == '|') {
                const char* color_code = "";
                switch (input[in_pos]) {
                    case 'r':
                        color_code = "\033[31m";
                        break;  // Red
                    case 'g':
                        color_code = "\033[32m";
                        break;  // Green
                    case 'y':
                        color_code = "\033[33m";
                        break;  // Yellow
                    case 'b':
                        color_code = "\033[34m";
                        break;  // Blue
                    case 'd':   // default色としてリセット
                        color_code = "\033[0m";
                        break;
                }

                if (*color_code) {
                    int code_len = strlen(color_code);
                    if (out_pos + code_len < max_len - 1) {
                        strcpy(output + out_pos, color_code);
                        out_pos += code_len;
                    }
                    in_pos += 2;  // "x|" をスキップ
                    continue;
                }
            }

            // カラー終了タグ (単独の|)
            if (input[in_pos] == '|') {
                if (color_enabled) {
                    const char* reset = "\033[0m";
                    int reset_len = strlen(reset);
                    if (out_pos + reset_len < max_len - 1) {
                        strcpy(output + out_pos, reset);
                        out_pos += reset_len;
                    }
                }
                in_pos++;
            } else {
                // 通常文字
                output[out_pos++] = input[in_pos++];
            }
        }
        output[out_pos] = '\0';
    }

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
        const char* level_str = get_level_string(entry.level);
        const char* filename = extract_filename(entry.filename);
        const char* color = get_level_color(entry.level);
        const char* reset = get_reset_color();

        // カラーメッセージを解析
        char colored_msg[256];
        parse_color_message(entry.message, colored_msg, sizeof(colored_msg));

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
        const char* level_str = "";
        switch (entry.level) {
            case LogLevel::DEBUG:
                level_str = "DEBUG";
                break;
            case LogLevel::INFO:
                level_str = "INFO";
                break;
            case LogLevel::WARNING:
                level_str = "WARNING";
                break;
            case LogLevel::ERROR:
                level_str = "ERROR";
                break;
        }

        const char* filename = strrchr(entry.filename, '/');
        if (filename == nullptr) {
            filename = strrchr(entry.filename, '\\');
        }
        filename = (filename != nullptr) ? filename + 1 : entry.filename;

        snprintf(output, max_len,
                 R"({"level":"%s","file":"%s","line":%d,"message":"%s"})",
                 level_str, filename, entry.line, entry.message);
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
        const char* level_str = "";
        switch (entry.level) {
            case LogLevel::DEBUG:
                level_str = "DEBUG";
                break;
            case LogLevel::INFO:
                level_str = "INFO";
                break;
            case LogLevel::WARNING:
                level_str = "WARN";
                break;
            case LogLevel::ERROR:
                level_str = "ERROR";
                break;
        }

        const char* filename = strrchr(entry.filename, '/');
        if (filename == nullptr) {
            filename = strrchr(entry.filename, '\\');
        }
        filename = (filename != nullptr) ? filename + 1 : entry.filename;

        snprintf(output, max_len, "[%s] %s:%d : %s", level_str, filename,
                 entry.line, entry.message);
    }
};

}  // namespace Formatters

#endif  // FORMATTERS_HPP