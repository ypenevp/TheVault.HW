#ifndef CUSTOM_CATEGORY_H
#define CUSTOM_CATEGORY_H

#include "Category.h"
#include <vector>
#include <string>

class CustomCategory : public Category {
private:
    std::vector<std::string> fieldNames;

public:
    CustomCategory(int id, const std::string& name);
    CustomCategory(int id, const std::string& name, const std::vector<std::string>& fieldNames);

    void addField(const std::string& fieldName);
    void removeField(const std::string& fieldName);

    std::vector<std::string> getCustomFields() const;
    std::string getData() const override;
    std::vector<std::string> getFields() const override;
    
    void setFields(const std::vector<std::string>& values) override;
    Category* clone() const override;
};

#endif