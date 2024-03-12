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

    PWSTR path;


};

class Library {
public:
    void printall();
    void add(PWSTR path);
protected:
    std::vector<book> collection;

};



