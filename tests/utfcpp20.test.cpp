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
#include "ftest.h"

TEST(UtfTests, test_append_to_utf8)
{
    using namespace utfcpp;
    std::u8string str;
    append_to_utf8(str, U'a');
    EXPECT_EQ(str, u8"a");

    append_to_utf8(str, U'л');
    EXPECT_EQ(str, u8"aл");
}

TEST(UtfTests, test_append_to_utf16)
{
    using namespace utfcpp;
    std::u16string str;
    append_to_utf16(str, U'a');
    EXPECT_EQ(str, u"a");

    append_to_utf16(str, U'л');
    EXPECT_EQ(str, u"aл");
}

TEST(UtfTests, test_utf8_to_16)
{
    using namespace utfcpp;
    EXPECT_EQ(utf8_to_16(u8"aл"), u"aл");

    // \xfa is invalid (unexpected continuation byte)
    const char utf8_invalid[] = "\xe6\x97\xa5\xd1\x88\xfa";
    std::u8string_view invalid_view(reinterpret_cast<const char8_t*>(utf8_invalid), strlen(utf8_invalid));
    EXPECT_THROW(utf8_to_16(invalid_view), decoding_error);
}

TEST(UtfTests, test_utf16_to_8)
{
    using namespace utfcpp;
    EXPECT_EQ(utf16_to_8(u"aл"), u8"aл");

    // 0xdd1e is invalid (unexpected tail surrogate)
    const char16_t utf16_invalid[] = {0x41, 0x0448, 0x65e5, 0xdd1e};
    std::u16string_view invalid_view(reinterpret_cast<const char16_t*>(utf16_invalid), 4);
    EXPECT_THROW(utf16_to_8(invalid_view), decoding_error);
}

TEST(UtfTests, test_invalid_utf8)
{
    using namespace utfcpp;
    // \xfa is invalid (unexpected continuation byte)
    const char utf8_invalid[] = "\xe6\x97\xa5\xd1\x88\xfa";
    std::u8string_view invalid_view(reinterpret_cast<const char8_t*>(utf8_invalid), strlen(utf8_invalid));
    EXPECT_EQ(find_invalid(invalid_view), 5);
}

TEST(UtfTests, test_invalid_utf16)
{
    using namespace utfcpp;
    // 0xdd1e is invalid (unexpected tail surrogate)
    const char16_t utf16_invalid[] = {0x41, 0x0448, 0x65e5, 0xdd1e};
    std::u16string_view invalid_view(reinterpret_cast<const char16_t*>(utf16_invalid), 4);
    EXPECT_EQ(find_invalid(invalid_view), 3);
}
