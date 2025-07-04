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

#include <algorithm>

TEST(UtfTests, test_append_to_utf8)
{
    std::u8string str;
    utfcpp::append_to_utf8(str, U'a');
    EXPECT_EQ(str, u8"a");

    utfcpp::append_to_utf8(str, U'л');
    EXPECT_EQ(str, u8"aл");
}

TEST(UtfTests, test_append_to_utf16)
{
    std::u16string str;
    utfcpp::append_to_utf16(str, U'a');
    EXPECT_EQ(str, u"a");

    utfcpp::append_to_utf16(str, U'л');
    EXPECT_EQ(str, u"aл");
}

TEST(UtfTests, test_utf8_to_16)
{
    EXPECT_EQ(utfcpp::utf8_to_16(u8"aл"), u"aл");

    // \xfa is invalid (unexpected continuation byte)
    const char utf8_invalid[] = "\xe6\x97\xa5\xd1\x88\xfa";
    std::u8string_view invalid_view(reinterpret_cast<const char8_t*>(utf8_invalid), strlen(utf8_invalid));
    EXPECT_THROW(utfcpp::utf8_to_16(invalid_view), utfcpp::exception);
}

TEST(UtfTests, test_utf16_to_8)
{
    EXPECT_EQ(utfcpp::utf16_to_8(u"aл"), u8"aл");

    // 0xdd1e is invalid (unexpected tail surrogate)
    const char16_t utf16_invalid[] = {0x41, 0x0448, 0x65e5, 0xdd1e};
    std::u16string_view invalid_view(reinterpret_cast<const char16_t*>(utf16_invalid), 4);
    EXPECT_THROW(utfcpp::utf16_to_8(invalid_view), utfcpp::exception);
}

TEST(u8_iteratorTests, test_iterator_construction)
{
    const std::u8string_view empty_view{u8""};
    EXPECT_EQ(utfcpp::u8_iterator::begin(empty_view), utfcpp::u8_iterator::end(empty_view));

    std::u8string_view cyrillic{u8"шницла"};
    EXPECT_NE(utfcpp::u8_iterator::begin(cyrillic), utfcpp::u8_iterator::end(cyrillic));
}

TEST(u8_iteratorTests, test_iterator_dereference)
{
    std::u8string_view cyrillic{u8"шницла"};
    utfcpp::u8_iterator it{utfcpp::u8_iterator::begin(cyrillic)};
    EXPECT_EQ(*it, U'ш');
}

TEST(u8_iteratorTests, test_iterator_iteration)
{
    std::u8string_view cyrillic{u8"шницла"};
    utfcpp::u8_iterator it{utfcpp::u8_iterator::begin(cyrillic)};
    EXPECT_EQ(*(++it), U'н');
    EXPECT_EQ(*(it++), U'н');
    EXPECT_EQ(*it, U'и');
    int counter = 0;
    auto cyr_end{utfcpp::u8_iterator::end(cyrillic)};
    for (it = utfcpp::u8_iterator::begin(cyrillic); it != cyr_end; ++it)
      ++counter;
    EXPECT_EQ(counter, 6);
}

TEST(u8_iteratorTests, test_iterator_std_find)
{
    std::u8string_view cyrillic{u8"шницла"};
    utfcpp::u8_iterator cyr_it{utfcpp::u8_iterator::begin(cyrillic)};
    utfcpp::u8_iterator cyr_end{utfcpp::u8_iterator::end(cyrillic)};

    auto it = std::find(cyr_it, cyr_end, U'ц');
    EXPECT_NE(it, cyr_end);

    it = std::find(cyr_it, cyr_end, U'w');
    EXPECT_EQ(it, cyr_end);
}

TEST(u8_iteratorTests, test_invalid_utf8_sequences)
{
    // Overlong encoding for 'A' (should be invalid)
    const char overlong_A[] = {char(0xC1), char(0x81), 0};
    std::u8string_view overlong_view(reinterpret_cast<const char8_t*>(overlong_A));
    EXPECT_THROW({
        utfcpp::u8_iterator it = utfcpp::u8_iterator::begin(overlong_view);
        (void)*it;
    }, utfcpp::exception);

    // Unexpected continuation byte
    const char cont_byte[] = {char(0x80), 0};
    std::u8string_view cont_view(reinterpret_cast<const char8_t*>(cont_byte));
    EXPECT_THROW({
        utfcpp::u8_iterator it = utfcpp::u8_iterator::begin(cont_view);
        (void)*it;
    }, utfcpp::exception);

    // Truncated multi-byte sequence
    const char truncated[] = {char(0xE2), char(0x82), 0};
    std::u8string_view truncated_view(reinterpret_cast<const char8_t*>(truncated));
    EXPECT_THROW({
        utfcpp::u8_iterator it = utfcpp::u8_iterator::begin(truncated_view);
        (void)*it;
    }, utfcpp::exception);

    // Invalid code point (surrogate half)
    const char surrogate[] = {char(0xED), char(0xA0), char(0x80), 0};
    std::u8string_view surrogate_view(reinterpret_cast<const char8_t*>(surrogate));
    EXPECT_THROW({
        utfcpp::u8_iterator it = utfcpp::u8_iterator::begin(surrogate_view);
        (void)*it;
    }, utfcpp::exception);

    // Check conversion throws as well
    EXPECT_THROW(utfcpp::utf8_to_16(overlong_view), utfcpp::exception);
    EXPECT_THROW(utfcpp::utf8_to_16(cont_view), utfcpp::exception);
    EXPECT_THROW(utfcpp::utf8_to_16(truncated_view), utfcpp::exception);
    EXPECT_THROW(utfcpp::utf8_to_16(surrogate_view), utfcpp::exception);
}


