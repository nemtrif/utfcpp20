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

#include "utfcpp20.hpp"
#include "core.hpp"

namespace utfcpp {

    void append_to_utf8(std::u8string& utf8string, char32_t code_point) {
        const internal::UTF_ERROR status = internal::encode_next_utf8(code_point, utf8string);
        if (status != internal::UTF_ERROR::OK)
            throw encoding_error();
    }

    void append_to_utf16(std::u16string& utf16string, char32_t code_point) {
        const internal::UTF_ERROR status = internal::encode_next_utf16(code_point, utf16string);
        if (status != internal::UTF_ERROR::OK)
            throw encoding_error();
    }

    std::u16string utf8_to_16(std::u8string_view utf8_string) {
        std::u8string_view remainder(utf8_string);
        std::u16string ret16;
        // TODO: pre-allocate return string in a smarter way
        ret16.reserve(utf8_string.length());
        while(!remainder.empty()) {
            char32_t next32char;
            const internal::UTF_ERROR status = internal::decode_next_utf8(
                remainder, next32char);
            if (status != internal::UTF_ERROR::OK)
                throw decoding_error();
            append_to_utf16(ret16, next32char);
        }
        return ret16;
    }

    std::u8string utf16_to_8(std::u16string_view utf16_string) {
        std::u16string_view remainder(utf16_string);
        std::u8string ret8;
        // TODO: pre-allocate return string in a smarter way
        ret8.reserve(utf16_string.length());
        while(!remainder.empty()) {
            char32_t next32char;
            const internal::UTF_ERROR status = internal::decode_next_utf16(
                remainder, next32char);
            if (status != internal::UTF_ERROR::OK)
                throw decoding_error();
            append_to_utf8(ret8, next32char);
        }
        return ret8;
    }

    std::u8string_view::size_type find_invalid(std::u8string_view utf8_string) {
        std::u8string_view remainder(utf8_string);
        while (!remainder.empty()) {
            char32_t ignore;
            const internal::UTF_ERROR status = internal::decode_next_utf8(
                remainder, ignore);
            if (status != internal::UTF_ERROR::OK)
                return (utf8_string.length() - remainder.length());
        }
        return std::u8string_view::npos;
    }

    std::u16string_view::size_type find_invalid(std::u16string_view utf16_string) {
        std::u16string_view remainder(utf16_string);
        while (!remainder.empty()) {
            char32_t ignore;
            const internal::UTF_ERROR status = internal::decode_next_utf16(
                remainder, ignore);
            if (status != internal::UTF_ERROR::OK)
                return (utf16_string.length() - remainder.length());
        }
        return std::u16string_view::npos;
    }

} // namespace utfcpp20
