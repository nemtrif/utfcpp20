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

    std::size_t sequence_length(char8_t lead_byte) {
        if (lead_byte < 0x80)
            return 1;
        else if ((lead_byte >> 5) == 0x6)
            return 2;
        else if ((lead_byte >> 4) == 0xe)
            return 3;
        else if ((lead_byte >> 3) == 0x1e)
            return 4;
        else
            return 0;
    }

    static constexpr bool is_overlong_sequence(const char32_t cp, const std::size_t length) {
        if (cp < 0x80) {
            if (length != 1) 
                return true;
        }
        else if (cp < 0x800) {
            if (length != 2) 
                return true;
        }
        else if (cp < 0x10000) {
            if (length != 3) 
                return true;
        }
        return false;
    }

    UTF_ERROR decode_next_utf8(std::u8string_view utf8str, char32_t& code_point) {
        if (utf8str.empty())
            return UTF_ERROR::NOT_ENOUGH_ROOM;

        // Expected byte length of the utf-8 sequence, according to the lead byte
        const std::size_t length = sequence_length(utf8str[0]);

        // Incomplete sequence may mean:
        // 1) utf8str does not contain the required number of bytes, or
        // 2) some of the expected trail bytes have invalid value
        if (utf8str.length() < length)
            return UTF_ERROR::INCOMPLETE_SEQUENCE;
        for (std::size_t i = 1; i < length; ++i)
            if (!is_utf8_trail(utf8str[i]))
                return UTF_ERROR::INCOMPLETE_SEQUENCE;

        // Actual decoding happens here
        switch (length) {
            case 0:
                code_point = REPLACEMENT_CHARACTER;
                return UTF_ERROR::INVALID_LEAD;
            break;
            case 1:
                code_point = utf8str[0];
                return UTF_ERROR::OK;
            break;
            case 2:
                code_point = ((utf8str[0] << 6) & 0x7ff) + (utf8str[1] & 0x3f);
            break;
            case 3:
                code_point = ((utf8str[0] << 12) & 0xffff) + ((utf8str[1] << 6) & 0xfff) + (utf8str[2] & 0x3f);
            break;
            case 4:
                code_point = ((utf8str[0] << 18) & 0x1fffff) + ((utf8str[1] << 12) & 0x3ffff)
                        + ((utf8str[2] << 6) & 0xfff) + (utf8str[4] & 0x3f);
            break;
        }

        // Decoding succeeded. Now, security checks...
        if (!is_code_point_valid(code_point))
            return UTF_ERROR::INVALID_CODE_POINT;
        if(is_overlong_sequence(code_point, length))
            return UTF_ERROR::OVERLONG_SEQUENCE;

        // Success!
        return UTF_ERROR::OK;
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

} // namespace utfcpp::internal
