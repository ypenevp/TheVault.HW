#include "Diode.h"
#include <stdexcept>
#include <sstream>
using namespace std;

Diode::Diode(int id, const string& name,
             double forwardVoltage, double maxCurrent)
    : Category(id, name),
      forwardVoltage(forwardVoltage),
      maxCurrent(maxCurrent) {}

double Diode::getForwardVoltage() const { 
    return this->forwardVoltage;
}
double Diode::getMaxCurrent() const { 
    return this->maxCurrent;
}

void Diode::setForwardVoltage(double forwardVoltage) {
    if (forwardVoltage < 0) throw invalid_argument("Forward voltage cannot be negative.");
    this->forwardVoltage = forwardVoltage;
}

void Diode::setMaxCurrent(double maxCurrent) {
    if (maxCurrent < 0) throw invalid_argument("Max current cannot be negative.");
    this->maxCurrent = maxCurrent;
}

string Diode::getData() const {
    ostringstream oss;
    oss << "Forward Voltage: " << this->forwardVoltage << " V | "
        << "Max Current: " << this->maxCurrent << " A";
    return oss.str();
}

vector<string> Diode::getFields() const {
    return {"Forward Voltage{V}", "Reverse Breakdown Voltage{V}", "Max Forward Current{A}", "Max Power Dissipation{W}"};
}

void Diode::setFields(const vector<string>& values) {
    if (values.size() != 2)
        throw invalid_argument("Diode requires exactly 2 field values.");
    this->setForwardVoltage(stod(values[0]));
    this->setMaxCurrent(stod(values[1]));
}

Category* Diode::clone() const {
    return new Diode(*this);
}