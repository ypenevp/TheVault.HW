#include "ICSpecification.h"
#include <stdexcept>
#include <algorithm>

using namespace std;

ICSpecification::ICSpecification(int componentId,
                                 const string& icType,
                                 int pinCount,
                                 const string& manufacturer,
                                 const string& protocol,
                                 const string& maxFrequency,
                                 const string& operatingVoltage,
                                 const string& operatingTemp)
    : componentId(componentId),
      icType(icType),
      pinCount(pinCount),
      manufacturer(manufacturer),
      protocol(protocol),
      maxFrequency(maxFrequency),
      operatingVoltage(operatingVoltage),
      operatingTemp(operatingTemp)
{
}

int ICSpecification::getComponentId() const {
    return componentId;
}

string ICSpecification::getICType() const {
    return icType;
}

int ICSpecification::getPinCount() const {
    return pinCount;
}

const vector<ICPin>& ICSpecification::getPins() const {
    return pins;
}

string ICSpecification::getManufacturer() const {
    return manufacturer;
}

string ICSpecification::getProtocol() const {
    return protocol;
}

string ICSpecification::getMaxFrequency() const {
    return maxFrequency;
}

string ICSpecification::getOperatingVoltage() const {
    return operatingVoltage;
}

string ICSpecification::getOperatingTemp() const {
    return operatingTemp;
}

void ICSpecification::addPin(int number,
                             const string& name,
                             const string& function)
{
    if (getPinByNumber(number) != nullptr)
        throw invalid_argument("Pin " + to_string(number) + " already exists.");

    pins.push_back({number, name, function});

    sort(pins.begin(), pins.end(),
         [](const ICPin& a, const ICPin& b) {
             return a.number < b.number;
         });
}

void ICSpecification::removePin(int number)
{
    auto it = find_if(pins.begin(), pins.end(),
        [number](const ICPin& p) {
            return p.number == number;
        });

    if (it != pins.end())
        pins.erase(it);
}

void ICSpecification::updatePin(int number,
                                const string& name,
                                const string& function)
{
    ICPin* pin = getPinByNumber(number);

    if (pin) {
        pin->name = name;
        pin->function = function;
    }
}

ICPin* ICSpecification::getPinByNumber(int number)
{
    for (auto& pin : pins) {
        if (pin.number == number)
            return &pin;
    }

    return nullptr;
}

void ICSpecification::setICType(const string& icType) {
    this->icType = icType;
}

void ICSpecification::setManufacturer(const string& manufacturer) {
    this->manufacturer = manufacturer;
}

void ICSpecification::setProtocol(const string& protocol) {
    this->protocol = protocol;
}

void ICSpecification::setMaxFrequency(const string& maxFrequency) {
    this->maxFrequency = maxFrequency;
}

void ICSpecification::setOperatingVoltage(const string& operatingVoltage) {
    this->operatingVoltage = operatingVoltage;
}

void ICSpecification::setOperatingTemp(const string& operatingTemp) {
    this->operatingTemp = operatingTemp;
}