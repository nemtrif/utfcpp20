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

#ifndef core_H_de558932_1371_4b17_a2e1_ceaad0fcb1cd
#define core_H_de558932_1371_4b17_a2e1_ceaad0fcb1cd

#include <string_view>
#include <string>
#include <cstddef> // std::size_t

namespace utfcpp::internal
{
    // Unicode constants

    // Leading (high) surrogates: d800 - dbff
    // Trailing (low) surrogates: dc00 - dfff
    constexpr char16_t LEAD_SURROGATE_MIN  {u'\xd800'};
    constexpr char16_t LEAD_SURROGATE_MAX  {u'\xdbff'};
    constexpr char16_t TRAIL_SURROGATE_MIN {u'\xdc00'};
    constexpr char16_t TRAIL_SURROGATE_MAX {u'\xdfff'};
    constexpr char32_t LEAD_OFFSET         {U'\xd7c0'};       // LEAD_SURROGATE_MIN - (0x10000 >> 10)
    constexpr char32_t SURROGATE_OFFSET    {U'\xfca02400'};   // 0x10000u - (LEAD_SURROGATE_MIN << 10) - TRAIL_SURROGATE_MIN

    // Maximum valid value for a Unicode code point
    constexpr char32_t CODE_POINT_MAX      {U'\U0010ffff'};

    // Replacement character
    constexpr char32_t REPLACEMENT_CHARACTER {U'\ufffd'};

    bool is_utf8_trail(char8_t ch);

    bool is_utf16_lead_surrogate(char16_t cp);
    bool is_utf16_trail_surrogate(char16_t cp);
    bool is_utf16_surrogate(char32_t cp);

    bool is_code_point_valid(char32_t cp);

    std::size_t sequence_length(char8_t lead_byte);

    enum class UTF_ERROR {OK, NOT_ENOUGH_ROOM, INVALID_LEAD, INCOMPLETE_SEQUENCE, OVERLONG_SEQUENCE, INVALID_CODE_POINT};

    UTF_ERROR decode_next_utf8(std::u8string_view& utf8str, char32_t& code_point);
    UTF_ERROR encode_next_utf8(const char32_t code_point, std::u8string& utf8str);

}  // namespace utfcpp::internal

#endif // core_H_de558932_1371_4b17_a2e1_ceaad0fcb1cd
