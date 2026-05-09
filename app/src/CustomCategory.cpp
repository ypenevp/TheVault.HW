#include "CustomCategory.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>
using namespace std;

CustomCategory::CustomCategory(int id, const string& name) : Category(id, name) {}

CustomCategory::CustomCategory(int id, const string& name, const vector<string>& fieldNames)
    : Category(id, name), fieldNames(fieldNames) {}


void CustomCategory::addField(const string& fieldName) {
    if (fieldName.empty()) throw invalid_argument("Field name cannot be empty!");
    if (find(this->fieldNames.begin(), this->fieldNames.end(), fieldName) != this->fieldNames.end()) throw invalid_argument("Field already exists!");

    this->fieldNames.push_back(fieldName);
}

void CustomCategory::removeField(const string& fieldName) {
    auto it = find(this->fieldNames.begin(), this->fieldNames.end(), fieldName);

    if (it == this->fieldNames.end()) throw invalid_argument("Field '" + fieldName + "' does not exist.");

    this->fieldNames.erase(it);
}


vector<string> CustomCategory::getCustomFields() const {
    return this->fieldNames;
}

string CustomCategory::getData() const {
    ostringstream oss;
    for (size_t i = 0; i < this->fieldNames.size(); ++i) {
        if (i > 0) oss << " | ";
        oss << this->fieldNames[i];
    }
    return oss.str();
}

vector<string> CustomCategory::getFields() const {
    return this->fieldNames;
}

void CustomCategory::setFields(const vector<string>& values) {
    if (values.empty())
        throw invalid_argument("Custom category must have at least one field.");

    this->fieldNames = values;
}

Category* CustomCategory::clone() const {
    return new CustomCategory(*this);
}