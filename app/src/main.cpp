#include "Inventory.h"
#include "Resistor.h"
#include "Transistor.h"
#include "Diode.h"
#include "IC.h"
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
#include <algorithm>

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
const string TECHNO_CYAN    = "\033[38;2;0;255;255m";
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
        if (cin >> value) { 
            cin.ignore();  // \n
            return value; 
        }
        cin.clear(); 
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
        cout << BRIGHT_RED << "  ✖ Invalid input. Please enter a number.\n" << RESET;
    }
}

static double readDouble(const string& prompt) {  
    double value;
    while (true) {
        cout << BRIGHT_CYAN << prompt << RESET;
        if (cin >> value) {
            cin.ignore(); 
            return value; 
        }
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

static string handleEmptyInput(const string& prompt) { 
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

    const int maxVisible = 21;

    while (true) {
        clearScreen();

        cout << BRIGHT_CYAN << BOLD;
        cout << "  ✦ ════════════════════════════════════════════════════════════════ ✦\n";
        cout << "    " << BRIGHT_GREEN << left << setw(64) << title << BRIGHT_CYAN << "\n";
        cout << "  ✦ ════════════════════════════════════════════════════════════════ ✦\n" << RESET << "\n";

        int startIndex = 0;
        if ((int)options.size() > maxVisible) {
            startIndex = choice - maxVisible / 2;
            if (startIndex < 0) startIndex = 0;
            if (startIndex > (int)options.size() - maxVisible)
                startIndex = (int)options.size() - maxVisible;
        }

        int endIndex = min(startIndex + maxVisible, (int)options.size());

        if (startIndex > 0) {
            cout << BRIGHT_WHITE << "   ...\n" << RESET;
        }

        for (int i = startIndex; i < endIndex; i++) {
            if (i == choice) {
                cout << "   " << BRIGHT_YELLOW << BOLD << "► " << left << setw(62) << options[i] << RESET << "\n";
            } else {
                cout << "      " << BRIGHT_WHITE << options[i] << RESET << "\n";
            }
        }

        if (endIndex < (int)options.size()) {
            cout << BRIGHT_WHITE << "   ...\n" << RESET;
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
         << " "   << setw(4)  << "ID"
         << " | " << setw(18) << "Model"
         << " | " << setw(13) << "Category"
         << " | " << setw(8)  << "Price"
         << " | " << setw(5)  << "Total"
         << " | " << setw(5)  << "Free"
         << " | " << setw(10) << "Datasheet"
         << " | " << setw(7)  << "Mount"
         << " | " << setw(10) << "Package"
         << " | " << setw(10) << "Location"
         << " | " << setw(17) << "Manufacturer PN" << "\n" << RESET;

    cout << BRIGHT_WHITE << string(138, '-') << "\n" << RESET;

    for (size_t i = 0; i < comps.size(); i++) {
        const Component* c = comps[i];
        int freeQty = inv.getFreeQuantity(c->getId());
        int totQty  = freeQty + inv.getAllocatedQuantity(c->getId());

        const Category* cat = c->getCategory();
        string catName = cat ? cat->getName() : "Unknown";

        string ds = c->getDatasheet();
        bool hasDs = (!ds.empty() && ds != "-");

        string pkg = c->getPackage();
        if (pkg.empty()) pkg = "-";

        stringstream ss;
        ss << fixed << setprecision(2) << c->getPrice() << " €";

        cout << " "
             << BOLD << SUPER_GREEN << left << setw(4) << c->getId() << RESET << " | "
             << BRIGHT_WHITE << left << setw(18) << c->getModel() << RESET << " | "
             << BOLD << BRIGHT_YELLOW << left << setw(13) << catName << RESET << " | "
             << BOLD << SUPER_GREEN << left << setw(10) << ss.str() << RESET << " | "
             << BRIGHT_WHITE << left << setw(5) << totQty << RESET << " | "
             << BOLD << SUPER_GREEN << left << setw(5) << freeQty << RESET << " | ";

        if (hasDs) {
            cout << BRIGHT_BLUE << "\033]8;;" << ds << "\033\\" << left << setw(10) << "Press Here" << "\033]8;;\033\\" << RESET << " | ";
        } else {
            cout << BRIGHT_WHITE << left << setw(10) << "-" << RESET << " | ";
        }

        cout << BRIGHT_WHITE << left << setw(7) << c->getMountingType() << RESET << " | "
             << BRIGHT_WHITE << left << setw(10) << pkg << RESET << " | "
             << BOLD << TECHNO_CYAN << left << setw(10) << c->getStorageLocation() << RESET << " | ";

        // show first 12 symbols of MPN then ..
        string mpn = c->getManufacturerPN();
        if (mpn.empty()) mpn = "-";
        if (mpn.length() > 17) {
            mpn = mpn.substr(0, 15) + "..";
        }
        cout << BRIGHT_WHITE << left << setw(17) << mpn << RESET << "\n";

        map<string, string> customVals = c->getCustomValues();
        if (!customVals.empty()) {
            string prefix = "       └─ " + BOLD + BRIGHT_MAGENTA + "Details:          " + RESET;
            cout << prefix;

            int currentLineLength = 17;
            int maxLineLength = 118;

            map<string, string>::const_iterator it;
            for (it = customVals.begin(); it != customVals.end(); it++) {
                string key = it->first;
                string value = it->second;
                string unit = "";

                // extract meassurment unit
                size_t s = key.find('{');
                size_t e = key.find('}');
                if (s != string::npos && e != string::npos && e > s) {
                    unit = key.substr(s + 1, e - s - 1);
                    key = key.substr(0, s);
                }

                int partLength = key.length() + 2 + value.length() + (unit.empty() ? 0 : 1 + unit.length());

                map<string, string>::const_iterator nextIt = it;
                nextIt++;

                if (nextIt != customVals.end()) {
                    partLength += 2;
                }

                // many custom values => need to wrap
                if (currentLineLength + partLength > maxLineLength) {
                    cout << "\n                   ";
                    currentLineLength = 19;
                }

                // add measurement units modifier
                cout << BRIGHT_WHITE << key << ": " << BRIGHT_ORANGE;
                string modifier = "";
                if (!value.empty() && isalpha(value.back()) && value.length() > 1 && isdigit(value[value.length() - 2])) {
                    modifier = value.back();
                    value.pop_back();
                }
                cout << value;
                if (!unit.empty()) {
                    cout << " " << modifier << unit;
                } else if (!modifier.empty()) {
                    cout << modifier;
                }
                cout << RESET;

                if (nextIt != customVals.end()) {
                    cout << BRIGHT_WHITE << ", " << RESET;
                }

                currentLineLength += partLength;
            }
            cout << "\n";
        }

        cout << BRIGHT_WHITE << string(138, '-') << "\n" << RESET;
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

    for (size_t i = 0; i < projects.size(); i++) {
        const Project& p = projects[i];
        string statusColor = (p.getStatus() == "active") ? BRIGHT_GREEN : BRIGHT_RED;
        cout << "  " << BOLD << BRIGHT_YELLOW << left << setw(5) << p.getId() << RESET << " | "
             << BOLD << BRIGHT_WHITE << left << setw(18) << p.getName() << RESET << " | "
             << BOLD << statusColor << left << setw(10) << p.getStatus() << RESET << " | "
             << BRIGHT_WHITE << left << setw(12) << p.getStartDate() << RESET << "\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

static int promptComponent(Inventory& inv, const string& title) { 
    vector<Component*> comps = inv.getAllComponents();
    if (comps.empty()) {
        cout << BRIGHT_YELLOW << "  ⚠ No components available.\n" << RESET;
        return -1;
    }
    vector<string> opts;
    for (size_t i = 0; i < comps.size(); i++) {
        Component* c = comps[i];
        int free = inv.getFreeQuantity(c->getId());
        int tot  = free + inv.getAllocatedQuantity(c->getId());
        string catName = c->getCategory() ? c->getCategory()->getName() : "N/A";
        ostringstream oss;
        oss << "ID: " << left << setw(4) << c->getId()
            << " | " << left << setw(18) << c->getModel()
            << " | Cat: " << left << setw(12) << catName
            << " | Tot: " << left << setw(4) << tot
            << " | Free: " << free;
        opts.push_back(oss.str());
    }
    opts.push_back("Back");
    int sel = selectFromMenu(opts, title);
    if (sel == (int)comps.size()) return -1;
    return comps[sel]->getId();
}

static int promptICComponent(Inventory& inv, const string& title)
{
    vector<Component*> all = inv.getAllComponents();
    vector<Component*> ics;

    for (size_t i = 0; i < all.size(); i++) {
        if (dynamic_cast<IC*>(all[i]->getCategory())) {
            ics.push_back(all[i]);
        }
    }

    if (ics.empty()) {
        cout << BRIGHT_YELLOW
             << "  ⚠ No IC components available.\n"
             << RESET;
        return -1;
    }

    vector<string> opts;

    for (size_t i = 0; i < ics.size(); i++) {
        Component* c = ics[i];

        ostringstream oss;
        oss << "ID: " << left << setw(4) << c->getId()
            << " | " << left << setw(18) << c->getModel();

        opts.push_back(oss.str());
    }

    opts.push_back("Back");

    int sel = selectFromMenu(opts, title);

    if (sel == (int)ics.size())
        return -1;

    return ics[sel]->getId();
}

static int promptCategory(Inventory& inv, const string& title) { 
    vector<Category*> cats = inv.getAllCategories();
    if (cats.empty()) {
        cout << BRIGHT_YELLOW << "  ⚠ No categories available.\n" << RESET;
        return -1;
    }
    vector<string> opts;
    for (size_t i = 0; i < cats.size(); i++) {
        Category* c = cats[i];
        opts.push_back("ID: " + to_string(c->getId()) + "  " + c->getName());
    }
    opts.push_back("Back");
    int sel = selectFromMenu(opts, title);
    if (sel == (int)cats.size()) return -1;
    return cats[sel]->getId();
}

static int promptProject(Inventory& inv, const string& title) { 
    vector<Project> projs = inv.getAllProjects();
    if (projs.empty()) {
        cout << BRIGHT_YELLOW << "  ⚠ No projects available.\n" << RESET;
        return -1;
    }
    vector<string> opts;
    for (size_t i = 0; i < projs.size(); i++) {
        const Project& p = projs[i];
        opts.push_back("ID: " + to_string(p.getId()) + "  " + p.getName() + "  [" + p.getStatus() + "]");
    }
    opts.push_back("Back");
    int sel = selectFromMenu(opts, title);
    if (sel == (int)projs.size()) return -1;
    return projs[sel].getId();
}

////////////////////////////////////////////////////////////////////////////////////////////

static void menuAddComponentToProject(Inventory& inv);
static void menuRemoveComponentFromProject(Inventory& inv);
static void menuAddICSpecification(Inventory& inv, int componentId);
static void menuViewICSpecifications(Inventory& inv);

////////////////////////////////////////////////////////////////////////////////////////////

static void menuAddComponent(Inventory& inv) {
    int catId = promptCategory(inv, "Select Category for Component");
    if (catId == -1) return;

    Category* cat = inv.getCategoryById(catId);

    printHeader("Add Component");

    string model;
    while (true) {
        model = readString("  ❖ Model: ");
        bool exists = false;
        vector<Component*> allComps = inv.getAllComponents();
        for (size_t i = 0; i < allComps.size(); i++) {
            if (allComps[i]->getModel() == model) {
                exists = true;
                break;
            }
        }

        if (exists) {
            cout << BRIGHT_RED << "  ✖ Component with this model already exists!!! Change model!\n\n" << RESET;
        } else {
            break;
        }
    }

    double price = readDouble("  ❖ Price (€): ");
    int qty = readInt("  ❖ Quantity: ");
    string mt = readMountingType();
    string loc = readString("  ❖ Storage Location: ");
    string packageType = handleEmptyInput("  ❖ Package: ");
    string datasheet = handleEmptyInput("  ❖ Datasheet: ");
    string mpn = handleEmptyInput("  ❖ Manufacturer PN: ");

    map<string, string> extraFields;
    vector<string> fields = cat->getFields();
    if (!fields.empty()) {
        cout << BRIGHT_CYAN << "\n  Fill in category-specific fields:\n" << RESET;
        for (size_t i = 0; i < fields.size(); i++) {
            string field = fields[i];
            string displayName = field;
            size_t limit = field.find('{');
            if (limit != string::npos) displayName = field.substr(0, limit);

            string val = handleEmptyInput("    ❖ " + displayName + ": ");
            extraFields[field] = val;
        }
    }

    try {
        inv.addComponent(model, price, qty, mt, loc, packageType, datasheet, cat, mpn, extraFields);

        if (dynamic_cast<IC*>(cat)) {
            Component* newComp = inv.getComponentById(inv.getAllComponents().back()->getId());
            menuAddICSpecification(inv, newComp->getId());
        }

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

    string model;
    while (true) {
        model = readStringOptional("  ❖ New Model", c->getModel());
        bool exists = false;
        vector<Component*> allComps = inv.getAllComponents();
        for (size_t i = 0; i < allComps.size(); i++) {
            if (allComps[i]->getId() != id && allComps[i]->getModel() == model) {
                exists = true;
                break;
            }
        }

        if (exists) {
            cout << BRIGHT_RED << "  ✖ Component with this model exists!! Enter a different one.\n\n" << RESET;
        } else {
            break;
        }
    }

    double price = readDoubleOptional("  ❖ New Price", c->getPrice(), " €");
    string mt = readMountingType(c->getMountingType());
    string loc = readStringOptional("  ❖ New Storage Location", c->getStorageLocation());
    string packageType = readStringOptional("  ❖ New Package", c->getPackage());
    string datasheet = readStringOptional("  ❖ New Datasheet URL", c->getDatasheet());
    string mpn = readStringOptional("  ❖ New Manufacturer PN", c->getManufacturerPN());

    map<string, string> extraFields = c->getCustomValues();
    vector<string> fields = c->getCategory()->getFields();

    if (!fields.empty()) {
        cout << BRIGHT_CYAN << "\n  Edit category-specific fields:\n" << RESET;
        for (size_t i = 0; i < fields.size(); i++) {
            string field = fields[i];
            string displayName = field;
            size_t limit = field.find('{');
            if (limit != string::npos) displayName = field.substr(0, limit);

            string currentVal = extraFields.count(field) ? extraFields[field] : "";
            string val = readStringOptional("    ❖ " + displayName, currentVal);

            extraFields[field] = val;
        }
    }

    try {
        inv.editComponent(id, model, price, mt, loc, packageType, datasheet, mpn, extraFields);
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

////////////////////////////////////////////////////////////////////////////////////////////

static void menuAddICSpecification(Inventory& inv, int componentId)
{
    Component* comp = inv.getComponentById(componentId);
    if (!comp || !dynamic_cast<IC*>(comp->getCategory())) {
        cout << BRIGHT_RED << "  ✖ This component is not an IC.\n" << RESET;
        pauseScreen();
        return;
    }

    printHeader("Add IC Specification Details");

    cout << BRIGHT_CYAN << "  Component: " << BRIGHT_WHITE << comp->getModel() << "\n\n" << RESET;

    string icType       = readString("  ❖ IC Type: ");
    string manufacturer = readString("  ❖ Manufacturer: ");
    string protocol     = readString("  ❖ Communication protocol: ");

    if (icType.empty())       icType = "-";
    if (manufacturer.empty()) manufacturer = "-";
    if (protocol.empty())     protocol = "-";

    cout << BRIGHT_CYAN << "  ❖ Max Frequency: " << RESET;
    string freqInput; 
    getline(cin, freqInput);

    string freqValue = freqInput;
    string freqPrefix = "";

    if (!freqValue.empty()) {
        char last = freqValue.back();
        if (isalpha(static_cast<unsigned char>(last))) {
            freqPrefix = last;
            freqValue.pop_back();
        }
        try { stod(freqValue); }
        catch (...) { freqValue = ""; freqPrefix = ""; }
    }

    string finalFreq = freqValue + freqPrefix;
    if (finalFreq.empty()) finalFreq = "-";

    cout << BRIGHT_CYAN << "  ❖ Operating Voltage (range)\n" << RESET;
    cout << "     Min V: ";
    string minV; getline(cin, minV);
    cout << "     Max V: ";
    string maxV; getline(cin, maxV);

    string opVoltage = "";
    if (!minV.empty() || !maxV.empty())
        opVoltage = minV + "V to " + maxV + "V";
    if (opVoltage.empty()) opVoltage = "-";

    cout << BRIGHT_CYAN << "  ❖ Operating Temperature (range)\n" << RESET;
    cout << "     Min °C: ";
    string minT; getline(cin, minT);
    cout << "     Max °C: ";
    string maxT; getline(cin, maxT);

    string opTemp = "";
    if (!minT.empty() || !maxT.empty())
        opTemp = minT + "°C to " + maxT + "°C";
    if (opTemp.empty()) opTemp = "-";

    int pinCount = readInt("\n  ❖ Pin Count: ");

    ICSpecification spec(
        componentId,
        icType,
        pinCount,
        manufacturer,
        protocol,
        finalFreq,
        opVoltage,
        opTemp
    );

    cout << BRIGHT_CYAN << "\n  Enter pin details:\n" << RESET;

    for (int i = 1; i <= pinCount; i++) {
        cout << BRIGHT_WHITE << "\n  --- Pin " << i << " of " << pinCount << " ---\n" << RESET;

        int pinNum = readInt("  ❖ Pin Number: ");
        string pinName = readString("  ❖ Pin Name: ");
        string pinFunc = readString("  ❖ Pin Function: ");

        try {
            spec.addPin(pinNum, pinName, pinFunc);
        }
        catch (const exception& e) {
            cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
            i--;
            continue;
        }
    }

    try {
        inv.addICSpecification(spec);
        cout << BRIGHT_GREEN << BOLD << "\n  ✔ IC Specification added successfully.\n" << RESET;
    }
    catch (const exception& e) {
        cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
    }

    pauseScreen();
}


static void menuViewICSpecifications(Inventory& inv)
{
    const auto& specs = inv.getAllICSpecifications();
    if (specs.empty()) {
        cout << BRIGHT_YELLOW << "  ⚠ No IC specifications found.\n" << RESET;
        pauseScreen();
        return;
    }

    vector<string> opts;
    for (const auto& spec : specs) {
        Component* comp = inv.getComponentById(spec.getComponentId());
        if (comp) {
            ostringstream oss;
            oss << BRIGHT_CYAN << "ID: " << spec.getComponentId() << RESET
                << " | " << BRIGHT_WHITE << comp->getModel() << RESET
                << " | " << BRIGHT_YELLOW << spec.getICType() << RESET;
            opts.push_back(oss.str());
        }
    }
    opts.push_back("Back");

    int sel = selectFromMenu(opts, "TheVault.HW - IC Specifications");
    if (sel == (int)specs.size()) return;

    const ICSpecification& spec = specs[sel];
    Component* comp = inv.getComponentById(spec.getComponentId());

    printHeader("IC Specification Details");

    cout << BRIGHT_CYAN << "  [ Component Info ]\n" << RESET;

    cout << "  ❖ Model              : " << TECHNO_CYAN << comp->getModel() << RESET << "\n";
    cout << "  ❖ Price              : " << SUPER_GREEN
         << fixed << setprecision(2) << comp->getPrice() << " €" << RESET << "\n";
    cout << "  ❖ Quantity           : " << BRIGHT_YELLOW << comp->getQuantity() << RESET << "\n";
    cout << "  ❖ Free Quantity      : " << SUPER_GREEN << inv.getFreeQuantity(comp->getId()) << RESET << "\n";
    cout << "  ❖ Mounting Type      : " << BRIGHT_WHITE << comp->getMountingType() << RESET << "\n";
    cout << "  ❖ Storage Location   : " << BRIGHT_CYAN << comp->getStorageLocation() << RESET << "\n";
    cout << "  ❖ Package            : " << BRIGHT_WHITE
         << (comp->getPackage().empty() ? "-" : comp->getPackage()) << RESET << "\n";
    cout << "  ❖ Manufacturer (MPN) : " << BRIGHT_WHITE
         << (comp->getManufacturerPN().empty() ? "-" : comp->getManufacturerPN()) << RESET << "\n";

    cout << "  ❖ Datasheet          : ";
    if (!comp->getDatasheet().empty() && comp->getDatasheet() != "-") {
        cout << BRIGHT_BLUE
             << "\033]8;;" << comp->getDatasheet() << "\033\\"
             << "Press Here"
             << "\033]8;;\033\\" << RESET;
    } else {
        cout << BRIGHT_WHITE << "-" << RESET;
    }
    cout << "\n\n";

    cout << BRIGHT_CYAN << "  [ IC Specification ]\n" << RESET;

    cout << "  ❖ IC Type            : " << BRIGHT_YELLOW << spec.getICType() << RESET << "\n";
    cout << "  ❖ Manufacturer       : " << BRIGHT_BLUE << spec.getManufacturer() << RESET << "\n";
    cout << "  ❖ Interface          : " << BRIGHT_MAGENTA << spec.getProtocol() << RESET << "\n";
    cout << "  ❖ Max Frequency      : " << BRIGHT_GREEN;
    {
        string freq = spec.getMaxFrequency();

        if (freq == "-" || freq.empty()) {
            cout << "-";
        } else {
            string value = freq;
            string prefix = "";

            char last = value.back();
            if (isalpha(static_cast<unsigned char>(last))) {
                prefix = last;
                value.pop_back();
            }

            cout << value << " " << prefix << "Hz";
        }
    }
    cout << RESET << "\n";
    cout << "  ❖ Operating Voltage  : " << BRIGHT_ORANGE << spec.getOperatingVoltage() << RESET << "\n";
    cout << "  ❖ Operating Temp     : " << BRIGHT_ORANGE << spec.getOperatingTemp() << RESET << "\n";
    cout << "  ❖ Pin Count          : " << TECHNO_CYAN << spec.getPinCount() << RESET << "\n\n";

    const auto& pins = spec.getPins();
    if (!pins.empty()) {
        cout << BRIGHT_CYAN << "  [ Pin Configuration ]\n" << RESET;

        cout << BOLD << BRIGHT_GREEN
             << "  Pin    | Name               | Function\n" << RESET;

        cout << BRIGHT_WHITE << "  " << string(60, '-') << "\n" << RESET;

        for (const auto& pin : pins) {
            cout << "  " << BRIGHT_YELLOW << left << setw(6) << pin.number << RESET << " | "
                 << BRIGHT_WHITE << left << setw(18) << pin.name << RESET << " | "
                 << BRIGHT_WHITE << pin.function << "\n";
        }
    }

    pauseScreen();
}

static void menuEditICSpecification(Inventory& inv)
{
    int compId = promptICComponent(inv, "Select IC Component to edit specification");
    if (compId == -1) return;

    Component* comp = inv.getComponentById(compId);
    if (!comp || !dynamic_cast<IC*>(comp->getCategory())) {
        cout << BRIGHT_RED << "  ✖ This component is not an IC.\n" << RESET;
        pauseScreen();
        return;
    }

    ICSpecification* spec = inv.getICSpecification(compId);
    if (!spec) {
        cout << BRIGHT_YELLOW
             << "  ⚠ No IC specification found. Add one first.\n"
             << RESET;
        pauseScreen();
        return;
    }

    printHeader("Edit IC Specification - General Info");

    string icType = readStringOptional("  ❖ IC Type", spec->getICType());
    string manufacturer = readStringOptional("  ❖ Manufacturer", spec->getManufacturer());
    string protocol = readStringOptional("  ❖ Protocol", spec->getProtocol());
    string maxFreq = readStringOptional("  ❖ Max Frequency", spec->getMaxFrequency());
    cout << BRIGHT_CYAN << "  ❖ Operating Voltage (range)\n" << RESET;
    string oldVolt = spec->getOperatingVoltage();
    string oldMinV = "-", oldMaxV = "-";

    if (oldVolt != "-" && oldVolt.find("V to ") != string::npos) {
        size_t pos = oldVolt.find("V to ");
        oldMinV = oldVolt.substr(0, pos);
        oldMaxV = oldVolt.substr(pos + 5);
        if (oldMaxV.size() > 1 && oldMaxV.back() == 'V') oldMaxV.pop_back();
    }

    cout << "     Min V [" << oldMinV << "]: ";
    string minV; getline(cin, minV);
    if (minV.empty()) minV = oldMinV;

    cout << "     Max V [" << oldMaxV << "]: ";
    string maxV; getline(cin, maxV);
    if (maxV.empty()) maxV = oldMaxV;

    string opVoltage = "-";
    if (minV != "-" || maxV != "-")
        opVoltage = minV + "V to " + maxV + "V";

    cout << BRIGHT_CYAN << "  ❖ Operating Temperature (range)\n" << RESET;

    string oldTemp = spec->getOperatingTemp();
    string oldMinT = "-", oldMaxT = "-";

    if (oldTemp != "-" && oldTemp.find("°C to ") != string::npos) {
        size_t pos = oldTemp.find("°C to ");
        oldMinT = oldTemp.substr(0, pos);
        oldMaxT = oldTemp.substr(pos + 6);
        if (oldMaxT.size() > 2 && oldMaxT.back() == 'C') oldMaxT.pop_back();
    }

    cout << "     Min °C [" << oldMinT << "]: ";
    string minT; getline(cin, minT);
    if (minT.empty()) minT = oldMinT;

    cout << "     Max °C [" << oldMaxT << "]: ";
    string maxT; getline(cin, maxT);
    if (maxT.empty()) maxT = oldMaxT;

    string opTemp = "-";
    if (minT != "-" || maxT != "-")
        opTemp = minT + "°C to " + maxT + "°C";

    spec->setICType(icType);
    spec->setManufacturer(manufacturer);
    spec->setProtocol(protocol);
    spec->setMaxFrequency(maxFreq);
    spec->setOperatingVoltage(opVoltage);
    spec->setOperatingTemp(opTemp);


    vector<string> pinMenuOpts = {
        "1. View / Edit existing pins",
        "2. Add a new pin",
        "3. Remove a pin",
        "4. Save & Finish"
    };

    while (true) {
        int choice = selectFromMenu(pinMenuOpts, "Manage Pins for " + comp->getModel());

        if (choice == 0) {
            vector<ICPin> currentPins = spec->getPins();
            if (currentPins.empty()) {
                cout << BRIGHT_YELLOW << "\n  ⚠ No pins configured yet.\n" << RESET;
                pauseScreen();
                continue;
            }

            sort(currentPins.begin(), currentPins.end(),
                 [](const ICPin& a, const ICPin& b) { return a.number < b.number; });

            vector<string> pinList;
            for (const auto& pin : currentPins) {
                pinList.push_back("Pin #" + to_string(pin.number) + " - " + pin.name + " (" + pin.function + ")");
            }
            pinList.push_back("Back");

            int pSel = selectFromMenu(pinList, "Select Pin to Edit");
            if (pSel == (int)currentPins.size()) continue;

            const ICPin& targetPin = currentPins[pSel];
            printHeader("Edit Pin #" + to_string(targetPin.number));

            string newName = readStringOptional("  ❖ Pin Name", targetPin.name);
            string newFunc = readStringOptional("  ❖ Pin Function", targetPin.function);

            spec->updatePin(targetPin.number, newName, newFunc);
            cout << BRIGHT_GREEN << "\n  ✔ Pin #" << targetPin.number << " updated successfully.\n" << RESET;
            pauseScreen();
        }
        else if (choice == 1) {
            printHeader("Add New Pin");

            int nextPinNum = 1;
            while (spec->getPinByNumber(nextPinNum) != nullptr) {
                nextPinNum++;
            }

            int pinNum = readDoubleOptional("  ❖ Pin Number", nextPinNum);
            if (spec->getPinByNumber(pinNum) != nullptr) {
                cout << BRIGHT_RED << "\n  ✖ Pin #" << pinNum << " already exists!\n" << RESET;
                pauseScreen();
                continue;
            }

            string pinName = readString("  ❖ Pin Name: ");
            string pinFunc = readString("  ❖ Pin Function: ");

            try {
                spec->addPin(pinNum, pinName, pinFunc);
                cout << BRIGHT_GREEN << "\n  ✔ Pin #" << pinNum << " added successfully.\n" << RESET;
            } catch (const exception& e) {
                cout << BRIGHT_RED << "\n  ✖ Error: " << e.what() << "\n" << RESET;
            }
            pauseScreen();
        }
        else if (choice == 2) {
            vector<ICPin> currentPins = spec->getPins();
            if (currentPins.empty()) {
                cout << BRIGHT_YELLOW << "\n  ⚠ No pins available to remove.\n" << RESET;
                pauseScreen();
                continue;
            }

            sort(currentPins.begin(), currentPins.end(),
                 [](const ICPin& a, const ICPin& b) { return a.number < b.number; });

            vector<string> pinList;
            for (const auto& pin : currentPins) {
                pinList.push_back("Pin #" + to_string(pin.number) + " - " + pin.name);
            }
            pinList.push_back("Back");

            int pSel = selectFromMenu(pinList, "Select Pin to Remove");
            if (pSel == (int)currentPins.size()) continue;

            int targetNum = currentPins[pSel].number;
            spec->removePin(targetNum);
            cout << BRIGHT_GREEN << "\n  ✔ Pin #" << targetNum << " removed successfully.\n" << RESET;
            pauseScreen();
        }
        else if (choice == 3) {
            break;
        }
    }

    vector<ICPin> finalPins = spec->getPins();
    ICSpecification updatedSpec(
        compId,
        spec->getICType(),
        (int)finalPins.size(),
        spec->getManufacturer(),
        spec->getProtocol(),
        spec->getMaxFrequency(),
        spec->getOperatingVoltage(),
        spec->getOperatingTemp()
    );

    for (const auto& pin : finalPins) {
        updatedSpec.addPin(pin.number, pin.name, pin.function);
    }
    *spec = updatedSpec;

    cout << BRIGHT_GREEN << BOLD << "\n  ✔ IC Specification updated successfully.\n" << RESET;
    pauseScreen();
}

////////////////////////////////////////////////////////////////////////////////////////////

static void menuComponentsMenu(Inventory& inv) {
    vector<string> options = {
        "1. View All",
        "2. Add Component",
        "3. Edit Component",
        "4. Remove Component",
        "5. Add to Project",
        "6. Remove from Project",
        "7. Back"
    };

    while (true) {
        int choice = selectFromMenu(options, "The Vault.HW - Components");

        switch (choice) {
            case 0: menuViewComponents(inv); break;
            case 1: menuAddComponent(inv); break;
            case 2: menuEditComponent(inv); break;
            case 3: menuRemoveComponent(inv); break;
            case 4: menuAddComponentToProject(inv); break;
            case 5: menuRemoveComponentFromProject(inv); break;
            case 6: return;
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
        inv.addCustomCategory(name, fields);
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
                vector<Category*> cats = inv.getAllCategories();
                cout << BRIGHT_CYAN << "  ❖ Available Categories:\n" << RESET;
                for (size_t i = 0; i < cats.size(); i++) {
                    Category* c = cats[i];
                    cout << "    [" << BOLD << BRIGHT_YELLOW << c->getId() << RESET << "] " << BOLD << BRIGHT_WHITE << c->getName() << RESET << "\n";
                }
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

    time_t t = time(nullptr);
    tm* now = localtime(&t);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", now);
    string date = buffer;

    cout << BRIGHT_CYAN << "  ❖ Start Date automatically set to: " << BRIGHT_WHITE << date << "\n" << RESET;

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
        string stColor = (p.getStatus() == "active") ? BRIGHT_GREEN : BRIGHT_RED;

        cout << BRIGHT_CYAN << "  [ Project Info ]\n" << RESET;
        cout << "  ❖ Name       : " << BOLD << BRIGHT_WHITE << p.getName() << RESET << "\n"
             << "  ❖ Description: " << p.getDescription() << "\n"
             << "  ❖ Status     : " << BOLD << stColor << p.getStatus() << RESET << "\n"
             << "  ❖ Start Date : " << p.getStartDate() << "\n"
             << "  ❖ Total Price: " << BRIGHT_GREEN << BOLD << p.getTotalPrice() << " €\n" << RESET << "\n"
             << BRIGHT_CYAN << "  [ Components ]\n" << RESET;

        const vector<UsedComponent>& ucomps = p.getComponents();
        for (size_t i = 0; i < ucomps.size(); i++) {
            const UsedComponent& uc = ucomps[i];
            cout << "    ► " << BOLD << BRIGHT_WHITE << uc.getComponent()->getModel() << RESET
                 << "  x" << BRIGHT_YELLOW << uc.getAllocatedQuantity() << RESET
                 << "  (" << BRIGHT_GREEN << uc.getComponent()->getPrice() * uc.getAllocatedQuantity() << " €" << RESET << ")\n";
        }
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

static void menuRemoveComponentFromProject(Inventory& inv) { 
    int projectId = promptProject(inv, "Select Project");
    if (projectId == -1) return;

    Project p = inv.getProjectDetails(projectId);
    const vector<UsedComponent>& comps = p.getComponents();

    if (comps.empty()) {
        cout << BRIGHT_YELLOW << "  ⚠ Project '" << p.getName() << "' has no components.\n" << RESET;
        pauseScreen();
        return;
    }

    vector<string> opts;
    for (size_t i = 0; i < comps.size(); i++) {
        const UsedComponent& uc = comps[i];
        opts.push_back("ID: " + to_string(uc.getComponent()->getId()) + "  "
                     + uc.getComponent()->getModel()
                     + "  [Allocated: " + to_string(uc.getAllocatedQuantity()) + "]");
    }
    opts.push_back("Back");

    int sel = selectFromMenu(opts, "Components in project '" + p.getName() + "'");

    if (sel == (int)comps.size()) return;

    int compId = comps[sel].getComponent()->getId();
    int currentQty = comps[sel].getAllocatedQuantity();
    string compModel = comps[sel].getComponent()->getModel();


    vector<string> removeOpts = {
        "Remove entirely",
        "Reduce quantity (Current qty: " + to_string(currentQty) + ")",
        "Back"
    };

    int removeAction = selectFromMenu(removeOpts, "Remove Component: " + compModel);

    try {
        if (removeAction == 0) {
            inv.removeComponentFromProject(projectId, compId);
            cout << BRIGHT_GREEN << BOLD << "\n  ✔ Component removed entirely from project.\n" << RESET;
            pauseScreen();
        }
        else if (removeAction == 1) {
            int toRemove = readInt("  ❖ How many to remove? ");
            if (toRemove <= 0 || toRemove > currentQty) {
                cout << BRIGHT_RED << "  ✖ Invalid quantity.\n" << RESET;
            } else if (toRemove == currentQty) {
                inv.removeComponentFromProject(projectId, compId);
                cout << BRIGHT_GREEN << BOLD << "\n  ✔ Component removed entirely from project.\n" << RESET;
            } else {
                inv.updateAllocation(projectId, compId, currentQty - toRemove);
                cout << BRIGHT_GREEN << BOLD << "\n  ✔ Quantity reduced.\n" << RESET;
            }
            pauseScreen();
        }
        else if (removeAction == 2) {
            return;
        }
    } catch (const exception& e) {
        cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
        pauseScreen();
    }
}

static void menuChangeProjectStatus(Inventory& inv) { 
    int id = promptProject(inv, "Select Project to Change Status");
    if (id == -1) return;

    vector<string> statusOptions = { "Archive", "Activate", "Back" };
    int statusChoice = selectFromMenu(statusOptions, "Project Status");

    try {
        if (statusChoice == 0) {
            inv.archiveProject(id);
            cout << BRIGHT_GREEN << BOLD << "  ✔ Project archived.\n" << RESET;
            pauseScreen();
        } else if (statusChoice == 1) {
            inv.activateProject(id);
            cout << BRIGHT_GREEN << BOLD << "  ✔ Project activated.\n" << RESET;
            pauseScreen();
        }
    } catch (const exception& e) {
        cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
        pauseScreen();
    }
}

static void menuProjectsMenu(Inventory& inv) { 
    vector<string> options = {
        "1. View Projects",
        "2. Add Project",
        "3. View Details",
        "4. Edit Project",
        "5. Add Component to Project",
        "6. Remove Component from Project",
        "7. Change Status (Active / Archive)",
        "8. Remove Project",
        "9. Generate BOM",
        "10. Back"
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
                Project p = inv.getProjectDetails(id);

                cout << BRIGHT_CYAN << "  ❖ Name (" << p.getName() << "): " << RESET;
                string name;
                getline(cin, name);
                if (name.empty()) name = p.getName();

                cout << BRIGHT_CYAN << "  ❖ Description (" << p.getDescription() << "): " << RESET;
                string desc;
                getline(cin, desc);
                if (desc.empty()) desc = p.getDescription();

                string date = p.getStartDate();

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
            case 5: menuRemoveComponentFromProject(inv); break;
            case 6: menuChangeProjectStatus(inv); break;
            case 7: {
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
            case 8: {
                int id = promptProject(inv, "Select Project to export BOM");
                if (id == -1) break;

                try {
                    inv.generateBOM(id);
                    cout << BRIGHT_GREEN << BOLD << "\n  ✔ BOM file successfully generated in exports/ folder.\n" << RESET;
                } catch (const exception& e) {
                    cout << BRIGHT_RED << "  ✖ Error: " << e.what() << "\n" << RESET;
                }
                pauseScreen();
                break;
            }
            case 9: return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

static void menuUpdateQuantity(Inventory& inv) { 
    int id = promptComponent(inv, "Select Component to Update Quantity");
    if (id == -1) return;

    Component* c = inv.getComponentById(id);
    if (!c) return;

    printHeader("Update Free Quantity");
    cout << BRIGHT_CYAN << "  ❖ Selected Component: " << c->getModel() << "\n" << RESET;
    cout << "  ❖ Current FREE Quantity: " << BOLD << SUPER_GREEN << c->getQuantity() << RESET << "\n\n";

    int newQty = readInt("  ❖ Enter new FREE quantity: ");
    if (newQty < 0) {
        cout << BRIGHT_RED << "  ✖ Free quantity cannot be negative.\n" << RESET;
    } else {
        c->setQuantity(newQty);
        cout << BRIGHT_GREEN << BOLD << "\n  ✔ Quantity updated successfully!" << ".\n" << RESET;
    }
    pauseScreen();
}

static void menuStock(Inventory& inv) { 
    vector<string> options = {
        "1. View Stock Distribution",
        "2. Update Quantity",
        "3. Back",
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
            case 1: menuUpdateQuantity(inv); break;
            case 2: return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

static void menuSearchMenu(Inventory& inv) { 
    vector<string> searchOptions = {
        "1. Search by Model",
        "2. Search by Category",
        "3. Search by Location",
        "4. Search by Price Range",
        "5. Back"
    };

    while (true) {
        int choice = selectFromMenu(searchOptions, "TheVault.HW - Search Components");

        switch (choice) {
            case 0: {
                printHeader("Search by Model");
                string name = readString("  ❖ Enter model: ");
                vector<Component*> results = inv.searchByName(name);
                printComponents(results, inv);
                pauseScreen();
                break;
            }
            case 1: {
                int id = promptCategory(inv, "Select Category to Search");
                if (id == -1) break;

                printHeader("Search by Category");
                vector<Component*> results = inv.searchByCategory(id);
                printComponents(results, inv);
                pauseScreen();
                break;
            }
            case 2: {
                printHeader("Search by Location");
                string loc = readString("  ❖ Enter location: ");
                vector<Component*> results = inv.searchByLocation(loc);
                printComponents(results, inv);
                pauseScreen();
                break;
            }
            case 3: {
                printHeader("Search by Price Range");
                double minP = readDouble("  ❖ Enter min price: ");
                double maxP = readDouble("  ❖ Enter max price: ");
                try {
                    vector<Component*> results = inv.searchByPriceRange(minP, maxP);
                    printComponents(results, inv);
                } catch(const exception& e) {
                    cout << BRIGHT_RED  << e.what() << "\n" << RESET;
                }
                pauseScreen();
                break;
            }
            case 4: return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

static void menuIntegratedCircuits(Inventory& inv)
{
    vector<string> options = {
        "1. View All IC Specifications",
        "2. Edit IC Specification",
        "3. Back"
    };

    while (true)
    {
        int choice = selectFromMenu(options, "The Vault.HW - Integrated Circuits");

        switch (choice)
        {
            case 0:
                menuViewICSpecifications(inv);
                break;

            case 1:
                menuEditICSpecification(inv);
                break;

            case 2:
                return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////

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
            string dbPath = "db/";
            vector<string> filesToDelete = {
                "components.txt",
                "categories.txt",
                "projects.txt"
            };

            bool success = true;
            try {
                for (size_t i = 0; i < filesToDelete.size(); i++) {
                    string file = filesToDelete[i];
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

int main()
{
    enableANSI();

    Inventory inv("db/", "exports/");

    vector<string> mainMenu = {
        "1. Components",
        "2. Categories",
        "3. Projects",
        "4. Integrated Circuits",
        "5. Search Components",
        "6. Stock Management",
        "7. Compare Components",
        "8. Generate Project BOM",
        "9. Import and analyse BOM",
        "10. Destroy The Vault",
        "11. Exit"
    };


    while (true)
    {
        int choice = selectFromMenu(mainMenu, "The Vault.HW - Main Menu");

        switch (choice)
        {
            case 0:  menuComponentsMenu(inv); break;
            case 1:  menuCategoriesMenu(inv); break;
            case 2:  menuProjectsMenu(inv); break;
            case 3:  menuIntegratedCircuits(inv); break;
            case 4:  menuSearchMenu(inv); break;
            case 5:  menuStock(inv); break;
        
            case 6:
            {
                int id1 = promptComponent(inv, "First Component");
                if (id1 == -1) break;
            
                int id2 = promptComponent(inv, "Second Component");
                if (id2 == -1) break;
            
                printHeader("Compare Components");
            
                try {
                    inv.compareComponents(id1, id2);
                }
                catch (const exception& e) {
                    cout << BRIGHT_RED
                         << "  ✖ Error: " << e.what() << "\n"
                         << RESET;
                }
            
                pauseScreen();
                break;
            }
        
            case 7:
            {
                int id = promptProject(inv, "Select Project to export BOM");
                if (id == -1) break;
            
                try {
                    inv.generateBOM(id);
                    cout << BRIGHT_GREEN << BOLD
                         << "\n  ✔ BOM file successfully generated in exports/ folder.\n"
                         << RESET;
                }
                catch (const exception& e) {
                    cout << BRIGHT_RED
                         << "  ✖ Error: " << e.what() << "\n"
                         << RESET;
                }
            
                pauseScreen();
                break;
            }
        
            case 8:
            {
                printHeader("Import BOM (CSV)");
            
                string path = readString("  ❖ Enter full path to CSV file: ");
            
                clearScreen();
            
                try {
                    inv.importBOM(path);
                }
                catch (const exception& e) {
                    cout << BRIGHT_RED
                         << "\n  ✖ Error: " << e.what() << "\n"
                         << RESET;
                }
            
                pauseScreen();
                break;
            }
        
            case 9:  confirmDrop(inv); break;
        
            case 10:
                inv.saveToFile();
                clearScreen();
        
                cout << BRIGHT_GREEN << BOLD
                     << "\n  ✔ Data saved. Thank you for using The Vault.HW\n\n"
                     << RESET;
        
                return 0;
        }
    }
}