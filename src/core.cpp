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

    int sequence_length(char8_t lead_byte) {
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

    static constexpr bool is_overlong_sequence(char32_t cp, int length)
    {
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



} // namespace utfcpp::internal
