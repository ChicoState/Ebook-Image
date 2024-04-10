#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
//only do this once
#include <fstream>
#include "Epub.h"
#include <zip.h>
#include <libzippp/libzippp.h>
#include <tinyxml2.h>
#include <string>
#include "stb_image.h"
#include "stb_image_write.h"
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
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
    curr.path = npath;
    return curr;
}

void Epub::grayscaleEpub(std::string path) {
    std::string npath = path;
    using namespace libzippp;
    ZipArchive zipArchive(npath);
    zipArchive.open(ZipArchive::Write);
    std::vector<ZipEntry> entries = zipArchive.getEntries();
    std::vector<ZipEntry>::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
        ZipEntry entry = *it;
        std::string name = entry.getName();
        // Check if the entry is within the "images" directory
        std::string fileExt = getFileExtension(name);
        //Only can do png and jpg for the moment
        if (fileExt == "png" || fileExt == "jpeg" || fileExt == "jpg") 
        {
            std::vector<char> data = entry.readAsBinary();
            // Read the image file from the epub archive
            cv::Mat raw = cv::Mat(data);
            cv::Mat image = cv::imdecode(raw, cv::IMREAD_COLOR);
            if (image.empty()) 
            {
                break;
			}
            cv::Mat gray;
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

            // Save the grayscale image
            std::ofstream of("out");
            cv::imwrite("temp.jpg", gray);
        }
    }
    //zipArchive.addEntry("C:/Users/Younitea/Documents/Books/tst/OEBPS/images/");
    // Save changes to the epub file
    zipArchive.close();
}
