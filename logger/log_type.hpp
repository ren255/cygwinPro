#ifndef LOG_TYPE_HPP
#define LOG_TYPE_HPP


namespace logger {
    class Logger;
    class LoggerConfig;
}  // namespace logger

/**
 * @brief グローバルLoggerインスタンスを取得
 * @details シングルトンパターンでLoggerインスタンスを提供
 * @return Loggerインスタンスへの参照
 */
logger::Logger& get_logger();
/**
 * @brief ロガー設定を取得
 * @return LoggerConfigインスタンスへの参照
 */
logger::LoggerConfig& get_logger_config();

namespace logger {
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
 * @brief ANSIカラーコードマップ
 */
static const std::map<char, const char*> ANSI_COLORS = {{'r', "\033[31m"},
                                                        {'g', "\033[32m"},
                                                        {'y', "\033[33m"},
                                                        {'b', "\033[34m"},
                                                        {'d', "\033[0m"}};

/**
 * @brief ログレベル用カラーマップ
 */
static const std::map<LogLevel, const char*> LEVEL_COLORS = {
    {LogLevel::DEBUG, ANSI_COLORS.at('b')},    // Blue
    {LogLevel::INFO, ANSI_COLORS.at('g')},     // Green
    {LogLevel::WARNING, ANSI_COLORS.at('y')},  // Yellow
    {LogLevel::ERROR, ANSI_COLORS.at('r')}     // Red
};

/**
 * @brief リセットコード
 */
static const char* RESET = ANSI_COLORS.at('d');
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
}  // namespace logger

#endif  // LOG_TYPE_HPP