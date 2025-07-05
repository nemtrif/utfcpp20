# utfcpp20

This is a simplified and incomplete version of the [utfcpp](https://github.com/nemtrif/utfcpp) library. utfcpp was initially developed in 2006 when the current standard was C++03 and although I added some modern C++ features on top of it as an option, I wondered how the library would look like if I could start with C++20 language from beginning.

Compared to utfcpp, utfcpp20:
- Takes advantage of modern char/string types
- Is not template based
- Offers a subset of the original functionality: there are no `is_valid` functions, iteration is simplified, error reporting is via exception only, etc.

At this point the API is not stable. You are welcome to test it out but I would not recommend using it in production yet.

## API Reference

See [API_REFERENCE.md](API_REFERENCE.md) for a detailed description of the public API, exception classes, and iterator usage.
