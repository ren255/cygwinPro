#ifndef LOG_CORE_HPP
#define LOG_CORE_HPP
namespace logger {
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
                     Utils::StringUtils::get_level_string(level), file, line,
                     message);
        }

        if (writer) {
            writer->write(formatted_message);
        }
    }

   public:
    /**
     * @brief パラメータ付きコンストラクタ
     * @param fmt フォーマッタのユニークポインタ
     * @param wrt ライターのユニークポインタ
     */
    Logger(std::unique_ptr<Formatters::IFormatter> fmt,
           std::unique_ptr<Writers::IWriter> wrt)
        : current_level(LogLevel::INFO),
          formatter(std::move(fmt)),
          writer(std::move(wrt)) {}

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

}  // namespace logger
#endif  // LOG_CORE_HPP