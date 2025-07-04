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
    const char* exception::what() const noexcept {
        return "Error from utfcpp20 library";
    }

    void append_to_utf8(std::u8string& utf8string, char32_t code_point) {
        internal::encode_next_utf8(code_point, utf8string);
    }

    void append_to_utf16(std::u16string& utf16string, char32_t code_point) {
        internal::encode_next_utf16(code_point, utf16string);
    }

    std::u16string utf8_to_16(std::u8string_view utf8_string) {
        auto it{utf8_string.begin()}, end_it{utf8_string.end()};
        std::u16string ret16;
        ret16.reserve(internal::estimate16(utf8_string));
        try {
            while (it != end_it)
                append_to_utf16(ret16, internal::decode_next_utf8(it, end_it));
        } catch (const exception& e) {
            size_t pos = static_cast<size_t>(std::distance(utf8_string.begin(), it));
            throw exception_with_position(pos, e.what());
        }
        return ret16;
    }

    std::u8string utf16_to_8(std::u16string_view utf16_string) {
        auto it{utf16_string.begin()}, end_it{utf16_string.end()};
        std::u8string ret8;
        const size_t u8_length = internal::estimate8(utf16_string);
        ret8.reserve(u8_length);
        try {
            while(it != end_it)
                append_to_utf8(ret8, internal::decode_next_utf16(it, end_it));
        } catch (const exception& e) {
            size_t pos = static_cast<size_t>(std::distance(utf16_string.begin(), it));
            throw exception_with_position(pos, e.what());
        }
        return ret8;
    }


    // Class u8_iterator

    /* static */ u8_iterator u8_iterator::begin(std::u8string_view str_view) {
      return u8_iterator(str_view.begin(), str_view.end());
    }

    /* static */ u8_iterator u8_iterator::end(std::u8string_view str_view) {
      return u8_iterator(str_view.end(), str_view.end());
    }

    u8_iterator::u8_iterator(std::u8string_view::iterator begin,
        std::u8string_view::iterator end): 
        it{begin}, end_it{end}
    {}

    char32_t u8_iterator::operator * () const {
        std::u8string_view::iterator temp_it{ it };
        return internal::decode_next_utf8(temp_it, end_it);
    }

    u8_iterator& u8_iterator::operator ++() {
        internal::decode_next_utf8(it, end_it);
        return *this;
    }

    u8_iterator u8_iterator::operator ++(int) {
        u8_iterator temp {*this};
        internal::decode_next_utf8(it, end_it);
        return temp;
    }
} // namespace utfcpp20
