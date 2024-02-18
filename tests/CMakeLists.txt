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

include(FetchContent)

FetchContent_Declare(
  ftest

  GIT_REPOSITORY https://github.com/nemtrif/ftest.git
  GIT_TAG        0.2)

FetchContent_MakeAvailable(ftest)

include(CTest)
add_library (ftest INTERFACE)
target_include_directories(ftest INTERFACE ${ftest_SOURCE_DIR})


add_executable(utfcpp20test utfcpp20.test.cpp)
target_link_libraries(utfcpp20test PRIVATE utfcpp20)
target_link_libraries(utfcpp20test PRIVATE ftest)
set_target_properties(utfcpp20test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

add_executable(coretest core.test.cpp)
target_include_directories(coretest PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(coretest PRIVATE utfcpp20)
target_link_libraries(coretest PRIVATE ftest)
set_target_properties(coretest PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

add_test(coretest coretest)
