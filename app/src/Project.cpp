#include "include/Project.h"
#include <stdexcept>
#include <algorithm>
using namespace std;

Project::Project(int id, const string& name, const string& description,
                 const string& startDate)
    : id(id), name(name), description(description),
      status("active"), startDate(startDate) {
    if (name.empty()) throw invalid_argument("Project name cannot be empty.");

    if (startDate.empty()) throw invalid_argument("Start date cannot be empty.");
}

int Project::getId() const { 
    return this->id;
}

string Project::getName() const { 
    return this->name;
}

string Project::getDescription() const { 
    return this->description;
}
string Project::getStatus() const { 
    return this->status;
}
string Project::getStartDate() const { 
    return this->startDate; 
}
bool Project::isArchived() const { 
    return this->status == "archived"; 
}

vector<UsedComponent>& Project::getComponents() { 
    return this->components; 
}
const vector<UsedComponent>& Project::getComponents() const { 
    return this->components;
}

void Project::setName(const string& name) {
    if (name.empty()) throw invalid_argument("Project name cannot be empty!");
    this->name = name;
}

void Project::setDescription(const string& description) {
    this->description = description;
}

void Project::setStartDate(const string& startDate) {
    if (startDate.empty()) throw invalid_argument("Start date cannot be empty!");
    this->startDate = startDate;
}

void Project::archive() {
    this->status = "archived";
}

void Project::activate() {
    this->status = "active";
}

void Project::addComponent(Component* component, int quantity) {
    if (component == nullptr) throw invalid_argument("Component cannot be null!");

    if (quantity <= 0) throw invalid_argument("Quantity must be positive!");
    
    if (this->isArchived())
        throw runtime_error("Cannot add components to an archived project!");

    for (auto& uc : this->components) {
        if (uc.getComponent()->getId() == component->getId()) {
            uc.setAllocatedQuantity(uc.getAllocatedQuantity() + quantity);
            return;
        }
    }
    this->components.emplace_back(component, quantity);
}

void Project::removeComponent(int componentId) {
    auto it = find_if(this->components.begin(), this->components.end(),
        [componentId](const UsedComponent& uc) {
            return uc.getComponent()->getId() == componentId;
        });

    if (it == this->components.end())
        throw invalid_argument("Component not found in project!");

    this->components.erase(it);
}

void Project::updateComponentQuantity(int componentId, int newQuantity) {
    if (newQuantity <= 0) throw invalid_argument("Quantity must be positive!");

    for (auto& uc : this->components) {
        if (uc.getComponent()->getId() == componentId) {
            uc.setAllocatedQuantity(newQuantity);
            return;
        }
    }
    throw invalid_argument("Component not found in project!");
}

double Project::getTotalPrice() const {
    double total = 0.0;
    for (const auto& uc : this->components)
        total += uc.getComponent()->getPrice() * uc.getAllocatedQuantity();
    return total;
}
