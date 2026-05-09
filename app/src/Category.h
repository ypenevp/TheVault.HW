#ifndef CATEGORY_H
#define CATEGORY_H

#include <string>
#include <vector>

class Category {
private:
    int id;
    std::string name;

public:
    Category(int id, const std::string& name);

    int getId() const;
    std::string getName() const;
    void setName(const std::string& name);

    virtual std::string getData() const = 0;
    virtual std::vector<std::string> getFields() const = 0;
    virtual void setFields(const std::vector<std::string>& values) = 0;
    virtual Category* clone() const = 0;

    virtual ~Category();
};

#endif