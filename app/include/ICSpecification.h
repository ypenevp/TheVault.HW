#ifndef IC_SPECIFICATION_H
#define IC_SPECIFICATION_H

#include <string>
#include <vector>

struct ICPin {
    int number;
    std::string name;
    std::string function;
};

class ICSpecification {
private:
    int componentId;
    std::string icType;
    int pinCount;
    std::vector<ICPin> pins;
    std::string manufacturer;
    std::string protocol;
    std::string maxFrequency;
    std::string operatingVoltage;
    std::string operatingTemp;

public:
    ICSpecification(int componentId,
                    const std::string& icType,
                    int pinCount,
                    const std::string& manufacturer,
                    const std::string& protocol,
                    const std::string& maxFrequency,
                    const std::string& operatingVoltage = "-",
                    const std::string& operatingTemp = "-");

    int getComponentId() const;
    std::string getICType() const;
    int getPinCount() const;
    const std::vector<ICPin>& getPins() const;

    std::string getManufacturer() const;
    std::string getProtocol() const;
    std::string getMaxFrequency() const;
    std::string getOperatingVoltage() const;
    std::string getOperatingTemp() const;

    void addPin(int number, const std::string& name, const std::string& function);
    void removePin(int number);
    void updatePin(int number, const std::string& name, const std::string& function);
    ICPin* getPinByNumber(int number);

    void setICType(const std::string& icType);
    void setManufacturer(const std::string& manufacturer);
    void setProtocol(const std::string& protocol);
    void setMaxFrequency(const std::string& maxFrequency);
    void setOperatingVoltage(const std::string& operatingVoltage);
    void setOperatingTemp(const std::string& operatingTemp);
};

#endif