#pragma once

#include "atcommander.h"

class ATBuilder
{
protected:
    struct _response_provider
    {
        void response(ATCommander& atc)
        {
            atc.check_for_ok();
        }
    };

public:
    //typedef ATCommander::_command_base command_base;
    template <class TProvider, class TMethodProvider = TProvider>
    struct command_base : ATCommander::_command_base
    {
        static void prefix(ATCommander& atc)
        {
            _command_base::prefix(atc, TProvider::CMD);
        }

        // Default response behavior is only to check for OK after a command
        // oftentimes this is NOT what you want, so be sure to overload (override-ish)
        static bool response(ATCommander& atc)
        {
            return atc.check_for_ok();
        }

        template <class ...TArgs>
        static void request(ATCommander& atc, TArgs...args)
        {
            prefix(atc);
            TMethodProvider::suffix(atc, args...);
            atc.send();
        }


        /*
        void response(ATCommander &atc)
        {
            atc.check_for_ok();
        } */

        /*
        // as discussed http://stackoverflow.com/questions/257288/is-it-possible-to-write-a-template-to-check-for-a-functions-existence
        // and here http://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature
        template <typename T>
        struct func_exists
        {
            typedef char one;
            typedef long two;

            template <typename C> static one test( typeof(&C::response) ) ;
            template <typename C> static two test(...);

            enum { value = sizeof(test<T>(0)) == sizeof(char) };
        }; */

        // Works, but don't need it - overloading does the trick
        template<typename T>
        struct HasUsedMemoryMethod
        {
            template<typename U, size_t (U::*)() const> struct SFINAE {};
            template<typename U> static char Test(SFINAE<U, &U::response>*);
            template<typename U> static int Test(...);
            static const bool Has = sizeof(Test<T>(0)) == sizeof(char);
        };

        // TODO: be mindful, this might be a C++14 only feature
        template <class ...TArgs>
        //static auto response(ATCommander& atc, TArgs...args) -> decltype(TMethodProvider::response(atc, args...))
        static void response(ATCommander& atc, TArgs...args)
        {
            //auto returnValue = TMethodProvider::response(atc, args...);
            //return returnValue;
            TMethodProvider::response(atc, args...);
        }

        template <class ...TArgs>
        static void run(ATCommander& atc, TArgs...args)
        {
            request(atc, args...);
            response(atc);
        }
    };


    template <class TProvider, typename TResponse>
    struct status_base_autoresponse
    {
        static TResponse response_suffix(ATCommander& atc)
        {
            TResponse input;

            atc.input(input);

            return input;
        }
    };


    template <class TProvider, class TMethodProvider = TProvider>
    struct status_base : ATCommander::_command_base
    {
        static void prefix(ATCommander& atc)
        {
            _command_base::prefix(atc, TProvider::CMD);
            atc.cout.put('?');
        }

        static void response_prefix(ATCommander& atc)
        {
            atc.input_match(TProvider::CMD);
            atc.input_match(": ");
        }

        // TODO: be mindful, this might be a C++14 only feature
        template <class ...TArgs>
        static auto response(ATCommander& atc, TArgs...args) -> decltype(TMethodProvider::response_suffix(atc, args...))
        //static void response(ATCommander& atc, TArgs...args)
        {
            response_prefix(atc);
            auto returnValue = TMethodProvider::response_suffix(atc, args...);
            return returnValue;
        }
    };

    /*
    template <char const &array[N], int N>
    struct command_base
    {

    }; */

public:

};
