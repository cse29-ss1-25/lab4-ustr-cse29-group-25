#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ustr.h"
#include "pset1.h"

/*
Initializes a new UStr with contents
*/
UStr new_ustr(char* contents) {
	int32_t bytes = strlen(contents);
	int32_t codepoints = utf8_strlen(contents);
	uint8_t is_ascii = bytes == codepoints;
	char* contents_copy = malloc(bytes + 1);
	strcpy(contents_copy, contents);
	UStr s = {
		codepoints,
		bytes,
		is_ascii,
		contents_copy
	};
	return s;
}

/*
Returns length of string
*/
int32_t len(UStr s) {
	return s.codepoints;
}

/*
Returns a substring of str, starting at index start (inclusive) 
and ending at index end (exclusive).

Returns an empty string on invalid range.
*/
UStr substring(UStr s, int32_t start, int32_t end) {
    // Debug: Add some print statements to see what's happening
    printf("DEBUG: substring called with start=%d, end=%d, codepoints=%d\n", start, end, s.codepoints);
    
    // Handle null or empty string
    if (!s.contents) {
        printf("DEBUG: null contents\n");
        return new_ustr("");
    }
    
    if (s.codepoints == 0) {
        printf("DEBUG: zero codepoints\n");
        return new_ustr("");
    }
    
    // Validate and normalize indices
    if (start < 0) start = 0;
    if (end > s.codepoints) end = s.codepoints;
    if (start >= end) {
        printf("DEBUG: start >= end (%d >= %d)\n", start, end);
        return new_ustr("");
    }
    
    printf("DEBUG: proceeding with start=%d, end=%d\n", start, end);
    
    // If all ASCII, we can work directly with bytes
    if (s.is_ascii) {
        printf("DEBUG: using ASCII path\n");
        int32_t sub_length = end - start;
        char* substring_chars = malloc(sub_length + 1);
        if (!substring_chars) {
            return new_ustr("");
        }
        
        strncpy(substring_chars, s.contents + start, sub_length);
        substring_chars[sub_length] = '\0';
        
        printf("DEBUG: created substring: '%s'\n", substring_chars);
        
        UStr result = new_ustr(substring_chars);
        free(substring_chars);
        return result;
    }
    
    // For UTF-8, we need to find byte positions
    printf("DEBUG: using UTF-8 path\n");
    int32_t start_byte = 0;
    int32_t end_byte = 0;
    int32_t char_count = 0;
    int32_t byte_pos = 0;
    
    // Find start position
    while (char_count < start && s.contents[byte_pos]) {
        unsigned char c = (unsigned char)s.contents[byte_pos];
        if (c < 0x80) {
            byte_pos += 1;
        } else if ((c & 0xE0) == 0xC0) {
            byte_pos += 2;
        } else if ((c & 0xF0) == 0xE0) {
            byte_pos += 3;
        } else if ((c & 0xF8) == 0xF0) {
            byte_pos += 4;
        } else {
            byte_pos += 1;
        }
        char_count++;
    }
    start_byte = byte_pos;
    
    // Find end position
    while (char_count < end && s.contents[byte_pos]) {
        unsigned char c = (unsigned char)s.contents[byte_pos];
        if (c < 0x80) {
            byte_pos += 1;
        } else if ((c & 0xE0) == 0xC0) {
            byte_pos += 2;
        } else if ((c & 0xF0) == 0xE0) {
            byte_pos += 3;
        } else if ((c & 0xF8) == 0xF0) {
            byte_pos += 4;
        } else {
            byte_pos += 1;
        }
        char_count++;
    }
    end_byte = byte_pos;
    
    printf("DEBUG: start_byte=%d, end_byte=%d\n", start_byte, end_byte);
    
    // Calculate substring length in bytes
    int32_t sub_length = end_byte - start_byte;
    
    // Create substring
    char* substring_chars = malloc(sub_length + 1);
    if (!substring_chars) {
        return new_ustr("");
    }
    
    strncpy(substring_chars, s.contents + start_byte, sub_length);
    substring_chars[sub_length] = '\0';
    
    printf("DEBUG: created UTF-8 substring: '%s'\n", substring_chars);
    
    UStr result = new_ustr(substring_chars);
    free(substring_chars);
    return result;
}

/*
Given 2 strings s1 and s2, returns a string that is the result of 
concatenating s1 and s2. 
*/
UStr concat(UStr s1, UStr s2) {
    // Handle null cases
    if (!s1.contents && !s2.contents) return new_ustr("");
    if (!s1.contents) return new_ustr(s2.contents);
    if (!s2.contents) return new_ustr(s1.contents);
    
    // Calculate total length in bytes
    int32_t total_bytes = s1.bytes + s2.bytes;
    
    // Allocate memory for concatenated string
    char* result = malloc(total_bytes + 1);
    if (!result) return new_ustr("");
    
    // Copy both strings
    strcpy(result, s1.contents);
    strcat(result, s2.contents);
    
    // Create new UStr (new_ustr will calculate codepoints and bytes)
    UStr concat_result = new_ustr(result);
    free(result);
    return concat_result; 
}

/*
Given a string s and an index, return a string with the character at index 
removed from the original string. 

Returns the original string if index is out of bounds.
*/
UStr removeAt(UStr s, int32_t index) {
    // Handle empty string or null
    if (!s.contents || s.codepoints == 0) {
        return new_ustr("");
    }
    
    // Check bounds
    if (index < 0 || index >= s.codepoints) {
        return new_ustr(s.contents);  // Return copy of original
    }
    
    // If all ASCII, simple byte removal
    if (s.is_ascii) {
        char* result = malloc(s.bytes);  // One less byte
        if (!result) return new_ustr("");
        
        // Copy before index
        strncpy(result, s.contents, index);
        // Copy after index
        strcpy(result + index, s.contents + index + 1);
        
        UStr remove_result = new_ustr(result);
        free(result);
        return remove_result;
    }
    
    // For UTF-8, find byte positions
    int32_t char_start_byte = 0;
    int32_t char_end_byte = 0;
    int32_t char_count = 0;
    int32_t byte_pos = 0;
    
    // Find the character to remove
    while (char_count <= index && s.contents[byte_pos]) {
        if (char_count == index) {
            char_start_byte = byte_pos;
        }
        
        // Move to next character
        unsigned char c = (unsigned char)s.contents[byte_pos];
        if (c < 0x80) {
            byte_pos += 1;
        } else if ((c & 0xE0) == 0xC0) {
            byte_pos += 2;
        } else if ((c & 0xF0) == 0xE0) {
            byte_pos += 3;
        } else if ((c & 0xF8) == 0xF0) {
            byte_pos += 4;
        } else {
            byte_pos += 1;
        }
        
        if (char_count == index) {
            char_end_byte = byte_pos;
        }
        char_count++;
    }
    
    // Calculate result length
    int32_t result_bytes = s.bytes - (char_end_byte - char_start_byte);
    
    // Allocate result
    char* result = malloc(result_bytes + 1);
    if (!result) return new_ustr("");
    
    // Copy before removed character
    strncpy(result, s.contents, char_start_byte);
    result[char_start_byte] = '\0';
    
    // Copy after removed character
    strcat(result, s.contents + char_end_byte);
    
    UStr remove_result = new_ustr(result);
    free(result);
    return remove_result;
}

/*
Given a string s, return s reversed. 

Example: reverse("apples🍎 and bananas🍌") = "🍌sananab dna 🍎selppa")
*/
UStr reverse(UStr s) {
    if (!s.contents || s.codepoints == 0) {
        return new_ustr("");
    }
    
    // If all ASCII, simple byte reversal
    if (s.is_ascii) {
        char* result = malloc(s.bytes + 1);
        if (!result) return new_ustr("");
        
        // Reverse ASCII characters
        for (int32_t i = 0; i < s.bytes; i++) {
            result[i] = s.contents[s.bytes - 1 - i];
        }
        result[s.bytes] = '\0';
        
        UStr reverse_result = new_ustr(result);
        free(result);
        return reverse_result;
    }
    
    // For UTF-8, we need to reverse character by character
    // First, collect all character positions
    int32_t* char_positions = malloc((s.codepoints + 1) * sizeof(int32_t));
    if (!char_positions) return new_ustr("");
    
    int32_t char_count = 0;
    int32_t byte_pos = 0;
    char_positions[0] = 0;
    
    while (s.contents[byte_pos] && char_count < s.codepoints) {
        unsigned char c = (unsigned char)s.contents[byte_pos];
        if (c < 0x80) {
            byte_pos += 1;
        } else if ((c & 0xE0) == 0xC0) {
            byte_pos += 2;
        } else if ((c & 0xF0) == 0xE0) {
            byte_pos += 3;
        } else if ((c & 0xF8) == 0xF0) {
            byte_pos += 4;
        } else {
            byte_pos += 1;
        }
        char_count++;
        char_positions[char_count] = byte_pos;
    }
    
    // Allocate result
    char* result = malloc(s.bytes + 1);
    if (!result) {
        free(char_positions);
        return new_ustr("");
    }
    
    // Copy characters in reverse order
    char* result_pos = result;
    for (int32_t i = s.codepoints - 1; i >= 0; i--) {
        int32_t char_start = char_positions[i];
        int32_t char_end = char_positions[i + 1];
        int32_t char_len = char_end - char_start;
        
        memcpy(result_pos, s.contents + char_start, char_len);
        result_pos += char_len;
    }
    *result_pos = '\0';
    
    free(char_positions);
    UStr reverse_result = new_ustr(result);
    free(result);
    return reverse_result;
}


void print_ustr(UStr s) {
	printf("%s [codepoints: %d | bytes: %d]", s.contents, s.codepoints, s.bytes);
}

void free_ustr(UStr s) {
	if (s.contents != NULL) {
		free(s.contents);
		s.contents = NULL;
	}
}

