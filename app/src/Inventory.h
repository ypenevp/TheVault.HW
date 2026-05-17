#ifndef INVENTIRY_H
#define INVENTORY_H

#include "Component.h"
#include "Category.h"
#include "Project.h"
#include <vector>
#include <string>
using namespace std;

class Inventory {
private:
    vector<Component*> components;
    vector<Category*> categories;
    vector<Project> projects;

    int nextComponentId;
    int nextCategoryId;
    int nextProjectId;

    string dbPath;
    string exportsPath;

    int getAllocatedQuantity(int componentId) const;

public:
    Inventory(const string& dbPath = "../db/",
              const string& exportsPath = "../exports/");
    Inventory(const Inventory&) = delete;
    Inventory& operator=(const Inventory&) = delete;

    void addComponent(const std::string& model, double price, int quantity,
                      const std::string& mountingType, const std::string& storageLocation,
                      const std::string& packageType, const std::string& datasheet,
                      Category* category,
                      const std::map<std::string, std::string>& customValues);
    void removeComponent(int id);
    void editComponent(int id, const std::string& model, double price,
                       const std::string& mountingType, const std::string& storageLocation,
                       const std::string& packageType, const std::string& datasheet,
                       const std::map<std::string, std::string>& extraFields);
    Component* getComponentById(int id) const;
    vector<Component*> getAllComponents() const;

    void addCategory(Category* category);
    void removeCategory(int id);
    void editCategory(int id, const string& name);
    Category* getCategoryById(int id) const;
    vector<Category*> getAllCategories() const;

    void addProject(const string& name, const string& description,
                    const string& startDate);
    void removeProject(int id);
    void editProject(int id, const string& name, const string& description,
                     const string& startDate);
    Project* getProjectById(int id);
    const Project* getProjectById(int id) const;
    vector<Project>& getAllProjects();
    Project getProjectDetails(int projectId) const;

    void addComponentToProject(int projectId, int componentId, int quantity);
    void removeComponentFromProject(int projectId, int componentId);
    void updateAllocation(int projectId, int componentId, int newQuantity);

    int getFreeQuantity(int componentId) const;
    void printDistribution(int componentId) const;

    void saveToFile() const;
    void loadFromFile();
    void clearAll();

    ~Inventory();
};

#endif
