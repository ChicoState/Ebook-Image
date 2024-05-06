#include "Library.h"
#include <string>
#include <windows.h>
#include <vector>
#include <iostream>
#include <libzippp/libzippp.h>
#include <zip.h>
#include <tinyxml2.h>
#include <sstream>
#include "Epub.h"
#include "AZW3.h"
#include "PDF.h"
#pragma once
std::string Library::printall()
{
    std::string allBooks = "";
    std::vector<book>::iterator it;
    for (it = collection.begin(); it != collection.end(); ++it)
    {

		book curr = *it;
		allBooks += ("<div class=\"form-control\"> <input type=\"checkbox\" name=\"title\" id= \"" + std::to_string(curr.ID) + "\"  value= \"" + curr.title + "\"/> <label for=\"" + std::to_string(curr.ID) + "\">" + curr.title + "</label> </div>");
		/*std::cout << "Author: " << curr.author << std::endl;
		std::cout << "Publisher: " << curr.publisher << std::endl;
		std::cout << "Contributor: " << curr.contributor << std::endl;
		std::cout << "Rights: " << curr.rights << std::endl;
		std::cout << "Format: " << curr.format << std::endl;
		std::cout << "Date: " << curr.date << std::endl;
		std::cout << "Language: " << curr.language << std::endl;
		std::cout << "Description: " << curr.description << std::endl;
		std::cout << "Rights: " << curr.rights << std::endl;    */
    }
    return allBooks;
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

std::string Library::getFileExtension(const std::string& filePath) 
{
    size_t pos = filePath.rfind('.');

    if (pos != std::string::npos) {
        return filePath.substr(pos + 1);
    }

    // Return an empty string if no '.' found
    return "";
}
int ID = 0;
std::string Library::add(PWSTR path)
{
    std::string f = getFileExtension(wstrtostr(path));
    if (f == "epub")
    {
        Epub e;
        e.grayscaleEpub(path);
        book curr = e.add(path);
        curr.ID = ID;
        ++ID;
        collection.push_back(curr);
        titles.push_back(curr.title);
        return curr.title;
	}
    else if (f == "AZW3" || f == "azw3")
    {
       /* AZW3 a;
        book curr = a.add(path);
        collection.push_back(curr);
        return curr.title;*/
	}
    else if (f == "pdf" || f == "PDF")
    {
        /*PDF p;
        book curr = p.add(path);
        collection.push_back(curr);
        return curr.title;*/
	}
    else
    {
		MessageBoxA(NULL, "File format not supported", "Error", MB_OK);
	}
    return "";
}

std::vector<std::string> Library::getBookTitles() {
    std::vector<std::string> titles;
    for (const book& b : collection) {
        titles.push_back(b.title);
    }
    return titles;
}

int Library::remove(int ID) // this actually removes the book from the library. 
{
    for (int i = 0; i < collection.size(); i++)
    {
        if (collection[i].ID == ID)
        {
			collection.erase(collection.begin() + i);
			return 1;
		}   
    }
    return -1;
}