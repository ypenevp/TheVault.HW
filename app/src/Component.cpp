#include "Component.h"

Component::Component(int id, const std::string &model, double price, int quantity,
                     const std::string &mountingType, const std::string &storageLocation,
                     const std::string &packageType, const std::string &datasheet,
                     Category *category,
                     const std::map<std::string, std::string> &customValues)
    : id(id), model(model), price(price), quantity(quantity),
      mountingType(mountingType), storageLocation(storageLocation),
      packageType(packageType), datasheet(datasheet),
      category(category), customValues(customValues) {}


int Component::getId() const { 
  return this->id;
}
std::string Component::getModel() const { 
  return this->model;
}

double Component::getPrice() const { 
  return this->price;
}

int Component::getQuantity() const { 
  return this->quantity;
}

std::string Component::getPackage() const {
  return this->packageType;
}

std::string Component::getDatasheet() const {
  return this->datasheet;
}

std::string Component::getMountingType() const { 
  return this->mountingType;
}

std::string Component::getStorageLocation() const { 
  return this->storageLocation;
}

Category *Component::getCategory() const { 
  return this->category;
}

const std::map<std::string, std::string> &Component::getExtraFields() const { 
  return this->extraFields;
}

const std::map<std::string, std::string> &Component::getCustomValues() const { 
  return customValues;
}


void Component::setModel(const std::string &model) { 
  this->model = model;
}

void Component::setPrice(double price) { 
  this->price = price;
}

void Component::setQuantity(int quantity) { 
  this->quantity = quantity;
}

void Component::setPackage(const std::string &packageType) {
  this->packageType = packageType;
}

void Component::setDatasheet(const std::string &datasheet) {
  this->datasheet = datasheet;
}

void Component::setMountingType(const std::string &mountingType) {
  this->mountingType = mountingType;
}

void Component::setStorageLocation(const std::string &storageLocation) { 
  this->storageLocation = storageLocation;
}

void Component::setCustomValue(const std::string &key, const std::string &value) { 
  customValues[key] = value;
}

