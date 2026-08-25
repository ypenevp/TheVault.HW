#ifndef IC_H
#define IC_H

#include "Category.h"
#include <string>

class IC : public Category {
public:
    IC(int id, const std::string& name);

    std::string getData() const override;
    std::vector<std::string> getFields() const override;
    void setFields(const std::vector<std::string>& values) override;
    Category* clone() const override;
};

#endif