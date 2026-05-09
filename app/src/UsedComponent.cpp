#include "UsedComponent.h"
#include <stdexcept>
using namespace std;

UsedComponent::UsedComponent(Component* component, int allocatedQuantity)
    : component(component), allocatedQuantity(allocatedQuantity) {

    if (component == nullptr) throw invalid_argument("Component cannot be null!");
    if (allocatedQuantity <= 0) throw invalid_argument("Allocated quantity must be positive!");
}

Component* UsedComponent::getComponent() const { 
    return this->component;
}
int UsedComponent::getAllocatedQuantity() const { 
    return this->allocatedQuantity;
}

void UsedComponent::setAllocatedQuantity(int quantity) {
    if (quantity <= 0) throw invalid_argument("Allocated quantity must be positive!");
    this->allocatedQuantity = quantity;
}
