#ifndef LOGGER_H
#define LOGGER_H


namespace Utils {

class ColorHelper {
   public:
    static const char* get_level_color(LogLevel level, bool color_enabled);
    static const char* get_reset_color(bool color_enabled);
    static void parse_color_tags(const char* input, char* output, int max_len,
                                 bool color_enabled);
    static void strip_color_tags(const char* input, char* output, int max_len);
};

class ValidationUtils {
   public:
    static constexpr bool validate_color_tags_compile_time(const char* input);
    static bool validate_color_tags_runtime(const char* input);
};

class StringUtils {
   public:
    static const char* extract_filename(const char* filepath);
    static const char* get_level_string(LogLevel level);
    static void safe_strcpy(char* dest, const char* src, int max_len);
};


}  // namespace Utils

#endif  // LOGGER_H