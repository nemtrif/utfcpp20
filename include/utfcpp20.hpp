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

#ifndef uftcpp20_H_de558932_1371_4b17_a2e1_ceaad0fcb1cd
#define uftcpp20_H_de558932_1371_4b17_a2e1_ceaad0fcb1cd

#include <string>
#include <string_view>

namespace utfcpp {
    // Base for the exceptions that may be thrown from the library
    class exception : public ::std::exception {
    };

    class encoding_error : public utfcpp::exception {
    };

    class decoding_error : public utfcpp::exception {
    };

    void append_to_utf8(std::u8string& utf8string, char32_t code_point);
    void append_to_utf16(std::u16string& utf16string, char32_t code_point);

    std::u16string utf8_to_16(std::u8string_view utf8_string);
    std::u8string  utf16_to_8(std::u16string_view utf16_string);

    std::u8string_view::iterator find_invalid(std::u8string_view utf8_string);
    std::u16string_view::iterator find_invalid(std::u16string_view utf16_string);

    bool is_valid(std::u8string_view  utf8_string);
    bool is_valid(std::u16string_view utf16_string);

    class u8_iterator {
    public:
        using value_type        = char32_t;
        using pointer           = char32_t*;
        using reference         = char32_t&;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
    public:
        static u8_iterator begin(std::u8string_view str_view);
        static u8_iterator end(std::u8string_view str_view);
        char32_t operator *() const;
        auto operator <=>(const u8_iterator&) const = default;
        u8_iterator& operator ++();
        u8_iterator  operator ++(int);
    private:
        u8_iterator(std::u8string_view::iterator begin,
                    std::u8string_view::iterator end);
        std::u8string_view::iterator it;
        std::u8string_view::iterator end_it;
    };
} // namespace utfcpp20

#endif // uftcpp20_H_de558932_1371_4b17_a2e1_ceaad0fcb1cd
