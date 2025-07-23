#include <stdio.h>
#include "ustr.h"

int main() {
    UStr s = new_ustr("🍌🍏ĀЖ▣");
    printf("Original: ");
    print_ustr(s);
    printf("\n");
    
    // Test normal substring
    UStr sub1 = substring(s, 1, 4);  
    printf("substring(1,4): ");
    print_ustr(sub1);
    printf("\n");
    
    // Test full string
    UStr sub2 = substring(s, 0, 5); 
    printf("substring(0,5): ");
    print_ustr(sub2);
    printf("\n");
    
    // Test edge case (what the original test is doing)
    UStr sub3 = substring(s, 0, 0);  // Should get ""
    printf("substring(0,0): ");
    print_ustr(sub3);
    printf("\n");
    
    free_ustr(s);
    free_ustr(sub1);
    free_ustr(sub2);
    free_ustr(sub3);
    return 0;
}
