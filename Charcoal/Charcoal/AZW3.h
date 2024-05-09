#pragma once
#include "Library.h"
#include <Windows.h>
#include <string>

std::string wstring_to_utf8(const std::wstring& wstr);

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