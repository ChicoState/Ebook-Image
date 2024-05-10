    #define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
//only do this once

#include "Epub.h"
#include <zip.h>
#include <libzippp/libzippp.h>
#include <tinyxml2.h>
#include <string>
#include "stb_image.h"
#include "stb_image_write.h"
#include "filesystem"
#include <vector>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define _CRT_SECURE_NO_WARNINGS
#pragma once
book Epub::add(std::string npath)
{
    book curr;
    using namespace libzippp;
    ZipArchive zf(npath);
    zf.open(ZipArchive::ReadOnly);
    std::vector<ZipEntry> entries = zf.getEntries();
    std::vector<ZipEntry>::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
        ZipEntry entry = *it;
        std::string name = entry.getName();
        int size = entry.getSize();



        //find the opf file that contains the metadata
        if (getFileExtension(name) == "opf")
        {
            //the length of textData will be given by 'size'
            std::string textData = entry.readAsText();
            //using tinyxml2 to parse the xml file
            tinyxml2::XMLDocument doc;
            doc.Parse(textData.c_str());
            tinyxml2::XMLElement* root = doc.FirstChildElement("package");
            tinyxml2::XMLElement* metadata = root->FirstChildElement("metadata");
            if (metadata == NULL)
            {
                MessageBoxA(NULL, "No metadata found", "File", MB_OK);
            }
            std::string title = "dc:title";
            std::string creator = "dc:creator";
            std::string contributor = "dc:contributor";
            std::string publisher = "dc:publisher";
            std::string language = "dc:language";
            std::string rights = "dc:rights";
            std::string date = "dc:date";
            std::string description = "dc:description";
            std::string format = "dc:format";
            tinyxml2::XMLElement* meta = metadata->FirstChildElement();
            while (meta != NULL)
            {
                if (meta->GetText() != NULL)
                {
                    if (meta->Name() == title)
                    {
                        curr.title = meta->GetText();
                    }
                    else if (meta->Name() == creator)
                    {
                        curr.author = meta->GetText();
                    }
                    else if (meta->Name() == contributor)
                    {
                        curr.contributor = meta->GetText();
                    }
                    else if (meta->Name() == publisher)
                    {
                        curr.publisher = meta->GetText();
                    }
                    else if (meta->Name() == language)
                    {
                        curr.language = meta->GetText();
                    }
                    else if (meta->Name() == rights)
                    {
                        curr.rights = meta->GetText();
                    }
                    else if (meta->Name() == date)
                    {
                        curr.date = meta->GetText();
                    }
                    else if (meta->Name() == description)
                    {
                        curr.description = meta->GetText();
                    }
                    else if (meta->Name() == format)
                    {
                        curr.format = meta->GetText();
                    }
                }
                meta = meta->NextSiblingElement();
            }
            
            collection.push_back(curr);
            //adding book to the collection
        }


    }
    zf.close();
    return curr;
}

void Epub::grayscaleEpub(std::string path) {
    using namespace libzippp;
    ZipArchive zipArchive(path);
    zipArchive.open(ZipArchive::Write);
    std::vector<ZipEntry> entries = zipArchive.getEntries();
    std::vector<ZipEntry>::iterator it;
    std::filesystem::path path_path = std::filesystem::temp_directory_path();
    std::string temp = path_path.string();
    std::vector<std::string> file_clean;
    for (it = entries.begin(); it != entries.end(); ++it) {
        ZipEntry entry = *it;
        std::string name = entry.getName();
        // Check if the entry is within the "images" directory
        std::string fileExt = getFileExtension(name);
        //Only can do png and jpg for the moment
        if (fileExt == "png" || fileExt == "jpeg" || fileExt == "jpg") {
            // Read the image file from the epub archive
            int width, height, channels;
            unsigned char* image = stbi_load_from_memory((unsigned char*)entry.readAsBinary(), entry.getSize(), &width, &height, &channels, 0);
           
            
            std::string base_filename = name.substr(name.find_last_of("/\\") + 1);
            if(fileExt == "jpeg" || fileExt == "jpg")
                stbi_write_jpg((temp + base_filename).c_str(), width, height, channels, image, 100);
            else if(fileExt == "png")
                stbi_write_png((temp + base_filename).c_str(), width, height, channels, image, (width * channels));
            // Write the modified image back to the epub archive
            cv::Mat read_image = cv::imread((temp + base_filename));
            cv::cvtColor(read_image, read_image, cv::COLOR_BGR2GRAY);
            cv::imwrite((temp + base_filename), read_image);
            zipArchive.deleteEntry(name);
            zipArchive.addFile(name, (temp + base_filename).c_str());
            file_clean.push_back(temp + base_filename);
            stbi_image_free(image);
        }
    }
    zipArchive.close();
    for (size_t i = 0; i < file_clean.size(); i++) {
        std::string file_name = file_clean.at(i);
        std::filesystem::remove(file_name);
    }
}
