#include "AZW3.h"
#include <mobi.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

using namespace std;
namespace fs = std::experimental::filesystem;

#pragma once

std::string wstring_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
    return strTo;
}

std::string safeCharPtrToString(const char* charPtr) {
    if (charPtr != nullptr) {
        return std::string(charPtr);
    }
    else {
        return std::string();
    }
}

MOBIData* initAndLoadMobi(const std::string& filePath) {
    MOBIData* mobiData = mobi_init();
    if (!mobiData) {
        std::cerr << "Memory allocation for MOBIData failed." << std::endl;
        return nullptr;
    }

    FILE* file;
    errno_t err = fopen_s(&file, filePath.c_str(), "rb");
    if (err != 0 || file == nullptr) {
        std::cerr << "Failed to open file: " << filePath << " with error code: " << err << std::endl;
        mobi_free(mobiData);
        return nullptr;
    }

    if (mobi_load_file(mobiData, file) != MOBI_SUCCESS) {
        std::cerr << "Failed to load MOBI file: " << filePath << std::endl;
        fclose(file);
        mobi_free(mobiData);
        return nullptr;
    }

    fclose(file);
    return mobiData;
}

book azw3::add(PWSTR path) {
    // Convert PWSTR to std::wstring
    std::wstring ws(path);
    // Now convert std::wstring to std::string
    std::string filePath = wstring_to_utf8(ws);

    // Initialize and load MOBI file
    MOBIData* mobiData = initAndLoadMobi(filePath);
    if (!mobiData) {
        // Handle error: unable to initialize and load MOBI
        std::cerr << "Unable to initialize and load MOBI data" << std::endl;
        return book(); // Return an empty book object
    }

    book curr;
    // Extract metadata
    curr.title = mobi_meta_get_title(mobiData);
    curr.author = safeCharPtrToString(mobi_meta_get_author(mobiData));
    curr.publisher = safeCharPtrToString(mobi_meta_get_publisher(mobiData));
    curr.contributor = safeCharPtrToString(mobi_meta_get_contributor(mobiData));
    curr.rights = safeCharPtrToString(mobi_meta_get_copyright(mobiData));
    curr.format = "AZW3";
    curr.date = safeCharPtrToString(mobi_meta_get_publishdate(mobiData));
    curr.language = safeCharPtrToString(mobi_meta_get_language(mobiData));
    curr.description = safeCharPtrToString(mobi_meta_get_description(mobiData));

    mobi_free(mobiData);

    return curr;
}

book mobi::add(PWSTR path) {
    std::wstring ws(path);
    std::string filePath = wstring_to_utf8(ws);

    // Initialize and load MOBI file
    MOBIData* mobiData = initAndLoadMobi(filePath);
    if (!mobiData) {
        // Handle error: unable to initialize and load MOBI
        std::cerr << "Unable to initialize and load MOBI data" << std::endl;
        return book(); // Return an empty book object
    }

    book curr;
    // Extract metadata
    curr.title = mobi_meta_get_title(mobiData);
    curr.author = safeCharPtrToString(mobi_meta_get_author(mobiData));
    curr.publisher = safeCharPtrToString(mobi_meta_get_publisher(mobiData));
    curr.contributor = safeCharPtrToString(mobi_meta_get_contributor(mobiData));
    curr.rights = safeCharPtrToString(mobi_meta_get_copyright(mobiData));
    curr.format = "MOBI";
    curr.date = safeCharPtrToString(mobi_meta_get_publishdate(mobiData));
    curr.language = safeCharPtrToString(mobi_meta_get_language(mobiData));
    curr.description = safeCharPtrToString(mobi_meta_get_description(mobiData));

    mobi_free(mobiData);

    return curr;
}

void extractTextAndOtherContent(const MOBIRawml* rawml, const std::string& outputDir) {
    for (MOBIPart* part = rawml->markup; part != NULL; part = part->next) {
        if (part->type == T_HTML) {
            std::string filename = outputDir + "/markup_" + std::to_string(part->uid) + ".html";
            std::ofstream outFile(filename, std::ios::binary);
            outFile.write(reinterpret_cast<char*>(part->data), part->size);
            outFile.close();
        }
    }
}

void extractImages(const MOBIRawml* rawml, const std::string& outputDir, int scaleFactor) {
    for (MOBIPart* part = rawml->resources; part != NULL; part = part->next) {
        if (part->type == T_JPG || part->type == T_PNG || part->type == T_GIF || part->type == T_BMP) {
            std::string extension;
            switch (part->type) {
            case T_JPG: extension = ".jpg"; break;
            case T_PNG: extension = ".png"; break;
            case T_GIF: extension = ".gif"; break;
            case T_BMP: extension = ".bmp"; break;
            default: continue;
            }
        }
    }
}

bool extractContentFromMobi(const std::string& filePath, const std::string& outputDir, int scaleFactor) {
    MOBIData* mobiData = initAndLoadMobi(filePath);
    if (!mobiData) return false;

    if (!fs::create_directories(outputDir) && !fs::exists(outputDir)) {
        std::cerr << "Failed to create output directory: " << outputDir << std::endl;
        mobi_free(mobiData);
        return false;
    }

    MOBIRawml* rawml = mobi_init_rawml(mobiData);
    if (!rawml || mobi_parse_rawml(rawml, mobiData) != MOBI_SUCCESS) {
        std::cerr << "Failed to initialize or parse MOBI rawml" << std::endl;
        mobi_free(mobiData); // Clean up mobiData
        return false;
    }

    // Clean up
    mobi_free_rawml(rawml);
    mobi_free(mobiData);

    return true;
}
