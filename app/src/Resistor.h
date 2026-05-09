#ifndef RESISTOR_H
#define RESISTOR_H

#include "Category.h"

class Resistor : public Category {
private:
    double resistance;
    double powerRating;
    double tolerance;
    double tcr;

public:
    Resistor(int id, const std::string& name,
             double resistance, double powerRating,
             double tolerance, double tcr);

    double getResistance() const;
    double getPowerRating() const;
    double getTolerance() const;
    double getTcr() const;

    void setResistance(double resistance);
    void setPowerRating(double powerRating);
    void setTolerance(double tolerance);
    void setTcr(double tcr);

    std::string getData() const override;
    std::vector<std::string> getFields() const override;
    void setFields(const std::vector<std::string>& values) override;
    Category* clone() const override;
};

#endif