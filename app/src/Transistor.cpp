#include "Transistor.h"
#include <stdexcept>
#include <sstream>
using namespace std;

Transistor::Transistor(int id, const string& name,
                       const string& transistorType, double maxCurrent,
                       double Vbe, double powerDissipation,
                       double beta, double alfa)
    : Category(id, name),
      transistorType(transistorType),
      maxCurrent(maxCurrent),
      Vbe(Vbe),
      powerDissipation(powerDissipation),
      beta(beta),
      alfa(alfa) {}


string Transistor::getTransistorType() const { 
    return this->transistorType;
}

double Transistor::getMaxCurrent() const { 
    return this->maxCurrent;
}

double Transistor::getVbe() const { 
    return this->Vbe;
}

double Transistor::getPowerDissipation() const { 
    return this->powerDissipation;
}

double Transistor::getBeta() const { 
    return this->beta;
}

double Transistor::getAlfa() const { 
    return this->alfa;
}

string Transistor::getData() const {
    ostringstream oss;
    oss << "Type: " << this->transistorType << " | "
        << "Max Current: " << this->maxCurrent << " A | "
        << "Vbe: " << this->Vbe << " V | "
        << "Power Dissipation: " << this->powerDissipation << " W | "
        << "Beta: " << this->beta << " | "
        << "Alfa: " << this->alfa;
    return oss.str();
}

vector<string> Transistor::getFields() const {
    return {"Type(NPN/PNP/P~N-MOS)", "Max Ic/Id{A}", "Vbe/Vth{V}", "Power Dissipation{W}", "Beta(hFE)", "Alfa"};
}

void Transistor::setTransistorType(const string& transistorType) {
    if (transistorType != "NPN" && transistorType != "PNP" &&
        transistorType != "NMOS" && transistorType != "PMOS" &&
        transistorType != "-") {
        throw invalid_argument("Transistor type must be NPN, PNP, NMOS, or PMOS!");
    }
    this->transistorType = transistorType;
}

void Transistor::setMaxCurrent(double maxCurrent) {
    if (maxCurrent < 0) throw invalid_argument("Max current cannot be negative!");
    this->maxCurrent = maxCurrent;
}

void Transistor::setVbe(double Vbe) {
    this->Vbe = Vbe;
}

void Transistor::setPowerDissipation(double powerDissipation) {
    if (powerDissipation < 0) throw invalid_argument("Power dissipation cannot be negative!");
    this->powerDissipation = powerDissipation;
}

void Transistor::setBeta(double beta) {
    this->beta = beta;
}

void Transistor::setAlfa(double alfa) {
    this->alfa = alfa;
}

void Transistor::setFields(const vector<string>& values) {
    if (values.size() != 6)
        throw invalid_argument("Transistor requires exactly 6 field values!");
    this->setTransistorType(values[0]);
    this->setMaxCurrent(stod(values[1]));
    this->setVbe(stod(values[2]));
    this->setPowerDissipation(stod(values[3]));
    this->setBeta(stod(values[4]));
    this->setAlfa(stod(values[5]));
}

Category* Transistor::clone() const {
    return new Transistor(*this);
}