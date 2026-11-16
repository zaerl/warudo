/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#ifndef WRD_JSON5_H
#define WRD_JSON5_H

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

// A small JSON5 (https://spec.json5.org) reader, used for the configuration file.
namespace wrd::json5 {

struct member;
struct value;

using array = std::vector<value>;
// Members keep the order of the document. Duplicate keys are kept, see value::find().
using object = std::vector<member>;

struct value {
    // Numbers without a fraction or an exponent that fit 64 bits are integers.
    using storage =
        std::variant<std::nullptr_t, bool, std::int64_t, double, std::string, array, object>;

    storage data;

    // Defined below, once member is a complete type.
    value();
    value(storage content);
    value(const value &other);
    value(value &&other) noexcept;
    value &operator=(const value &other);
    value &operator=(value &&other) noexcept;
    ~value();

    template <typename T> bool is() const {
        return std::holds_alternative<T>(data);
    }

    // The contained T, or nullptr when the value has another type.
    template <typename T> const T *get_if() const {
        return std::get_if<T>(&data);
    }

    // The member of an object with the given key, the last one when the key is duplicated.
    // nullptr when the value is not an object or has no such member.
    const value *find(std::string_view key) const;
};

struct member {
    std::string key;
    value val;
};

inline value::value() : data(nullptr) {
}

inline value::value(storage content) : data(std::move(content)) {
}

inline value::value(const value &other) = default;
inline value::value(value &&other) noexcept = default;
inline value &value::operator=(const value &other) = default;
inline value &value::operator=(value &&other) noexcept = default;
inline value::~value() = default;

enum class errc {
    none,
    // The document has no value: it is empty or only has white space and comments.
    empty,
    syntax,
    // Arrays and objects are nested deeper than max_depth.
    depth,
};

struct error {
    errc code = errc::none;
    // 1-based position of the error. The column counts bytes.
    std::size_t line = 0;
    std::size_t column = 0;
    std::string message;
};

inline constexpr std::size_t max_depth = 64;

// Parse a UTF-8 JSON5 document. On failure returns std::nullopt and fills err, if given.
std::optional<value> parse(std::string_view text, error *err = nullptr);

} // namespace wrd::json5

#endif // WRD_JSON5_H
