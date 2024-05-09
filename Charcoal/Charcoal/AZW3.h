#pragma once
#include "Library.h"
#include <Windows.h>
#include <string>

std::string wstring_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
    return strTo;
}

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