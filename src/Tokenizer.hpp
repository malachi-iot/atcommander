//#include "atcommander.h"

namespace experimental {

//template<class TTraits>
//constexpr char Tokenizer<TTraits>::class_name[];

template<class TTraits>
// retrieves a text string in input up to max size
// leaves any discovered delimiter cached
size_t Tokenizer<TTraits>::tokenize(fstd::istream& cin, char *input, size_t max) const
{
    int ch;
    size_t len = 0;

    while (!is_delimiter(ch = cin.peek()) && len < max && ch >= 0)
    {
        *input++ = ch;
        len++;
        cin.ignore();
    }

    // FIX: check if input can be assigned to still
    // (ensure we haven't exceeded max)
    *input = 0;

    /*
    if (ch != -1)
        // FIX: be sure to check len also
        cin.unget(ch); */

    return len;
}


}