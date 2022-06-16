#ifndef DIALOGJSONPARSERINTERFACE_H
#define DIALOGJSONPARSERINTERFACE_H

#include "EntityJsonParserInterface.h"

class DialogJsonParserInterface : public EntityJsonParserInterface
{
public:
    DialogJsonParserInterface() = default;
    
    virtual bool jsonToDialogs (const QJsonValue &json,
                                std::vector<std::unique_ptr<DialogEntityBase>> &dialogs) const = 0;
    virtual bool jsonToFullName(const QJsonValue &json,
                                QString &fullName) = 0;
};

#endif // DIALOGJSONPARSERINTERFACE_H
