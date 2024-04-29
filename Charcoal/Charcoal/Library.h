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
    //Optional paramaters for quality and image resize, 
    //g for grayscale, c for compress, including a percent (1-100) for x, s for size, including a height then width as x and y
    void processImage(std::string path, char flag, std::string destination, int x = -1, int y = -1);

};



