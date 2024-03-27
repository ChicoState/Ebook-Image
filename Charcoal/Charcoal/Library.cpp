#include "Library.h"
#include <string>
#include <windows.h>
#include <vector>
#include <iostream>
#include <libzippp/libzippp.h>
#include <zip.h>
#include <tinyxml2.h>
#include <sstream>
void Library::printall()
{
    std::vector<book>::iterator it;
    for (it = collection.begin(); it != collection.end(); ++it)
    {
		book curr = *it;
		std::cout << "Title: " << curr.title << std::endl;
		std::cout << "Author: " << curr.author << std::endl;
		std::cout << "Publisher: " << curr.publisher << std::endl;
		std::cout << "Contributor: " << curr.contributor << std::endl;
		std::cout << "Rights: " << curr.rights << std::endl;
		std::cout << "Format: " << curr.format << std::endl;
		std::cout << "Date: " << curr.date << std::endl;
		std::cout << "Language: " << curr.language << std::endl;
		std::cout << "Description: " << curr.description << std::endl;
		std::cout << "Rights: " << curr.rights << std::endl;    
    }
}

std::string Library::getStringData(std::string name)
{
    std::ostringstream meta;
    std::string data;
    for (int i = 0; i < collection.size(); i++)
    {
        if (collection[i].title == name)
        {
            meta << "Title: " << collection[i].title << std::endl;
            meta << "Author: " << collection[i].author << std::endl;
            meta << "Publisher: " << collection[i].publisher << std::endl;
            meta << "Contributor: " << collection[i].contributor << std::endl;
            meta << "Rights: " << collection[i].rights << std::endl;
            meta << "Format: " << collection[i].format << std::endl;
            meta << "Date: " << collection[i].date << std::endl;
            meta << "Language: " << collection[i].language << std::endl;
            meta << "Description: " << collection[i].description << std::endl;
            meta << "Rights: " << collection[i].rights << std::endl;
		}
	}
    data = meta.str();
    return data;

}

std::string Library::wstrtostr(const std::wstring& wstr)
{
    std::string strTo;
    char* szTo = new char[wstr.length() + 1];
    szTo[wstr.size()] = '\0';
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
    strTo = szTo;
    delete[] szTo;
    return strTo;
}

std::string getFileExtension(const std::string& filePath) 
{
    size_t pos = filePath.rfind('.');

    if (pos != std::string::npos) {
        return filePath.substr(pos + 1);
    }

    // Return an empty string if no '.' found
    return "";
}
std::string Library::add(PWSTR path)
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
				return NULL;
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
    return curr.title;
}