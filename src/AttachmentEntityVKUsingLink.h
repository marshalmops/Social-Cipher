#ifndef ATTACHMENTENTITYVKUSINGLINK_H
#define ATTACHMENTENTITYVKUSINGLINK_H

#include "AttachmentEntityVK.h"

#include <QUrl>

class AttachmentEntityVKUsingLink : public AttachmentEntityVK
{
public:
    AttachmentEntityVKUsingLink(const EntityId attachmentId, 
                                const EntityId peerId,
                                const QUrl &link,
                                const QString &fileName = QString{});
    
    virtual bool isValid() const override;
    virtual RelatedToAttachmentsProcessingVK::AttachmentOriginFlag getProcessingFlag() const override;
    
    const QString getFileName ();
    const QString getExtension();
    const QUrl    getUrl      ();
    
protected:
    QUrl    m_link;
    QString m_fileName;
};

#endif // ATTACHMENTENTITYVKUSINGLINK_H
