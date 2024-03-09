#include <string>
#include <windows.h>
#include <vector>
#include "library.h"
#include <iostream>
void library::printall()
{
    for (int i = 0; i < collection.size(); i++) {
        std::cout << collection[i].name << std::endl;
    }
}

void library::parsemetadata(PWSTR path)
{
    const char* cpath = (const char*)path;
    //zip* z = zip_open(cpath, 0, NULL);
    if (0 == NULL) {
        std::cout << "Error opening zip" << std::endl;
        return;
    }

}
void library::add(PWSTR path)
{

}