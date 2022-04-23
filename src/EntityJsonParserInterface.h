#ifndef ENTITYJSONPARSERINTERFACE_H
#define ENTITYJSONPARSERINTERFACE_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <bitset>

#include "MessageEntity.h"
#include "DialogEntity.h"

class EntityJsonParserInterface
{
public:
    using ParsingFlag = uint16_t;
    
    EntityJsonParserInterface() = default;
    virtual ~EntityJsonParserInterface() = default;
    
    virtual bool jsonToMessages(const QJsonValue &json,
                                const ParsingFlag flags,
                                std::vector<MessageEntity> &messages) const = 0;
    virtual bool jsonToDialogs (const QJsonValue &json,
                                std::vector<DialogEntity> &dialogs)   const = 0;
};

#endif // ENTITYJSONPARSERINTERFACE_H
