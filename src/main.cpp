#include "../include/main.h"

WareHouse *backup = nullptr;
bool to_restore = false;

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

    while (to_restore)
    {
        wareHouse = backup;
        to_restore = false;
        wareHouse->open();
    }

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