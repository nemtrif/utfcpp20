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
}
