#include "catch.hpp"
#include <atcommander.h>

//int fakeStream = 0;
//ATCommander<int> atc(fakeStream, fakeStream);

/*
template <class ...TRef>
ATCommander::command_helper2(TRef...dummy) ->
    ATCommander::command_helper2<dummy...>;
    */

struct FakeCommand
{
    static constexpr char CMD = 'V';
};

static void fake_request_suffix(ATCommander& atc, int value)
{

}

template <class ...TArgs>
void _deducer(void (*_func)(ATCommander&, TArgs...))
{

}

/*
template <class ...TArgs>
//auto
typename ATCommander::command_helper2<FakeCommand, TArgs...>
    deducer(void (*_func)(ATCommander&, TArgs...)) //->
    //decltype(ATCommander::command_helper2<FakeCommand, TArgs...>::template helper3<_func>)
    //decltype(declval(ATCommander::command_helper2<FakeCommand, TArgs...>))
{
    typedef typename ATCommander::command_helper2<FakeCommand, TArgs...> temp1;
    //typedef typename temp1::template helper3<_func> temp2;
    //::helper3<_func> temp;
    temp1 temp2;
    //return (temp1*) nullptr;
    //return declval(ATCommander::command_helper2<FakeCommand, TArgs...>);
}
*/

TEST_CASE( "Make sure catch.hpp is running", "[meta]" )
{
    REQUIRE(1 == 1);

    ATCommander atc(fstd::cin, fstd::cout);

    /*
    //_deducer(fake_request_suffix);
    auto value = deducer(fake_request_suffix);
    //value->helper3<fake_request_suffix>::request(atc, 5);
    typedef decltype(value) test2;
    decltype(value)::helper3<fake_request_suffix>::request(atc, 5);
    //test2::request(atc, 5);
    //decltype(deducer(fake_request_suffix)) deduced;
    ATCommander::command_helper2<FakeCommand, int>::helper3<fake_request_suffix>::request(atc, 3); */
    //ATCommander::command_helper2 val(1, 2, 3);
}
