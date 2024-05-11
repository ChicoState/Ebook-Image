#include "AZW3.h"
#include <mobi.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <zip.h>
#include <shlobj.h>
#include "stb_image.h"
#include "stb_image_write.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

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

std::wstring string_to_wstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
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

book azw3::add(std::string path) {
    std::wstring ws = string_to_wstring(path);
    std::string filePath = wstring_to_utf8(ws);

    // Initialize and load MOBI file
    MOBIData* mobiData = initAndLoadMobi(filePath);
    if (!mobiData) {
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

book mobi::add(std::string path) {
    std::wstring ws = string_to_wstring(path);
    std::string filePath = wstring_to_utf8(ws);

    // Initialize and load MOBI file
    MOBIData* mobiData = initAndLoadMobi(filePath);
    if (!mobiData) {
        std::cerr << "Unable to initialize and load MOBI data" << std::endl;
        return book();
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

std::string getDesktopPath() {
    char path[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, path) != S_OK) {
        std::cerr << "Error obtaining desktop path." << std::endl;
        return "";
    }
    return std::string(path) + "\\";
}

bool saveModifiedMobi(MOBIData* mobiData, const string& outputPath) {
    FILE* outFile;
    errno_t fileErr = fopen_s(&outFile, outputPath.c_str(), "wb");
    if (fileErr != 0 || outFile == nullptr) {
        cerr << "Failed to open file for writing: " << outputPath << endl;
        return false;
    }

    // Using mobi_write_file to save the entire MOBIData structure to the file
    MOBI_RET ret = mobi_write_file(outFile, mobiData);
    if (ret != MOBI_SUCCESS) {
        cerr << "Failed to write modified MOBI file. Error code: " << ret << endl;
        fclose(outFile); // Make sure to close the file on failure
        return false;
    }

    fclose(outFile); // Close the file after successful writing
    cout << "Modified MOBI file saved successfully to " << outputPath << endl;
    return true;
}

void mobi::grayscaleMOBI(const std::string& filePath) {
    MOBIData* mobiData = mobi_init();
    if (!mobiData) {
        std::cerr << "Failed to initialize MOBI data." << std::endl;
        return;
    }

    FILE* file;
    errno_t err = fopen_s(&file, filePath.c_str(), "rb");
    if (err != 0 || file == nullptr) {
        std::cerr << "Failed to open file: " << filePath << " with error code: " << err << std::endl;
        mobi_free(mobiData);
        return;
    }

    if (mobi_load_file(mobiData, file) != MOBI_SUCCESS) {
        std::cerr << "Failed to load MOBI data from file." << std::endl;
        fclose(file);
        mobi_free(mobiData);
        return;
    }
    fclose(file); // Close the file after loading

    MOBIRawml* rawml = mobi_init_rawml(mobiData);
    if (!rawml || mobi_parse_rawml(rawml, mobiData) != MOBI_SUCCESS) {
        std::cerr << "Failed to initialize or parse MOBI rawml" << std::endl;
        mobi_free(mobiData);
        return;
    }

    std::string desktopPath = getDesktopPath();
    MOBIPart* part = rawml->resources;
    while (part) {
        if (part->type == T_JPG || part->type == T_PNG || part->type == T_GIF || part->type == T_BMP) {
            int width, height, channels;
            unsigned char* imgData = stbi_load_from_memory(part->data, part->size, &width, &height, &channels, 0);
            if (imgData) {
                cv::Mat colorImg(height, width, channels == 4 ? CV_8UC4 : channels == 3 ? CV_8UC3 : CV_8UC1, imgData);
                cv::Mat grayImg;
                cv::cvtColor(colorImg, grayImg, cv::COLOR_BGR2GRAY);

                std::string filename = desktopPath + "image_" + std::to_string(part->uid) + ".jpg";
                cv::imwrite(filename, grayImg);

                stbi_image_free(imgData);
                std::cout << "Saved grayscaled image to " << filename << std::endl;
            }
            else {
                std::cerr << "Failed to load image data for processing." << std::endl;
            }
        }
        part = part->next;
    }

    mobi_free_rawml(rawml);
    mobi_free(mobiData);
}

void azw3::grayscaleAZW3(const std::string& filePath) {
    MOBIData* mobiData = mobi_init();
    if (!mobiData) {
        std::cerr << "Failed to initialize MOBI data." << std::endl;
        return;
    }

    FILE* file;
    errno_t err = fopen_s(&file, filePath.c_str(), "rb");
    if (err != 0 || file == nullptr) {
        std::cerr << "Failed to open file: " << filePath << " with error code: " << err << std::endl;
        mobi_free(mobiData);
        return;
    }

    if (mobi_load_file(mobiData, file) != MOBI_SUCCESS) {
        std::cerr << "Failed to load MOBI data from file." << std::endl;
        fclose(file);
        mobi_free(mobiData);
        return;
    }
    fclose(file); // Close the file after loading

    MOBIRawml* rawml = mobi_init_rawml(mobiData);
    if (!rawml || mobi_parse_rawml(rawml, mobiData) != MOBI_SUCCESS) {
        std::cerr << "Failed to initialize or parse MOBI rawml" << std::endl;
        mobi_free(mobiData);
        return;
    }

    std::string desktopPath = getDesktopPath();
    MOBIPart* part = rawml->resources;
    while (part) {
        if (part->type == T_JPG || part->type == T_PNG || part->type == T_GIF || part->type == T_BMP) {
            int width, height, channels;
            unsigned char* imgData = stbi_load_from_memory(part->data, part->size, &width, &height, &channels, 0);
            if (imgData) {
                cv::Mat colorImg(height, width, channels == 4 ? CV_8UC4 : channels == 3 ? CV_8UC3 : CV_8UC1, imgData);
                cv::Mat grayImg;
                cv::cvtColor(colorImg, grayImg, cv::COLOR_BGR2GRAY);

                std::string filename = desktopPath + "image_" + std::to_string(part->uid) + ".jpg";
                cv::imwrite(filename, grayImg);

                stbi_image_free(imgData);
                std::cout << "Saved grayscaled image to " << filename << std::endl;
            }
            else {
                std::cerr << "Failed to load image data for processing." << std::endl;
            }
        }
        part = part->next;
    }

    mobi_free_rawml(rawml);
    mobi_free(mobiData);
}