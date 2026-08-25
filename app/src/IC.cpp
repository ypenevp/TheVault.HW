#include "IC.h"

IC::IC(int id, const std::string& name)
    : Category(id, name) {}

std::string IC::getData() const {
    return "";  // returns no details, it haves Specification
}

std::vector<std::string> IC::getFields() const {
    return {};  // returns no details fields
}

void IC::setFields(const std::vector<std::string>& values) {
}

Category* IC::clone() const {
    return new IC(*this);
}