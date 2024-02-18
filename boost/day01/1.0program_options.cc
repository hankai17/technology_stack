#include <boost/program_options.hpp>
#include <iostream>

namespace opt = boost::program_options;

int main(int argc, char *argv[])
{
    opt::options_description desc("All options");
    desc.add_options()
        ("apples", opt::value<int>(), "how many apples do you have")
        ("oranges", opt::value<int>(), "how many oranges do you have")
        ("help", "produce help message");

    opt::variables_map vm;
    opt::store(opt::parse_command_line(argc, argv, desc), vm);
    opt::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    std::cout << "Fruits count: " 
            << vm["apples"].as<int>() + vm["oranges"].as<int>() << std::endl;

    return 0;
}

// g++ -Wall main.cpp -lboost_program_options -L/usr/local/lib\
// ./a.out --apples=10 --oranges=5
