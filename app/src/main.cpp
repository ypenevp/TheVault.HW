#include "Inventory.h"
#include "Resistor.h"
#include "Transistor.h"
#include "Diode.h"
#include "CustomCategory.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <conio.h>
#include <filesystem>
#include <sstream>
#include <set>

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////

const string RESET          = "\033[0m";
const string BOLD           = "\033[1m";
const string UNDERLINE      = "\033[4m";
const string BRIGHT_MAGENTA = "\033[95m";
const string BRIGHT_RED     = "\033[91m";
const string BRIGHT_GREEN   = "\033[92m";
const string SUPER_GREEN    = "\033[38;5;46m";
const string BRIGHT_YELLOW  = "\033[38;5;226m";
const string BRIGHT_CYAN    = "\033[96m";
const string BRIGHT_WHITE   = "\033[97m";
const string BRIGHT_ORANGE  = "\033[38;5;208m";
const string BRIGHT_BLUE    = "\033[94m";


static void enableANSI() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void pauseScreen() {
    cout << BRIGHT_CYAN << "\n  ► Press any key to continue..." << RESET;
    _getch();
}

////////////////////////////////////////////////////////////////////////////////////////////

static int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << BRIGHT_CYAN << prompt << RESET;
        if (cin >> value) { cin.ignore(); return value; }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << BRIGHT_RED << "  ✖ Invalid input. Please enter a number.\n" << RESET;
    }
}

static double readDouble(const string& prompt) {
    double value;
    while (true) {
        cout << BRIGHT_CYAN << prompt << RESET;
        if (cin >> value) { cin.ignore(); return value; }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << BRIGHT_RED << "  ✖ Invalid input. Please enter a number.\n" << RESET;
    }
}

static string readString(const string& prompt) {
    string value;
    cout << BRIGHT_CYAN << prompt << RESET;
    getline(cin, value);
    return value;
}

static string readStringOptional(const string& prompt, const string& defaultVal) {
    if (defaultVal == "-") {
        cout << BRIGHT_CYAN << prompt << ":"  << RESET;
    } else {
        cout << BRIGHT_CYAN << prompt << " [" << defaultVal << "]: " << RESET;
    }

    string value;
    getline(cin, value);
    if (value.empty()) return defaultVal;
    return value;
}

static double readDoubleOptional(const string& prompt, double defaultVal, const string& unit = "") {
    while (true) {
        cout << BRIGHT_CYAN << prompt << " [" << defaultVal << unit << "]: " << RESET;
        string value;
        getline(cin, value);
        if (value.empty()) return defaultVal;
        try {
            return stod(value);
        } catch (...) {
            cout << BRIGHT_RED << "  ✖ Invalid input. Please enter a number.\n" << RESET;
        }
    }
}

static string readDashOptional(const string& prompt) {
    cout << BRIGHT_CYAN << prompt << RESET;
    string input;
    getline(cin, input);
    if (input.empty()) return "-";
    return input;
}

static string readMountingType(const string& defaultVal = "") {
    while (true) {
        string prompt = defaultVal.empty()
            ? "  ❖ Mounting Type (SMD / THT / Module): "
            : "  ❖ Mounting Type (SMD / THT / Module)";

        string mt = defaultVal.empty() ? readString(prompt) : readStringOptional(prompt, defaultVal);

        if (mt == "SMD" || mt == "THT" || mt == "Module") return mt;
        cout << BRIGHT_RED << "  ✖ Must be SMD, THT, or Module.\n" << RESET;
    }
}

static void printHeader(const string& title) {
    clearScreen();

    cout << BRIGHT_CYAN << BOLD;

    cout << "  ▐▓█▓▌  "
         << BRIGHT_WHITE << "THEVAULT.HW"
         << BRIGHT_CYAN << " :: "
         << title << "\n";

    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
         << RESET << "\n";
}

////////////////////////////////////////////////////////////////////////////////////////////

static int selectFromMenu(const vector<string>& options, const string& title) {
    int choice = 0;
    int key;

    while (true) {
        clearScreen();
        cout << BRIGHT_CYAN << BOLD;
        cout << "  ✦ ════════════════════════════════════════════════════════════════ ✦\n";
        cout << "    " << BRIGHT_GREEN << left << setw(64) << title << BRIGHT_CYAN << "\n";
        cout << "  ✦ ════════════════════════════════════════════════════════════════ ✦\n" << RESET << "\n";

        for (size_t i = 0; i < options.size(); ++i) {
            if (i == (size_t)choice) {
                cout << "   " << BRIGHT_YELLOW << BOLD << "► " << left << setw(62) << options[i] << RESET << "\n";
            } else {
                cout << "      " << BRIGHT_WHITE << options[i] << RESET << "\n";
            }
        }

        key = _getch();

        if (key == 0 || key == 224) {
            key = _getch();
            if (key == 72) {
                choice--;
                if (choice < 0) choice = (int)options.size() - 1;
            } else if (key == 80) {
                choice++;
                if (choice >= (int)options.size()) choice = 0;
            }
        } else if (key == 13) {
            return choice;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

static void printComponents(const vector<Component*>& comps, const Inventory& inv) {
    if (comps.empty()) {
        cout << BRIGHT_RED << "  No components found.\n" << RESET;
        return;
    }

    cout << "\n" << BOLD << BRIGHT_CYAN << left
         << " "   << setw(5)  << "ID"
         << " | " << setw(18) << "Model"
         << " | " << setw(12) << "Category"
         << " | " << setw(8)  << "Price"
         << " | " << setw(5)  << "Total"
         << " | " << setw(5)  << "Free"
         << " | " << setw(10) << "Datasheet"
         << " | " << setw(7)  << "Mount"
         << " | " << setw(10) << "Package"
         << " | " << setw(15) << "Location" << "\n" << RESET;

    cout << BRIGHT_WHITE << string(120, '-') << "\n" << RESET;

    for (const auto* c : comps) {
        int totQty = c->getQuantity();
        int freeQty = inv.getFreeQuantity(c->getId());

        const Category* cat = c->getCategory();
        string catName = cat ? cat->getName() : "Unknown";

        string ds = c->getDatasheet();
        bool hasDs = (!ds.empty() && ds != "-");

        string pkg = c->getPackage();
        if (pkg.empty()) pkg = "-";

        stringstream ss;
        ss << fixed << setprecision(2) << c->getPrice() << " €";

        cout << " "
             << BOLD << SUPER_GREEN << left << setw(5) << c->getId() << RESET << " | "
             << BRIGHT_WHITE << left << setw(18) << c->getModel() << RESET << " | "
             << BOLD << BRIGHT_YELLOW << left << setw(12) << catName << RESET << " | "
             << BOLD << SUPER_GREEN << left << setw(10) << ss.str() << RESET << " | "
             << BRIGHT_WHITE << left << setw(5) << totQty << RESET << " | "
             << BOLD << SUPER_GREEN << left << setw(5) << freeQty << RESET << " | ";

        if (hasDs) {
            cout << BRIGHT_BLUE << "\033]8;;" << ds << "\033\\" << left << setw(10) << "Press Here" << "\033]8;;\033\\" << RESET << " | ";
        } else {
            cout << BRIGHT_BLUE << left << setw(10) << "-" << RESET << " | ";
        }

        cout << BRIGHT_WHITE << left << setw(7) << c->getMountingType() << RESET << " | "
             << BRIGHT_WHITE << left << setw(10) << pkg << RESET << " | "
             << BRIGHT_WHITE << left << setw(15) << c->getStorageLocation() << RESET << "\n";

        auto customVals = c->getCustomValues();
        if (!customVals.empty()) {
            string prefix = "       └─ " + BOLD + BRIGHT_MAGENTA + "Details: " + RESET;
            cout << prefix;

            int currentLineLength = 17;
            int maxLineLength = 118;

            for (auto it = customVals.begin(); it != customVals.end(); ++it) {
                string key = it->first;
                string value = it->second;
                string unit = "";

                size_t s = key.find('{');
                size_t e = key.find('}');
                if (s != string::npos && e != string::npos && e > s) {
                    unit = key.substr(s + 1, e - s - 1);
                    key = key.substr(0, s);
                }

                int partLength = key.length() + 2 + value.length() + (unit.empty() ? 0 : 1 + unit.length());
                if (next(it) != customVals.end()) {
                    partLength += 2;
                }

                if (currentLineLength + partLength > maxLineLength) {
                    cout << "\n                   ";
                    currentLineLength = 19;
                }

                cout << BRIGHT_WHITE << key << ": " << BRIGHT_ORANGE << value;
                if (!unit.empty()) {
                    cout << " " << unit;
                }
                cout << RESET;

                if (next(it) != customVals.end()) {
                    cout << BRIGHT_WHITE << ", " << RESET;
                }

                currentLineLength += partLength;
            }
            cout << "\n";
        }

        cout << BRIGHT_WHITE << string(120, '-') << "\n" << RESET;
    }
}

static void printProjects(const vector<Project>& projects) {
    if (projects.empty()) {
        cout << BRIGHT_RED << "  ⚠ No projects found.\n" << RESET;
        return;
    }
    cout << "\n  " << BOLD << BRIGHT_CYAN
         << left << setw(5) << "ID" << " | "
         << left << setw(18) << "Name" << " | "
         << left << setw(10) << "Status" << " | "
         << left << setw(12) << "Start Date" << RESET << "\n";
    cout << BRIGHT_WHITE << "  " << string(55, '-') << RESET << "\n";

    for (const auto& p : projects) {
        string statusColor = (p.getStatus() == "Active") ? BRIGHT_GREEN : BRIGHT_RED;
        cout << "  " << BOLD << BRIGHT_YELLOW << left << setw(5) << p.getId() << RESET << " | "
             << BOLD << BRIGHT_WHITE << left << setw(18) << p.getName() << RESET << " | "
             << BOLD << statusColor << left << setw(10) << p.getStatus() << RESET << " | "
             << BRIGHT_WHITE << left << setw(12) << p.getStartDate() << RESET << "\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

static int promptComponent(Inventory& inv, const string& title) {
    auto comps = inv.getAllComponents();
    if (comps.empty()) {
        cout << BRIGHT_YELLOW << "  ⚠ No components available.\n" << RESET;
        return -1;
    }
    vector<string> opts;
    for (auto* c : comps) {
        string catName = c->getCategory() ? c->getCategory()->getName() : "N/A";
        ostringstream oss;
        oss << "ID: " << left << setw(4) << c->getId()
            << " | " << left << setw(18) << c->getModel()
            << " | Cat: " << left << setw(12) << catName
            << " | Tot: " << left << setw(4) << c->getQuantity()
            << " | Free: " << inv.getFreeQuantity(c->getId());
        opts.push_back(oss.str());
    }
    opts.push_back("Back");
    int sel = selectFromMenu(opts, title);
    if (sel == (int)comps.size()) return -1;
    return comps[sel]->getId();
}

static int promptCategory(Inventory& inv, const string& title) {
    auto cats = inv.getAllCategories();
    if (cats.empty()) {
        cout << BRIGHT_YELLOW << "  ⚠ No categories available.\n" << RESET;
        return -1;
    }
    vector<string> opts;
    for (auto* c : cats) {
        opts.push_back("ID: " + to_string(c->getId()) + "  " + c->getName());
    }
    opts.push_back("Back");
    int sel = selectFromMenu(opts, title);
    if (sel == (int)cats.size()) return -1;
    return cats[sel]->getId();
}

static int promptProject(Inventory& inv, const string& title) {
    auto projs = inv.getAllProjects();
    if (projs.empty()) {
        cout << BRIGHT_YELLOW << "  ⚠ No projects available.\n" << RESET;
        return -1;
    }
    vector<string> opts;
    for (const auto& p : projs) {
        opts.push_back("ID: " + to_string(p.getId()) + "  " + p.getName() + "  [" + p.getStatus() + "]");
    }
    opts.push_back("Back");
    int sel = selectFromMenu(opts, title);
    if (sel == (int)projs.size()) return -1;
    return projs[sel].getId();
}

////////////////////////////////////////////////////////////////////////////////////////////

static void menuAddComponentToProject(Inventory& inv);

static void menuAddComponent(Inventory& inv) {
    int catId = promptCategory(inv, "Select Category for Component");
    if (catId == -1) return;

    Category* cat = inv.getCategoryById(catId);

    printHeader("Add Component");
    string model = readString("  ❖ Model: ");
    double price = readDouble("  ❖ Price (€): ");
    int qty = readInt("  ❖ Quantity: ");
    string mt = readMountingType();
    string loc = readString("  ❖ Storage Location: ");
    string packageType = readDashOptional("  ❖ Package: ");
    string datasheet = readDashOptional("  ❖ Datasheet: ");

    map<string, string> extraFields;
    auto fields = cat->getFields();
    if (!fields.empty()) {
        cout << BRIGHT_CYAN << "\n  Fill in category-specific fields:\n" << RESET;
        for (const auto& field : fields) {
            string displayName = field;
            size_t limit = field.find('{');
            if (limit != string::npos) displayName = field.substr(0, limit);

            string val = readDashOptional("    ❖ " + displayName + ": ");
            extraFields[field] = val;
        }
    }

    try {
        inv.addComponent(model, price, qty, mt, loc, packageType, datasheet, cat, extraFields);
        cout << BRIGHT_GREEN << BOLD << "\n  ✔ Component added successfully.\n" << RESET;
    } catch (const exception& e) {
        cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
    }
    pauseScreen();
}

static void menuEditComponent(Inventory& inv) {
    int id = promptComponent(inv, "Select Component to edit");
    if (id == -1) return;

    Component* c = inv.getComponentById(id);

    printHeader("Edit Component (Press Enter to keep current values)");

    string model = readStringOptional("  ❖ New Model", c->getModel());
    double price = readDoubleOptional("  ❖ New Price", c->getPrice(), " €");
    string mt = readMountingType(c->getMountingType());
    string loc = readStringOptional("  ❖ New Storage Location", c->getStorageLocation());
    string packageType = readStringOptional("  ❖ New Package", c->getPackage());
    string datasheet = readStringOptional("  ❖ New Datasheet URL", c->getDatasheet());

    map<string, string> extraFields = c->getCustomValues();
    auto fields = c->getCategory()->getFields();

    if (!fields.empty()) {
        cout << BRIGHT_CYAN << "\n  Edit category-specific fields:\n" << RESET;
        for (const auto& field : fields) {
            string displayName = field;
            size_t limit = field.find('{');
            if (limit != string::npos) displayName = field.substr(0, limit);

            string currentVal = extraFields.count(field) ? extraFields[field] : "";
            string val = readStringOptional("    ❖ " + displayName, currentVal);

            extraFields[field] = val;
        }
    }

    try {
        inv.editComponent(id, model, price, mt, loc, packageType, datasheet, extraFields);
        cout << BRIGHT_GREEN << BOLD << "\n  ✔ Component updated successfully.\n" << RESET;
    } catch (const exception& e) {
        cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
    }
    pauseScreen();
}

static void menuRemoveComponent(Inventory& inv) {
    int id = promptComponent(inv, "Select Component to remove");
    if (id == -1) return;

    try {
        inv.removeComponent(id);
        cout << BRIGHT_GREEN << BOLD << "\n  ✔ Component removed.\n" << RESET;
    } catch (const exception& e) {
        cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
    }
    pauseScreen();
}

static void menuViewComponents(Inventory& inv) {
    printHeader("Inventory");
    printComponents(inv.getAllComponents(), inv);
    pauseScreen();
}

static void menuComponentsMenu(Inventory& inv) {
    vector<string> options = {
        "1. View All",
        "2. Add Component",
        "3. Edit Component",
        "4. Remove Component",
        "5. Add to Project",
        "6. Back"
    };

    while (true) {
        int choice = selectFromMenu(options, "The Vault.HW - Components");

        switch (choice) {
            case 0: menuViewComponents(inv); break;
            case 1: menuAddComponent(inv); break;
            case 2: menuEditComponent(inv); break;
            case 3: menuRemoveComponent(inv); break;
            case 4: menuAddComponentToProject(inv); break;
            case 5: return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

static void menuAddCategory(Inventory& inv) {
    printHeader("Add Custom Category");
    string name = readString("  ❖ Category name: ");
    int fieldCount = readInt("  ❖ Number of custom fields: ");

    if (fieldCount <= 0) {
        cout << BRIGHT_RED << "  ✖ Invalid field count.\n" << RESET;
        pauseScreen();
        return;
    }

    vector<string> fields;
    for (int i = 0; i < fieldCount; ++i) {
        string field = readString("\n  ❖ Field " + to_string(i + 1) + " name: ");
        string unit = readString("  ❖ Measurement Unit (Leave empty for none): ");

        if (!field.empty()) {
            if (!unit.empty()) field += "{" + unit + "}";
            fields.push_back(field);
        }
    }

    try {
        auto* cat = new CustomCategory(100 + inv.getAllCategories().size(), name, fields);
        inv.addCategory(cat);
        delete cat;
        cout << BRIGHT_GREEN << BOLD << "\n  ✔ Custom category added successfully.\n" << RESET;
    } catch (const exception& e) {
        cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
    }
    pauseScreen();
}

static void menuCategoriesMenu(Inventory& inv) {
    vector<string> options = {
        "1. View Categories",
        "2. Add Custom Category",
        "3. Remove Category",
        "4. Back"
    };

    while (true) {
        int choice = selectFromMenu(options, "The Vault.HW - Categories");

        switch (choice) {
            case 0: {
                printHeader("Categories");
                auto cats = inv.getAllCategories();
                cout << BRIGHT_CYAN << "  ❖ Available Categories:\n" << RESET;
                for (auto* c : cats)
                    cout << "    [" << BOLD << BRIGHT_YELLOW << c->getId() << RESET << "] " << BOLD << BRIGHT_WHITE << c->getName() << RESET << "\n";
                pauseScreen();
                break;
            }
            case 1: menuAddCategory(inv); break;
            case 2: {
                int id = promptCategory(inv, "Select Category to remove");
                if (id == -1) break;

                try {
                    inv.removeCategory(id);
                    cout << BRIGHT_GREEN << BOLD << "\n  ✔ Category removed.\n" << RESET;
                } catch (const exception& e) {
                    cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
                }
                pauseScreen();
                break;
            }
            case 3: return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

static void menuAddProject(Inventory& inv) {
    printHeader("Add Project");
    string name = readString("  ❖ Project Name: ");
    string desc = readString("  ❖ Description: ");
    string date = readString("  ❖ Start Date (YYYY-MM-DD): ");

    try {
        inv.addProject(name, desc, date);
        cout << BRIGHT_GREEN << BOLD << "\n  ✔ Project created.\n" << RESET;
    } catch (const exception& e) {
        cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
    }
    pauseScreen();
}

static void menuProjectDetails(Inventory& inv) {
    int id = promptProject(inv, "Select Project to View Details");
    if (id == -1) return;

    printHeader("Project Details");
    try {
        Project p = inv.getProjectDetails(id);
        string stColor = (p.getStatus() == "Active") ? BRIGHT_GREEN : BRIGHT_RED;

        cout << BRIGHT_CYAN << "  [ Project Info ]\n" << RESET;
        cout << "  ❖ Name       : " << BOLD << BRIGHT_WHITE << p.getName() << RESET << "\n"
             << "  ❖ Description: " << p.getDescription() << "\n"
             << "  ❖ Status     : " << BOLD << stColor << p.getStatus() << RESET << "\n"
             << "  ❖ Start Date : " << p.getStartDate() << "\n"
             << "  ❖ Total Price: " << BRIGHT_GREEN << BOLD << p.getTotalPrice() << " €\n" << RESET << "\n"
             << BRIGHT_CYAN << "  [ Components ]\n" << RESET;

        for (const auto& uc : p.getComponents())
            cout << "    ► " << BOLD << BRIGHT_WHITE << uc.getComponent()->getModel() << RESET
                 << "  x" << BRIGHT_YELLOW << uc.getAllocatedQuantity() << RESET
                 << "  (" << BRIGHT_GREEN << uc.getComponent()->getPrice() * uc.getAllocatedQuantity() << " €" << RESET << ")\n";
    } catch (const exception& e) {
        cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
    }
    pauseScreen();
}

static void menuAddComponentToProject(Inventory& inv) {
    int projectId = promptProject(inv, "Select Project");
    if (projectId == -1) return;

    int componentId = promptComponent(inv, "Select Component");
    if (componentId == -1) return;

    printHeader("Add Component to Project");
    int qty = readInt("  ❖ Quantity: ");

    try {
        inv.addComponentToProject(projectId, componentId, qty);
        cout << BRIGHT_GREEN << BOLD << "\n  ✔ Component added to project.\n" << RESET;
    } catch (const exception& e) {
        cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
    }
    pauseScreen();
}

static void menuProjectsMenu(Inventory& inv) {
    vector<string> options = {
        "1. View Projects",
        "2. Add Project",
        "3. View Details",
        "4. Edit Project",
        "5. Add Component to Project",
        "6. Remove Project",
        "7. Back"
    };

    while (true) {
        int choice = selectFromMenu(options, "The Vault.HW - Projects");

        switch (choice) {
            case 0: {
                printHeader("Projects");
                printProjects(inv.getAllProjects());
                pauseScreen();
                break;
            }
            case 1: menuAddProject(inv); break;
            case 2: menuProjectDetails(inv); break;
            case 3: {
                int id = promptProject(inv, "Select Project to edit");
                if (id == -1) break;

                printHeader("Edit Project");
                string name = readString("  ❖ New Name: ");
                string desc = readString("  ❖ New Description: ");
                string date = readString("  ❖ New Start Date: ");
                try {
                    inv.editProject(id, name, desc, date);
                    cout << BRIGHT_GREEN << BOLD << "\n  ✔ Project updated.\n" << RESET;
                } catch (const exception& e) {
                    cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
                }
                pauseScreen();
                break;
            }
            case 4: menuAddComponentToProject(inv); break;
            case 5: {
                int id = promptProject(inv, "Select Project to remove");
                if (id == -1) break;

                try {
                    inv.removeProject(id);
                    cout << BRIGHT_GREEN << BOLD << "  ✔ Project removed.\n" << RESET;
                } catch (const exception& e) {
                    cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
                }
                pauseScreen();
                break;
            }
            case 6: return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

static void menuStock(Inventory& inv) {
    vector<string> options = {
        "1. View Stock Distribution",
        "2. Back",
    };

    while (true) {
        int choice = selectFromMenu(options, "The Vault.HW - Stock Management");

        switch (choice) {
            case 0: {
                int id = promptComponent(inv, "Select Component");
                if (id == -1) break;
                printHeader("Stock Distribution");
                try { inv.printDistribution(id); }
                catch (const exception& e) { cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET; }
                pauseScreen();
                break;
            }
            case 1: return;
        }
    }
}

static void confirmDrop(Inventory& inv) {
    vector<string> options = {
        "Back",
        "Delete"
    };

    int choice = selectFromMenu(options, "The Vault.HW - Delete");

    if (choice == 1) {
        string confirm;
        printHeader("Delete The Vault");
        cout << BRIGHT_RED << BOLD << "  Are you sure you want to DELETE ALL DATA for inventory?\n" << RESET;
        cout << BRIGHT_CYAN << "  Type \"delete\" if you are absolutely sure: " << RESET;
        cin >> confirm;

        if (confirm == "delete") {
            string dbPath = "../db/";
            vector<string> filesToDelete = {
                "components.txt",
                "categories.txt",
                "projects.txt"
            };

            bool success = true;
            try {
                for (const auto& file : filesToDelete) {
                    string fullPath = dbPath + file;
                    if (std::filesystem::exists(fullPath)) {
                        std::filesystem::remove(fullPath);
                    }
                }

                inv.clearAll();

            } catch (const std::exception& e) {
                success = false;
                cout << BRIGHT_RED << "  ✖ Error deleting files: " << e.what() << "\n" << RESET;
            }

            if (success) {
                cout << BRIGHT_GREEN << BOLD << "\n  ✔ All data files and active session cleared successfully!\n" << RESET;
            }
            pauseScreen();
        } else {
            cout << BRIGHT_CYAN << "\n  ❖ Aborted.\n" << RESET;
            pauseScreen();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

int main() {
    enableANSI();

    Inventory inv("../db/", "../exports/");

    vector<string> mainMenu = {
        "1. Components",
        "2. Categories",
        "3. Projects",
        "4. Stock Management",
        "5. Compare Components",
        "6. Destroy The Vault",
        "7. Exit"
    };

    while (true) {
        int choice = selectFromMenu(mainMenu, "TheVault.HW - Main menu");

        switch (choice) {
            case 0: menuComponentsMenu(inv); break;
            case 1: menuCategoriesMenu(inv); break;
            case 2: menuProjectsMenu(inv); break;
            case 3: menuStock(inv); break;
            case 4: {
                int id1 = promptComponent(inv, "First Component");
                if (id1 == -1) break;

                int id2 = promptComponent(inv, "Second Component");
                if (id2 == -1) break;

                printHeader("Compare Components");
                try {
                    inv.compareComponents(id1, id2);
                }
                catch (const exception& e) {
                    cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
                }
                pauseScreen();
                break;
            }
            case 5: confirmDrop(inv); break;
            case 6:
                inv.saveToFile();
                clearScreen();
                cout << BRIGHT_GREEN << BOLD << "\n  ✔ Data saved. Thank you for using TheVault.HW\n\n" << RESET;
                return 0;
        }
    }
}