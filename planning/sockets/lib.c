#include "lib.h"
#include <stdlib.h>
#include <string.h>

/**
 * Gets the position of <i>needle</i> in <i>haystack</i>, if found.
 * 
 * @see http://stackoverflow.com/questions/2091825/strpos-in-c-how-does-it-work
 * @param haystack The string to search in.
 * @param needle The string to search for.
 * @return The starting position of <i>needle</i> in <i>haystack</i>, or -1 if
 * not found.
 */
int indexOf(const char *haystack, const char *needle) {
    char *p = strstr(haystack, needle);
    if (p) {
        return p - haystack;        
    }
    return -1;
}