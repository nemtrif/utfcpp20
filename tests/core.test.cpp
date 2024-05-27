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
#include "ftest.h"

TEST(CoreTests, test_is_utf8_trail)
{
    using namespace utfcpp::internal;
    EXPECT_FALSE(is_utf8_trail('a'));
    EXPECT_FALSE(is_utf8_trail('\x7F'));

    EXPECT_TRUE(is_utf8_trail((unsigned char)'\x80'));
    EXPECT_TRUE(is_utf8_trail((unsigned char)'\x99'));
}

TEST(CoreTests, test_is_utf16_lead_surrogate)
{
    using namespace utfcpp::internal;
    EXPECT_FALSE(is_utf16_lead_surrogate(u'\u0000'));
    EXPECT_FALSE(is_utf16_lead_surrogate(u'z'));
    EXPECT_FALSE(is_utf16_lead_surrogate(u'\ud7ff'));

    EXPECT_TRUE(is_utf16_lead_surrogate(u'\xd800'));
    EXPECT_TRUE(is_utf16_lead_surrogate(u'\xdab0'));
    EXPECT_TRUE(is_utf16_lead_surrogate(u'\xdbff'));

    EXPECT_FALSE(is_utf16_lead_surrogate(u'\xdc00'));
    EXPECT_FALSE(is_utf16_lead_surrogate(u'\xdfff'));

    EXPECT_FALSE(is_utf16_lead_surrogate(u'\uefff'));
}

TEST(CoreTests, test_is_utf16_trail_surrogate)
{
    using namespace utfcpp::internal;
    EXPECT_FALSE(is_utf16_trail_surrogate(u'\u0000'));
    EXPECT_FALSE(is_utf16_trail_surrogate(u'z'));
    EXPECT_FALSE(is_utf16_trail_surrogate(u'\ud7ff'));

    EXPECT_FALSE(is_utf16_trail_surrogate(u'\xd800'));
    EXPECT_FALSE(is_utf16_trail_surrogate(u'\xdbff'));

    EXPECT_TRUE(is_utf16_trail_surrogate(u'\xdc00'));
    EXPECT_TRUE(is_utf16_trail_surrogate(u'\xde02'));
    EXPECT_TRUE(is_utf16_trail_surrogate(u'\xdfff'));
    EXPECT_FALSE(is_utf16_trail_surrogate(u'\xe000'));

    EXPECT_FALSE(is_utf16_lead_surrogate(u'\uefff'));
}

TEST(CoreTests, test_is_utf16_surrogate)
{
    using namespace utfcpp::internal;
    EXPECT_FALSE(is_utf16_surrogate(u'\u0000'));
    EXPECT_FALSE(is_utf16_surrogate(U'z'));
    EXPECT_FALSE(is_utf16_surrogate(u'\ud7ff'));

    EXPECT_TRUE(is_utf16_surrogate(u'\xd800'));
    EXPECT_TRUE(is_utf16_surrogate(u'\xdbff'));
    EXPECT_TRUE(is_utf16_surrogate(u'\xdc00'));
    EXPECT_TRUE(is_utf16_surrogate(u'\xde02'));
    EXPECT_TRUE(is_utf16_surrogate(u'\xdfff'));
    EXPECT_FALSE(is_utf16_surrogate(u'\xe000'));

    EXPECT_FALSE(is_utf16_surrogate(u'\uefff'));
    EXPECT_FALSE(is_utf16_surrogate(U'\U001012af'));
}

TEST(CoreTests, test_is_code_point_valid)
{
    using namespace utfcpp::internal;
    EXPECT_FALSE(is_code_point_valid(U'\xdbff'));
    EXPECT_FALSE(is_code_point_valid(U'\x11ffff'));

    EXPECT_TRUE(is_code_point_valid(U'\x80'));
    EXPECT_TRUE(is_code_point_valid(U'\x99'));
}

TEST(CoreTests, test_sequence_length)
{
    using namespace utfcpp::internal;
    EXPECT_EQ(sequence_length(u8'Z'), 1);
    EXPECT_EQ(sequence_length(u8'\x79'), 1);
    EXPECT_EQ(sequence_length((unsigned char)'\xc2'), 2);
    EXPECT_EQ(sequence_length((unsigned char)'\xe0'), 3);
    EXPECT_EQ(sequence_length((unsigned char)'\xf0'), 4);
}

TEST(CoreTests, test_decode_next_utf8)
{
    using namespace utfcpp::internal;
    char32_t cp = 0;

    std::u8string_view ascii{u8"abcdxyz"};
    EXPECT_EQ(decode_next_utf8(ascii, cp), UTF_ERROR::OK);
    EXPECT_EQ(cp, U'a');
    EXPECT_EQ(ascii, u8"bcdxyz");

    std::u8string_view cyrillic {u8"шницла"}; // "steak"
    EXPECT_EQ(decode_next_utf8(cyrillic, cp), UTF_ERROR::OK);
    EXPECT_EQ(cp, U'ш');
    EXPECT_EQ(cyrillic, u8"ницла");

    std::u8string_view chinese {u8"水手"}; // "sailor"
    EXPECT_EQ(decode_next_utf8(chinese, cp), UTF_ERROR::OK);
    EXPECT_EQ(cp, U'水');
    EXPECT_EQ(chinese, u8"手");

    std::u8string_view etruscan {u8"𐌀"};
    EXPECT_EQ(decode_next_utf8(etruscan, cp), UTF_ERROR::OK);
    EXPECT_EQ(cp, U'𐌀');
    EXPECT_TRUE(etruscan.empty());
}

TEST(CoreTests, test_encode_next_utf8)
{
    using namespace utfcpp::internal;
    std::u8string utf8str;
    EXPECT_EQ(encode_next_utf8(U'a', utf8str), UTF_ERROR::OK);
    EXPECT_EQ(utf8str, u8"a");

    utf8str.clear();
    EXPECT_EQ(encode_next_utf8(U'ц', utf8str), UTF_ERROR::OK);
    EXPECT_EQ(utf8str, u8"ц");

    utf8str.clear();
    EXPECT_EQ(encode_next_utf8(U'水', utf8str), UTF_ERROR::OK);
    EXPECT_EQ(utf8str, u8"水");

    utf8str.clear();
    EXPECT_EQ(encode_next_utf8(U'𐌀', utf8str), UTF_ERROR::OK);
    EXPECT_EQ(utf8str, u8"𐌀");
}

TEST(CoreTests, test_decode_next_utf16)
{
    using namespace utfcpp::internal;
    char32_t cp = 0;

    std::u16string_view ascii{u"abcdxyz"};
    EXPECT_EQ(decode_next_utf16(ascii, cp), UTF_ERROR::OK);
    EXPECT_EQ(cp, U'a');
    EXPECT_EQ(ascii, u"bcdxyz");

    std::u16string_view cyrillic {u"шницла"}; // "steak"
    EXPECT_EQ(decode_next_utf16(cyrillic, cp), UTF_ERROR::OK);
    EXPECT_EQ(cp, U'ш');
    EXPECT_EQ(cyrillic, u"ницла");

    std::u16string_view etruscan {u"𐌀"};
    EXPECT_EQ(decode_next_utf16(etruscan, cp), UTF_ERROR::OK);
    EXPECT_EQ(cp, U'𐌀');
    EXPECT_TRUE(etruscan.empty());
}
