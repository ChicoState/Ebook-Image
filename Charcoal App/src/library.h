#include <string>
#include<windows.h>
#include <vector>
class book {
public:
    int id;
    std::string name;
    std::string author;
    std::string publisher;
    PWSTR path;


};

class library {
public:
    void printall();
    void add(PWSTR path);
protected:
    std::vector<book> collection;

    void parsemetadata(PWSTR path);
};

