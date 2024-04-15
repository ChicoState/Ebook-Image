#pragma once
#include "Library.h"
class mobi : public Library
{
    public:
        book add(PWSTR path);
};

class azw3 : public Library
{
public:
    book add(PWSTR path);
};