#include "Library.h"
#include <string>
#include <windows.h>
#include <vector>
#include <iostream>
void Library::printall()
{
    for (int i = 0; i < collection.size(); i++) {
        std::cout << collection[i].name << std::endl;
    }
}

void Library::parsemetadata(PWSTR path)
{
    const char* cpath = (const char*)path;
    //izip* z = zip_open(cpath, 0, NULL);
    if (0 == NULL) {
        std::cout << "Error opening zip" << std::endl;
        return;
    }

}
void Library::add(PWSTR path)
{

}