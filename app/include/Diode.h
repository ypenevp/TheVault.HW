#ifndef DIODE_H
#define DIODE_H

#include "Category.h"

class Diode : public Category {
private:
    double forwardVoltage;
    double maxCurrent;

public:
    Diode(int id, const std::string& name,
          double forwardVoltage, double maxCurrent);

    double getForwardVoltage() const;
    double getMaxCurrent() const;

    void setForwardVoltage(double forwardVoltage);
    void setMaxCurrent(double maxCurrent);

    std::string getData() const override;
    std::vector<std::string> getFields() const override;
    void setFields(const std::vector<std::string>& values) override;
    Category* clone() const override;
};

#endif