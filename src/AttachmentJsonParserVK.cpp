#include "AttachmentJsonParserVK.h"

AttachmentJsonParserVK::AttachmentJsonParserVK()
{
    
}

bool AttachmentJsonParserVK::jsonToAttachments(const QJsonValue &json,
                                               const RelatedToEntityProcessingBase::EntityProcessingFlagType attachmentsProcessingFlag,
                                               std::vector<std::shared_ptr<AttachmentEntityBase> > &attachments) const
{
    if (attachmentsProcessingFlag == RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_INVALID)
        return false;
    
    if (json.isNull()) return false;
    
    uint8_t  attachmentOriginFlag{static_cast<uint8_t> (attachmentsProcessingFlag & 0b0000'0000'1111'1111)};
    RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag attachmentTypeFlag{static_cast<uint16_t>(attachmentsProcessingFlag & 0b1111'1111'0000'0000)};
    
    switch (attachmentOriginFlag) {
        case RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_EVENTIONAL: {
            if (!jsonEventsToAttachments(json, attachments)) return false;
            
            break;
        }
        case RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_STANDARD: {
            if (!jsonStandardToAttachments(json, attachments)) return false;
            
            break;
        }
        case RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_UPLOADED: {
            if (!jsonUploadedToAttachments(json, attachmentTypeFlag, attachments)) return false;
            
            break;
        }
    }
    
    return true;
}

bool AttachmentJsonParserVK::jsonEventsToAttachments(const QJsonValue &json,
                                                     std::vector<std::shared_ptr<AttachmentEntityBase>> &attachments) const
{
    if (!json.isObject()) return false;
    
    QJsonObject attachmentsJsonObject{json.toObject()};
    
    if (attachmentsJsonObject.isEmpty()) return true;
    
    QString currentAttachKey{};
    
    for (auto i = 1; true; ++i) {
        currentAttachKey = "attach" + QString::number(i);
        
        QString currentAttachmentType = currentAttachKey + "_type";
        
        if (!attachmentsJsonObject.contains(currentAttachKey)
         || !attachmentsJsonObject.contains(currentAttachmentType))
        {
            break;
        }
        
        QString attachmentPeerAndIdString{attachmentsJsonObject[currentAttachKey].toString()};
        
        if (attachmentPeerAndIdString.isEmpty()) return false;
        
        QStringList attachmentPeerAndIdStringList{attachmentPeerAndIdString.split('_')};
        
        if (attachmentPeerAndIdStringList.length() != 2) return false;
        
        bool isConvOK{false};
        
        AttachmentEntityBase::EntityId attachmentPeer = attachmentPeerAndIdStringList[0].toLongLong(&isConvOK);
        
        if (!isConvOK) return false;
        
        AttachmentEntityBase::EntityId attachmentId = attachmentPeerAndIdStringList[1].toLongLong(&isConvOK);
        
        if (!isConvOK) return false;
        
        QString attachmentType = attachmentsJsonObject[currentAttachmentType].toString();
        
        if (attachmentType.isEmpty()) return false;
        
        AttachmentEntityVKUsingIdString attachment{attachmentId, attachmentPeer, attachmentType};
        
        if (!attachment.isValid()) return false;
    
        attachments.push_back(std::make_shared<AttachmentEntityVKUsingIdString>(std::move(attachment)));
    }
    
    return true;
}

bool AttachmentJsonParserVK::jsonStandardToAttachments(const QJsonValue &json,
                                                       std::vector<std::shared_ptr<AttachmentEntityBase>> &attachments) const
{
    if (!json.isArray()) return false;
    
    QJsonArray attachmentsJsonArray = json.toArray();
    
    foreach (const auto &attachmentJsonValue, attachmentsJsonArray) {
        if (!attachmentJsonValue.isObject()) return false;
        
        QJsonObject attachmentJsonObject{attachmentJsonValue.toObject()};
        
        if (!attachmentJsonObject.contains(C_ATTACHMENT_TYPE_FIELD_NAME)) return false;
        
        QString attachmentType{attachmentJsonObject[C_ATTACHMENT_TYPE_FIELD_NAME].toString()};
        
        if (!attachmentJsonObject.contains(attachmentType)) return false;
        
        auto attachmentContentJsonValue = attachmentJsonObject[attachmentType];
        
        if (!attachmentContentJsonValue.isObject()) return false;
        
        QJsonObject attachmentContentJsonObject{attachmentContentJsonValue.toObject()};
        
        QUrl    attachmentUrl     {};
        QString attachmentFileName{};
        
        switch (AttachmentDefinerVK::getProcessingWayByTypeString(attachmentType)) {
        case RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_PHOTO_PROCESSING: {
            if (!attachmentContentJsonObject.contains("sizes"))  return false;
            if (!attachmentContentJsonObject["sizes"].isArray()) return false;
            
            QJsonArray attachmentPhotoSizesJsonArray = attachmentContentJsonObject["sizes"].toArray();
            
            if (attachmentPhotoSizesJsonArray.size() <= 0) return false;
            
            QJsonValue attachmentPhotoBiggestSizeJsonValue = attachmentPhotoSizesJsonArray.last();
            
            if (!attachmentPhotoBiggestSizeJsonValue.isObject()) return false;
            
            QJsonObject attachmentPhotoBiggestSizeJsonObject = attachmentPhotoBiggestSizeJsonValue.toObject();
            
            if (!attachmentPhotoBiggestSizeJsonObject.contains(C_ATTACHMENT_URL_FIELD_NAME))   return false;
            if (!attachmentPhotoBiggestSizeJsonObject[C_ATTACHMENT_URL_FIELD_NAME].isString()) return false;
            
            attachmentUrl = attachmentPhotoBiggestSizeJsonObject[C_ATTACHMENT_URL_FIELD_NAME].toString();
            
            break;
        }
        case RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_DOCUMENT_PROCESSING: {
            if (!attachmentContentJsonObject.contains(C_ATTACHMENT_URL_FIELD_NAME))   return false;
            if (!attachmentContentJsonObject[C_ATTACHMENT_URL_FIELD_NAME].isString()) return false;
            
            attachmentUrl = attachmentContentJsonObject[C_ATTACHMENT_URL_FIELD_NAME].toString();
            
            // getting document extension...
            
            if (!attachmentContentJsonObject.contains(C_ATTACHMENT_DOC_FILENAME_FIELD_NAME))   return false;
            if (!attachmentContentJsonObject[C_ATTACHMENT_DOC_FILENAME_FIELD_NAME].isString()) return false;
            
            attachmentFileName = attachmentContentJsonObject[C_ATTACHMENT_DOC_FILENAME_FIELD_NAME].toString();
            
            break;
        }
        default: return false;
        }
        
        if (!attachmentContentJsonObject.contains(C_ATTACHMENT_ID_FIELD_NAME)) return false;
        if (!attachmentContentJsonObject[C_ATTACHMENT_ID_FIELD_NAME].toVariant().canConvert<AttachmentEntityBase::EntityId>()) return false;
        
        AttachmentEntityBase::EntityId attachmentId{attachmentContentJsonObject[C_ATTACHMENT_ID_FIELD_NAME].toVariant().toLongLong()};
        
        if (!attachmentContentJsonObject.contains(C_ATTACHMENT_OWNER_FIELD_NAME)) return false;
        if (!attachmentContentJsonObject[C_ATTACHMENT_OWNER_FIELD_NAME].toVariant().canConvert<AttachmentEntityBase::EntityId>()) return false;
        
        AttachmentEntityBase::EntityId attachmentPeer{attachmentContentJsonObject[C_ATTACHMENT_OWNER_FIELD_NAME].toVariant().toLongLong()};
        
        AttachmentEntityVKUsingLink newAttachment{attachmentId, attachmentPeer, attachmentUrl, attachmentFileName};
        
        if (!newAttachment.isValid()) return false;
        
        attachments.push_back(std::make_shared<AttachmentEntityVKUsingLink>(std::move(newAttachment)));
    }
    
    return true;
}

bool AttachmentJsonParserVK::jsonUploadedToAttachments(const QJsonValue &json,
                                                       const RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag attachmentTypeFlag,
                                                       std::vector<std::shared_ptr<AttachmentEntityBase>> &attachments) const
{
    auto uploadedJsonToAttachmentLambda = [&](const QJsonValue &attachmentJsonValue) -> bool {
        if (!attachmentJsonValue.isObject()) return false;
        
        QJsonObject attachmentJsonObject{attachmentJsonValue.toObject()};
        
        QString attachmentType{AttachmentDefinerVK::getTypeStringByProcessingWay(attachmentTypeFlag)};
        
        if (attachmentType.isEmpty()) return false;
        
        if (!attachmentJsonObject.contains(C_ATTACHMENT_ID_FIELD_NAME)) return false;
        if (!attachmentJsonObject[C_ATTACHMENT_ID_FIELD_NAME].toVariant().canConvert<AttachmentEntityBase::EntityId>()) return false;
        
        AttachmentEntityBase::EntityId attachmentId{attachmentJsonObject[C_ATTACHMENT_ID_FIELD_NAME].toVariant().toLongLong()};
        
        if (!attachmentJsonObject.contains(C_ATTACHMENT_OWNER_FIELD_NAME)) return false;
        if (!attachmentJsonObject[C_ATTACHMENT_OWNER_FIELD_NAME].toVariant().canConvert<AttachmentEntityBase::EntityId>()) return false;
        
        AttachmentEntityBase::EntityId attachmentPeer{attachmentJsonObject[C_ATTACHMENT_OWNER_FIELD_NAME].toVariant().toLongLong()};
        
        AttachmentEntityVKUsingIdString newAttachment{attachmentId, attachmentPeer, attachmentType};
        
        if (!newAttachment.isValid()) return false;
        
        attachments.push_back(std::make_shared<AttachmentEntityVKUsingIdString>(std::move(newAttachment)));
        
        return true;
    };
    
    if (json.isArray()) {
        QJsonArray attachmentsJsonArray = json.toArray();
        
        foreach (const auto &attachmentJsonValue, attachmentsJsonArray)
            if (!uploadedJsonToAttachmentLambda(attachmentJsonValue)) return false;
        
    } else if (json.isObject()) {
        QJsonObject jsonObject{json.toObject()};
        
        if (!jsonObject.contains(C_ATTACHMENT_TYPE_FIELD_NAME))            return false;
        if (jsonObject[C_ATTACHMENT_TYPE_FIELD_NAME].toString().isEmpty()) return false;
        
        QString typeString{jsonObject[C_ATTACHMENT_TYPE_FIELD_NAME].toString()};
        
        if (!jsonObject.contains(typeString))   return false;
        if (!jsonObject[typeString].isObject()) return false;
        
        if (!uploadedJsonToAttachmentLambda(jsonObject[typeString])) return false;
        
    } else return false;
    
    return true;
}
