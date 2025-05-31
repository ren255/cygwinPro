#ifndef LOG_UTILS_HPP
#define LOG_UTILS_HPP

/**
 * @brief ユーティリティ機能を提供する名前空間
 */
namespace Utils {

/**
 * @brief カラー処理統合クラス
 * @details カラータグの解析、ANSIコード変換などを一元管理
 */
class ColorHelper {
   public:
    /**
     * @brief ログレベルに応じたカラーコードを取得
     * @param level ログレベル
     * @param color_enabled カラー出力が有効か
     * @return ANSIカラーコード
     */
    static const char* get_level_color(LogLevel level, bool color_enabled) {
        if (!color_enabled) return "";

        auto it = ColorMap::LEVEL_COLORS.find(level);
        return (it != ColorMap::LEVEL_COLORS.end()) ? it->second : "";
    }

    /**
     * @brief カラーリセットコードを取得
     * @param color_enabled カラー出力が有効か
     * @return ANSIリセットコード
     */
    static const char* get_reset_color(bool color_enabled) {
        return color_enabled ? ColorMap::RESET : "";
    }

    /**
     * @brief カラータグ付きメッセージを解析してANSIコードに変換
     * @param input 入力メッセージ
     * @param output 出力バッファ
     * @param max_len 最大長
     * @param color_enabled カラー出力が有効か
     */
    static void parse_color_tags(const char* input, char* output, int max_len,
                                 bool color_enabled) {
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
                auto it = ColorMap::ANSI_COLORS.find(input[in_pos]);
                if (it != ColorMap::ANSI_COLORS.end()) {
                    const char* color_code = it->second;
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
                    int reset_len = strlen(ColorMap::RESET);
                    if (out_pos + reset_len < max_len - 1) {
                        strcpy(output + out_pos, ColorMap::RESET);
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

    /**
     * @brief メッセージからカラータグを除去
     * @param input 入力メッセージ
     * @param output 出力バッファ
     * @param max_len 最大長
     */
    static void strip_color_tags(const char* input, char* output, int max_len) {
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

            // カラータグ開始処理 (x|形式) - スキップ
            if (input[in_pos] != '|' && in_pos + 1 < input_len &&
                input[in_pos + 1] == '|' &&
                ColorMap::ANSI_COLORS.find(input[in_pos]) !=
                    ColorMap::ANSI_COLORS.end()) {
                in_pos += 2;  // "x|" をスキップ
                continue;
            }

            // カラー終了タグ (単独の|) - スキップ
            if (input[in_pos] == '|') {
                in_pos++;
            } else {
                // 通常文字
                output[out_pos++] = input[in_pos++];
            }
        }
        output[out_pos] = '\0';
    }
};

/**
 * @brief 検証処理統合クラス
 * @details カラータグなどの妥当性検証を統合
 */
class ValidationUtils {
   public:
    /**
     * @brief カラータグの妥当性をチェック（コンパイル時）
     * @param input チェック対象の文字列
     * @return true: 妥当, false: 不正
     * @details
     * |が偶数個であること、連続していないことを検証（||はエスケープとして扱う）
     */
    static constexpr bool validate_color_tags_compile_time(const char* input) {
        int pipe_count = 0;
        int input_len = 0;

        // 文字列長を計算
        while (input[input_len] != '\0') {
            input_len++;
        }

        for (int i = 0; i < input_len; i++) {
            if (input[i] == '|') {
                if (i + 1 < input_len && input[i + 1] == '|') {
                    // ||はエスケープされた|として扱う - スキップ
                    i++;  // 次の|もスキップ
                    continue;
                }
                // 単一の|
                pipe_count++;
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
    static bool validate_color_tags_runtime(const char* input) {
        int pipe_count = 0;
        int input_len = strlen(input);

        for (int i = 0; i < input_len; i++) {
            if (input[i] == '|') {
                if (i + 1 < input_len && input[i + 1] == '|') {
                    // ||はエスケープされた|として扱う - スキップ
                    i++;  // 次の|もスキップ
                    continue;
                }
                // 単一の|
                pipe_count++;
            }
        }

        // |が偶数個かチェック
        return (pipe_count % 2 == 0);
    }
};

/**
 * @brief 文字列処理統合クラス
 * @details 共通の文字列操作を統合
 */
class StringUtils {
   public:
    /**
     * @brief ファイルパスからファイル名のみを抽出
     * @param filepath フルパス
     * @return ファイル名部分
     */
    static const char* extract_filename(const char* filepath) {
        const char* filename = strrchr(filepath, '/');
        if (filename == nullptr) {
            filename = strrchr(filepath, '\\');
        }
        return (filename != nullptr) ? filename + 1 : filepath;
    }

    /**
     * @brief ログレベル文字列を取得
     * @param level ログレベル
     * @return レベル文字列
     */
    static const char* get_level_string(LogLevel level) {
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
     * @brief 安全な文字列コピー
     * @param dest 宛先バッファ
     * @param src ソース文字列
     * @param max_len 最大長
     */
    static void safe_strcpy(char* dest, const char* src, int max_len) {
        strncpy(dest, src, max_len - 1);
        dest[max_len - 1] = '\0';
    }
};
}  // namespace Utils
#endif  // LOG_UTILS_HPP