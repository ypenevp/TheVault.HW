#ifndef PROJECT_H
#define PROJECT_H

#include "UsedComponent.h"
#include <vector>
#include <string>
using namespace std;

class Project {
private:
    int id;
    string name;
    string description;
    string status;
    string startDate;
    vector<UsedComponent> components;

public:
    Project(int id, const string& name, const string& description, const string& startDate);

    int getId() const;
    string getName() const;
    string getDescription() const;
    string getStatus() const;
    string getStartDate() const;
    bool isArchived() const;

    vector<UsedComponent>& getComponents();
    const vector<UsedComponent>& getComponents() const;

    void setName(const string& name);
    void setDescription(const string& description);
    void setStartDate(const string& startDate);

    void archive();
    void activate();

    void addComponent(Component* component, int quantity);
    void removeComponent(int componentId);
    void updateComponentQuantity(int componentId, int newQuantity);

    double getTotalPrice() const;
};

#endif