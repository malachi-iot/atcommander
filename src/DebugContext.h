#pragma once

template <const char* subsystem, const char* prefix = nullptr>
class DebugContext
{
#ifdef DEBUG_ATC_CONTEXT
    char localBuf[2];
        const char* cmd;
#endif
public:

    inline void set(const char* cmd)
    {
#ifdef DEBUG_ATC_CONTEXT
        this->cmd = cmd;
#endif
    }


    inline void set(char cmd)
    {
#ifdef DEBUG_ATC_CONTEXT
        this->cmd = localBuf;
        localBuf[0] = cmd;
        localBuf[1] = 0;
#endif
    }

    inline void dump(fstd::ostream& clog)
    {
#ifdef DEBUG_ATC_CONTEXT
        fstd::clog << '(';
        if(prefix != nullptr) fstd::clog << prefix << ':';
        fstd::clog << subsystem << cmd << ") ";
#else
#endif
    }

};