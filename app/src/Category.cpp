#include "Category.h"
#include <stdexcept>
using namespace std;


Category::Category(int id, const string& name)
    : id(id), name(name) {
}
int Category::getId() const {
    return this->id;
}

string Category::getName() const {
    return this->name;
}

void Category::setName(const string& name) {
    if (name.empty()) throw invalid_argument("Name cannot be empty.");
    this->name = name;
}

Category::~Category() {}