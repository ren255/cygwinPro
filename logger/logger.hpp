/**
 * @file logger.hpp
 * @brief C++用高機能ログライブラリ
 * @details
 * カスタムログレベル、カラー出力、フォーマット機能を提供するヘッダーオンリーライブラリ
 * @author Your Name
 * @date 2024
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>  // exit関数用
#include "utils.h"   // printF関数の定義

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

// 前方宣言
class Logger;
class ConsoleFormatter;

/**
 * @brief ログ機能のプライベート実装
 * @details 内部実装の詳細を隠蔽するための名前空間
 */
namespace LoggerPrivate {

/**
 * @brief カラーコード列挙型
 * @details コンソール出力用のANSIカラーコードを定義
 */
enum class ColorCode {
    RED,     ///< 赤色
    GREEN,   ///< 緑色
    YELLOW,  ///< 黄色
    BLUE,    ///< 青色
    RESET    ///< リセット（デフォルト色）
};

/**
 * @brief カラーコードを取得
 * @param color カラーコード列挙値
 * @return ANSIカラーコード文字列
 */
const char* get_color_code(ColorCode color);

/**
 * @brief ログレベルの文字列表現を取得
 * @param level ログレベル
 * @return ログレベル文字列
 */
const char* get_level_string(LogLevel level);

/**
 * @brief ログレベルに対応するカラーコードを取得
 * @param level ログレベル
 * @return 対応するカラーコード
 */
ColorCode get_level_color(LogLevel level);

/**
 * @brief カラータグを含むメッセージをパース
 * @details "r|text|" 形式のカラータグをANSIカラーコードに変換
 * @param input 入力メッセージ（カラータグ含む）
 * @param output 出力バッファ（ANSIコード変換済み）
 * @param max_len 出力バッファの最大長
 * @throws コンパイルエラー時にエラーを発生
 */
void parse_color_message(const char* input, char* output, int max_len);

/**
 * @brief カラータグの妥当性をチェック（コンパイル時）
 * @details |が偶数個であること、連続していないこと、エスケープ処理を考慮
 * @param input チェック対象の文字列
 * @return true: 妥当, false: 不正
 */
constexpr bool validate_color_tags_constexpr(const char* input);

/**
 * @brief カラータグの妥当性をチェック（実行時）
 * @details |が偶数個であること、連続していないこと、エスケープ処理を考慮
 * @param input チェック対象の文字列
 * @return true: 妥当, false: 不正
 */
bool validate_color_tags(const char* input);

/**
 * @brief フルパスからファイル名のみを抽出
 * @param filepath フルファイルパス
 * @return ファイル名部分のポインタ
 */
const char* extract_filename(const char* filepath);

}  // namespace LoggerPrivate

/**
 * @brief コンソール出力フォーマッタクラス
 * @details ログメッセージの整形とカラー化を担当
 */
class ConsoleFormatter {
   public:
    /**
     * @brief デフォルトコンストラクタ
     */
    ConsoleFormatter() {}

    /**
     * @brief ログメッセージをフォーマット
     * @param level ログレベル
     * @param filename ソースファイル名
     * @param function 関数名
     * @param line 行番号
     * @param message ログメッセージ
     * @param output 出力バッファ
     * @param max_len 出力バッファの最大長
     */
    void format(LogLevel level, const char* filename, const char* function,
                int line, const char* message, char* output, int max_len) {
        // レベル文字列を取得（角括弧内に配置）
        const char* level_str = LoggerPrivate::get_level_string(level);

        // ファイル名のみを取得
        const char* file_only = LoggerPrivate::extract_filename(filename);

        // カラーコードを取得
        const char* color = LoggerPrivate::get_color_code(
            LoggerPrivate::get_level_color(level));
        const char* reset =
            LoggerPrivate::get_color_code(LoggerPrivate::ColorCode::RESET);

        // カラーメッセージをパース
        char colored_msg[256];
        // カラーメッセージの妥当性チェック（実行時エラーで停止）
        if (!LoggerPrivate::validate_color_tags(message)) {
            printF("error!!");
            // const char* prefix = "r|logger error| with ";
            // strcpy(colored_msg, prefix);   // prefixをmessageにコピー
            // strcat(colored_msg, message);  // 退避したメッセージを連結
            // printf(colored_msg);
        }
        LoggerPrivate::parse_color_message(message, colored_msg,
                                           sizeof(colored_msg));

        // レベル部分を8文字にパディング（[LEVEL] + スペース）
        char level_padded[8];
        snprintf(level_padded, sizeof(level_padded), "[%s]%-*s", level_str,
                 (int)(8 - strlen(level_str) - 2), "");

        // 最終フォーマット: [LEVEL]   filename:line        : message
        snprintf(
            output, max_len, "%s%s%s %s:%d%*s : %s", color, level_padded, reset,
            file_only, line,
            (int)(13 - strlen(file_only) - snprintf(nullptr, 0, "%d", line)),
            "", colored_msg);
    }
};

/**
 * @brief メインLoggerクラス
 * @details ログ出力の中心となるクラス。シングルトンパターンで実装
 */
class Logger {
   private:
    LogLevel current_level;      ///< 現在のログレベル
    ConsoleFormatter formatter;  ///< フォーマッタインスタンス

   public:
    /**
     * @brief デフォルトコンストラクタ
     * @details 初期ログレベルをINFOに設定
     */
    Logger() : current_level(LogLevel::INFO) {}

    /**
     * @brief グローバルログレベルを設定
     * @param level 設定するログレベル
     */
    void global_lvl_set(LogLevel level) { current_level = level; }

    /**
     * @brief 現在のログレベルを取得
     * @return 現在のログレベル
     */
    LogLevel get_level() const { return current_level; }

    /**
     * @brief DEBUGレベルログを出力
     * @param file ソースファイル名
     * @param func 関数名
     * @param line 行番号
     * @param fmt フォーマット文字列
     * @param ... 可変引数
     */
    void debug(const char* file, const char* func, int line, const char* fmt,
               ...) {
        if (current_level <= LogLevel::DEBUG) {
            char message[256];
            va_list args;
            va_start(args, fmt);
            vsnprintf(message, sizeof(message), fmt, args);
            va_end(args);

            char output[512];
            formatter.format(LogLevel::DEBUG, file, func, line, message, output,
                             sizeof(output));
            printF("%s\n", output);
        }
    }

    /**
     * @brief INFOレベルログを出力
     * @param file ソースファイル名
     * @param func 関数名
     * @param line 行番号
     * @param fmt フォーマット文字列
     * @param ... 可変引数
     */
    void info(const char* file, const char* func, int line, const char* fmt,
              ...) {
        if (current_level <= LogLevel::INFO) {
            char message[256];
            va_list args;
            va_start(args, fmt);
            vsnprintf(message, sizeof(message), fmt, args);
            va_end(args);

            char output[512];
            formatter.format(LogLevel::INFO, file, func, line, message, output,
                             sizeof(output));
            printF("%s\n", output);
        }
    }

    /**
     * @brief WARNINGレベルログを出力
     * @param file ソースファイル名
     * @param func 関数名
     * @param line 行番号
     * @param fmt フォーマット文字列
     * @param ... 可変引数
     */
    void warning(const char* file, const char* func, int line, const char* fmt,
                 ...) {
        if (current_level <= LogLevel::WARNING) {
            char message[256];
            va_list args;
            va_start(args, fmt);
            vsnprintf(message, sizeof(message), fmt, args);
            va_end(args);

            char output[512];
            formatter.format(LogLevel::WARNING, file, func, line, message,
                             output, sizeof(output));
            printF("%s\n", output);
        }
    }

    /**
     * @brief ERRORレベルログを出力
     * @param file ソースファイル名
     * @param func 関数名
     * @param line 行番号
     * @param fmt フォーマット文字列
     * @param ... 可変引数
     */
    void error(const char* file, const char* func, int line, const char* fmt,
               ...) {
        if (current_level <= LogLevel::ERROR) {
            char message[256];
            va_list args;
            va_start(args, fmt);
            vsnprintf(message, sizeof(message), fmt, args);
            va_end(args);

            char output[512];
            formatter.format(LogLevel::ERROR, file, func, line, message, output,
                             sizeof(output));
            printF("%s\n", output);
        }
    }
};

/**
 * @brief グローバルLoggerインスタンスを取得
 * @details シングルトンパターンでLoggerインスタンスを提供
 * @return Loggerインスタンスへの参照
 */
Logger& get_logger() {
    static Logger instance;
    return instance;
}

/**
 * @brief DEBUGログ出力マクロ（カラータグ検証付き）
 * @param fmt フォーマット文字列
 * @param ... 可変引数
 */
#define LOG_DEBUG(fmt, ...)                                              \
    do {                                                                 \
        static_assert(LoggerPrivate::validate_color_tags_constexpr(fmt), \
                      "Invalid color tags in LOG_DEBUG: Check for even " \
                      "number of | and no consecutive ||");              \
        get_logger().debug(__FILE__, __FUNCTION__, __LINE__, fmt,        \
                           ##__VA_ARGS__);                               \
    } while (0)

/**
 * @brief INFOログ出力マクロ（カラータグ検証付き）
 * @param fmt フォーマット文字列
 * @param ... 可変引数
 */
#define LOG_INFO(fmt, ...)                                                     \
    do {                                                                       \
        static_assert(LoggerPrivate::validate_color_tags_constexpr(fmt),       \
                      "Invalid color tags in LOG_INFO: Check for even number " \
                      "of | and no consecutive ||");                           \
        get_logger().info(__FILE__, __FUNCTION__, __LINE__, fmt,               \
                          ##__VA_ARGS__);                                      \
    } while (0)

/**
 * @brief WARNINGログ出力マクロ（カラータグ検証付き）
 * @param fmt フォーマット文字列
 * @param ... 可変引数
 */
#define LOG_WARNING(fmt, ...)                                              \
    do {                                                                   \
        static_assert(LoggerPrivate::validate_color_tags_constexpr(fmt),   \
                      "Invalid color tags in LOG_WARNING: Check for even " \
                      "number of | and no consecutive ||");                \
        get_logger().warning(__FILE__, __FUNCTION__, __LINE__, fmt,        \
                             ##__VA_ARGS__);                               \
    } while (0)

/**
 * @brief ERRORログ出力マクロ（カラータグ検証付き）
 * @param fmt フォーマット文字列
 * @param ... 可変引数
 */
#define LOG_ERROR(fmt, ...)                                              \
    do {                                                                 \
        static_assert(LoggerPrivate::validate_color_tags_constexpr(fmt), \
                      "Invalid color tags in LOG_ERROR: Check for even " \
                      "number of | and no consecutive ||");              \
        get_logger().error(__FILE__, __FUNCTION__, __LINE__, fmt,        \
                           ##__VA_ARGS__);                               \
    } while (0)

// プライベート名前空間の実装
namespace LoggerPrivate {

const char* get_color_code(ColorCode color) {
    switch (color) {
        case ColorCode::RED:
            return "\033[31m";
        case ColorCode::GREEN:
            return "\033[32m";
        case ColorCode::YELLOW:
            return "\033[33m";
        case ColorCode::BLUE:
            return "\033[34m";
        case ColorCode::RESET:
            return "\033[0m";
        default:
            return "";
    }
}

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

ColorCode get_level_color(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:
            return ColorCode::BLUE;
        case LogLevel::INFO:
            return ColorCode::GREEN;
        case LogLevel::WARNING:
            return ColorCode::YELLOW;
        case LogLevel::ERROR:
            return ColorCode::RED;
        default:
            return ColorCode::RESET;
    }
}

constexpr bool validate_color_tags_constexpr(const char* input) {
    int pipe_count = 0;
    int input_len = 0;
    bool last_was_pipe = false;

    // 文字列長を計算
    while (input[input_len] != '\0') {
        input_len++;
    }

    for (int i = 0; i < input_len; i++) {
        if (input[i] == '\\' && i + 1 < input_len) {
            // エスケープシーケンス - 次の文字をスキップ
            i++;
            last_was_pipe = false;
            continue;
        }

        if (input[i] == '|') {
            // 連続する|をチェック
            if (last_was_pipe) {
                return false;  // 連続する|はエラー
            }
            pipe_count++;
            last_was_pipe = true;
        } else {
            last_was_pipe = false;
        }
    }

    // |が偶数個かチェック
    return (pipe_count % 2 == 0);
}

bool validate_color_tags(const char* input) {
    int pipe_count = 0;
    int input_len = strlen(input);
    bool last_was_pipe = false;

    for (int i = 0; i < input_len; i++) {
        if (input[i] == '\\' && i + 1 < input_len) {
            // エスケープシーケンス - 次の文字をスキップ
            i++;
            last_was_pipe = false;
            continue;
        }

        if (input[i] == '|') {
            // 連続する|をチェック
            if (last_was_pipe) {
                return false;  // 連続する|はエラー
            }
            pipe_count++;
            last_was_pipe = true;
        } else {
            last_was_pipe = false;
        }
    }

    // |が偶数個かチェック
    return (pipe_count % 2 == 0);
}

void parse_color_message(const char* input, char* output, int max_len) {
    int in_pos = 0, out_pos = 0;
    int input_len = strlen(input);

    while (in_pos < input_len && out_pos < max_len - 1) {
        // エスケープシーケンスの処理
        if (input[in_pos] == '\\' && in_pos + 1 < input_len) {
            // 次の文字をそのまま出力（エスケープ）
            in_pos++;  // \をスキップ
            output[out_pos++] = input[in_pos++];
            continue;
        }

        if (input[in_pos] == 'r' && in_pos + 1 < input_len &&
            input[in_pos + 1] == '|') {
            // 赤色開始
            const char* red_code = get_color_code(ColorCode::RED);
            int code_len = strlen(red_code);
            if (out_pos + code_len < max_len - 1) {
                strcpy(output + out_pos, red_code);
                out_pos += code_len;
            }
            in_pos += 2;  // "r|"をスキップ
        } else if (input[in_pos] == 'g' && in_pos + 1 < input_len &&
                   input[in_pos + 1] == '|') {
            // 緑色開始
            const char* green_code = get_color_code(ColorCode::GREEN);
            int code_len = strlen(green_code);
            if (out_pos + code_len < max_len - 1) {
                strcpy(output + out_pos, green_code);
                out_pos += code_len;
            }
            in_pos += 2;  // "g|"をスキップ
        } else if (input[in_pos] == 'y' && in_pos + 1 < input_len &&
                   input[in_pos + 1] == '|') {
            // 黄色開始
            const char* yellow_code = get_color_code(ColorCode::YELLOW);
            int code_len = strlen(yellow_code);
            if (out_pos + code_len < max_len - 1) {
                strcpy(output + out_pos, yellow_code);
                out_pos += code_len;
            }
            in_pos += 2;  // "y|"をスキップ
        } else if (input[in_pos] == 'b' && in_pos + 1 < input_len &&
                   input[in_pos + 1] == '|') {
            // 青色開始
            const char* blue_code = get_color_code(ColorCode::BLUE);
            int code_len = strlen(blue_code);
            if (out_pos + code_len < max_len - 1) {
                strcpy(output + out_pos, blue_code);
                out_pos += code_len;
            }
            in_pos += 2;  // "b|"をスキップ
        } else if (input[in_pos] == '|') {
            // 色終了
            const char* reset_code = get_color_code(ColorCode::RESET);
            int code_len = strlen(reset_code);
            if (out_pos + code_len < max_len - 1) {
                strcpy(output + out_pos, reset_code);
                out_pos += code_len;
            }
            in_pos++;
        } else {
            // 通常文字
            output[out_pos++] = input[in_pos++];
        }
    }
    output[out_pos] = '\0';
}

const char* extract_filename(const char* filepath) {
    const char* filename = strrchr(filepath, '/');
    if (filename == nullptr) {
        filename = strrchr(filepath, '\\');
    }
    return (filename != nullptr) ? filename + 1 : filepath;
}

}  // namespace LoggerPrivate

#endif  // LOGGER_HPP