#    Copyright 2024 Nemanja Trifunovic

#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at

#        http://www.apache.org/licenses/LICENSE-2.0

#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${utfcpp20_SOURCE_DIR}/include/*.hpp")

set (src_files
    core.hpp
    core.cpp
    utfcpp20.cpp
    ../include/utfcpp20.hpp
)

add_library(utfcpp20 ${src_files} ${HEADER_LIST})

target_include_directories(utfcpp20 PUBLIC ../include)

set_target_properties(utfcpp20 PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_compile_options(utfcpp20 PUBLIC
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Wconversion>)

