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
    EXPECT_FALSE(is_valid(invalid_view));

    std::u8string_view valid_view(u8"aл");
    EXPECT_EQ(find_invalid(valid_view), std::u8string_view::npos);
    EXPECT_TRUE(is_valid(valid_view));
}

TEST(UtfTests, test_invalid_utf16)
{
    using namespace utfcpp;
    // 0xdd1e is invalid (unexpected tail surrogate)
    const char16_t utf16_invalid[] = {0x41, 0x0448, 0x65e5, 0xdd1e};
    std::u16string_view invalid_view(reinterpret_cast<const char16_t*>(utf16_invalid), 4);
    EXPECT_EQ(find_invalid(invalid_view), 3);

    std::u16string_view valid_view(u"aл");
    EXPECT_EQ(find_invalid(valid_view), std::u16string_view::npos);
    EXPECT_TRUE(is_valid(valid_view));
}

TEST(u8_iteratorTests, test_iterator_construction)
{
    using namespace utfcpp;
    const std::u8string_view empty_view{u8""};
    u8_iterator itempty{empty_view};
    EXPECT_EQ(itempty, empty_view.end());

    std::u8string_view cyrillic{u8"шницла"};
    u8_iterator it{cyrillic};
    EXPECT_NE(it, cyrillic.end());

    u8_iterator end_cyr{cyrillic.end()};
    EXPECT_EQ(end_cyr, cyrillic.end());
}

TEST(u8_iteratorTests, test_iterator_dereference)
{
    using namespace utfcpp;

    u8_iterator it{u8"шницла"};
    EXPECT_EQ(*it, U'ш');
}

TEST(u8_iteratorTests, test_iterator_iteration)
{
    using namespace utfcpp;

    std::u8string_view cyrillic{u8"шницла"};
    u8_iterator it{cyrillic};
    EXPECT_EQ(*(++it), U'н');
    EXPECT_EQ(*(it++), U'н');
    EXPECT_EQ(*it, U'и');
    int counter = 0;
    for (it = cyrillic; it != cyrillic.end(); ++it)
      ++counter;
    EXPECT_EQ(counter, 6);
}

TEST(u8_iteratorTests, test_iterator_std_find)
{
    using namespace utfcpp;

    std::u8string_view cyrillic{u8"шницла"};
    u8_iterator cyr_it{cyrillic};
    u8_iterator cyr_end{cyrillic.end()};

    auto it = std::find(cyr_it, cyr_end, U'ц');
    EXPECT_NE(it, cyr_end);

    it = std::find(cyr_it, cyr_end, U'w');
    EXPECT_EQ(it, cyr_end);
}


