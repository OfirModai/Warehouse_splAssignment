#include "../include/main.h"

WareHouse *backup = nullptr;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "usage: warehouse <config_path>" << std::endl;
        return 0;
    }
    string configurationFile = argv[1];
    WareHouse *wareHouse = new WareHouse(configurationFile);
    wareHouse->start();
    
    // make sure to delete warehouse objects
    if (backup != nullptr)
    {
        delete backup;
        backup = nullptr;
    }
    if (wareHouse != nullptr)
    {
        delete wareHouse;
        wareHouse = nullptr;
    }
    return 0;
}