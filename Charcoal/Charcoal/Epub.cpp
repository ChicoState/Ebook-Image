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

void Epub::grayscaleBook(PWSTR path) {
    std::string npath = wstrtostr(path);
    libzippp::ZipArchive zipArchive(npath);

    zipArchive.open(libzippp::ZipArchive::Write);
    using namespace libzippp;
    // Process each entry in the epub archive
    std::vector<ZipEntry> entries = zipArchive.getEntries();
    std::vector<ZipEntry>::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
        ZipEntry entry = *it;
        if (!entry.isFile()) continue; // Skip directories
        std::string name = entry.getName();
        int size = entry.getSize();
        // Check if the entry is within the "images" directory
        if (entry.getName().find("images/") != 0) continue;

        // Read the image file from the epub archive
        unsigned char* data = (unsigned char*)zipArchive.readEntry(name);

        int width, height, channels;
        unsigned char* imageData = stbi_load_from_memory(data, size, &width, &height, &channels, STBI_rgb_alpha);

        // Grayscale the image
        grayscaleImage(imageData, width, height);

        // Remove the original image file from the epub archive
        zipArchive.deleteEntry(entry);

        // Write the modified image back to the epub archive
        std::vector<unsigned char> modifiedImageData(imageData, imageData + width * height * 3);
        zipArchive.addData(entry.getName(), modifiedImageData.data(), modifiedImageData.size());

        stbi_image_free(imageData);
    }

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
