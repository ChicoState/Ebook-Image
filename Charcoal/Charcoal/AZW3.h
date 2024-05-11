#pragma once
#include "Library.h"
#include "mobi.h"
#include <Windows.h>
#include <string>

std::string wstring_to_utf8(const std::wstring& wstr);

class mobi : public Library
{
    public:
        book add(std::string path);
        void grayscaleMOBI(const std::string& filePath);
};

class azw3 : public Library
{
    public:
        book add(std::string path);
        void grayscaleAZW3(const std::string& filePath);
};

//Utility functions that are used across different files
std::string wstring_to_utf8(const std::wstring& wstr);
std::string safeCharPtrToString(const char* charPtr);

//Function declarations related to MOBI library handling
MOBIData* initAndLoadMobi(const std::string& filePath);
void extractTextAndOtherContent(const MOBIRawml* rawml, const std::string& outputDir);
void extractImages(const MOBIRawml* rawml, const std::string& outputDir, int scaleFactor);
bool extractContentFromMobi(const std::string& filePath, const std::string& outputDir, int scaleFactor);