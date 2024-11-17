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

TEST(CoreTests, test_decode_next_utf8)
{
    using namespace utfcpp::internal;

    const std::u8string_view ascii{u8"abcdxyz"};
    auto [cp, next_cp] = decode_next_utf8(ascii.begin(), ascii.end());
    EXPECT_EQ(cp, U'a');
    EXPECT_EQ(next_cp - ascii.begin(), 1);

    const std::u8string_view cyrillic{u8"шницла"}; // "steak"
    std::tie(cp, next_cp) = decode_next_utf8(cyrillic.begin(), cyrillic.end());
    EXPECT_EQ(cp, U'ш');

    const std::u8string_view chinese {u8"水手"}; // "sailor"
    std::tie(cp, next_cp) = decode_next_utf8(chinese.begin(), chinese.end());
    EXPECT_EQ(cp, U'水');

    const std::u8string_view etruscan {u8"𐌀"};
    std::tie(cp, next_cp) = decode_next_utf8(etruscan.begin(), etruscan.end());
    EXPECT_EQ(cp, U'𐌀');
}

TEST(CoreTests, test_encode_next_utf8)
{
    using namespace utfcpp::internal;
    std::u8string utf8str;
    encode_next_utf8(U'a', utf8str);
    EXPECT_EQ(utf8str, u8"a");

    utf8str.clear();
    encode_next_utf8(U'ц', utf8str);
    EXPECT_EQ(utf8str, u8"ц");

    utf8str.clear();
    encode_next_utf8(U'水', utf8str);
    EXPECT_EQ(utf8str, u8"水");

    utf8str.clear();
    encode_next_utf8(U'𐌀', utf8str);
    EXPECT_EQ(utf8str, u8"𐌀");
}

TEST(CoreTests, test_decode_next_utf16)
{
    using namespace utfcpp::internal;

    std::u16string_view ascii{u"abcdxyz"};
    auto [cp, next_cp] = decode_next_utf16(ascii.begin(), ascii.end());
    EXPECT_EQ(cp, U'a');

    std::u16string_view cyrillic {u"шницла"}; // "steak"
    std::tie(cp, next_cp) = decode_next_utf16(cyrillic.begin(), cyrillic.end());
    EXPECT_EQ(cp, U'ш');

    std::u16string_view etruscan {u"𐌀"};
    std::tie(cp, next_cp) = decode_next_utf16(etruscan.begin(), etruscan.end());
    EXPECT_EQ(cp, U'𐌀');
    EXPECT_EQ(next_cp, etruscan.end());
}

TEST(CoreTests, test_estimate16)
{
    using namespace utfcpp::internal;
    const std::u8string_view ascii{ u8"abcdxyz" };
    EXPECT_EQ(estimate16(ascii), 7);

    const std::u8string_view cyrillic{ u8"шницла" }; // "steak"
    EXPECT_EQ(estimate16(cyrillic), 6);

    const std::u8string_view chinese{ u8"水手" }; // "sailor"
    EXPECT_EQ(estimate16(chinese), 2);

    const std::u8string_view etruscan{ u8"𐌀" };
    EXPECT_EQ(estimate16(etruscan), 2);
}

TEST(CoreTests, test_estimate8)
{
    using namespace utfcpp::internal;
    EXPECT_EQ(estimate8(u"A valid ascii string"), 20);
    EXPECT_EQ(estimate8(u"Ћирилица"), 16);
    EXPECT_EQ(estimate8(u"水手"), 6);
    EXPECT_EQ(estimate8(u"𐌀"), 4);
}
