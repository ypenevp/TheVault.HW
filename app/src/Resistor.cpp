#include "Resistor.h"
#include <stdexcept>
#include <sstream>
using namespace std;

Resistor::Resistor(int id, const string& name,
    double resistance, double powerRating,
    double tolerance, double tcr) : Category(id, name),
            resistance(resistance), powerRating(powerRating),
            tolerance(tolerance), tcr(tcr) {}

double Resistor::getResistance() const { 
    return this->resistance; 
}

double Resistor::getPowerRating() const { 
    return this->powerRating; 
}

double Resistor::getTolerance() const { 
    return this->tolerance; 
}

double Resistor::getTcr() const { 
    return this->tcr; 
}

string Resistor::getData() const {
    ostringstream oss;
    oss << "Resistance: " << this->resistance << " Ohm | "
        << "Power Rating: " << this->powerRating << " W | "
        << "Tolerance: " << this->tolerance << " % | "
        << "TCR: " << this->tcr << " ppm/degC";

   return oss.str();
}

vector<string> Resistor::getFields() const {
    return {"Resistance{Ohm}", "Tolerance{%}", "Power Rating{W}", "TCR{ppm/degC}"};
}


void Resistor::setResistance(double resistance) {
    if (resistance < 0) throw invalid_argument("Resistance cannot be negative.");
    this->resistance = resistance;
}

void Resistor::setPowerRating(double powerRating) {
    if (powerRating < 0) throw invalid_argument("Power rating cannot be negative.");
    this->powerRating = powerRating;
}

void Resistor::setTolerance(double tolerance) {
    if (tolerance < 0) throw invalid_argument("Tolerance cannot be negative.");
    this->tolerance = tolerance;
}

void Resistor::setTcr(double tcr) {
    if (tcr < 0) throw invalid_argument("TCR cannot be negative.");
    this->tcr = tcr;
}

void Resistor::setFields(const vector<string>& values) {
    if (values.size() != 4)
        throw invalid_argument("Resistor requires exactly 4 field values.");
    this->setResistance(stod(values[0]));
    this->setPowerRating(stod(values[1]));
    this->setTolerance(stod(values[2]));
    this->setTcr(stod(values[3]));
}

Category* Resistor::clone() const {
    return new Resistor(*this);
}