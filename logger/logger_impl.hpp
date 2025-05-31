/**
 * @file logger_impl.hpp
 * @brief ログライブラリのコア実装
 * @details Writer、Utils、Loggerクラスの実装を提供
 * @author ren255
 */

#ifndef LOGGER_IMPL_HPP
#define LOGGER_IMPL_HPP

#include "logger.hpp"
#include "formatters.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <memory>

/**
 * @brief 出力機能を提供する名前空間
 */
namespace Writers {

/**
 * @brief 出力インターフェース
 * @details 全ての出力先が実装すべき基底クラス
 */
class IWriter {
   public:
    virtual ~IWriter() = default;

    /**
     * @brief メッセージを出力
     * @param message 出力するメッセージ
     */
    virtual void write(const char* message) = 0;
};

/**
 * @brief コンソール出力クラス
 * @details 標準出力へのメッセージ出力を担当
 */
class ConsoleWriter : public IWriter {
   public:
    /**
     * @brief コンソールにメッセージを出力
     * @param message 出力するメッセージ
     */
    void write(const char* message) override { printF("%s\n", message); }
};

/**
 * @brief バッファ付き出力クラス
 * @details メッセージをバッファリングして出力
 */
class BufferedWriter : public IWriter {
   private:
    static const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    int buffer_pos = 0;
    std::unique_ptr<IWriter> underlying_writer;

   public:
    /**
     * @brief コンストラクタ
     * @param writer 実際の出力を行うライター
     */
    explicit BufferedWriter(std::unique_ptr<IWriter> writer)
        : underlying_writer(std::move(writer)) {
        buffer[0] = '\0';
    }

    /**
     * @brief バッファにメッセージを追加
     * @param message 追加するメッセージ
     */
    void write(const char* message) override {
        int msg_len = strlen(message);

        // バッファに余裕がない場合はフラッシュ
        if (buffer_pos + msg_len + 1 >= BUFFER_SIZE) {
            flush();
        }

        // バッファに追加
        strncpy(buffer + buffer_pos, message, BUFFER_SIZE - buffer_pos - 1);
        buffer_pos += msg_len;
        buffer[buffer_pos] = '\0';

        // 改行文字が含まれていたらフラッシュ
        if (strchr(message, '\n') != nullptr) {
            flush();
        }
    }

    /**
     * @brief バッファの内容を出力してクリア
     */
    void flush() {
        if (buffer_pos > 0 && underlying_writer) {
            underlying_writer->write(buffer);
            buffer_pos = 0;
            buffer[0] = '\0';
        }
    }

    /**
     * @brief デストラクタ - バッファをフラッシュ
     */
    ~BufferedWriter() { flush(); }
};

}  // namespace Writers

/**
 * @brief ユーティリティ機能を提供する名前空間
 */
namespace Utils {

/**
 * @brief カラータグの妥当性をチェック（コンパイル時）
 * @param input チェック対象の文字列
 * @return true: 妥当, false: 不正
 * @details
 * |が偶数個であること、連続していないことを検証（||はエスケープとして扱う）
 */
constexpr bool validate_color_tags_compile_time(const char* input) {
    int pipe_count = 0;
    int input_len = 0;
    bool last_was_pipe = false;

    // 文字列長を計算
    while (input[input_len] != '\0') {
        input_len++;
    }

    for (int i = 0; i < input_len; i++) {
        if (input[i] == '|') {
            if (i + 1 < input_len && input[i + 1] == '|') {
                // ||はエスケープされた|として扱う - スキップ
                i++;  // 次の|もスキップ
                last_was_pipe = false;
                continue;
            }

            // 単一の|
            pipe_count++;
            last_was_pipe = true;
        } else {
            last_was_pipe = false;
        }
    }

    // |が偶数個かチェック
    return (pipe_count % 2 == 0);
}

/**
 * @brief カラータグの妥当性をチェック（実行時）
 * @param input チェック対象の文字列
 * @return true: 妥当, false: 不正
 */
bool validate_color_tags_runtime(const char* input) {
    int pipe_count = 0;
    int input_len = strlen(input);
    bool last_was_pipe = false;

    for (int i = 0; i < input_len; i++) {
        if (input[i] == '|') {
            if (i + 1 < input_len && input[i + 1] == '|') {
                // ||はエスケープされた|として扱う - スキップ
                i++;  // 次の|もスキップ
                last_was_pipe = false;
                continue;
            }

            // 単一の|
            pipe_count++;
            last_was_pipe = true;
        } else {
            last_was_pipe = false;
        }
    }

    // |が偶数個かチェック
    return (pipe_count % 2 == 0);
}

}  // namespace Utils

/**
 * @brief メインLoggerクラス
 * @details ログ出力の統括管理を行うオーケストレータ
 */
class Logger {
   private:
    LogLevel current_level;
    std::unique_ptr<Formatters::IFormatter> formatter;
    std::unique_ptr<Writers::IWriter> writer;

    /**
     * @brief 内部ログ出力処理
     * @param level ログレベル
     * @param file ファイル名
     * @param line 行番号
     * @param message メッセージ
     */
    void log_internal(LogLevel level, const char* file, int line,
                      const char* message) {
        if (level < current_level) {
            return;  // レベルが低い場合は出力しない
        }

        // LogEntry作成
        LogEntry entry;
        entry.level = level;
        entry.filename = file;
        entry.line = line;
        entry.function = nullptr;  // 将来実装

        // 実行時バリデーション
        if (!Utils::validate_color_tags_runtime(message)) {
            entry.level = LogLevel::ERROR;
            entry.message = "Invalid color tags: check || pairing";

            // フォーマットして出力
            char error_formatted[512];
            if (formatter) {
                formatter->format(entry, error_formatted,
                                  sizeof(error_formatted));
            } else {
                snprintf(error_formatted, sizeof(error_formatted),
                         "[ERROR] %s:%d : Invalid color tags: check || pairing",
                         file, line);
            }

            if (writer) {
                writer->write(error_formatted);
            }
            return;  // エラーメッセージのみ出力して元のメッセージは出力しない
        }

        entry.message = message;

        // フォーマットして出力
        char formatted_message[512];
        if (formatter) {
            formatter->format(entry, formatted_message,
                              sizeof(formatted_message));
        } else {
            snprintf(formatted_message, sizeof(formatted_message),
                     "[%s] %s:%d : %s",
                     level == LogLevel::DEBUG     ? "DEBUG"
                     : level == LogLevel::INFO    ? "INFO"
                     : level == LogLevel::WARNING ? "WARN"
                                                  : "ERROR",
                     file, line, message);
        }

        if (writer) {
            writer->write(formatted_message);
        }
    }

   public:
    /**
     * @brief デフォルトコンストラクタ
     */
    Logger() : current_level(LogLevel::INFO) {
        // デフォルトの設定
        formatter = std::make_unique<Formatters::ConsoleFormatter>(true);
        writer = std::make_unique<Writers::ConsoleWriter>();
    }

    /**
     * @brief 最小ログレベルを設定
     * @param level 設定するログレベル
     */
    void set_level(LogLevel level) { current_level = level; }

    /**
     * @brief 現在のログレベルを取得
     * @return 現在のログレベル
     */
    LogLevel get_level() const { return current_level; }

    /**
     * @brief フォーマッタを設定
     * @param new_formatter 新しいフォーマッタ
     */
    void set_formatter(std::unique_ptr<Formatters::IFormatter> new_formatter) {
        formatter = std::move(new_formatter);
    }

    /**
     * @brief ライターを設定
     * @param new_writer 新しいライター
     */
    void set_writer(std::unique_ptr<Writers::IWriter> new_writer) {
        writer = std::move(new_writer);
    }

    /**
     * @brief DEBUGレベルログを出力
     * @param file ファイル名
     * @param line 行番号
     * @param fmt フォーマット文字列
     * @param ... 可変引数
     */
    void debug(const char* file, int line, const char* fmt, ...) {
        char message[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(message, sizeof(message), fmt, args);
        va_end(args);

        log_internal(LogLevel::DEBUG, file, line, message);
    }

    /**
     * @brief INFOレベルログを出力
     * @param file ファイル名
     * @param line 行番号
     * @param fmt フォーマット文字列
     * @param ... 可変引数
     */
    void info(const char* file, int line, const char* fmt, ...) {
        char message[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(message, sizeof(message), fmt, args);
        va_end(args);

        log_internal(LogLevel::INFO, file, line, message);
    }

    /**
     * @brief WARNINGレベルログを出力
     * @param file ファイル名
     * @param line 行番号
     * @param fmt フォーマット文字列
     * @param ... 可変引数
     */
    void warning(const char* file, int line, const char* fmt, ...) {
        char message[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(message, sizeof(message), fmt, args);
        va_end(args);

        log_internal(LogLevel::WARNING, file, line, message);
    }

    /**
     * @brief ERRORレベルログを出力
     * @param file ファイル名
     * @param line 行番号
     * @param fmt フォーマット文字列
     * @param ... 可変引数
     */
    void error(const char* file, int line, const char* fmt, ...) {
        char message[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(message, sizeof(message), fmt, args);
        va_end(args);

        log_internal(LogLevel::ERROR, file, line, message);
    }
};

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

#endif  // LOGGER_IMPL_HPP