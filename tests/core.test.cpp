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

    EXPECT_TRUE(is_utf8_trail('\x80'));
    EXPECT_TRUE(is_utf8_trail('\x99'));
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
    EXPECT_FALSE(is_utf16_surrogate(u'z'));
    EXPECT_FALSE(is_utf16_surrogate(u'\ud7ff'));

    EXPECT_TRUE(is_utf16_surrogate(u'\xd800'));
    EXPECT_TRUE(is_utf16_surrogate(u'\xdbff'));
    EXPECT_TRUE(is_utf16_surrogate(u'\xdc00'));
    EXPECT_TRUE(is_utf16_surrogate(u'\xde02'));
    EXPECT_TRUE(is_utf16_surrogate(u'\xdfff'));
    EXPECT_FALSE(is_utf16_surrogate(u'\xe000'));

    EXPECT_FALSE(is_utf16_surrogate(u'\uefff'));
}
