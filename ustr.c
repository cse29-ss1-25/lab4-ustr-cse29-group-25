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
    uint8_t all_ascii = bytes == codepoints;
    char* contents_copy = malloc(bytes + 1);
    strcpy(contents_copy, contents);
    UStr s = {
        codepoints,
        bytes,
        all_ascii,
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
	// TODO: implement this
    int32_t start_byte = 0;
    int32_t end_byte = 0;
    int32_t result_bytes = 0;
    char* result_contents = NULL;
    
    // Validate input parameters first
    if (start < 0 || end < start || s.contents == NULL || s.codepoints == 0) {
    	return new_ustr("");
    }    
    // Handle empty string case
    if (s.codepoints == 0 || s.bytes == 0) {
        return new_ustr("");
    }
    
    // Clamp end to string length
    if (end > s.codepoints) {
        end = s.codepoints;
    }
    
    // Handle case where start equals end (empty substring)
    if (start == end) {
        return new_ustr("");
    }
    
    // Find byte positions for start and end character indices
    // Only call bi_of_cpi with validated parameters
    start_byte = bi_of_cpi(s.contents, start);
    end_byte = bi_of_cpi(s.contents, end);
    
    // Validate that bi_of_cpi returned valid positions
    if (start_byte < 0 || end_byte < start_byte || end_byte > s.bytes) {
        return new_ustr("");
    }
    
    // Calculate result size
    result_bytes = end_byte - start_byte;
    
    // Handle zero-length result
    if (result_bytes <= 0) {
        return new_ustr("");
    }
    
    // Allocate memory for result
    result_contents = malloc(result_bytes + 1);
    if (result_contents == NULL) {
        return new_ustr("");  // Handle allocation failure
    }
    
    // Extract substring bytes
    strncpy(result_contents, s.contents + start_byte, result_bytes);
    result_contents[result_bytes] = '\0';  // Ensure null termination
    
    // Create new UStr and clean up
    UStr result = new_ustr(result_contents);
    free(result_contents);
    
    return result;
}

/*
Given 2 strings s1 and s2, returns a string that is the result of 
concatenating s1 and s2. 
*/
UStr concat(UStr s1, UStr s2) {
	// TODO: implement this
    // Calculate total size needed
    int32_t total_bytes = s1.bytes + s2.bytes;
    
    // Allocate memory for concatenated string
    char* new_contents = malloc(total_bytes + 1);
    
    // Copy first string
    strcpy(new_contents, s1.contents);
    
    // Append second string
    strcat(new_contents, s2.contents);
    
    // Create new UStr (this will recalculate metadata)
    UStr result = new_ustr(new_contents);
    
    // Clean up temporary allocation
    free(new_contents);
    
    return result;
}

/*
Given a string s and an index, return a string with the character at index 
removed from the original string. 

Returns the original string if index is out of bounds.
*/
UStr removeAt(UStr s, int32_t index) {
    // Check bounds
    if (index < 0 || index >= s.codepoints) {
        // Return copy of original string if index out of bounds
        return new_ustr(s.contents);
    }
    
    // Find byte position of character to remove
    int32_t remove_start = bi_of_cpi(s.contents, index);
    int32_t remove_end = bi_of_cpi(s.contents, index + 1);
    int32_t char_bytes = remove_end - remove_start;
    
    // Calculate new string size
    int32_t new_bytes = s.bytes - char_bytes;
    char* new_contents = malloc(new_bytes + 1);
    
    // Copy before the character to remove
    strncpy(new_contents, s.contents, remove_start);
    
    // Copy after the character to remove
    strcpy(new_contents + remove_start, s.contents + remove_end);
    
    // Create new UStr
    UStr result = new_ustr(new_contents);
    free(new_contents);
    
    return result;
}

/*
Given a string s, return s reversed. 

Example: reverse("apples🍎 and bananas🍌") = "🍌sananab dna 🍎selppa")
*/
UStr reverse(UStr s) {
    if (s.codepoints <= 1) {
        return new_ustr(s.contents);  // Nothing to reverse
    }
    
    // Create array to store character byte positions and lengths
    int32_t* char_starts = malloc(s.codepoints * sizeof(int32_t));
    int32_t* char_lengths = malloc(s.codepoints * sizeof(int32_t));
    
    // Find start position and length of each character
    for (int32_t i = 0; i < s.codepoints; i++) {
        char_starts[i] = bi_of_cpi(s.contents, i);
        if (i < s.codepoints - 1) {
            char_lengths[i] = bi_of_cpi(s.contents, i + 1) - char_starts[i];
        } else {
            char_lengths[i] = s.bytes - char_starts[i];
        }
    }
    
    // Build reversed string
    char* new_contents = malloc(s.bytes + 1);
    int32_t write_pos = 0;
    
    // Copy characters in reverse order
    for (int32_t i = s.codepoints - 1; i >= 0; i--) {
        strncpy(new_contents + write_pos, 
                s.contents + char_starts[i], 
                char_lengths[i]);
        write_pos += char_lengths[i];
    }
    
    new_contents[s.bytes] = '\0';
    
    // Clean up and create result
    free(char_starts);
    free(char_lengths);
    
    UStr result = new_ustr(new_contents);
    free(new_contents);
    
    return result;
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

