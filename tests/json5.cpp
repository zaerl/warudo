/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <cmath>
#include <cstdint>
#include <string>

#include "../src/json5.h"
#include "test.h"

namespace json5 = wrd::json5;

// The error code of a document, errc::none when it is valid.
static json5::errc parse_error(std::string_view text) {
    json5::error error;

    json5::parse(text, &error);

    return error.code;
}

// The integer value of a document, -1 when it is not an integer.
static std::int64_t parse_integer(std::string_view text) {
    std::optional<json5::value> value = json5::parse(text);
    const std::int64_t *integer = value ? value->get_if<std::int64_t>() : nullptr;

    return integer ? *integer : -1;
}

// The floating point value of a document, NaN when it is not a floating point number.
static double parse_double(std::string_view text) {
    std::optional<json5::value> value = json5::parse(text);
    const double *number = value ? value->get_if<double>() : nullptr;

    return number ? *number : std::nan("");
}

// The string value of a document, "<invalid>" when it is not a string.
static std::string parse_string(std::string_view text) {
    std::optional<json5::value> value = json5::parse(text);
    const std::string *string = value ? value->get_if<std::string>() : nullptr;

    return string ? *string : "<invalid>";
}

ATT_TEST(json5) {
    // Empty documents.
    ATT_ASSERT(parse_error(""), json5::errc::empty, "Empty document");
    ATT_ASSERT(parse_error(" \t\r\n"), json5::errc::empty, "White space only");
    ATT_ASSERT(parse_error("// comment"), json5::errc::empty, "Line comment only");
    ATT_ASSERT(parse_error("/* comment */ \xEF\xBB\xBF\xC2\xA0"), json5::errc::empty,
        "Comment and Unicode spaces");
    ATT_ASSERT(parse_error("/* comment"), json5::errc::syntax, "Unterminated comment");

    // Literals.
    ATT_ASSERT(json5::parse("null")->is<std::nullptr_t>(), true, "null");
    ATT_ASSERT(*json5::parse("true")->get_if<bool>(), true, "true");
    ATT_ASSERT(*json5::parse(" false ")->get_if<bool>(), false, "false");
    ATT_ASSERT(parse_error("nul"), json5::errc::syntax, "Truncated literal");
    ATT_ASSERT(parse_error("nullx"), json5::errc::syntax, "Literal followed by a name");
    ATT_ASSERT(parse_error("True"), json5::errc::syntax, "Literals are case sensitive");

    // Integers.
    ATT_ASSERT(parse_integer("0"), 0, "Zero");
    ATT_ASSERT(parse_integer("42"), 42, "Integer");
    ATT_ASSERT(parse_integer("-42"), -42, "Negative integer");
    ATT_ASSERT(parse_integer("+42"), 42, "Explicit plus sign");
    ATT_ASSERT(parse_integer("0x1F"), 31, "Hexadecimal");
    ATT_ASSERT(parse_integer("-0Xff"), -255, "Negative hexadecimal");
    ATT_ASSERT(parse_integer("9223372036854775807"), INT64_MAX, "Biggest integer");
    ATT_ASSERT(parse_integer("-9223372036854775808"), INT64_MIN, "Smallest integer");
    ATT_ASSERT(
        parse_double("9223372036854775808"), 9223372036854775808.0, "Too big for an integer");
    ATT_ASSERT(parse_error("012"), json5::errc::syntax, "Leading zero");
    ATT_ASSERT(parse_error("0x"), json5::errc::syntax, "Hexadecimal without digits");
    ATT_ASSERT(parse_error("0xFFFFFFFFFFFFFFFFF"), json5::errc::syntax, "Hexadecimal out of range");
    ATT_ASSERT(parse_error("1a"), json5::errc::syntax, "Number followed by a name");
    ATT_ASSERT(parse_error("-"), json5::errc::syntax, "Sign only");

    // Floating point numbers.
    ATT_ASSERT(parse_double("1.5"), 1.5, "Fraction");
    ATT_ASSERT(parse_double(".5"), 0.5, "Leading decimal point");
    ATT_ASSERT(parse_double("5."), 5.0, "Trailing decimal point");
    ATT_ASSERT(parse_double("-1e3"), -1000.0, "Exponent");
    ATT_ASSERT(parse_double("2.5E-1"), 0.25, "Negative exponent");
    ATT_ASSERT(std::isinf(parse_double("Infinity")), true, "Infinity");
    ATT_ASSERT(parse_double("-Infinity") < 0, true, "Negative infinity");
    ATT_ASSERT(json5::parse("NaN").has_value(), true, "NaN");
    ATT_ASSERT(parse_error("."), json5::errc::syntax, "Decimal point only");
    ATT_ASSERT(parse_error("1e"), json5::errc::syntax, "Exponent without digits");

    // Strings.
    ATT_ASSERT(parse_string("\"double\""), "double", "Double quoted");
    ATT_ASSERT(parse_string("'single'"), "single", "Single quoted");
    ATT_ASSERT(parse_string("'it\\'s \"ok\"'"), "it's \"ok\"", "Quotes in a string");
    ATT_ASSERT(parse_string("'\\b\\f\\n\\r\\t\\v'"), "\b\f\n\r\t\v", "Single character escapes");
    ATT_ASSERT(parse_string("'\\x41\\xE8'"), "A\xC3\xA8", "Hexadecimal escapes");
    ATT_ASSERT(parse_string("'\\u00e8\\u4e16'"), "\xC3\xA8\xE4\xB8\x96", "Unicode escapes");
    ATT_ASSERT(parse_string("'\\uD83D\\uDE00'"), "\xF0\x9F\x98\x80", "Surrogate pair");
    ATT_ASSERT(parse_string("'\\uD83D'"), "\xEF\xBF\xBD", "Lone surrogate");
    ATT_ASSERT(parse_string("'a\\\nb\\\r\nc'"), "abc", "Line continuations");
    ATT_ASSERT(parse_string("'\\q\\/'"), "q/", "Other escaped characters");
    ATT_ASSERT(parse_string("'\xE4\xB8\x96'"), "\xE4\xB8\x96", "UTF-8 text");
    ATT_ASSERT(parse_string("'a\\0b'").size(), 3, "NUL escape");
    ATT_ASSERT(parse_error("'open"), json5::errc::syntax, "Unterminated string");
    ATT_ASSERT(parse_error("'a\nb'"), json5::errc::syntax, "Line break in a string");
    ATT_ASSERT(parse_error("'\\1'"), json5::errc::syntax, "Octal escape");
    ATT_ASSERT(parse_error("'\\xZZ'"), json5::errc::syntax, "Invalid hexadecimal escape");
    ATT_ASSERT(parse_error("'\\u12'"), json5::errc::syntax, "Invalid Unicode escape");
    ATT_ASSERT(parse_error("'a\" "), json5::errc::syntax, "Mismatched quotes");

    // Arrays.
    std::optional<json5::value> array = json5::parse("[1, 'two', [3], null,]");

    ATT_ASSERT(array.has_value(), true, "Array");
    ATT_ASSERT(array->get_if<json5::array>()->size(), 4, "Array size");
    ATT_ASSERT(*array->get_if<json5::array>()->at(0).get_if<std::int64_t>(), 1, "Array integer");
    ATT_ASSERT(*array->get_if<json5::array>()->at(1).get_if<std::string>(), "two", "Array string");
    ATT_ASSERT(array->get_if<json5::array>()->at(2).is<json5::array>(), true, "Nested array");
    ATT_ASSERT(array->find("key") == nullptr, true, "find() on an array");
    ATT_ASSERT(json5::parse("[]")->get_if<json5::array>()->empty(), true, "Empty array");
    ATT_ASSERT(parse_error("[1 2]"), json5::errc::syntax, "Missing comma in an array");
    ATT_ASSERT(parse_error("[,]"), json5::errc::syntax, "Comma only array");
    ATT_ASSERT(parse_error("[1,,]"), json5::errc::syntax, "Double comma in an array");
    ATT_ASSERT(parse_error("[1"), json5::errc::syntax, "Unterminated array");

    // Objects.
    std::optional<json5::value> object =
        json5::parse("{\n"
                     "    // A comment\n"
                     "    unquoted: 1, 'single': 2, \"double\": 3, /* inline */ $_a1: 4,\n"
                     "    nested: { list: [true] },\n"
                     "    \\u0061b: 5,\n"
                     "    dup: 6, dup: 7,\n"
                     "}\n");

    ATT_ASSERT(object.has_value(), true, "Object");
    ATT_ASSERT(object->get_if<json5::object>()->size(), 8, "Object size");
    ATT_ASSERT(
        object->get_if<json5::object>()->front().key, "unquoted", "Members keep their order");
    ATT_ASSERT(*object->find("unquoted")->get_if<std::int64_t>(), 1, "Unquoted key");
    ATT_ASSERT(*object->find("single")->get_if<std::int64_t>(), 2, "Single quoted key");
    ATT_ASSERT(*object->find("double")->get_if<std::int64_t>(), 3, "Double quoted key");
    ATT_ASSERT(*object->find("$_a1")->get_if<std::int64_t>(), 4, "Key with symbols and digits");
    ATT_ASSERT(object->find("nested")->find("list")->is<json5::array>(), true, "Nested object");
    ATT_ASSERT(*object->find("ab")->get_if<std::int64_t>(), 5, "Escaped key");
    ATT_ASSERT(*object->find("dup")->get_if<std::int64_t>(), 7, "Last duplicated key");
    ATT_ASSERT(object->find("missing") == nullptr, true, "Missing key");
    ATT_ASSERT(json5::parse("{}")->get_if<json5::object>()->empty(), true, "Empty object");
    ATT_ASSERT(parse_error("{test"), json5::errc::syntax, "Unterminated object");
    ATT_ASSERT(parse_error("{a 1}"), json5::errc::syntax, "Missing colon");
    ATT_ASSERT(parse_error("{a: 1 b: 2}"), json5::errc::syntax, "Missing comma in an object");
    ATT_ASSERT(parse_error("{1a: 1}"), json5::errc::syntax, "Key starting with a digit");
    ATT_ASSERT(parse_error("{a: }"), json5::errc::syntax, "Missing value");
    ATT_ASSERT(parse_error("{,}"), json5::errc::syntax, "Comma only object");
    ATT_ASSERT(parse_error("{} {}"), json5::errc::syntax, "Two documents");

    // Nesting limit.
    std::string nested(json5::max_depth, '[');

    nested.append(json5::max_depth, ']');
    ATT_ASSERT(parse_error(nested), json5::errc::none, "Deepest allowed nesting");
    ATT_ASSERT(parse_error("[" + nested + "]"), json5::errc::depth, "Nested too deeply");
    ATT_ASSERT(
        parse_error(std::string(100000, '[')), json5::errc::depth, "Unbalanced deep nesting");

    // Error position.
    json5::error error;

    ATT_ASSERT(
        json5::parse("{\n  port: 1,\n  name ?\n}", &error).has_value(), false, "Invalid document");
    ATT_ASSERT(error.line, 3, "Error line");
    ATT_ASSERT(error.column, 8, "Error column");
    ATT_ASSERT(error.message.empty(), false, "Error message");

    ATT_ASSERT(json5::parse("1", &error).has_value(), true, "Valid document");
    ATT_ASSERT(error.code, json5::errc::none, "Error is reset");
}
