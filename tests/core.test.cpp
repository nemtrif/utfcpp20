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

TEST(CoreTests, test_decode_next_utf8)
{
    using namespace utfcpp::internal;

    const std::u8string_view ascii{u8"abcdxyz"};
    auto next_cp = ascii.begin();
    auto cp = decode_next_utf8(next_cp, ascii.end());
    EXPECT_EQ(cp, U'a');
    EXPECT_EQ(next_cp - ascii.begin(), 1);

    const std::u8string_view cyrillic{u8"шницла"}; // "steak"
    next_cp = cyrillic.begin();
    cp = decode_next_utf8(next_cp, cyrillic.end());
    EXPECT_EQ(cp, U'ш');

    const std::u8string_view chinese {u8"水手"}; // "sailor"
    next_cp = chinese.begin();
    cp = decode_next_utf8(next_cp, chinese.end());
    EXPECT_EQ(cp, U'水');

    const std::u8string_view etruscan {u8"𐌀"};
    next_cp = etruscan.begin();
    cp = decode_next_utf8(next_cp, etruscan.end());
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
    auto next_cp = ascii.begin();
    auto cp = decode_next_utf16(next_cp, ascii.end());
    EXPECT_EQ(cp, U'a');

    std::u16string_view cyrillic {u"шницла"}; // "steak"
    next_cp = cyrillic.begin();
    cp = decode_next_utf16(next_cp, cyrillic.end());
    EXPECT_EQ(cp, U'ш');

    std::u16string_view etruscan {u"𐌀"};
    next_cp = etruscan.begin();
    cp = decode_next_utf16(next_cp, etruscan.end());
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
