#ifndef INVENTORY_H
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

public:
    Inventory(const string& dbPath = "db/",
              const string& exportsPath = "exports/");
    int getAllocatedQuantity(int componentId) const;

    void addComponent(const std::string& model, double price, int quantity,
                      const std::string& mountingType, const std::string& storageLocation,
                      const std::string& packageType, const std::string& datasheet,
                      Category* category, const std::string& manufacturerPN,
                      const std::map<std::string, std::string>& customValues);
    void removeComponent(int id);
    void editComponent(int id, const std::string& model, double price,
                       const std::string& mountingType, const std::string& storageLocation,
                       const std::string& packageType, const std::string& datasheet,
                       const std::string& manufacturerPN,
                       const std::map<std::string, std::string>& extraFields);
    Component* getComponentById(int id) const;
    vector<Component*> getAllComponents() const;

    void addCustomCategory(const std::string& name, const std::vector<std::string>& fields);
    void removeCategory(int id);
    void editCategory(int id, const string& name);
    Category* getCategoryById(int id) const;
    vector<Category*> getAllCategories() const;

    void addProject(const string& name, const string& description, const string& startDate);
    void removeProject(int id);
    void editProject(int id, const string& name, const string& description, const string& startDate);
    Project* getProjectById(int id);
    const Project* getProjectById(int id) const; // for generateBOM andgetProjectDetails where is used const object
    vector<Project>& getAllProjects();
    Project getProjectDetails(int projectId) const;
    void archiveProject(int id);
    void activateProject(int id);

    void addComponentToProject(int projectId, int componentId, int quantity);
    void removeComponentFromProject(int projectId, int componentId);
    void updateAllocation(int projectId, int componentId, int newQuantity);
    int getFreeQuantity(int componentId) const;
    void printDistribution(int componentId) const;

    vector<Component*> searchByName(const string& name) const;
    vector<Component*> searchByCategory(int categoryId) const;
    vector<Component*> searchByLocation(const string& location) const;
    vector<Component*> searchByPriceRange(double minPrice, double maxPrice) const;

    void compareComponents(int id1, int id2) const;
    void generateBOM(int projectId) const;
    void importBOM(const string& filename) const;

    void saveToFile() const;
    void loadFromFile();
    void clearAll();

    ~Inventory();
};

#endif
