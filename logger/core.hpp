#ifndef CORE_HPP
#define CORE_HPP

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
        if (!Utils::ValidationUtils::validate_color_tags_runtime(message)) {
            entry.level = LogLevel::ERROR;
            entry.message = "Invalid color tags: check | pairing";

            // フォーマットして出力
            char error_formatted[512];
            if (formatter) {
                formatter->format(entry, error_formatted,
                                  sizeof(error_formatted));
            } else {
                snprintf(error_formatted, sizeof(error_formatted),
                         "[ERROR] %s:%d : Invalid color tags: check | pairing",
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
                     Utils::StringUtils::get_level_string(level), file, line,
                     message);
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

#endif  // CORE_HPP