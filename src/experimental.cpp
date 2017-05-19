#include "atcommander.h"

namespace experimental {
/*
// retrieves a text string in input up to max size
// leaves any discovered delimiter cached
size_t Tokenizer::tokenize(fstd::istream& cin, char *input, size_t max)
{
    int ch;
    size_t len = 0;

    while (!is_delimiter(ch = cin.get()) && len < max && ch != -1)
    {
        *input++ = ch;
        len++;
    }

    // FIX: check if input can be assigned to still
    // (ensure we haven't exceeded max)
    *input = 0;

    if (ch != -1)
        // FIX: be sure to check len also
        cin.unget(ch);

    return len;
}
*/

}