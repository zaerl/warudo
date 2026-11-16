/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <charconv>
#include <cstdlib>
#include <limits>

#include "json5.h"

namespace wrd::json5 {

const value *value::find(std::string_view key) const {
    const object *members = get_if<object>();

    if(!members) {
        return nullptr;
    }

    for(auto it = members->rbegin(); it != members->rend(); ++it) {
        if(it->key == key) {
            return &it->val;
        }
    }

    return nullptr;
}

namespace {

constexpr bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

constexpr int hex_digit(char c) {
    if(c >= '0' && c <= '9') {
        return c - '0';
    }

    if(c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }

    if(c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }

    return -1;
}

constexpr bool is_identifier_start(char c) {
    // Bytes of a multi-byte UTF-8 sequence are accepted as they are.
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '$' ||
        static_cast<unsigned char>(c) >= 0x80;
}

void append_utf8(std::string &output, char32_t code_point) {
    if(code_point < 0x80) {
        output += static_cast<char>(code_point);
    } else if(code_point < 0x800) {
        output += static_cast<char>(0xC0 | (code_point >> 6));
        output += static_cast<char>(0x80 | (code_point & 0x3F));
    } else if(code_point < 0x10000) {
        output += static_cast<char>(0xE0 | (code_point >> 12));
        output += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
        output += static_cast<char>(0x80 | (code_point & 0x3F));
    } else {
        output += static_cast<char>(0xF0 | (code_point >> 18));
        output += static_cast<char>(0x80 | ((code_point >> 12) & 0x3F));
        output += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
        output += static_cast<char>(0x80 | (code_point & 0x3F));
    }
}

class parser {
  public:
    explicit parser(std::string_view text) : text_(text) {
    }

    std::optional<value> parse_document(error *err) {
        std::optional<value> result;

        if(skip_blanks()) {
            if(at_end()) {
                fail(errc::empty, "the document is empty");
            } else {
                result = parse_value(0);
            }
        }

        if(result && skip_blanks() && !at_end()) {
            fail(errc::syntax, "unexpected content after the value");
        }

        if(error_.code != errc::none) {
            result.reset();

            if(err) {
                locate();
                *err = error_;
            }
        } else if(err) {
            *err = {};
        }

        return result;
    }

  private:
    bool at_end() const {
        return pos_ >= text_.size();
    }

    // The current byte, or NUL at the end of the document.
    char peek(std::size_t offset = 0) const {
        return pos_ + offset < text_.size() ? text_[pos_ + offset] : '\0';
    }

    bool starts_with(std::string_view prefix) const {
        return text_.substr(pos_).starts_with(prefix);
    }

    bool consume(std::string_view prefix) {
        if(!starts_with(prefix)) {
            return false;
        }

        pos_ += prefix.size();

        return true;
    }

    // Record the first error. Always returns false.
    bool fail(errc code, const char *message) {
        if(error_.code == errc::none) {
            error_.code = code;
            error_.message = message;
            error_pos_ = pos_;
        }

        return false;
    }

    // Compute the line and column of the recorded error.
    void locate() {
        std::size_t end = error_pos_ < text_.size() ? error_pos_ : text_.size();

        error_.line = 1;
        error_.column = 1;

        for(std::size_t i = 0; i < end; ++i) {
            if(text_[i] == '\n') {
                ++error_.line;
                error_.column = 1;
            } else {
                ++error_.column;
            }
        }
    }

    // Length in bytes of the white space character at the current position, 0 if none.
    std::size_t blank_length() const {
        switch(peek()) {
            case ' ':
            case '\t':
            case '\n':
            case '\v':
            case '\f':
            case '\r':
                return 1;
            default:
                break;
        }

        // U+00A0, U+FEFF, U+2028, U+2029 and the Unicode space separators.
        static constexpr std::string_view spaces[] = {"\xC2\xA0", "\xEF\xBB\xBF", "\xE2\x80\xA8",
            "\xE2\x80\xA9", "\xE1\x9A\x80", "\xE2\x80\x80", "\xE2\x80\x81", "\xE2\x80\x82",
            "\xE2\x80\x83", "\xE2\x80\x84", "\xE2\x80\x85", "\xE2\x80\x86", "\xE2\x80\x87",
            "\xE2\x80\x88", "\xE2\x80\x89", "\xE2\x80\x8A", "\xE2\x80\xAF", "\xE2\x81\x9F",
            "\xE3\x80\x80"};

        if(static_cast<unsigned char>(peek()) >= 0x80) {
            for(std::string_view space : spaces) {
                if(starts_with(space)) {
                    return space.size();
                }
            }
        }

        return 0;
    }

    // Skip white space and comments. False on an unterminated comment.
    bool skip_blanks() {
        while(!at_end()) {
            if(std::size_t length = blank_length()) {
                pos_ += length;
            } else if(starts_with("//")) {
                while(!at_end() && peek() != '\n' && peek() != '\r' &&
                    !starts_with("\xE2\x80\xA8") && !starts_with("\xE2\x80\xA9")) {
                    ++pos_;
                }
            } else if(starts_with("/*")) {
                std::size_t end = text_.find("*/", pos_ + 2);

                if(end == std::string_view::npos) {
                    return fail(errc::syntax, "unterminated comment");
                }

                pos_ = end + 2;
            } else {
                break;
            }
        }

        return true;
    }

    // True when the keyword is at the current position and is not part of a longer name.
    bool consume_keyword(std::string_view keyword) {
        if(!starts_with(keyword)) {
            return false;
        }

        char next = peek(keyword.size());

        if(is_identifier_start(next) || is_digit(next)) {
            return false;
        }

        pos_ += keyword.size();

        return true;
    }

    std::optional<value> parse_value(std::size_t depth) {
        char c = peek();

        if(c == '{') {
            return parse_object(depth);
        } else if(c == '[') {
            return parse_array(depth);
        } else if(c == '"' || c == '\'') {
            std::string text;

            if(!parse_string(text)) {
                return std::nullopt;
            }

            return value{std::move(text)};
        } else if(consume_keyword("null")) {
            return value{nullptr};
        } else if(consume_keyword("true")) {
            return value{true};
        } else if(consume_keyword("false")) {
            return value{false};
        } else if(is_digit(c) || c == '+' || c == '-' || c == '.' || c == 'I' || c == 'N') {
            return parse_number();
        }

        fail(errc::syntax, at_end() ? "unexpected end of the document" : "unexpected character");

        return std::nullopt;
    }

    std::optional<value> parse_object(std::size_t depth) {
        if(depth >= max_depth) {
            fail(errc::depth, "the document is nested too deeply");

            return std::nullopt;
        }

        object members;

        // Skip the opening brace.
        ++pos_;

        for(;;) {
            if(!skip_blanks()) {
                return std::nullopt;
            }

            if(consume("}")) {
                break;
            }

            member entry;

            if(!parse_key(entry.key) || !skip_blanks()) {
                return std::nullopt;
            }

            if(!consume(":")) {
                fail(errc::syntax, "expected ':' after an object key");

                return std::nullopt;
            }

            if(!skip_blanks()) {
                return std::nullopt;
            }

            std::optional<value> parsed = parse_value(depth + 1);

            if(!parsed) {
                return std::nullopt;
            }

            entry.val = std::move(*parsed);
            members.push_back(std::move(entry));

            if(!skip_blanks()) {
                return std::nullopt;
            }

            if(consume("}")) {
                break;
            }

            if(!consume(",")) {
                fail(errc::syntax, "expected ',' or '}' in an object");

                return std::nullopt;
            }
        }

        return value{std::move(members)};
    }

    std::optional<value> parse_array(std::size_t depth) {
        if(depth >= max_depth) {
            fail(errc::depth, "the document is nested too deeply");

            return std::nullopt;
        }

        array items;

        // Skip the opening bracket.
        ++pos_;

        for(;;) {
            if(!skip_blanks()) {
                return std::nullopt;
            }

            if(consume("]")) {
                break;
            }

            std::optional<value> parsed = parse_value(depth + 1);

            if(!parsed) {
                return std::nullopt;
            }

            items.push_back(std::move(*parsed));

            if(!skip_blanks()) {
                return std::nullopt;
            }

            if(consume("]")) {
                break;
            }

            if(!consume(",")) {
                fail(errc::syntax, "expected ',' or ']' in an array");

                return std::nullopt;
            }
        }

        return value{std::move(items)};
    }

    // An object key: a quoted string or an identifier.
    bool parse_key(std::string &key) {
        char c = peek();

        if(c == '"' || c == '\'') {
            return parse_string(key);
        }

        if(!is_identifier_start(c) && c != '\\') {
            return fail(errc::syntax, "expected an object key");
        }

        while(!at_end()) {
            c = peek();

            if(c == '\\') {
                char32_t code_point = 0;

                ++pos_;

                if(!consume("u") || !parse_unicode_escape(code_point)) {
                    return fail(errc::syntax, "invalid escape in an object key");
                }

                append_utf8(key, code_point);
            } else if(is_identifier_start(c) || is_digit(c)) {
                // A non ASCII byte can also be the start of a white space character.
                if(static_cast<unsigned char>(c) >= 0x80 && blank_length()) {
                    break;
                }

                key += c;
                ++pos_;
            } else {
                break;
            }
        }

        return true;
    }

    // Four hexadecimal digits.
    bool parse_hex4(char32_t &output) {
        output = 0;

        for(int i = 0; i < 4; ++i) {
            int digit = hex_digit(peek());

            if(digit < 0) {
                return false;
            }

            output = (output << 4) | static_cast<char32_t>(digit);
            ++pos_;
        }

        return true;
    }

    // The digits of a \uXXXX escape, joining surrogate pairs.
    bool parse_unicode_escape(char32_t &code_point) {
        if(!parse_hex4(code_point)) {
            return false;
        }

        if(code_point >= 0xD800 && code_point <= 0xDBFF && starts_with("\\u")) {
            std::size_t saved = pos_;
            char32_t low = 0;

            pos_ += 2;

            if(parse_hex4(low) && low >= 0xDC00 && low <= 0xDFFF) {
                code_point = 0x10000 + ((code_point - 0xD800) << 10) + (low - 0xDC00);

                return true;
            }

            pos_ = saved;
        }

        // A lone surrogate cannot be encoded in UTF-8.
        if(code_point >= 0xD800 && code_point <= 0xDFFF) {
            code_point = 0xFFFD;
        }

        return true;
    }

    bool parse_string(std::string &output) {
        char quote = peek();

        ++pos_;

        for(;;) {
            if(at_end()) {
                return fail(errc::syntax, "unterminated string");
            }

            char c = peek();

            if(c == quote) {
                ++pos_;

                return true;
            }

            if(c == '\n' || c == '\r') {
                return fail(errc::syntax, "unescaped line break in a string");
            }

            ++pos_;

            if(c != '\\') {
                output += c;
                continue;
            }

            if(at_end()) {
                return fail(errc::syntax, "unterminated string");
            }

            c = peek();
            ++pos_;

            switch(c) {
                case 'b':
                    output += '\b';
                    break;
                case 'f':
                    output += '\f';
                    break;
                case 'n':
                    output += '\n';
                    break;
                case 'r':
                    output += '\r';
                    break;
                case 't':
                    output += '\t';
                    break;
                case 'v':
                    output += '\v';
                    break;
                case '0':
                    if(is_digit(peek())) {
                        return fail(errc::syntax, "octal escapes are not allowed");
                    }

                    output += '\0';
                    break;
                case 'x': {
                    int high = hex_digit(peek());
                    int low = hex_digit(peek(1));

                    if(high < 0 || low < 0) {
                        return fail(errc::syntax, "invalid \\x escape");
                    }

                    pos_ += 2;
                    append_utf8(output, static_cast<char32_t>(high * 16 + low));
                    break;
                }
                case 'u': {
                    char32_t code_point = 0;

                    if(!parse_unicode_escape(code_point)) {
                        return fail(errc::syntax, "invalid \\u escape");
                    }

                    append_utf8(output, code_point);
                    break;
                }
                case '\r':
                    // Line continuation: \r\n counts as one line break.
                    consume("\n");
                    break;
                case '\n':
                    break;
                default:
                    if(c >= '1' && c <= '9') {
                        return fail(errc::syntax, "octal escapes are not allowed");
                    }

                    // Line continuation with U+2028 or U+2029.
                    if(c == '\xE2' && (starts_with("\x80\xA8") || starts_with("\x80\xA9"))) {
                        pos_ += 2;
                        break;
                    }

                    // Any other character stands for itself.
                    output += c;
                    break;
            }
        }
    }

    std::optional<value> parse_number() {
        std::size_t start = pos_;
        bool negative = false;

        if(peek() == '+' || peek() == '-') {
            negative = peek() == '-';
            ++pos_;
        }

        if(consume_keyword("Infinity")) {
            double infinity = std::numeric_limits<double>::infinity();

            return value{negative ? -infinity : infinity};
        }

        if(consume_keyword("NaN")) {
            return value{std::numeric_limits<double>::quiet_NaN()};
        }

        if(peek() == '0' && (peek(1) == 'x' || peek(1) == 'X')) {
            pos_ += 2;

            return parse_hexadecimal(negative);
        }

        std::size_t digits_start = pos_;
        bool is_integer = true;

        while(is_digit(peek())) {
            ++pos_;
        }

        std::size_t integer_digits = pos_ - digits_start;
        std::size_t fraction_digits = 0;

        if(integer_digits > 1 && text_[digits_start] == '0') {
            pos_ = digits_start;
            fail(errc::syntax, "numbers cannot have leading zeros");

            return std::nullopt;
        }

        if(peek() == '.') {
            is_integer = false;
            ++pos_;

            while(is_digit(peek())) {
                ++pos_;
                ++fraction_digits;
            }
        }

        if(integer_digits + fraction_digits == 0) {
            pos_ = start;
            fail(errc::syntax, "invalid number");

            return std::nullopt;
        }

        if(peek() == 'e' || peek() == 'E') {
            is_integer = false;
            ++pos_;

            if(peek() == '+' || peek() == '-') {
                ++pos_;
            }

            if(!is_digit(peek())) {
                fail(errc::syntax, "invalid number exponent");

                return std::nullopt;
            }

            while(is_digit(peek())) {
                ++pos_;
            }
        }

        if(is_identifier_start(peek())) {
            fail(errc::syntax, "invalid number");

            return std::nullopt;
        }

        // The sign is part of the conversion, a leading '+' is not accepted by from_chars.
        std::string_view digits = text_.substr(start, pos_ - start);

        if(digits.front() == '+') {
            digits.remove_prefix(1);
        }

        if(is_integer) {
            std::int64_t integer = 0;
            auto [end, code] =
                std::from_chars(digits.data(), digits.data() + digits.size(), integer);

            if(code == std::errc() && end == digits.data() + digits.size()) {
                return value{integer};
            }

            // Too big for 64 bits: keep it as a floating point number.
        }

        return value{std::strtod(std::string(digits).c_str(), nullptr)};
    }

    std::optional<value> parse_hexadecimal(bool negative) {
        std::size_t digits_start = pos_;

        while(hex_digit(peek()) >= 0) {
            ++pos_;
        }

        if(pos_ == digits_start || is_identifier_start(peek())) {
            fail(errc::syntax, "invalid hexadecimal number");

            return std::nullopt;
        }

        std::string_view digits = text_.substr(digits_start, pos_ - digits_start);
        std::uint64_t magnitude = 0;
        auto [end, code] =
            std::from_chars(digits.data(), digits.data() + digits.size(), magnitude, 16);
        constexpr std::uint64_t max = std::numeric_limits<std::int64_t>::max();

        if(code != std::errc() || magnitude > max) {
            pos_ = digits_start;
            fail(errc::syntax, "hexadecimal number out of range");

            return std::nullopt;
        }

        std::int64_t integer = static_cast<std::int64_t>(magnitude);

        return value{negative ? -integer : integer};
    }

    std::string_view text_;
    std::size_t pos_ = 0;
    std::size_t error_pos_ = 0;
    error error_;
};

} // namespace

std::optional<value> parse(std::string_view text, error *err) {
    return parser(text).parse_document(err);
}

} // namespace wrd::json5
