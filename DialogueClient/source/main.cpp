#include <ConfigApplication/ConfigApplication.hpp>
#include <iostream>

int main()
{
    try
    {
        ConfigApplication client;
        client.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}