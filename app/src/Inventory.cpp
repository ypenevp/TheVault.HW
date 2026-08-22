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
#include <ctime>
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

#define C_RESET     "\033[0m"
#define C_BOLD      "\033[1m"
#define C_CYAN      "\033[96m"
#define C_WHITE     "\033[97m"
#define C_RED       "\033[91m"
#define C_MAG       "\033[95m"
#define C_GREEN     "\033[38;5;46m"
#define C_YELLOW    "\033[38;5;226m"
#define C_ORANGE    "\033[38;5;208m"
#define C_DEEP_RED  "\033[38;5;196m"
#define C_TEAL      "\033[38;2;0;255;230m"
#define C_BLUE      "\033[94m"

int Inventory::getAllocatedQuantity(int componentId) const
{
    int total = 0;

    for (size_t i = 0; i < this->projects.size(); i++) {

        if (this->projects[i].isArchived())
            continue;

        const vector<UsedComponent>& p_components = this->projects[i].getComponents();
        for (size_t j = 0; j < p_components.size(); j++) {

            if (p_components[j].getComponent()->getId() == componentId) {
                total += p_components[j].getAllocatedQuantity();
            }
        }
    }

    return total;
}

////////////////////////////////////////////////////////////////////////////////////////////

void Inventory::addComponent(const string &model, double price, int quantity,
                             const string &mountingType, const string &storageLocation,
                             const string &packageType, const string &datasheet,
                             Category *category, const std::string& manufacturerPN,
                             const map<string, string> &customValues)
{
    for (size_t i = 0; i < this->components.size(); i++) {
        if (this->components[i]->getModel() == model) {
            throw invalid_argument("This component already exists!!! Change model.");
        }
    }

    Component *comp = new Component(this->nextComponentId++, model, price,
                                    quantity, mountingType, storageLocation,
                                    packageType, datasheet, category, manufacturerPN);

    map<string, string>::const_iterator it;
    for (it = customValues.begin(); it != customValues.end(); it++) {
        comp->setCustomValue(it->first, it->second);
    }

    this->components.push_back(comp);
}

void Inventory::removeComponent(int id)
{
    if (getAllocatedQuantity(id) > 0) {
        throw runtime_error("Cannot remove!!! Component is used in active project.");
    }

    int indexToRemove = -1;
    for (size_t i = 0; i < this->components.size(); i++) {
        if (this->components[i]->getId() == id) {
            indexToRemove = static_cast<int>(i);
            break;
        }
    }

    if (indexToRemove == -1) {
        throw invalid_argument("Component not found.");
    }

    delete this->components[indexToRemove];
    this->components.erase(this->components.begin() + indexToRemove);
}

void Inventory::editComponent(int id, const string &model, double price,
                              const string &mountingType, const string &storageLocation,
                              const string &packageType, const string &datasheet,
                              const std::string& manufacturerPN,
                              const map<string, string> &extraFields)
{
    Component *comp = getComponentById(id);

    for (size_t i = 0; i < this->components.size(); i++) {
        if (this->components[i]->getId() != id && this->components[i]->getModel() == model) {
            throw invalid_argument("A component with this model already exists!!!");
        }
    }

    comp->setModel(model);
    comp->setPrice(price);
    comp->setMountingType(mountingType);
    comp->setStorageLocation(storageLocation);
    comp->setPackage(packageType);
    comp->setDatasheet(datasheet);
    comp->setManufacturerPN(manufacturerPN);

    map<string, string>::const_iterator it;
    for (it = extraFields.begin(); it != extraFields.end(); it++) {
        comp->setCustomValue(it->first, it->second);
    }
}

Component *Inventory::getComponentById(int id) const
{
    for (size_t i = 0; i < this->components.size(); i++) {
        if (this->components[i]->getId() == id) {
            return this->components[i];
        }
    }
    throw invalid_argument("Component with id " + to_string(id) + " not found.");
}

vector<Component *> Inventory::getAllComponents() const
{
    return this->components;
}

////////////////////////////////////////////////////////////////////////////////////////////

void Inventory::addCustomCategory(const std::string& name, const std::vector<std::string>& fields)
{
    if (name.empty())
        throw invalid_argument("Category name cannot be empty.");
    if (fields.empty())
        throw invalid_argument("Custom category must have at least one field.");

    Category* cat = new CustomCategory(this->nextCategoryId++, name, fields);
    this->categories.push_back(cat);
}

void Inventory::removeCategory(int id)
{
    for (size_t i = 0; i < this->components.size(); i++) {
        Category* currCategory = this->components[i]->getCategory();
        if (currCategory != nullptr && currCategory->getId() == id) {
            throw runtime_error("Cannot remove category: it is used by existing components.");
        }
    }

    int indexToRemove = -1;
    for (size_t i = 0; i < this->categories.size(); i++) {
        if (this->categories[i]->getId() == id) {
            indexToRemove = static_cast<int>(i);
            break;
        }
    }

    if (indexToRemove == -1) {
        throw invalid_argument("Category not found.");
    }

    delete this->categories[indexToRemove];
    this->categories.erase(this->categories.begin() + indexToRemove);
}

Category *Inventory::getCategoryById(int id) const
{
    for (size_t i = 0; i < this->categories.size(); i++) {
        if (this->categories[i]->getId() == id) {
            return this->categories[i];
        }
    }
    throw invalid_argument("Category with id " + to_string(id) + " not found.");
}

void Inventory::editCategory(int id, const string &name)
{
    getCategoryById(id)->setName(name);
}

vector<Category *> Inventory::getAllCategories() const
{
    return this->categories;
}

////////////////////////////////////////////////////////////////////////////////////////////

void Inventory::addProject(const string &name, const string &description,
                           const string &startDate)
{
    Project * newP = new Project(this->nextProjectId++,name,description,startDate);
    this->projects.push_back(*newP);
    delete newP;
}


void Inventory::removeProject(int id) {
    int indexToRemove = -1;
    for (size_t i = 0; i < this->projects.size(); i++) {
        if (this->projects[i].getId() == id) {
            indexToRemove = static_cast<int>(i);
            break;
        }
    }

    if (indexToRemove == -1) {
        throw invalid_argument("Project not found.");
    }

    Project& projectRef = this->projects[indexToRemove];

    if (!projectRef.isArchived()) {
        const vector<UsedComponent>& p_components = projectRef.getComponents();
        for (size_t i = 0; i < p_components.size(); i++) {
            Component* comp = getComponentById(p_components[i].getComponent()->getId());
            comp->setQuantity(comp->getQuantity() + p_components[i].getAllocatedQuantity());
        }
    }

    this->projects.erase(this->projects.begin() + indexToRemove);
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
    for (size_t i = 0; i < this->projects.size(); i++) {
        if (this->projects[i].getId() == id) {
            return &this->projects[i];
        }
    }
    throw invalid_argument("Project with id " + to_string(id) + " not found.");
}

const Project *Inventory::getProjectById(int id) const
{
    for (size_t i = 0; i < this->projects.size(); i++) {
        if (this->projects[i].getId() == id) {
            return &this->projects[i];
        }
    }
    throw invalid_argument("Project with id " + to_string(id) + " not found.");
}

vector<Project> &Inventory::getAllProjects() {
    return this->projects; 
}

Project Inventory::getProjectDetails(int projectId) const
{
    return *getProjectById(projectId);
}

////////////////////////////////////////////////////////////////////////////////////////////

void Inventory::addComponentToProject(
    int projectId,
    int componentId,
    int quantity)
{
    Project* project = getProjectById(projectId);

    if (project->isArchived())
        throw runtime_error(
            "Cannot add components to an archived project."
        );

    Component* comp = getComponentById(componentId);

    if (quantity > comp->getQuantity())
        throw runtime_error(
            "Insufficient stock. Available: " +
            to_string(comp->getQuantity())
        );

    comp->setQuantity(
        comp->getQuantity() - quantity
    );

    project->addComponent(comp, quantity);
}

void Inventory::removeComponentFromProject(
    int projectId,
    int componentId)
{
    Project* project = getProjectById(projectId);
    const vector<UsedComponent>& p_components = project->getComponents();

    for (size_t i = 0; i < p_components.size(); i++) {
        if (p_components[i].getComponent()->getId() == componentId) {
            if (!project->isArchived()) {
                Component* comp = getComponentById(componentId);
                comp->setQuantity(comp->getQuantity() + p_components[i].getAllocatedQuantity());
            }
            break;
        }
    }

    project->removeComponent(componentId);
}

void Inventory::updateAllocation(
    int projectId,
    int componentId,
    int newQuantity)
{
    Project* project = getProjectById(projectId);

    if (project->isArchived())
        throw runtime_error("Cannot update archived project.");

    UsedComponent* target;
    vector<UsedComponent>& p_components = project->getComponents();

    for (size_t i = 0; i < p_components.size(); i++) {
        if (p_components[i].getComponent()->getId() == componentId) {
            target = &p_components[i];
            break;
        }
    }

    if (!target)
        throw invalid_argument("Component not found in project.");

    int oldQty = target->getAllocatedQuantity();
    int diff   = newQuantity - oldQty;

    Component* comp = getComponentById(componentId);

    if (diff > comp->getQuantity()) {
        throw runtime_error("Insufficient stock.");
    }

    comp->setQuantity(comp->getQuantity() - diff);
    project->updateComponentQuantity(componentId, newQuantity);
}

void Inventory::archiveProject(int id)
{
    Project* project = getProjectById(id);

    if (project->isArchived())
        return;

    const vector<UsedComponent>& p_components = project->getComponents();
    for (size_t i = 0; i < p_components.size(); i++) {
        Component* comp = getComponentById(p_components[i].getComponent()->getId());
        comp->setQuantity(comp->getQuantity() + p_components[i].getAllocatedQuantity());
    }

    project->archive();
}

void Inventory::activateProject(int id)
{
    Project* project = getProjectById(id);

    if (!project->isArchived())
        return;

    const vector<UsedComponent>& p_components = project->getComponents();
    for (size_t i = 0; i < p_components.size(); i++) {
        Component* comp = getComponentById(p_components[i].getComponent()->getId());
        if (p_components[i].getAllocatedQuantity() > comp->getQuantity()) {
            throw runtime_error(
                "Cannot activate project: insufficient stock for '"
                + comp->getModel()
                + "'. Need "
                + to_string(p_components[i].getAllocatedQuantity())
                + ", available "
                + to_string(comp->getQuantity())
                + "."
            );
        }
    }

    for (size_t i = 0; i < p_components.size(); i++) {
        Component* comp = getComponentById(p_components[i].getComponent()->getId());
        comp->setQuantity(comp->getQuantity() - p_components[i].getAllocatedQuantity());
    }

    project->activate();
}

////////////////////////////////////////////////////////////////////////////////////////////

int Inventory::getFreeQuantity(int componentId) const
{
    return getComponentById(componentId)->getQuantity();
}

void Inventory::printDistribution(int componentId) const
{
    Component *c = getComponentById(componentId);
    int freeQty      = getFreeQuantity(componentId);
    int allocatedQty = getAllocatedQuantity(componentId);
    int totalQty     = freeQty + allocatedQty;

    cout << "\n  " << C_BOLD << C_CYAN << "[ Stock Management for " << C_YELLOW << c->getModel() << C_CYAN << " ]\n"
         << C_RESET;
    cout << "  ❖ Total Stock: " << C_WHITE << totalQty    << C_RESET << "\n";
    cout << "  ❖ Free to Use: " << C_GREEN << C_BOLD << freeQty << C_RESET << "\n\n";

    cout << C_BOLD << C_CYAN << left
         << "  " << setw(6) << "ProjID"
         << " | " << setw(20) << "Project Name"
         << " | " << setw(10) << "Status"
         << " | " << setw(10) << "Allocated" << "\n"
         << C_RESET;

    cout << C_WHITE << "  " << string(55, '-') << C_RESET << "\n";

    bool found = false;
    for (size_t i = 0; i < this->projects.size(); i++)
    {
        const vector<UsedComponent>& p_components = this->projects[i].getComponents();
        for (size_t j = 0; j < p_components.size(); j++)
        {
            if (p_components[j].getComponent()->getId() == componentId)
            {
                found = true;
                string stCol = (this->projects[i].getStatus() == "active") ? C_GREEN : C_RED;

                cout << "  " << C_BOLD << C_YELLOW << left << setw(6) << this->projects[i].getId() << C_RESET << " | "
                     << C_WHITE << left << setw(20) << this->projects[i].getName() << C_RESET << " | "
                     << C_BOLD << stCol << left << setw(10) << this->projects[i].getStatus() << C_RESET << " | "
                     << C_WHITE << C_BOLD << left << setw(10) << p_components[j].getAllocatedQuantity() << C_RESET << "\n";
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

void printRow(const string& field,
              const string& val1,
              const string& val2,
              const string& color)
{
    cout << C_BOLD << C_CYAN;
    cout << " " << left << setw(20) << field;
    cout << C_RESET << " | ";

    cout << color << left << setw(30) << val1;
    cout << C_RESET << " | ";

    cout << color << left << setw(30) << val2;
    cout << C_RESET << "\n";
}

void Inventory::compareComponents(int id1, int id2) const
{
    const Component *c1 = getComponentById(id1);
    const Component *c2 = getComponentById(id2);

    cout << "\n"
         << C_BOLD << C_CYAN
         << left << " " << setw(20) << "Field"
         << " | " << setw(30) << ("Component 1 (ID: " + to_string(c1->getId()) + ")")
         << " | " << setw(30) << ("Component 2 (ID: " + to_string(c2->getId()) + ")")
         << "\n"
         << C_RESET;

    cout << C_WHITE << string(88, '-') << C_RESET << "\n";

    printRow("Model",
             c1->getModel(),
             c2->getModel(),
             C_WHITE);

    ostringstream p1, p2;

    p1 << fixed << setprecision(2)
       << c1->getPrice() << " \xE2\x82\xAC";

    p2 << fixed << setprecision(2)
       << c2->getPrice() << " \xE2\x82\xAC";

    // for € (with 2 more " ")
    cout << C_BOLD << C_CYAN 
         << " " << left << setw(20) << "Price"
         << C_RESET << " | "

         << "\033[38;5;46m"
         << left << setw(32) << p1.str()
         << C_RESET << " | "

         << "\033[38;5;46m"
         << left << setw(30) << p2.str()
         << C_RESET << "\n";

    printRow("Category",
             (c1->getCategory() ? c1->getCategory()->getName() : "-"),
             (c2->getCategory() ? c2->getCategory()->getName() : "-"),
             C_WHITE);

    printRow("Quantity (Total)",
             to_string(c1->getQuantity()),
             to_string(c2->getQuantity()),
             C_WHITE);

    printRow("Quantity (Free)",
             to_string(getFreeQuantity(c1->getId())),
             to_string(getFreeQuantity(c2->getId())),
             C_WHITE);

    printRow("Mounting",
             c1->getMountingType(),
             c2->getMountingType(),
             C_WHITE);

    printRow("Location",
             c1->getStorageLocation(),
             c2->getStorageLocation(),
             C_WHITE);

    printRow("Package",
             c1->getPackage(),
             c2->getPackage(),
             C_WHITE);

    string ds1 = c1->getDatasheet();
    string ds2 = c2->getDatasheet();

    cout << C_BOLD << C_CYAN
         << " " << left << setw(20) << "Datasheet"
         << C_RESET << " | ";

    if (ds1 != "-") {
        string visible = "link";

        cout << "\033[38;5;33m"
             << "\033]8;;" << ds1 << "\033\\"
             << visible
             << "\033]8;;\033\\"
             << C_RESET;

        cout << string(30 - visible.length(), ' ');
    } else {
        cout << left << setw(30) << "-";
    }

    cout << " | ";

    if (ds2 != "-") {
        string visible = "link";

        cout << "\033[38;5;33m"
             << "\033]8;;" << ds2 << "\033\\"
             << visible
             << "\033]8;;\033\\"
             << C_RESET;

        cout << string(30 - visible.length(), ' ');
    } else {
        cout << left << setw(30) << "-";
    }

    cout << "\n";
    
    set<string> keys;

    for (const auto &kv : c1->getCustomValues())
        keys.insert(kv.first);

    for (const auto &kv : c2->getCustomValues())
        keys.insert(kv.first);

    if (!keys.empty())
    {

        cout << C_WHITE << string(88, '-') << C_RESET << "\n";

        cout << " "
             << C_BOLD << C_MAG
             << "--- Specification Details ---"
             << C_RESET << "\n";

        for (const auto &key : keys)
        {
            string rawKey = key;
            string unit = "";

            // extract meassurment unit
            size_t s = rawKey.find('{');
            size_t e = rawKey.find('}');

            if (s != string::npos && e != string::npos && e > s) {
                unit = rawKey.substr(s + 1, e - s - 1);
                rawKey = rawKey.substr(0, s);
            }

            auto it1 = c1->getCustomValues().find(key);
            auto it2 = c2->getCustomValues().find(key);

            string v1 = "-";
            if (it1 != c1->getCustomValues().end()) {
                string value = it1->second;
                string modifier = "";
                if (!value.empty() && isalpha(value.back()) && value.length() > 1 && isdigit(value[value.length() - 2])) {
                    modifier = value.back();
                    value.pop_back();
                }
                v1 = value;
                if (!unit.empty()) {
                    v1 += " " + modifier + unit;
                } else if (!modifier.empty()) {
                    v1 += modifier;
                }
            }

            string v2 = "-";
            if (it2 != c2->getCustomValues().end()) {
                string value = it2->second;
                string modifier = "";
                if (!value.empty() && isalpha(value.back()) && value.length() > 1 && isdigit(value[value.length() - 2])) {
                    modifier = value.back();
                    value.pop_back();
                }
                v2 = value;
                if (!unit.empty()) {
                    v2 += " " + modifier + unit;
                } else if (!modifier.empty()) {
                    v2 += modifier;
                }
            }

            printRow(rawKey, v1, v2, C_ORANGE);
        }
    }

    cout << C_WHITE
         << string(88, '-')
         << C_RESET << "\n";
}

////////////////////////////////////////////////////////////////////////////////////////////

vector<Component *> Inventory::searchByName(const string &name) const {
    vector<Component *> results;
    string lowerName = name;
    transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (auto *c : this->components) {
        string lowerModel = c->getModel();
        transform(lowerModel.begin(), lowerModel.end(), lowerModel.begin(), ::tolower);
        if (lowerModel.find(lowerName) != string::npos)
            results.push_back(c);
    }
    return results;
}

vector<Component *> Inventory::searchByCategory(int categoryId) const {
    vector<Component *> results;
    for (auto *c : this->components)
        if (c->getCategory() && c->getCategory()->getId() == categoryId)
            results.push_back(c);
    return results;
}

vector<Component *> Inventory::searchByLocation(const string &location) const {
    vector<Component *> results;
    for (auto *c : this->components)
        if (c->getStorageLocation() == location)
            results.push_back(c);
    return results;
}

vector<Component *> Inventory::searchByPriceRange(double minPrice, double maxPrice) const {
    if (minPrice > maxPrice)
        throw invalid_argument("Min price cannot exceed max price!!!");
    vector<Component *> results;
    for (auto *c : this->components)
        if (c->getPrice() >= minPrice && c->getPrice() <= maxPrice)
            results.push_back(c);
    return results;
}


////////////////////////////////////////////////////////////////////////////////////////////


void Inventory::generateBOM(int projectId) const {
    const Project *project = getProjectById(projectId);
    if (!project) throw invalid_argument("Project not found.");

    string filename = this->exportsPath + "BOM_" + project->getName() + ".txt";
    replace(filename.begin(), filename.end(), ' ', '_');

    ofstream file;
    file.open(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file!");
    }

    time_t now = time(nullptr);
    tm* local = localtime(&now);

    file << "======================================================================================================================\n";
    file << "                                                   BILL OF MATERIALS                                                  \n";
    file << "======================================================================================================================\n";
    file << " Project     : " << project->getName() << "\n";
    file << " Start Date  : " << project->getStartDate() << "\n";
    file << " Export Date : " << setfill('0') << local->tm_year + 1900 << "-" << setw(2) <<  local->tm_mon + 1 << "-" << setw(2) <<  local->tm_mday  << setfill(' ') << "\n";
    file << " Description : " << project->getDescription() << "\n";
    file << "======================================================================================================================\n";
    file << left << " " << setw(23) << "Model"
         << setw(26) << "Manufacturer PN"
         << setw(16) << "Category"
         << setw(18) << "Location"
         << setw(8)  << "Qty"
         << setw(17) << "Unit Price(€)"
         << setw(14) << "Total(€)" << "\n";
    file << "----------------------------------------------------------------------------------------------------------------------\n";

    double grandTotal = 0.0;
    for (const auto &uc : project->getComponents()) {
        Component *comp = uc.getComponent();
        double lineTotal = comp->getPrice() * uc.getAllocatedQuantity();
        grandTotal += lineTotal;

        const Category *cat = comp->getCategory();
        string catName = cat ? cat->getName() : "Unknown";

        string mpn = comp->getManufacturerPN();
        if(mpn.empty()) mpn = "-";

        file << left << " " << setw(23) << comp->getModel()
             << setw(26) << mpn
             << setw(16) << catName
             << setw(18) << comp->getStorageLocation()
             << setw(8) << uc.getAllocatedQuantity()
             << fixed << setprecision(2)
             << setw(16) << comp->getPrice()
             << setw(14) << lineTotal << "\n";
    }

    file << "----------------------------------------------------------------------------------------------------------------------\n";
    file << right << setw(107) << "Grand Total: " << fixed << setprecision(2) << grandTotal << " €\n";
    file << "======================================================================================================================\n";

    file.close();
}

////////////////////////////////////////////////////////////////////////////////////////////

static void alignColumn(const std::string& text, std::size_t width) {
    std::cout << text;
    if (text.size() < width)
        std::cout << std::string(width - text.size(), ' ');
}

void Inventory::importBOM(const std::string& filename) const {
    ifstream file;
    file.open(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file!");
    }
    struct BOMItem {
        std::string value;
        int quantity = 0;
        std::string reference;
        std::string mpn;
        std::string footprint;
        std::string datasheet;
    };

    // Table 1
    const int VALUE_WIDTH     = 30;
    const int QUANTITY_WIDTH  = 8;
    const int REFERENCE_WIDTH = 30;
    const int MPN_WIDTH       = 45;

    // Table 2
    const int DATASHEET_WIDTH = 10;
    const int FOOTPRINT_WIDTH = 45;

    std::vector<BOMItem> items;

    int valueIndex     = -1;
    int quantityIndex  = -1;
    int referenceIndex = -1;
    int mpnIndex       = -1;
    int footprintIndex = -1;
    int datasheetIndex = -1;

    bool isHeaderRow = true;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        std::vector<std::string> cells;
        std::string currentCell;
        bool insideQuotes = false;

        for (char ch : line) {
            if (ch == '"') {
                insideQuotes = !insideQuotes;
            } else if (ch == ',' && !insideQuotes) {
                cells.push_back(currentCell);
                currentCell.clear();
            } else {
                currentCell += ch;
            }
        }
        cells.push_back(currentCell);

        for (std::string& cell : cells) {
            if (cell.size() >= 2 && cell.front() == '"' && cell.back() == '"')
                cell = cell.substr(1, cell.size() - 2);
        }

        if (isHeaderRow) {
            for (int i = 0; i < static_cast<int>(cells.size()); ++i) {
                std::string header = cells[i];
                for (char& c : header) {
                    c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
                }

                if (header == "name" || header == "value")
                    valueIndex = i;
                else if (header == "qty" || header == "quantity")
                    quantityIndex = i;
                else if (header == "designator" || header == "reference")
                    referenceIndex = i;
                else if (header == "manufacturerpn" || header == "mpn" || header == "part number")
                    mpnIndex = i;
                else if (header == "footprint")
                    footprintIndex = i;
                else if (header == "datasheet" || header == "data sheet")
                    datasheetIndex = i;
            }
            isHeaderRow = false;
            continue;
        }

        BOMItem item;
        item.value = (valueIndex >= 0 && valueIndex < static_cast<int>(cells.size()) && !cells[valueIndex].empty()) ? cells[valueIndex] : "-";
        std::string quantityText = (quantityIndex >= 0 && quantityIndex < static_cast<int>(cells.size())) ? cells[quantityIndex] : "0";
        try { item.quantity = std::stoi(quantityText); } catch (...) { item.quantity = 0; } // If conversion fails, default to 0
        item.reference = (referenceIndex >= 0 && referenceIndex < static_cast<int>(cells.size()) && !cells[referenceIndex].empty()) ? cells[referenceIndex] : "-";
        item.mpn = (mpnIndex >= 0 && mpnIndex < static_cast<int>(cells.size()) && !cells[mpnIndex].empty()) ? cells[mpnIndex] : "-";
        item.footprint = (footprintIndex >= 0 && footprintIndex < static_cast<int>(cells.size()) && !cells[footprintIndex].empty()) ? cells[footprintIndex] : "-";
        item.datasheet = (datasheetIndex >= 0 && datasheetIndex < static_cast<int>(cells.size()) && !cells[datasheetIndex].empty()) ? cells[datasheetIndex] : "-";

        items.push_back(item);
    }

    std::cout << "\n" << C_BOLD << C_CYAN << "  ▐▓█▓▌ IMPORTED BOM ANALYSIS\n" << C_RESET;
    std::cout << C_WHITE << std::string(125, '=') << "\n" << C_RESET;
    std::cout << C_BOLD << C_CYAN << std::left;

    std::cout << " "; alignColumn("Name / Value", VALUE_WIDTH);
    std::cout << " | "; alignColumn("Quantity",  QUANTITY_WIDTH);
    std::cout << " | "; alignColumn("Designator", REFERENCE_WIDTH);
    std::cout << " | "; alignColumn("Manufacturer PN", MPN_WIDTH);
    std::cout << "\n" << C_RESET;
    std::cout << C_WHITE << std::string(125, '-') << "\n" << C_RESET;

    for (const BOMItem& item : items) {
        std::cout << C_CYAN << " " << C_TEAL;
        alignColumn(item.value, VALUE_WIDTH);
        std::cout << C_RESET << " | " << C_YELLOW;
        alignColumn(std::to_string(item.quantity), QUANTITY_WIDTH);
        std::cout << C_RESET << " | " << C_ORANGE;
        alignColumn(item.reference, REFERENCE_WIDTH);
        std::cout << C_RESET << " | " << C_WHITE;
        alignColumn(item.mpn, MPN_WIDTH);
        std::cout << C_RESET << "\n";
    }
    std::cout << C_WHITE << std::string(125, '=') << "\n\n" << C_RESET;

    
    if (footprintIndex != -1 || datasheetIndex != -1) {
        std::cout << "\n" << C_BOLD << C_CYAN << "  [ COMPONENT RESOURCES ]\n" << C_RESET;
        std::cout << C_WHITE << std::string(95, '=') << "\n" << C_RESET;
        std::cout << C_BOLD << C_CYAN << std::left;

        std::cout << " "; alignColumn("Name / Value", VALUE_WIDTH);
        std::cout << " | "; alignColumn("Datasheet", DATASHEET_WIDTH);
        std::cout << " | "; alignColumn("Footprint", FOOTPRINT_WIDTH);
        std::cout << "\n" << C_RESET;
        std::cout << C_WHITE << std::string(95, '-') << "\n" << C_RESET;

        for (const BOMItem& item : items) {
            std::cout << C_TEAL << " ";
            alignColumn(item.value, VALUE_WIDTH);
            std::cout << C_RESET << " | ";

            if (item.datasheet != "-" && !item.datasheet.empty()) {
                if (item.datasheet.rfind("http://", 0) == 0 || item.datasheet.rfind("https://", 0) == 0) {
                    std::cout << C_BLUE << "\033]8;;" << item.datasheet << "\033\\link\033]8;;\033\\" << C_RESET;
                    std::cout << std::string(DATASHEET_WIDTH - 4, ' ');
                } else {
                    std::cout << C_WHITE;
                    alignColumn(item.datasheet, DATASHEET_WIDTH);
                    std::cout << C_RESET;
                }
            } else {
                std::cout << C_WHITE;
                alignColumn("-", DATASHEET_WIDTH);
                std::cout << C_RESET;
            }

            std::cout << " | " << C_WHITE;
            alignColumn(item.footprint, FOOTPRINT_WIDTH);
            std::cout << C_RESET << "\n";
        }
        std::cout << C_WHITE << std::string(95, '=') << "\n\n" << C_RESET;
    }

    std::cout << C_BOLD << C_CYAN << "  [ INVENTORY STATUS ]\n\n" << C_RESET;

    std::vector<std::string> availableItems;
    std::vector<std::string> partiallyAvailableItems;
    std::vector<std::string> unavailableItems;

    for (const BOMItem& item : items) {
        Component* match = nullptr;
        if (item.mpn != "-" && !item.mpn.empty()) {
            for (auto* c : this->components) {
                if (c->getManufacturerPN() == item.mpn) {
                    match = c;
                    break;
                }
            }
        }

        if (match == nullptr) {
            unavailableItems.push_back(
                "  ► " + item.value + " (" + item.reference + ") -> MPN: " + item.mpn + " -> Needed: "
                + C_DEEP_RED + std::to_string(item.quantity) + C_RESET);
            continue;
        }

        const int freeQty = getFreeQuantity(match->getId());

        if (freeQty >= item.quantity) {
            availableItems.push_back(
                "  ► " + item.value + " (" + item.reference + ") -> Have "
                + C_GREEN + std::to_string(freeQty) + C_RESET
                + " -> Located in " + match->getStorageLocation());
        } else {
            partiallyAvailableItems.push_back(
                "  ► " + item.value + " (" + item.reference + ") -> Have "
                + C_YELLOW + std::to_string(freeQty) + C_RESET
                + " -> Needed: " + std::to_string(item.quantity - freeQty) + " (" + item.mpn + ")"
                + " -> Located in " + match->getStorageLocation());
        }
    }

    if (!availableItems.empty()) {
        std::cout << C_GREEN << C_BOLD << "  ✔ FULLY AVAILABLE:\n" << C_RESET;
        for (const std::string& entry : availableItems)
            std::cout << C_WHITE << entry << "\n" << C_RESET;
        std::cout << "\n";
    }

    if (!partiallyAvailableItems.empty()) {
        std::cout << C_YELLOW << C_BOLD << "  ⚠ PARTIALLY AVAILABLE:\n" << C_RESET;
        for (const std::string& entry : partiallyAvailableItems)
            std::cout << C_WHITE << entry << "\n" << C_RESET;
        std::cout << "\n";
    }

    if (!unavailableItems.empty()) {
        std::cout << C_DEEP_RED << C_BOLD << "  ✖ UNAVAILABLE:\n" << C_RESET;
        for (const std::string& entry : unavailableItems)
            std::cout << C_WHITE << entry << "\n" << C_RESET;
        std::cout << "\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////


void Inventory::saveToFile() const
{
    //Save categories
    // Format: id|type|name|field1|field2|...
    ofstream catFile;
    catFile.open(this->dbPath + "categories.txt");
    if (!catFile.is_open())
        throw runtime_error("Could not open categories file!");

    for (size_t i = 0; i < this->categories.size(); i++)
    {
        Category* cat = this->categories[i];
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
            vector<string> fields = cat->getFields();
            for (size_t j = 0; j < fields.size(); j++)
            {

                catFile << "|";
                catFile << fields[j];
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
    ofstream compFile;
    compFile.open(this->dbPath + "components.txt");
    if (!compFile.is_open())
        throw runtime_error("Could not open components file!");

    for (size_t i = 0; i < this->components.size(); i++)
    {
        Component* comp = this->components[i];
        compFile << comp->getId() << "|"
                 << comp->getModel() << "|"
                 << comp->getPrice() << "|"
                 << comp->getQuantity() << "|"
                 << comp->getMountingType() << "|"
                 << comp->getStorageLocation() << "|"
                 << comp->getPackage() << "|"
                 << comp->getDatasheet() << "|"
                 << comp->getManufacturerPN() << "|"
                 << (comp->getCategory() ? comp->getCategory()->getId() : 0);

        map<string, string> cv = comp->getCustomValues();
        map<string, string>::const_iterator mapIt;
        for (mapIt = cv.begin(); mapIt != cv.end(); mapIt++) {
            compFile << "|" << mapIt->first << ":" << mapIt->second;
        }

        compFile << "\n";
    }
    compFile.close();

    //Save projects
    ofstream projFile;
    projFile.open(this->dbPath + "projects.txt");
    if (!projFile.is_open())
        throw runtime_error("Could not open projects file!");

    for (size_t i = 0; i < this->projects.size(); i++)
    {
        projFile << "PROJECT|" << this->projects[i].getId() << "|"
                 << this->projects[i].getName() << "|"
                 << this->projects[i].getDescription() << "|"
                 << this->projects[i].getStatus() << "|"
                 << this->projects[i].getStartDate() << "\n";

        const vector<UsedComponent>& p_components = this->projects[i].getComponents();
        for (size_t j = 0; j < p_components.size(); j++)
        {
            projFile << "COMPONENT|"
                     << p_components[j].getComponent()->getId() << "|"
                     << p_components[j].getAllocatedQuantity() << "\n";
        }
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
            if (tokens.size() < 10)
                continue;

            int id = stoi(tokens[0]);
            string model = tokens[1];
            double price = stod(tokens[2]);
            int qty = stoi(tokens[3]);
            string mountingType = tokens[4];
            string location = tokens[5];
            string packageType = tokens[6];
            string datasheet = tokens[7];
            string manufacturerPN = tokens[8];

            int catId = 0;
            if (tokens.size() > 9)
                catId = stoi(tokens[9]);

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
                                            packageType, datasheet, cat, manufacturerPN);

            for (size_t i = 10; i < tokens.size(); ++i)
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

                Project * newP = new Project(id, tokens[2], tokens[3], tokens[5]);
                this->projects.push_back(*newP);
                delete newP;
                
                std::string status = tokens[4];
                std::transform(status.begin(), status.end(), status.begin(), ::tolower);
                if (status == "archived")
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
                    currentProject->loadComponent(comp, allocQty);
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
    for (size_t i = 0; i < this->components.size(); i++) {
        delete this->components[i];
    }
    this->components.clear();

    for (size_t i = 0; i < this->categories.size(); i++) {
        delete this->categories[i];
    }
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