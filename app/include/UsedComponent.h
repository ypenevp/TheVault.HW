#ifndef USED_COMPONENT_H
#define USED_COMPONENT_H

#include "Component.h"

class UsedComponent {

private:
    Component* component;
    int allocatedQuantity;

public:
    UsedComponent(Component* component, int allocatedQuantity);

    Component* getComponent() const;
    int getAllocatedQuantity() const;
    
    void setAllocatedQuantity(int quantity);
};

#endif