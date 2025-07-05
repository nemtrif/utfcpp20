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
#include "utfcpp20.hpp"

#include <bit>
#include <cstdint>

namespace utfcpp::internal
{
    using u8_diff_type = std::u8string_view::difference_type;

    class internal_decoding_8_error : public utfcpp::exception {
    public:
        internal_decoding_8_error(const char* msg) :
            message(msg)
        {}
        const char * what() const noexcept override
        { return message.c_str(); }
    private:
        std::string message;
    };

    class internal_decoding_16_error : public utfcpp::exception {
    public:
        internal_decoding_16_error(const char* msg) :
            message(msg)
        {}
        const char * what() const noexcept override
        { return message.c_str(); }
    private:
        std::string message;
    };

    class internal_encoding_8_error : public utfcpp::exception {
    public:
        internal_encoding_8_error(const char* msg) :
            message(msg)
        {}
        const char * what() const noexcept override
        { return message.c_str(); }
    private:
        std::string message;
    };

    class internal_encoding_16_error : public utfcpp::exception {
    public:
        internal_encoding_16_error(const char* msg) :
            message(msg)
        {}
    private:
        std::string message;
    };

    // Is the byte a utf-8 trail?
    static constexpr bool 
    is_utf8_trail(char8_t ch) {
        return ((ch >> 6) == 0x2);
    }

    static constexpr bool 
    is_utf16_lead_surrogate(char16_t cp) {
        return (cp >= LEAD_SURROGATE_MIN && cp <= LEAD_SURROGATE_MAX);
    }

    static constexpr bool
    is_utf16_trail_surrogate(char16_t cp) {
        return (cp >= TRAIL_SURROGATE_MIN && cp <= TRAIL_SURROGATE_MAX);
    }

    static constexpr bool 
    is_utf16_surrogate(char32_t cp) {
        return (cp >= LEAD_SURROGATE_MIN && cp <= TRAIL_SURROGATE_MAX);
    }

    static constexpr bool
    is_code_point_valid(char32_t cp) {
        return (cp <= CODE_POINT_MAX && !is_utf16_surrogate(cp));
    }

    static constexpr bool
    is_in_bmp(char32_t cp) {
        return cp < U'\U00010000';
    }


    static constexpr bool
    is_overlong_sequence(const char32_t cp, const u8_diff_type length) {
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

    static constexpr u8_diff_type
    utf8_cp_length(char8_t lead_byte) {
        switch (std::countl_one(uint8_t(lead_byte))) {
            case 0: return 1;
            case 2: return 2;
            case 3: return 3;
            case 4: return 4;
            default: return 0; // invalid lead
        }        
    }

    static constexpr size_t
    utf8_cp_length(char16_t utf16_lead) {
        if (utf16_lead < 0x80)                          return 1;
        else if (utf16_lead < 0x800)                    return 2;
        else if (!is_utf16_surrogate(utf16_lead))       return 3;
        else if (is_utf16_lead_surrogate(utf16_lead))   return 4;
        else                                            return 0; // invalid lead
    }

    size_t estimate16(std::u8string_view utf8str) {
        size_t utf16units{0};
        for (auto c : utf8str) {
            if (is_utf8_trail(c))       continue;
            else if ((c >> 3) == 0x1e)  utf16units += 2;
            else                        utf16units++;
        }
        return utf16units;
    }

    size_t estimate8(std::u16string_view utf16str) {
        size_t utf8_units{0};
        for (auto c : utf16str) {
            if      (!is_utf16_surrogate(c))     utf8_units += utf8_cp_length(c);
            else if (is_utf16_lead_surrogate(c)) utf8_units += 4;
        }
        return utf8_units;
    }

    char32_t decode_next_utf8(std::u8string_view::iterator& it, std::u8string_view::iterator end_it) {
        const u8_diff_type max_length = end_it - it;
        if (max_length < 1)
            throw internal_decoding_8_error("Incomplete sequence");

        // Actual decoding
        char32_t code_point{0};
        const u8_diff_type length{utf8_cp_length(*it)};
        if (length > max_length)
            throw internal_decoding_8_error("Incomplete sequence");
        switch (length) {
        case 1:
            // Shortcut - no need for security checks here
            return static_cast<char32_t>(*it++);
            break;
        case 2:
            code_point = ((*it++ << 6) & 0x7ff);
            code_point += (*it++ & 0x3f);
            break;
        case 3:
            code_point = ((*it++ << 12) & 0xffff);
            code_point += ((*it++ << 6) & 0xfff);
            code_point += (*it++ & 0x3f);
            break;
        case 4:
            code_point = ((*it++ << 18) & 0x1fffff);
            code_point += ((*it++ << 12) & 0x3ffff);
            code_point += ((*it++ << 6) & 0xfff);
            code_point += (*it++ & 0x3f);
            break;
        default:
            throw internal_decoding_8_error("Invalid lead");
        }

        // Decoding succeeded. Now, security checks...
        if (!is_code_point_valid(code_point))
            throw internal_decoding_8_error("Invalid code point");
        if (is_overlong_sequence(code_point, length))
            throw internal_decoding_8_error("Overlong sequence");

        // Success!
        return code_point;
    }

    static void add_capacity_if_needed(std::u8string& str, const std::size_t additional_size) {
        const std::size_t desired_size = str.size() + additional_size;
        if (str.capacity() < desired_size)
            str.reserve(desired_size);
    }

    void encode_next_utf8(const char32_t code_point, std::u8string& utf8str) {
        if (!is_code_point_valid(code_point))
            throw internal_encoding_8_error("Invalid code point");

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
    }

    char32_t decode_next_utf16(std::u16string_view::iterator& it, std::u16string_view::iterator end_it) {
        if (it >= end_it)
            throw internal_decoding_16_error("Incomplete sequence");
        const char16_t first_word = static_cast<char16_t>(*it);
        if (!is_utf16_surrogate(first_word)) {
            ++it;
            return first_word;
        } else {
            if (!is_utf16_lead_surrogate(first_word))
                throw internal_decoding_16_error("Invalid lead");
            ++it;
            if (it >= end_it)
                throw internal_decoding_16_error("Incomplete sequence");
            const char16_t second_word = static_cast<char16_t>(*it);
            if (!is_utf16_trail_surrogate(second_word))
                throw internal_decoding_16_error("Incomplete sequence");
            ++it;
            const char32_t code_point = (static_cast<char32_t>(first_word - LEAD_SURROGATE_MIN) << 10)
                                      + (static_cast<char32_t>(second_word - TRAIL_SURROGATE_MIN))
                                      + 0x10000;
            return code_point;
        }
    }

    void encode_next_utf16(const char32_t code_point, std::u16string& utf16str) {
        if (!is_code_point_valid(code_point))
            throw internal_encoding_16_error("Invalid code point");
        if (is_in_bmp(code_point))
            utf16str.append(1, static_cast<char16_t>(code_point));
        else {
            // Code points from the supplementary planes are encoded via surrogate pairs
            utf16str.append(1, static_cast<char16_t>(LEAD_OFFSET + (code_point >> 10)));
            utf16str.append(1, static_cast<char16_t>(TRAIL_SURROGATE_MIN + (code_point & 0x3FF)));
        }
    }

} // namespace utfcpp::internal
