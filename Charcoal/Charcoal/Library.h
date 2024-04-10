#include <string>
#include<windows.h>
#include <vector>
#include <string>
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

    std::string path;


};

class Library {
public:
    std::string printall();
    std::string add(PWSTR path);
    std::string getStringData(std::string name);
    std::string wstrtostr(const std::wstring wstr);
    std::string getFileExtension(const std::string filePath);
    void grayscale(std::string path);
    std::vector<book> collection;
protected:
    void push(book b);

};



