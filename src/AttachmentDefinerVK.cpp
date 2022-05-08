#include "AttachmentDefinerVK.h"

const QHash<QString, RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag> &AttachmentDefinerVK::getProcessingWayTypeStringHash()
{
    static const QHash<QString, RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag> processingWayTypeStringHash = {
        {"photo", RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_PHOTO_PROCESSING},
        {"doc",   RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_DOCUMENT_PROCESSING}
    };
    
    return processingWayTypeStringHash;
}

RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag AttachmentDefinerVK::getProcessingWayByTypeString(const QString &typeString)
{
    if (typeString.isEmpty()) 
        return RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_INVALID;
    
    const auto & processingWayTypeStringHash{getProcessingWayTypeStringHash()};
    
    return (processingWayTypeStringHash.contains(typeString)
            ? processingWayTypeStringHash[typeString]
              : RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_INVALID);
}

RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag AttachmentDefinerVK::getProcessingWayByExtension(const QString &extension)
{
    static const QHash<QString, RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag> attachmentExtensionProcessingWayHash = {
        {"png",                                           RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_PHOTO_PROCESSING},
        {"jpg",                                           RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_PHOTO_PROCESSING},
        {"gif",                                           RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_PHOTO_PROCESSING},
        {NetworkSettings::C_ENCODED_ATTACHMENT_EXTENSION, RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_DOCUMENT_PROCESSING},
        {"txt",                                           RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_DOCUMENT_PROCESSING},
        {"doc",                                           RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_DOCUMENT_PROCESSING},
        {"docx",                                          RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_DOCUMENT_PROCESSING}
    };
    
    return (attachmentExtensionProcessingWayHash.contains(extension)
            ? attachmentExtensionProcessingWayHash[extension]
              : RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_INVALID);
}

QString AttachmentDefinerVK::getTypeStringByProcessingWay(const RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag processingWay)
{
    if (processingWay == RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_INVALID)
        return QString{};
        
    const auto &processingWayTypeStringHash{getProcessingWayTypeStringHash()};
    
    for (auto iter = processingWayTypeStringHash.begin(); iter != processingWayTypeStringHash.end(); ++iter) {
        const auto curProcessingWay{iter.value()};
        const auto curTypeString   {iter.key()};
        
        if (curProcessingWay == processingWay)
            return curTypeString;
    }
    
    return QString{};
}
