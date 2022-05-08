#ifndef ATTACHMENTDEFINERBASE_H
#define ATTACHMENTDEFINERBASE_H

#include <QMetaType>
#include <QHash>

#include "NetworkSettings.h"

class AttachmentDefinerBase
{
public:
    using ContentExtensionIdType = uint8_t;
    
    enum ContentExtensionId : ContentExtensionIdType {
        CEI_INVALID = 0,
        CEI_ENCRYPTED,
        CEI_JPG,
        CEI_PNG,
        CEI_GIF,
        CEI_TXT,
        CEI_DOC,
        CEI_DOCX
    };
    
    static const QHash<ContentExtensionId, QString>& getContentExtensionHash();
    
    static QMetaType::Type    getMetaTypeByFilename           (const QString &filename);
    
    static ContentExtensionId getContentExtensionIdByFilename (const QString &filename);
    static ContentExtensionId getContentExtensionIdByExtension(const QString &extension);
    static const QString      getExtensionByContentExtensionId(const ContentExtensionId contentExtensionId);   
    
    AttachmentDefinerBase() = delete;
    
    static const QString getExtensionByFilename(const QString &filename);
};

#endif // ATTACHMENTDEFINERBASE_H
