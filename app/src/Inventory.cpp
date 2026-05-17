#include "Inventory.h"
#include "Resistor.h"
#include "Transistor.h"
#include "Diode.h"
#include "CustomCategory.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <set>
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////

Inventory::Inventory(const string &dbPath, const string &exportsPath)
    : nextComponentId(1), nextCategoryId(1), nextProjectId(1),
      dbPath(dbPath), exportsPath(exportsPath)
{

    loadFromFile();

    if (this->categories.empty())
    {
        this->categories.push_back(new Resistor(this->nextCategoryId++, "Resistor", 0, 0, 0, 0));
        this->categories.push_back(new Transistor(this->nextCategoryId++, "Transistor", "-", 0, 0, 0, 0, 0));
        this->categories.push_back(new Diode(this->nextCategoryId++, "Diode", 0, 0));
    }
}

Inventory::~Inventory()
{
    for (auto *c : this->components)
        delete c;
    for (auto *cat : this->categories)
        delete cat;
}

////////////////////////////////////////////////////////////////////////////////////////////

const string C_RESET = "\033[0m";
const string C_BOLD = "\033[1m";
const string C_CYAN = "\033[96m";
const string C_YELLOW = "\033[38;5;226m";
const string C_ORANGE = "\033[38;5;208m";
const string C_GREEN = "\033[38;5;46m";
const string C_WHITE = "\033[97m";
const string C_RED = "\033[91m";
const string C_MAG = "\033[95m";

int Inventory::getAllocatedQuantity(int componentId) const
{
    int total = 0;
    for (const auto &p : this->projects)
    {
        if (p.isArchived())
            continue;
        for (const auto &uc : p.getComponents())
            if (uc.getComponent()->getId() == componentId)
                total += uc.getAllocatedQuantity();
    }
    return total;
}

////////////////////////////////////////////////////////////////////////////////////////////

void Inventory::addComponent(const string &model, double price, int quantity,
                             const string &mountingType, const string &storageLocation,
                             const string &packageType, const string &datasheet,
                             Category *category,
                             const map<string, string> &customValues)
{
    Component *comp = new Component(this->nextComponentId++, model, price,
                                    quantity, mountingType, storageLocation,
                                    packageType, datasheet, category);
    for (const auto &cv : customValues)
        comp->setCustomValue(cv.first, cv.second);
    this->components.push_back(comp);
}

void Inventory::removeComponent(int id)
{

    if (getAllocatedQuantity(id) > 0)
        throw runtime_error("Cannot remove!!! Component is used in active project.");

    auto it = find_if(this->components.begin(), this->components.end(),
                      [id](Component *c)
                      { return c->getId() == id; });

    if (it == this->components.end())
        throw invalid_argument("Component not found.");

    delete *it;
    this->components.erase(it);
}

void Inventory::editComponent(int id, const string &model, double price,
                              const string &mountingType, const string &storageLocation,
                              const string &packageType, const string &datasheet,
                              const map<string, string> &extraFields)
{
    Component *comp = getComponentById(id);
    comp->setModel(model);
    comp->setPrice(price);
    comp->setMountingType(mountingType);
    comp->setStorageLocation(storageLocation);
    comp->setPackage(packageType);
    comp->setDatasheet(datasheet);

    for (const auto &kv : extraFields)
    {
        comp->setCustomValue(kv.first, kv.second);
    }
}

Component *Inventory::getComponentById(int id) const
{
    for (auto *c : this->components)
        if (c->getId() == id)
            return c;
    throw invalid_argument("Component with id " + to_string(id) + " not found.");
}

vector<Component *> Inventory::getAllComponents() const
{
    return this->components;
}

////////////////////////////////////////////////////////////////////////////////////////////

void Inventory::addCategory(Category *category)
{
    if (!category)
        throw invalid_argument("Category cannot be null.");
    this->categories.push_back(category->clone());
    this->nextCategoryId = max(this->nextCategoryId, category->getId() + 1);
}

void Inventory::removeCategory(int id)
{
    for (auto *c : this->components)
        if (c->getCategory() && c->getCategory()->getId() == id)
            throw runtime_error("Cannot remove category: it is used by existing components.");

    auto it = find_if(this->categories.begin(), this->categories.end(),
                      [id](Category *cat)
                      { return cat->getId() == id; });

    if (it == this->categories.end())
        throw invalid_argument("Category not found.");

    delete *it;
    this->categories.erase(it);
}

void Inventory::editCategory(int id, const string &name)
{
    getCategoryById(id)->setName(name);
}

Category *Inventory::getCategoryById(int id) const
{
    for (auto *cat : this->categories)
        if (cat->getId() == id)
            return cat;
    throw invalid_argument("Category with id " + to_string(id) + " not found.");
}

vector<Category *> Inventory::getAllCategories() const
{
    return this->categories;
}

////////////////////////////////////////////////////////////////////////////////////////////

void Inventory::addProject(const string &name, const string &description,
                           const string &startDate)
{
    this->projects.emplace_back(this->nextProjectId++, name, description, startDate);
}

void Inventory::removeProject(int id)
{
    auto it = find_if(this->projects.begin(), this->projects.end(),
                      [id](const Project &p)
                      { return p.getId() == id; });

    if (it == this->projects.end())
        throw invalid_argument("Project not found.");

    if (!it->isArchived())
    {
        for (const auto &uc : it->getComponents())
        {
            Component *comp = getComponentById(uc.getComponent()->getId());
            comp->setQuantity(comp->getQuantity() + uc.getAllocatedQuantity());
        }
    }
    this->projects.erase(it);
}

void Inventory::editProject(int id, const string &name, const string &description,
                            const string &startDate)
{
    Project *p = getProjectById(id);
    p->setName(name);
    p->setDescription(description);
    p->setStartDate(startDate);
}

Project *Inventory::getProjectById(int id)
{
    for (auto &p : this->projects)
        if (p.getId() == id)
            return &p;
    throw invalid_argument("Project with id " + to_string(id) + " not found.");
}

const Project *Inventory::getProjectById(int id) const
{
    for (const auto &p : this->projects)
        if (p.getId() == id)
            return &p;
    throw invalid_argument("Project with id " + to_string(id) + " not found.");
}

vector<Project> &Inventory::getAllProjects() { return this->projects; }

Project Inventory::getProjectDetails(int projectId) const
{
    return *getProjectById(projectId);
}

////////////////////////////////////////////////////////////////////////////////////////////

void Inventory::addComponentToProject(int projectId, int componentId, int quantity)
{
    Project *project = getProjectById(projectId);
    if (project->isArchived())
        throw runtime_error("Cannot add components to an archived project.");

    Component *comp = getComponentById(componentId);
    int freeQty = getFreeQuantity(componentId);

    if (quantity > freeQty)
        throw runtime_error("Insufficient stock. Available: " + to_string(freeQty));

    project->addComponent(comp, quantity);
}

void Inventory::removeComponentFromProject(int projectId, int componentId)
{
    getProjectById(projectId)->removeComponent(componentId);
}

void Inventory::updateAllocation(int projectId, int componentId, int newQuantity)
{
    Project *project = getProjectById(projectId);
    if (project->isArchived())
        throw runtime_error("Cannot update allocation in an archived project.");

    int currentAllocation = 0;
    for (const auto &uc : project->getComponents())
        if (uc.getComponent()->getId() == componentId)
            currentAllocation = uc.getAllocatedQuantity();

    int diff = newQuantity - currentAllocation;
    if (diff > 0 && diff > getFreeQuantity(componentId))
        throw runtime_error("Insufficient stock for the requested allocation.");

    project->updateComponentQuantity(componentId, newQuantity);
}

////////////////////////////////////////////////////////////////////////////////////////////

int Inventory::getFreeQuantity(int componentId) const
{
    Component *comp = getComponentById(componentId);
    return comp->getQuantity() - getAllocatedQuantity(componentId);
}

void Inventory::printDistribution(int componentId) const
{
    Component *c = getComponentById(componentId);
    int freeQty = getFreeQuantity(componentId);

    cout << "\n  " << C_BOLD << C_CYAN << "[ Stock Management for " << C_YELLOW << c->getModel() << C_CYAN << " ]\n"
         << C_RESET;
    cout << "  ❖ Total Stock: " << C_WHITE << c->getQuantity() << C_RESET << "\n";
    cout << "  ❖ Free to Use: " << C_GREEN << C_BOLD << freeQty << C_RESET << "\n\n";

    cout << C_BOLD << C_CYAN << left
         << "  " << setw(6) << "ProjID"
         << " | " << setw(20) << "Project Name"
         << " | " << setw(10) << "Status"
         << " | " << setw(10) << "Allocated" << "\n"
         << C_RESET;

    cout << C_WHITE << "  " << string(55, '-') << C_RESET << "\n";

    bool found = false;
    for (const auto &p : projects)
    {
        for (const auto &uc : p.getComponents())
        {
            if (uc.getComponent()->getId() == componentId)
            {
                found = true;
                string stCol = (p.getStatus() == "Active") ? C_GREEN : C_RED;

                cout << "  " << C_BOLD << C_YELLOW << left << setw(6) << p.getId() << C_RESET << " | "
                     << C_WHITE << left << setw(20) << p.getName() << C_RESET << " | "
                     << C_BOLD << stCol << left << setw(10) << p.getStatus() << C_RESET << " | "
                     << C_WHITE << C_BOLD << left << setw(10) << uc.getAllocatedQuantity() << C_RESET << "\n";
            }
        }
    }

    if (!found)
    {
        cout << "  " << C_YELLOW << "No projects are currently allocating this component.\n"
             << C_RESET;
    }

    cout << C_WHITE << "  " << string(55, '-') << C_RESET << "\n";
}

////////////////////////////////////////////////////////////////////////////////////////////

void Inventory::saveToFile() const
{
    //Save categories
    // Format: id|type|name|field1|field2|...
    ofstream catFile(this->dbPath + "categories.txt");
    if (!catFile.is_open())
        return;

    for (auto *cat : this->categories)
    {

        string type = "Custom";
        if (dynamic_cast<Resistor *>(cat))
            type = "Resistor";
        else if (dynamic_cast<Transistor *>(cat))
            type = "Transistor";
        else if (dynamic_cast<Diode *>(cat))
            type = "Diode";

        catFile << cat->getId() << "|" << type << "|" << cat->getName();

        if (type == "Custom")
        {
            auto fields = cat->getFields();
            for (size_t i = 0; i < fields.size(); ++i)
            {
                if (i > 0)
                    catFile << "|";
                else
                    catFile << "|";
                catFile << fields[i];
            }
        }
        else
        {
            catFile << "|" << cat->getData();
        }

        catFile << "\n";
    }
    catFile.close();

    //Save components
    ofstream compFile(this->dbPath + "components.txt");
    if (!compFile.is_open())
        return;

    for (auto *comp : this->components)
    {
        compFile << comp->getId() << "|"
                 << comp->getModel() << "|"
                 << comp->getPrice() << "|"
                 << comp->getQuantity() << "|"
                 << comp->getMountingType() << "|"
                 << comp->getStorageLocation() << "|"
                 << comp->getPackage() << "|"
                 << comp->getDatasheet() << "|"
                 << (comp->getCategory() ? comp->getCategory()->getId() : 0);

        for (const auto &cv : comp->getCustomValues())
            compFile << "|" << cv.first << ":" << cv.second;

        compFile << "\n";
    }
    compFile.close();

    //Save projects
    ofstream projFile(this->dbPath + "projects.txt");
    if (!projFile.is_open())
        return;

    for (const auto &p : this->projects)
    {
        projFile << "PROJECT|" << p.getId() << "|"
                 << p.getName() << "|"
                 << p.getDescription() << "|"
                 << p.getStatus() << "|"
                 << p.getStartDate() << "\n";

        for (const auto &uc : p.getComponents())
            projFile << "COMPONENT|"
                     << uc.getComponent()->getId() << "|"
                     << uc.getAllocatedQuantity() << "\n";
    }
    projFile.close();
}

void Inventory::loadFromFile()
{
    //Load categories
    ifstream catFile(this->dbPath + "categories.txt");
    if (catFile.is_open())
    {
        string line;
        while (getline(catFile, line))
        {
            if (line.empty())
                continue;
            istringstream ss(line);
            string token;
            vector<string> tokens;
            while (getline(ss, token, '|'))
                tokens.push_back(token);
            if (tokens.size() < 3)
                continue;

            int id = stoi(tokens[0]);
            string type = tokens[1];
            string name = tokens[2];

            Category *cat = nullptr;
            if (type == "Resistor")
                cat = new Resistor(id, name, 0, 0, 0, 0);
            else if (type == "Transistor")
                cat = new Transistor(id, name, "NPN", 0, 0, 0, 0, 0);
            else if (type == "Diode")
                cat = new Diode(id, name, 0, 0);
            else if (type == "Custom")
            {
                vector<string> fields;
                for (size_t i = 3; i < tokens.size(); ++i)
                {
                    string field = tokens[i];
                    field.erase(0, field.find_first_not_of(" "));
                    field.erase(field.find_last_not_of(" ") + 1);
                    if (!field.empty())
                        fields.push_back(field);
                }
                cat = new CustomCategory(id, name, fields);
            }

            if (cat)
            {
                this->categories.push_back(cat);
                this->nextCategoryId = max(this->nextCategoryId, id + 1);
            }
        }
        catFile.close();
    }

    //Load components
    ifstream compFile(this->dbPath + "components.txt");
    if (compFile.is_open())
    {
        string line;
        while (getline(compFile, line))
        {
            if (line.empty())
                continue;
            istringstream ss(line);
            string token;
            vector<string> tokens;
            while (getline(ss, token, '|'))
                tokens.push_back(token);
            if (tokens.size() < 7)
                continue;

            int id = stoi(tokens[0]);
            string model = tokens[1];
            double price = stod(tokens[2]);
            int qty = stoi(tokens[3]);
            string mountingType = tokens[4];
            string location = tokens[5];
            string packageType = tokens[6];
            string datasheet = tokens[7];
            int catId = 0;
            if (tokens.size() > 8)
                catId = stoi(tokens[8]);

            Category *cat = nullptr;
            try
            {
                cat = getCategoryById(catId);
            }
            catch (...)
            {
            }

            Component *comp = new Component(id, model, price, qty,
                                            mountingType, location,
                                            packageType, datasheet, cat);

            for (size_t i = 9; i < tokens.size(); ++i)
            {
                auto pos = tokens[i].find(':');
                if (pos != string::npos)
                    comp->setCustomValue(tokens[i].substr(0, pos),
                                         tokens[i].substr(pos + 1));
            }

            this->components.push_back(comp);
            this->nextComponentId = max(this->nextComponentId, id + 1);
        }
        compFile.close();
    }

    //Load projects
    ifstream projFile(this->dbPath + "projects.txt");
    if (projFile.is_open())
    {
        string line;
        Project *currentProject = nullptr;

        while (getline(projFile, line))
        {
            if (line.empty())
                continue;
            istringstream ss(line);
            string token;
            vector<string> tokens;
            while (getline(ss, token, '|'))
                tokens.push_back(token);
            if (tokens.empty())
                continue;

            if (tokens[0] == "PROJECT" && tokens.size() >= 6)
            {
                int id = stoi(tokens[1]);
                this->projects.emplace_back(id, tokens[2], tokens[3], tokens[5]);
                if (tokens[4] == "archived")
                    this->projects.back().archive();
                currentProject = &this->projects.back();
                this->nextProjectId = max(this->nextProjectId, id + 1);
            }
            else if (tokens[0] == "COMPONENT" && tokens.size() >= 3 && currentProject)
            {
                int compId = stoi(tokens[1]);
                int allocQty = stoi(tokens[2]);
                try
                {
                    Component *comp = getComponentById(compId);
                    currentProject->addComponent(comp, allocQty);
                }
                catch (...)
                {
                }
            }
        }
        projFile.close();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

void Inventory::clearAll()
{
    for (auto *c : this->components)
        delete c;
    this->components.clear();

    for (auto *cat : this->categories)
        delete cat;
    this->categories.clear();
    this->projects.clear();

    this->nextComponentId = 1;
    this->nextProjectId = 1;

    this->nextCategoryId = 1;
    this->categories.push_back(new Resistor(this->nextCategoryId++, "Resistor", 0, 0, 0, 0));
    this->categories.push_back(new Transistor(this->nextCategoryId++, "Transistor", "-", 0, 0, 0, 0, 0));
    this->categories.push_back(new Diode(this->nextCategoryId++, "Diode", 0, 0));

    this->saveToFile();
}