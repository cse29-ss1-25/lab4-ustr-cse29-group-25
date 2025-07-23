#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ustr.h"
#include "list.h"

/*
Returns an empty list of initial_capacity
*/
List new_list(int32_t initial_capacity) {
    List list;
    list.size = 0;
    list.capacity = initial_capacity;
    list.data = malloc(initial_capacity * sizeof(struct UStr));
    return list;
}

/*
Initializes an list of length size with elements from array
*/
List new_list_from_array(struct UStr* array, int32_t size) {
    List list = new_list(size);
    for (int32_t i = 0; i < size; i++) {
        list.data[i] = array[i];
    }
    list.size = size;
    return list;
}

/*
Given a list of strings and a separator string, returns a single string 
containing all the strings in list joined by the separator.
*/
UStr join(List* list, UStr separator) {
    // Handle empty list
    if (list->size == 0) {
        return new_ustr("");
    }
    
    // Handle single element
    if (list->size == 1) {
        return new_ustr(list->data[0].contents);
    }
    
    // Calculate total bytes needed
    int32_t total_bytes = 0;
    for (int32_t i = 0; i < list->size; i++) {
        total_bytes += list->data[i].bytes;
    }
    // Add separators (n-1 separators for n elements)
    total_bytes += separator.bytes * (list->size - 1);
    
    // Allocate memory for result
    char* result_contents = malloc(total_bytes + 1);
    result_contents[0] = '\0';  // Start with empty string
    
    // Build the joined string
    for (int32_t i = 0; i < list->size; i++) {
        strcat(result_contents, list->data[i].contents);
        
        // Add separator if not the last element
        if (i < list->size - 1) {
            strcat(result_contents, separator.contents);
        }
    }
    
    // Create result UStr
    UStr result = new_ustr(result_contents);
    free(result_contents);
    
    return result;
}

/*
Inserts string s into list at index s, shifting elements to the right.
Expands the list's capacity if necessary (double the capacity, or set to 1 if 0).

Returns 1 on success, 0 if the index is invalid (out of bounds).
*/
int8_t insert(List* list, UStr s, int32_t index) {
    // Check bounds
    if (index < 0 || index > list->size) {
        return 0;  // Invalid index
    }
    
    // Expand capacity if needed
    if (list->size >= list->capacity) {
        int32_t new_capacity = (list->capacity == 0) ? 1 : list->capacity * 2;
        UStr* new_data = realloc(list->data, new_capacity * sizeof(UStr));
        if (new_data == NULL) {
            return 0;  // Memory allocation failed
        }
        list->data = new_data;
        list->capacity = new_capacity;
    }
    
    // Shift elements to the right
    for (int32_t i = list->size; i > index; i--) {
        list->data[i] = list->data[i-1];
    }
    
    // Insert new element (create a copy)
    list->data[index] = new_ustr(s.contents);
    list->size++;
    
    return 1;  // Success
}

/*
Removes the element at the given index and shifts all subsequent elements left.

Returns 1 on success, 0 if the index is invalid (out of bounds).
*/
int8_t listRemoveAt(List* list, int32_t index) {
    // Check bounds
    if (index < 0 || index >= list->size) {
        return 0;  // Invalid index
    }
    
    // Free the memory of the element being removed
    free_ustr(list->data[index]);
    
    // Shift elements to the left
    for (int32_t i = index; i < list->size - 1; i++) {
        list->data[i] = list->data[i + 1];
    }
    
    // Decrease size
    list->size--;
    
    return 1;  // Success
}

/*
Splits the given string s into substrings separated by the given delimiter string.

Returns a List of String objects, each containing a segment between delimiters.

If the delimiter is the empty string, return a list containing the original 
string as a single element.

If the input string ends with the delimiter, include an empty string at the 
end of the result.

Note that the delimiter could be of a length of more than 1 character
*/
List split(UStr s, UStr separator) {
    List result = new_list(1);  // Start with capacity 1
    
    // Handle empty separator - return list with original string
    if (separator.bytes == 0) {
        UStr copy = new_ustr(s.contents);
        insert(&result, copy, 0);
        free_ustr(copy);
        return result;
    }
    
    int32_t start = 0;  // Start of current segment (byte position)
    int32_t pos = 0;    // Current search position (byte position)
    
    // Search for separator in the string
    while (pos <= s.bytes - separator.bytes) {
        // Check if separator matches at current position
        if (strncmp(s.contents + pos, separator.contents, separator.bytes) == 0) {
            // Found separator - extract substring
            int32_t segment_bytes = pos - start;
            char* segment_contents = malloc(segment_bytes + 1);
            strncpy(segment_contents, s.contents + start, segment_bytes);
            segment_contents[segment_bytes] = '\0';
            
            // Add segment to result list
            UStr segment = new_ustr(segment_contents);
            insert(&result, segment, result.size);
            free_ustr(segment);
            free(segment_contents);
            
            // Move past the separator
            pos += separator.bytes;
            start = pos;
        } else {
            pos++;
        }
    }
    
    // Add the final segment (after last separator or entire string if no separators)
    int32_t final_segment_bytes = s.bytes - start;
    char* final_contents = malloc(final_segment_bytes + 1);
    strncpy(final_contents, s.contents + start, final_segment_bytes);
    final_contents[final_segment_bytes] = '\0';
    
    UStr final_segment = new_ustr(final_contents);
    insert(&result, final_segment, result.size);
    free_ustr(final_segment);
    free(final_contents);
    
    return result;
}

