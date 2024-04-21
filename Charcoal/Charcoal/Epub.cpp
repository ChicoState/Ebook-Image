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
#define _CRT_SECURE_NO_WARNINGS
#pragma once
book Epub::add(PWSTR path)
{
    book curr;
    std::string npath = wstrtostr(path);
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

void Epub::grayscaleEpub(PWSTR path) {
    std::string npath = wstrtostr(path);
    using namespace libzippp;
    ZipArchive zipArchive(npath);
    zipArchive.open(ZipArchive::Write);
    std::vector<ZipEntry> entries = zipArchive.getEntries();
    std::vector<ZipEntry>::iterator it;
    std::string temp = "C:\\Users\\Younitea\\Documents\\Books\\tst\\OEBPS\\images\\"; //TODO MAKE RELATIVE/through memory, not temp file
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

            // Grayscale the image
            grayscaleImage(image, width, height);

            int value = 10;
            // Contrast Image
            contrastImage(image, width, height, value);
            
            std::string base_filename = name.substr(name.find_last_of("/\\") + 1);
            if(fileExt == "jpeg" || fileExt == "jpg")
                stbi_write_jpg((temp + base_filename).c_str(), width, height, channels, image, 100);
            else if(fileExt == "png")
                stbi_write_png((temp + base_filename).c_str(), width, height, channels, image, (width * channels));
            // Write the modified image back to the epub archive
            zipArchive.deleteEntry(name);
            zipArchive.addFile(name, (temp + base_filename).c_str());
            stbi_image_free(image);
        }
    }
    //zipArchive.addEntry("C:/Users/Younitea/Documents/Books/tst/OEBPS/images/");
    // Save changes to the epub file
    zipArchive.close();
}

void Epub::grayscaleImage(unsigned char* imageData, int width, int height) {
    // Convert RGB image to grayscale
    for (int i = 0; i < width * height; ++i) {
        unsigned char gray = static_cast<unsigned char>(
            0.21f * imageData[3 * i] +
            0.72f * imageData[3 * i + 1] +
            0.07f * imageData[3 * i + 2]
            );

        imageData[3 * i] = gray;
        imageData[3 * i + 1] = gray;
        imageData[3 * i + 2] = gray;
    }
}

void Epub::contrastImage(unsigned char* imageData, int width, int height, int value) {
    // Convert RGB image to grayscale
    for (int i = 0; i < width * height; ++i) {
        unsigned char gray = static_cast<unsigned char>(
            value * imageData[3 * i]
            );

        if (gray > UCHAR_MAX) {
            imageData[3 * i] = UCHAR_MAX;
            imageData[3 * i + 1] = UCHAR_MAX;
            imageData[3 * i + 2] = UCHAR_MAX;
        } else {
            imageData[3 * i] = gray;
            imageData[3 * i + 1] = gray;
            imageData[3 * i + 2] = gray;
        }
    }
}