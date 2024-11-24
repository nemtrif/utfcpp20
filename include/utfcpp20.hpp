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

#ifndef uftcpp20_H_de558932_1371_4b17_a2e1_ceaad0fcb1cd
#define uftcpp20_H_de558932_1371_4b17_a2e1_ceaad0fcb1cd

#include <string>
#include <string_view>

/**
 * \namespace utfcpp
 */
namespace utfcpp {
/**
 * \brief Base class for the exceptions that may be thrown from the library.
 * 
 * All encoding/decoding errors will result in an exception of this type.
 * Use `what` function to get a description.
 */
    class exception : public ::std::exception {
    public:
        /**
         * \brief Returns the description string.
         * 
         * The null-terminated string with the description of the error that caused the exception.
         * 
         * \return Pointer to a C-style string with the description of the error.
         * The memory is owned by the exception object.
         */
        const char* what() const noexcept override;
    };

    /**
     * \brief Appends a code point to a UTF-8 string.
     * 
     * Encodes the code pont as UTF-8 and appends it to the UTF-8 string.
     * 
     * \param utf8string A UTF-8 encoded string to which a new code point will be appended.
     * \param code_point Code point to encode as UTF-8 and append to the string.
     */
    void append_to_utf8(std::u8string& utf8string, char32_t code_point);

    /**
     * \brief Appends a code point to a UTF-16 string.
     * 
     * Encodes the code pont as UTF-16 and appends it to the UTF-16 string.
     * 
     * \param utf16string A UTF-16 encoded string to which a new code point will be appended.
     * \param code_point Code point to encode as UTF-16 and append to the string.
     */
    void append_to_utf16(std::u16string& utf16string, char32_t code_point);

    /**
     * \brief Converts a UTF-8 encoded string to UTF-16.
     * 
     * Decodes a UTF-8 string and encodes its content as UTF-16.
     * 
     * \param utf8_string A view to a UTF-8 encoded string to convert to UTF-16.
     * \return A UTF-16 encoded string.
     */
    std::u16string utf8_to_16(std::u8string_view utf8_string);

    /**
     * \brief Converts a UTF-16 encoded string to UTF-8.
     * 
     * Decodes a UTF-16 string and encodes its content as UTF-8.
     * 
     * \param utf16_string A view to a UTF-8 encoded string to convert to UTF-16.
     * \return A UTF-16 encoded string.
     */
    std::u8string  utf16_to_8(std::u16string_view utf16_string);

/// \file

/**
 * \brief Class for iterating over a UTF-8 encoded string.
 * 
 * The class is an input iterator that holds references to the current position and end of 
 * the UTF-8 encoded sequence.
 */
    class u8_iterator {
    public:
        using value_type        = char32_t;
        using pointer           = char32_t*;
        using reference         = char32_t&;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
    public:
        /**
         * \brief Returns a `u8_iterator` to the beginning of the given UTF-8 string view.
         * 
         * Given a `std::u8string_view` object, constructs `u8_iterator` to its beginning.
         * 
         * \param str_view A string view referencing a UTF-8 encoded string
         * \return `u8_iterator` pointing to the beginning of the sequence
         */
        static u8_iterator begin(std::u8string_view str_view);
        /**
         * \brief Returns a `u8_iterator` to the end of the given UTF-8 string view.
         * 
         * Given a `std::u8string_view` object, constructs `u8_iterator` to its end.
         * 
         * \param str_view A string view referencing a UTF-8 encoded string
         * \return `u8_iterator` pointing to the end of the sequence
         */
        static u8_iterator end(std::u8string_view str_view);
        /**
         * \brief Decodes the next utf-8 sequence.
         * 
         * Decodes the sequence the iterator is currently pointing to.
         * 
         * \return The code point of the current UTF-8 sequence.
         */
        char32_t operator *() const;
        /**
         * \brief The three way comparison operator.
         * 
         * Compares two `u8_iterator` objects.
         * 
         * \param it An iterator to compare with.
         * \return Zero if the two iterators are equal.
         */
        auto operator <=>(const u8_iterator&) const = default;
        /**
         * \brief The prefix increment.
         * 
         * Increments the iterator to point to the next code point.
         * 
         * \return Reference to the incremented iterator.
         */
        u8_iterator& operator ++();
        /**
         * \brief The postfix increment.
         * 
         * Increments the iterator to point to the next code point and returns the current one.
         * 
         * \return The original value of the iterator.
         */
        u8_iterator  operator ++(int);
    private:
        u8_iterator(std::u8string_view::iterator begin,
                    std::u8string_view::iterator end);
        std::u8string_view::iterator it;
        std::u8string_view::iterator end_it;
    };
} // namespace utfcpp20

#endif // uftcpp20_H_de558932_1371_4b17_a2e1_ceaad0fcb1cd
