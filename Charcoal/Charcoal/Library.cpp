#include "Library.h"
#include <string>
#include <windows.h>
#include <vector>
#include <iostream>
#include <libzippp/libzippp.h>
#include <zip.h>
#include <tinyxml2.h>
void Library::printall()
{
    for (int i = 0; i < collection.size(); i++) {
        std::cout << collection[i].title << std::endl;
    }
}

std::string wstrtostr(const std::wstring& wstr)
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
void Library::add(PWSTR path)
{
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
				return;
			}
            book curr;
            curr.title = metadata->FirstChildElement("dc:title")->GetText();
            curr.author = metadata->FirstChildElement("dc:creator")->GetText();
            curr.contributor = metadata->FirstChildElement("dc:contributor")->GetText();
            curr.publisher = metadata->FirstChildElement("dc:publisher")->GetText();
            collection.push_back(curr);
            //test
            //adding book to the collection
        }

    }
    zf.close();
}