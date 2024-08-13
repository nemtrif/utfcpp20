//    Copyright 2024 Nemanja Trifunovic

//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at

//        http://www.apache.org/licenses/LICENSE-2.0

//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

#include "core.hpp"

namespace utfcpp::internal
{
    bool is_utf8_trail(char8_t ch) {
        return (ch > 0x7fu);
    }

    bool is_utf16_lead_surrogate(char16_t cp) {
        return (cp >= LEAD_SURROGATE_MIN && cp <= LEAD_SURROGATE_MAX);
    }

    bool is_utf16_trail_surrogate(char16_t cp) {
        return (cp >= TRAIL_SURROGATE_MIN && cp <= TRAIL_SURROGATE_MAX);
    }

    bool is_utf16_surrogate(char32_t cp) {
        return (cp >= LEAD_SURROGATE_MIN && cp <= TRAIL_SURROGATE_MAX);
    }

    bool is_code_point_valid(char32_t cp) {
        return (cp <= CODE_POINT_MAX && !is_utf16_surrogate(cp));
    }

    static constexpr bool is_in_bmp(char32_t cp) {
        return cp < U'\U00010000';
    }

    static constexpr bool is_overlong_sequence(const char32_t cp, const std::size_t length) {
        if (cp < 0x80) {
            if (length != 1) 
                return true;
        } else if (cp < 0x800) {
            if (length != 2) 
                return true;
        } else if (cp < 0x10000) {
            if (length != 3) 
                return true;
        }
        return false;
    }

    std::tuple<char32_t, std::u8string_view::iterator, UTF_ERROR>
    decode_next_utf8(std::u8string_view::iterator begin_it, std::u8string_view::iterator end_it) {
        const std::u8string_view::difference_type max_length = end_it - begin_it;
        if (max_length < 1)
            return std::make_tuple(0, begin_it, UTF_ERROR::INCOMPLETE_SEQUENCE);

        // Actual decoding
        ptrdiff_t length{0};
        char32_t code_point{0};
        auto it{begin_it};
        if ((*it) < 0x80) {
            return std::make_tuple(*it++, it, UTF_ERROR::OK);
        } else if (((*it) >> 5) == 0x6) {
            length = 2;
            if (length > max_length)
                return std::make_tuple(0, begin_it, UTF_ERROR::INCOMPLETE_SEQUENCE);
            code_point = ((*it++ << 6) & 0x7ff) + (*it++ & 0x3f);
        } else if (((*it) >> 4) == 0xe) {
            length = 3;
            if (length > max_length)
                return std::make_tuple(0, begin_it, UTF_ERROR::INCOMPLETE_SEQUENCE);
            code_point = ((*it++ << 12) & 0xffff) + ((*it++ << 6) & 0xfff) + (*it++ & 0x3f);
        } else if ((*it >> 3) == 0x1e) {
            length = 4;
            if (length > max_length)
                return std::make_tuple(0, begin_it, UTF_ERROR::INCOMPLETE_SEQUENCE);
            code_point = ((*it++ << 18) & 0x1fffff) + ((*it++ << 12) & 0x3ffff) +
                            ((*it++ << 6) & 0xfff) + (*it++ & 0x3f);
        } else {
            return std::make_tuple(0, begin_it, UTF_ERROR::INVALID_LEAD);
        }

        // Decoding succeeded. Now, security checks...
        if (!is_code_point_valid(code_point))
            return std::make_tuple(code_point, begin_it, UTF_ERROR::INVALID_CODE_POINT);
        if (is_overlong_sequence(code_point, length))
            return std::make_tuple(code_point, begin_it, UTF_ERROR::OVERLONG_SEQUENCE);

        // Success!
        return std::make_tuple(code_point, it, UTF_ERROR::OK);
    }

    static void add_capacity_if_needed(std::u8string& str, const std::size_t additional_size) {
        const std::size_t desired_size = str.size() + additional_size;
        if (str.capacity() < desired_size)
            str.reserve(desired_size);
    }

    UTF_ERROR encode_next_utf8(const char32_t code_point, std::u8string& utf8str) {
        if (!is_code_point_valid(code_point))
            return UTF_ERROR::INVALID_CODE_POINT;

        if (code_point < 0x80) {                     // 1 byte
            utf8str.append(1u ,static_cast<char8_t>(code_point));
        } else if (code_point < 0x800) {             // 2 bytes
            add_capacity_if_needed(utf8str, 2);
            utf8str.append(1, static_cast<char8_t>((code_point >> 6)          | 0xc0));
            utf8str.append(1, static_cast<char8_t>((code_point & 0x3f)        | 0x80));
        } else if (code_point < 0x10000) {           // 3 bytes
            add_capacity_if_needed(utf8str, 3);
            utf8str.append(1, static_cast<char8_t>((code_point >> 12)         | 0xe0));
            utf8str.append(1, static_cast<char8_t>(((code_point >> 6) & 0x3f) | 0x80));
            utf8str.append(1, static_cast<char8_t>((code_point & 0x3f)        | 0x80));
        } else {                                     // 4 bytes
            add_capacity_if_needed(utf8str, 4);
            utf8str.append(1, static_cast<char8_t>((code_point >> 18)         | 0xf0));
            utf8str.append(1, static_cast<char8_t>(((code_point >> 12) & 0x3f)| 0x80));
            utf8str.append(1, static_cast<char8_t>(((code_point >> 6) & 0x3f) | 0x80));
            utf8str.append(1, static_cast<char8_t>((code_point & 0x3f)        | 0x80));
        }

        return UTF_ERROR::OK;
    }

    std::tuple<char32_t, std::u16string_view::iterator, UTF_ERROR>
    decode_next_utf16(std::u16string_view::iterator begin_it,
                      std::u16string_view::iterator end_it) {
        if (begin_it >= end_it)
            return std::make_tuple(0, begin_it, UTF_ERROR::INCOMPLETE_SEQUENCE);
        auto it{begin_it};
        const char16_t first_word = *it++;
        if (!is_utf16_surrogate(first_word)) {
            return std::make_tuple(first_word, it, UTF_ERROR::OK);
        } else {
            if (begin_it == end_it)
                return std::make_tuple(0, begin_it, UTF_ERROR::INCOMPLETE_SEQUENCE);
            else if (is_utf16_lead_surrogate(first_word)) {
                const char16_t second_word = *it++;
                if (is_utf16_trail_surrogate(second_word)) {
                    const char32_t code_point = (first_word << 10) + second_word + SURROGATE_OFFSET;
                    return std::make_tuple(code_point, it, UTF_ERROR::OK);
                } else {
                  return std::make_tuple(0, begin_it, UTF_ERROR::INCOMPLETE_SEQUENCE);
                }
            } else {
              return std::make_tuple(0, begin_it, UTF_ERROR::INVALID_LEAD);
            }
        }
    }


    UTF_ERROR encode_next_utf16(const char32_t code_point, std::u16string& utf16str) {
        if (!is_code_point_valid(code_point))
            return UTF_ERROR::INVALID_CODE_POINT;
        if (is_in_bmp(code_point))
            utf16str.append(1, static_cast<char16_t>(code_point));
        else {
            // Code points from the supplementary planes are encoded via surrogate pairs
            utf16str.append(1, static_cast<char16_t>(LEAD_OFFSET + (code_point >> 10)));
            utf16str.append(1, static_cast<char16_t>(TRAIL_SURROGATE_MIN + (code_point & 0x3FF)));
        }
        return UTF_ERROR::OK;
    }

} // namespace utfcpp::internal
