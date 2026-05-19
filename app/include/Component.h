#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <map>
#include "Category.h"

class Component{

private:
    int id;
    std::string model;
    double price;
    int quantity;
    std::string mountingType;
    std::string storageLocation;
    std::string packageType;
    std::string datasheet;
    Category *category;
    std::map<std::string, std::string> extraFields;

public:
    Component(int id, const std::string &model, double price, int quantity,
                  const std::string &mountingType, const std::string &storageLocation,
                  const std::string &packageType, const std::string &datasheet,
                  Category *category,
                  const std::map<std::string, std::string> &customValues = {});

    int getId() const;
    std::string getModel() const;
    double getPrice() const;
    int getQuantity() const;
    std::string getMountingType() const;
    std::string getStorageLocation() const;
    std::string getPackage() const;
    std::string getDatasheet() const;
    Category *getCategory() const;
    std::map<std::string, std::string> customValues;
    const std::map<std::string, std::string> &getCustomValues() const;
    const std::map<std::string, std::string> &getExtraFields() const;
    
    void setModel(const std::string &model);
    void setPackage(const std::string &packageType);
    void setDatasheet(const std::string &datasheet);
    void setPrice(double price);
    void setQuantity(int quantity);
    void setMountingType(const std::string &mountingType);
    void setStorageLocation(const std::string &storageLocation);
    void setCustomValue(const std::string &key, const std::string &value);

};

#endif