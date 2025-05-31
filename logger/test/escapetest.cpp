/**
 * @file escapetest.cpp
 * @brief パイプ文字のエスケープテスト
 */

#include <cstdio>
#include <cstring>

/**
 * @brief シンプル化されたエスケープ解析関数
 * @param input 入力文字列
 */
void test_escape_parsing(const char* input) {
    printf("Input: \"%s\"\n", input);
    printf("Raw bytes: ");
    
    // 入力文字列の生バイトを表示
    int len = strlen(input);
    for (int i = 0; i < len; i++) {
        printf("0x%02X('%c') ", (unsigned char)input[i], 
               (input[i] >= 32 && input[i] <= 126) ? input[i] : '?');
    }
    printf("\n");
    
    // パイプ文字とエスケープの解析
    printf("Analysis: ");
    for (int i = 0; i < len; i++) {
        if (input[i] == '\\' && i + 1 < len) {
            printf("[ESCAPE: \\%c] ", input[i + 1]);
            i++; // 次の文字をスキップ
        } else if (input[i] == '|') {
            printf("[PIPE] ");
        } else {
            printf("%c", input[i]);
        }
    }
    printf("\n");
    
    // パイプカウント（エスケープを考慮）
    int pipe_count = 0;
    for (int i = 0; i < len; i++) {
        if (input[i] == '\\' && i + 1 < len) {
            i++; // エスケープシーケンス - 次の文字をスキップ
            continue;
        }
        if (input[i] == '|') {
            pipe_count++;
        }
    }
    printf("Pipe count (escaped): %d\n", pipe_count);
    printf("Valid: %s\n", (pipe_count % 2 == 0) ? "YES" : "NO");
    printf("----------------------------------------\n");
}

/**
 * @brief カラータグバリデーション関数（logger_impl.hppから抜粋）
 */
bool validate_color_tags_simple(const char* input) {
    int pipe_count = 0;
    int input_len = strlen(input);
    bool last_was_pipe = false;
    
    for (int i = 0; i < input_len; i++) {
        if (input[i] == '\\' && i + 1 < input_len) {
            // エスケープシーケンス - 次の文字をスキップ
            printf("  [DEBUG] Found escape at pos %d: \\%c\n", i, input[i + 1]);
            i++;
            last_was_pipe = false;
            continue;
        }
        
        if (input[i] == '|') {
            // 連続する|をチェック
            if (last_was_pipe) {
                printf("  [ERROR] Consecutive pipes at pos %d\n", i);
                return false;  // 連続する|はエラー
            }
            printf("  [DEBUG] Found pipe at pos %d\n", i);
            pipe_count++;
            last_was_pipe = true;
        } else {
            last_was_pipe = false;
        }
    }
    
    printf("  [DEBUG] Total pipes: %d\n", pipe_count);
    // |が偶数個かチェック
    return (pipe_count % 2 == 0);
}

int main() {
    printf("=== Pipe Escape Testing ===\n\n");
    
    // テストケース1: 通常のパイプ
    printf("Test 1: Normal pipe\n");
    test_escape_parsing("Hello | World");
    
    // テストケース2: 単一バックスラッシュでエスケープ
    printf("Test 2: Single backslash escape\n");
    test_escape_parsing("Hello \\| World");
    
    // テストケース3: ダブルバックスラッシュ
    printf("Test 3: Double backslash\n");
    test_escape_parsing("Hello \\\\| World");
    
    // テストケース4: リテラル文字列での表現
    printf("Test 4: Raw string literal\n");
    const char* raw_string = R"(Hello \| World)";
    test_escape_parsing(raw_string);
    
    // テストケース5: 複数のエスケープパターン
    printf("Test 5: Multiple escape patterns\n");
    test_escape_parsing("\\|pipe1\\| and \\|pipe2\\|");
    
    // テストケース6: 代替エスケープ文字のテスト
    printf("Test 6: Alternative escape characters\n");
    test_escape_parsing("Hello ^| World");  // ^ をエスケープ文字として
    test_escape_parsing("Hello #| World");  // # をエスケープ文字として
    test_escape_parsing("Hello @| World");  // @ をエスケープ文字として
    
    printf("\n=== Validation Function Testing ===\n");
    
    // バリデーション関数のテスト
    const char* test_strings[] = {
        "Normal text",
        "Text with | pipe",
        "Text with \\| escaped pipe",
        "r|Red text|",
        "r|Unclosed red",
        "r||Double pipes||",
        "\\|Escaped\\| pipes",
        "Mixed r|colored| and \\|escaped\\| text"
    };
    
    int num_tests = sizeof(test_strings) / sizeof(test_strings[0]);
    
    for (int i = 0; i < num_tests; i++) {
        printf("\nValidation Test %d: \"%s\"\n", i + 1, test_strings[i]);
        bool is_valid = validate_color_tags_simple(test_strings[i]);
        printf("Result: %s\n", is_valid ? "VALID" : "INVALID");
    }
    
    printf("\n=== C String Literal Testing ===\n");
    
    // C文字列リテラルでの表現テスト
    printf("Literal 1: \"Hello \\\\| World\" -> ");
    test_escape_parsing("Hello \\| World");
    
    printf("Literal 2: R\"(Hello \\| World)\" -> ");
    test_escape_parsing(R"(Hello \| World)");
    
    return 0;
}