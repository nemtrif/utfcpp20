# utfcpp20 API Reference

This document provides an overview of the public API for the utfcpp20 library. For detailed usage and examples, see the README.md.

---

## Exception Classes

### class `utfcpp::exception` : `std::exception`
Base class for all exceptions thrown by the library.
- `const char* what() const noexcept` — Returns a description of the error.
- `virtual size_t position() const noexcept` — Returns the position in the string where the error occurred, or `npos` if not set.
- `static constexpr size_t npos` — Value indicating no position is set.

### class `utfcpp::exception_with_position` : `utfcpp::exception`
Exception that includes the position of the error.
- `exception_with_position(size_t pos, std::string msg = "UTF error at position")`
- `size_t position() const noexcept override` — Returns the error position.
- `const char* what() const noexcept override` — Returns a message with position info.

---

## Encoding/Decoding Functions

### `void utfcpp::append_to_utf8(std::u8string& utf8string, char32_t code_point)`
Appends a code point to a UTF-8 string.

### `void utfcpp::append_to_utf16(std::u16string& utf16string, char32_t code_point)`
Appends a code point to a UTF-16 string.

### `std::u16string utfcpp::utf8_to_16(std::u8string_view utf8_string)`
Converts a UTF-8 encoded string to UTF-16. Throws `utfcpp::exception_with_position` on error.

### `std::u8string utfcpp::utf16_to_8(std::u16string_view utf16_string)`
Converts a UTF-16 encoded string to UTF-8. Throws `utfcpp::exception_with_position` on error.

---

## UTF-8 Iterator

### class `utfcpp::u8_iterator`
Input iterator for traversing a UTF-8 encoded string.
- `static u8_iterator begin(std::u8string_view str_view)` — Iterator to the beginning.
- `static u8_iterator end(std::u8string_view str_view)` — Iterator to the end.
- `char32_t operator*() const` — Decodes the current UTF-8 sequence.
- `u8_iterator& operator++()` — Prefix increment.
- `u8_iterator operator++(int)` — Postfix increment.
- `auto operator<=>(const u8_iterator&) const = default;` — Three-way comparison.

---

For more details, see the Doxygen-generated HTML documentation in the `doc/html` directory.
