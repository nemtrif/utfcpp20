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

namespace utfcpp20 {

    void append_to_utf8(std::u8string& utf8string, const char32_t code_point) {
        internal::encode_next_utf8(code_point, utf8string);
    }

    void append_to_utf16(std::u16string& utf16string, const char32_t code_point) {

    }

    std::u16string utf8_to_16(std::u8string_view utf8_string) {
        std::u8string_view remainder(utf_string);
        std::u16string ret16;
        // TODO: pre-allocate return string in a smarter way
        ret16.reserve(utf8_string.length());
        while(!remainder.empty()) {
            char32_t next32char;
            const UTF_ERROR decode_status = internal::decode_next_utf8 (
                remainder, next32char);
            // TODO: handle errors
        }
        return ret16;
    }
} // namespace utfcpp20
