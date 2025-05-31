#ifndef WRITERS_HPP
#define WRITERS_HPP


#include <cstdio>


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
    void write(const char* message) override { printf("%s\n", message); }
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

#endif  // WRITERS_HPP