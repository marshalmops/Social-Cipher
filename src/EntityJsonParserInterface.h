#ifndef ENTITYJSONPARSERINTERFACE_H
#define ENTITYJSONPARSERINTERFACE_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <bitset>

#include "MessageEntityBase.h"
#include "DialogEntityBase.h"

#include "RelatedToMessagesProcessingBase.h"

class EntityJsonParserInterface
{
public:
             EntityJsonParserInterface() = default;
    virtual ~EntityJsonParserInterface() = default;
};

#endif // ENTITYJSONPARSERINTERFACE_H
