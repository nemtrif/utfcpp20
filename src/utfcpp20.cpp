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
        auto it{utf8_string.begin()}, end_it{utf8_string.end()};
        std::u16string ret16;
        // TODO: pre-allocate return string in a smarter way
        ret16.reserve(utf8_string.length());
        while (it != end_it) {
            const auto [next32char, next_cp, status] = internal::decode_next_utf8(it, end_it);
            if (status != internal::UTF_ERROR::OK)
                throw decoding_error();
            append_to_utf16(ret16, next32char);
            it = next_cp;
        }
        return ret16;
    }

    std::u8string utf16_to_8(std::u16string_view utf16_string) {
        auto it{utf16_string.begin()}, end_it{utf16_string.end()};
        std::u8string ret8;
        // TODO: pre-allocate return string in a smarter way
        ret8.reserve(utf16_string.length());
        while(it != end_it) {
            const auto [next32char, next_cp, status] = internal::decode_next_utf16(it, end_it);
            if (status != internal::UTF_ERROR::OK)
                throw decoding_error();
            append_to_utf8(ret8, next32char);
            it = next_cp;
        }
        return ret8;
    }

    std::u8string_view::size_type find_invalid(std::u8string_view utf8_string) {
        auto it{utf8_string.begin()}, end_it{utf8_string.end()};
        while (it != end_it) {
            const auto [ignore, next_cp, status] = internal::decode_next_utf8(it, end_it);
            it = next_cp;
            if (status != internal::UTF_ERROR::OK)
                return (it - utf8_string.begin());
        }
        return std::u8string_view::npos;
    }

    std::u16string_view::size_type find_invalid(std::u16string_view utf16_string) {
        auto it{utf16_string.begin()}, end_it{utf16_string.end()};
        while (it != end_it) {
            const auto [ignore, next_cp, status] = internal::decode_next_utf16(it, end_it);
            it = next_cp;
            if (status != internal::UTF_ERROR::OK)
                return (it - utf16_string.begin());
        }
        return std::u16string_view::npos;
    }

    bool is_valid(std::u8string_view utf8_string) {
        return (find_invalid(utf8_string) == std::u8string_view::npos);
    }

    bool is_valid(std::u16string_view utf16_string) {
        return (find_invalid(utf16_string) == std::u16string_view::npos);
    }

    // Class u8_iterator
    u8_iterator::u8_iterator(std::u8string_view str_view):
        it{str_view.begin()}, end_it{str_view.end()}
    {}
    
    u8_iterator::u8_iterator(std::u8string_view::iterator end_seq_it):
    it{end_seq_it}, end_it{end_seq_it}
    {}

    char32_t u8_iterator::operator * () const {
        auto [code_point, ignore, status] = internal::decode_next_utf8(it, end_it);
        if (status != internal::UTF_ERROR::OK)
            throw decoding_error();
        return code_point;
    }

    u8_iterator& u8_iterator::operator ++() {
        auto [code_point, next_cp, status] = internal::decode_next_utf8(it, end_it);
        if (status != internal::UTF_ERROR::OK)
            throw decoding_error();
        it = next_cp;
        return *this;
    }

    u8_iterator u8_iterator::operator ++(int) {
        u8_iterator temp {*this};
        auto [code_point, next_cp, status] = internal::decode_next_utf8(it, end_it);
        if (status != internal::UTF_ERROR::OK)
            throw decoding_error();
        it = next_cp;
        return temp;
    }
} // namespace utfcpp20
