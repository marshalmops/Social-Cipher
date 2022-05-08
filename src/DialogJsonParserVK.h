#ifndef DIALOGJSONPARSERVK_H
#define DIALOGJSONPARSERVK_H

#include "DialogJsonParserInterface.h"

#include "DialogEntityBase.h"

class DialogJsonParserVK : public DialogJsonParserInterface
{
public:
    DialogJsonParserVK();
    
    virtual bool jsonToDialogs (const QJsonValue &json,
                                std::vector<std::unique_ptr<DialogEntityBase>> &dialogs) const override;
    
private:
    bool findPeerNameInProfilesGroups(const EntityInterface::EntityId peerId,
                                      const QJsonArray &profilesJsonArray,
                                      const QJsonArray &groupsJsonArray,
                                      QString &peerName) const;
    
    bool checkPeerNameInJsonObject(const QStringList &peerNameParamsNames,
                                   const EntityInterface::EntityId peerId,
                                   const QJsonValue &jsonVal,
                                   QString &peerName) const;
};

#endif // DIALOGJSONPARSERVK_H
