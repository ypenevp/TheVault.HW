#ifndef TRANSISTOR_H
#define TRANSISTOR_H

#include "Category.h"

class Transistor : public Category {

private:
    std::string transistorType;
    double maxCurrent;
    double Vbe;
    double powerDissipation;
    double beta;
    double alfa;

public:
    Transistor(int id, const std::string& name,
               const std::string& transistorType, double maxCurrent,
               double Vbe, double powerDissipation,
               double beta, double alfa);

    std::string getTransistorType() const;
    double getMaxCurrent() const;
    double getVbe() const;
    double getPowerDissipation() const;
    double getBeta() const;
    double getAlfa() const;
    std::string getData() const override;
    std::vector<std::string> getFields() const override;

    void setTransistorType(const std::string& transistorType);
    void setMaxCurrent(double maxCurrent);
    void setVbe(double Vbe);
    void setPowerDissipation(double powerDissipation);
    void setBeta(double beta);
    void setAlfa(double alfa);
    void setFields(const std::vector<std::string>& values) override;
    
    Category* clone() const override;
};

#endif