#include "conversion.hpp"

#include <string>

int char_to_channel(char ch) {
    return ch == 'r' ? 2 : ch == 'g' ? 1 : ch == 'b' ? 0 : ch == 'a' ? 3 : -1;
}

uint8_t hex2byte(const char* byte) {
    char buffer[4] = {};

    buffer[0] = byte[0];
    buffer[1] = byte[1];

    return std::stoul(buffer, nullptr, 16);
}