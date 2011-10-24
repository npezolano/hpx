
#include <iostream>

#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/iostreams.hpp>

#include <hpx/util/high_resolution_timer.hpp>
#include <hpx/util/function.hpp>

using boost::program_options::variables_map;
using boost::program_options::options_description;
using boost::program_options::value;

using hpx::cout;
using hpx::flush;
using hpx::init;
using hpx::finalize;
using hpx::actions::plain_action2;
using hpx::naming::id_type;
using hpx::lcos::async;
using hpx::find_all_localities;
using hpx::util::function;
using hpx::util::high_resolution_timer;

void f(function<void()> f, int)
{
    f();
}

typedef
    plain_action2<
        function<void()>
      , int
      , &f
    > f_action;

HPX_REGISTER_PLAIN_ACTION(f_action);

struct g
{
    void operator()()
    {
        cout << "Hello World\n" << flush;
    }

    template <typename Archive>
    void serialize(Archive &, unsigned)
    {}
};

int hpx_main(variables_map &)
{
    {
        function<void()> f = g();
        std::vector<id_type> prefixes = find_all_localities();
    
        BOOST_FOREACH(id_type const & prefix, prefixes)
        {
            async<f_action>(prefix, f, 0).get();
        }
    }
    finalize();
    return 0;
}

int main(int argc, char **argv)
{
    options_description
        cmdline("usage: " HPX_APPLICATION_STRING " [options]");

    return init(cmdline, argc, argv);

}