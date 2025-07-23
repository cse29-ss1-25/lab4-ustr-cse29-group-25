#include <stdint.h>
#include "pset1.h"

uint8_t is_ascii(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        if ((unsigned char)str[i] > 127) {
            return 0;
        }
    }
    return 1;
}

uint8_t is_continuation_byte(unsigned char byte) {
    return (byte & 0xC0) == 0x80;
}

int8_t utf8_codepoint_size(char c) {
    unsigned char first_byte = (unsigned char)c;
    if (first_byte < 0x80) {
        return 1;
    } else if ((first_byte & 0xE0) == 0xC0) {
        return 2;
    } else if ((first_byte & 0xF0) == 0xE0) {
        return 3;
    } else if ((first_byte & 0xF8) == 0xF0) {
        return 4;
    } else {
        return -1;
    }
}

int32_t utf8_strlen(char str[]) {
    int32_t codepoint_count = 0;
    int32_t byte_index = 0;

    while (str[byte_index] != '\0') {
        unsigned char first_byte = (unsigned char)str[byte_index];
        int bytes_in_codepoint = utf8_codepoint_size(first_byte);
        if(bytes_in_codepoint < 0) {
            return -1; // Invalid UTF-8 encoding
        }
        byte_index += bytes_in_codepoint;
        codepoint_count++;
    }

    return codepoint_count;
}

int32_t cpi_of_bi(char str[], int32_t byte_index) {
    if (byte_index < 0 || str[byte_index] == '\0') {
        return -1; // Invalid byte index
    }

    int32_t codepoint_index = 0;
    int32_t current_byte_index = 0;

    while (current_byte_index < byte_index && str[current_byte_index] != '\0') {
        unsigned char first_byte = (unsigned char)str[current_byte_index];
        int bytes_in_codepoint = utf8_codepoint_size(first_byte);
        if (bytes_in_codepoint < 0) {
            return -1; // Invalid UTF-8 encoding
        }
        current_byte_index += bytes_in_codepoint;
        codepoint_index++;
    }
    if(str[current_byte_index] == '\0' && current_byte_index < byte_index) {
        return -1; // Byte index out of bounds
    }

    return codepoint_index;
}

int32_t bi_of_cpi(char str[], int32_t codepoint_index) {
    if (codepoint_index < 0) {
        return -1; // Invalid codepoint index
    }

    int32_t byte_index = 0;
    int32_t current_codepoint_index = 0;

    while (current_codepoint_index < codepoint_index && str[byte_index] != '\0') {
        unsigned char first_byte = (unsigned char)str[byte_index];
        int bytes_in_codepoint = utf8_codepoint_size(first_byte);
        if (bytes_in_codepoint < 0) {
            return -1; // Invalid UTF-8 encoding
        }
        byte_index += bytes_in_codepoint;
        current_codepoint_index++;
    }
    if (str[byte_index] == '\0' && current_codepoint_index < codepoint_index) {
        return -1; // Codepoint index out of bounds
    }

    return byte_index;
}
