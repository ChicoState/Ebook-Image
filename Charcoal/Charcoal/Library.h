#include <string>
#include<windows.h>
#include <vector>
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
    void printall();
    std::string add(PWSTR path);
    std::string getStringData(std::string name);
    std::string wstrtostr(const std::wstring& wstr);
protected:
    std::vector<book> collection;

};



