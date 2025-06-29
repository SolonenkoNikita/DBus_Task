#include <ConfigurationManager/ConfigurationManager.hpp>
#include <JsonConfigFileManager/JsonConfigFileManager.hpp>

#include <iostream>

int main()
{
    try 
    {
        auto configManager = 
                std::make_unique<ConfigurationManager>(std::make_unique<JsonConfigFileManager>());
        
        configManager->run();
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}