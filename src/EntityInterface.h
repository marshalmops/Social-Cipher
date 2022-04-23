#ifndef ENTITYINTERFACE_H
#define ENTITYINTERFACE_H

#include <QJsonObject>

class EntityInterface
{
public:
    using EntityId = int64_t;
    
    EntityInterface()          = default;
    virtual ~EntityInterface() = default;
    
    virtual bool isValid() const = 0;
};

#endif // ENTITYINTERFACE_H
