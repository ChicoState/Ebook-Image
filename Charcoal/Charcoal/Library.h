#include <string>
#include<windows.h>
#include <vector>
#pragma once
class book {
public:
    int id;
    std::string title;
    std::string author;
    std::string publisher;
    std::string contributor;
    std::string rights;
    std::string format;
    std::string date;
    std::string language;
    std::string description;

    float contrastValue;

    PWSTR path;


};

class Library {
public:
    std::string printall();
    std::string add(PWSTR path);
    std::string getStringData(std::string name);
    std::string wstrtostr(const std::wstring& wstr);
    std::string getFileExtension(const std::string& filePath);
protected:
    std::vector<book> collection;
    void push(book b);

};



