#include "Epub.h"
#include <zip.h>
#include <libzippp/libzippp.h>
#include <tinyxml2.h>
#include <string>
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

void Epub::remove_drm(PWSTR path)
{
    book curr;
    std::string npath = wstrtostr(path);
    using namespace libzippp;
    ZipArchive zf(npath);
    zf.open(ZipArchive::Write);
    std::vector<ZipEntry> entries = zf.getEntries();
    std::vector<ZipEntry>::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
        ZipEntry entry = *it;
        std::string name = entry.getName();
        //int size = entry.getSize();
        if (getFileExtension(name) == "drm") {
            zf.deleteEntry(name);
        }
    }
    zf.close();
}