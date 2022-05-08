#include "NetworkAttachmentFacadeVK.h"

NetworkAttachmentFacadeVK::NetworkAttachmentFacadeVK(const std::shared_ptr<AttachmentJsonParserInterface> &attachmentParser,
                                                     const std::shared_ptr<NetworkRequestExecutorInterface> &executor)
    : NetworkAttachmentFacadeInterface{attachmentParser, executor}
{
    
}

Error NetworkAttachmentFacadeVK::getAttachmentUrlById(const QString &attachmentIdString,
                                                      const RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag attachmentType,
                                                      QUrl &attachmentUrl)
{
    Error err{"Attachment url getting error!", true};
    
    if (attachmentIdString.isEmpty()) return err;
    
    auto networkSettings = NetworkSettings::getInstance();
    
    QString   requestString{NetworkSettings::getUrlBySocialNetwork(networkSettings->getSocialNetwork())};
    QUrlQuery payload;
    
    switch (attachmentType) {
        case RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_PHOTO_PROCESSING: {
            requestString += QString("/photos.getById");
            
            payload.addQueryItem("photos", attachmentIdString);
            
            break;
        }
        case RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_DOCUMENT_PROCESSING: {
            requestString += QString("/docs.getById");
            
            payload.addQueryItem("docs", attachmentIdString);
            
            break;
        }
        default: return err;
    }
    
    payload.addQueryItem("access_token", QString::fromUtf8(networkSettings->getToken()));
    payload.addQueryItem("v", networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_API_VERSION));
    
    QJsonObject response;
    
    if (!m_requestExecutor->executePostRequest(QUrl(requestString, QUrl::TolerantMode), payload.toString(QUrl::FullyEncoded).toUtf8(), response))
        return (m_lastError = Error{"Getting attachment url error!", true});
    
    if (response.contains("error"))
        return (m_lastError = Error{QString("Getting attachment url error! ") + response["error"].toString(), true});

    if (!response.contains("response"))
        return (m_lastError = Error{"Getting attachment url error!", true});

    MessageEntityBase::AttachmentsList attachments;

    if (!m_attachmentParser->jsonToAttachments(response["response"], RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_STANDARD, attachments))
        return (m_lastError = Error{"Parsing attachment url error!", true});

    if (attachments.size() <= 0) 
        (m_lastError = Error{"Parsing attachment url error!", true});
    
    attachmentUrl = dynamic_cast<AttachmentEntityVKUsingLink*>(attachments.front().get())->getUrl();

    return Error{};
}

Error NetworkAttachmentFacadeVK::downloadAttachment(const std::shared_ptr<AttachmentEntityBase> &attachmentToDownload,
                                                    const RelatedToEntityProcessingBase::EntityProcessingFlagType attachmentDownloadingFlag,
                                                    std::unique_ptr<AttachmentContentBase> &downloadedAttachmentContent)
{
    if (!attachmentToDownload->isValid()) return Error{"Downloading attachment is invalid", true};
    
    Error err{"Downloading error!", true};
    
    // getting downloading url:
    
    AttachmentEntityVK  *attachmentToDownloadVK{dynamic_cast<AttachmentEntityVK*>(attachmentToDownload.get())};
    
    RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag attachmentType         {attachmentDownloadingFlag};
    QUrl                                                          attachmentToDownloadUrl{};
    
    AttachmentEntityVKUsingLink *attachmentToDownloadVKUsingLink{nullptr};
    
    switch (attachmentToDownloadVK->getProcessingFlag()) {
    case RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_EVENTIONAL: {
//        AttachmentEntityVKUsingIdString *attachmentToDownloadVKUsingIdString{dynamic_cast<AttachmentEntityVKUsingIdString*>(attachmentToDownloadVK)};
//        QString                          attachmentTypeIdString             {attachmentToDownloadVKUsingIdString->getIdString()};
        
//        if ((err = getAttachmentUrlById(attachmentTypeIdString, attachmentType, attachmentToDownloadUrl)).isValid())
//            return err;
        
//        break;
        
        return err;
    }
    case RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_STANDARD: {
        attachmentToDownloadVKUsingLink = dynamic_cast<AttachmentEntityVKUsingLink*>(attachmentToDownloadVK);

        attachmentToDownloadUrl = attachmentToDownloadVKUsingLink->getUrl();
        
        break;
    }
    default: return err;
    }
    
    if (attachmentToDownloadUrl.isEmpty())
        return (m_lastError = err);
    
    // actually downloading:
    
    QString    attachmentFileName       {attachmentToDownloadVKUsingLink->getFileName()};//attachmentToDownloadUrl.fileName()};
    QByteArray attachmentDownloadedBytes{};
    
    if (!m_requestExecutor->downloadByLink(attachmentToDownloadUrl, attachmentDownloadedBytes))
        return (m_lastError = err);
    
    if (attachmentDownloadedBytes.isEmpty())
        return (m_lastError = err);
    
    downloadedAttachmentContent = std::make_unique<AttachmentContentBase>(attachmentFileName, attachmentDownloadedBytes);
    
    return Error{};
}

Error NetworkAttachmentFacadeVK::uploadDocument(const std::unique_ptr<AttachmentContentBase> &attachmentContentToUpload,
                                                const RelatedToEntityProcessingBase::EntityProcessingFlagType attachmentUploadingFlag,
                                                std::shared_ptr<AttachmentEntityBase> &uploadedAttachment)
{
    Error err{"Document uploading error!"};
    
    if (attachmentContentToUpload->getByteArray().isEmpty())
        return (m_lastError = err);
    
    // getting uploading server:
    
    auto networkSettings = NetworkSettings::getInstance();
    
    QString   gettingUploadingServerRequestString{NetworkSettings::getUrlBySocialNetwork(networkSettings->getSocialNetwork()) + QString("/docs.getMessagesUploadServer")};
    QUrlQuery gettingUploadingServerPayload;
    
    gettingUploadingServerPayload.addQueryItem("access_token", QString::fromUtf8(networkSettings->getToken()));
    gettingUploadingServerPayload.addQueryItem("v", networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_API_VERSION));
    
    QJsonObject gettingUploadingServerResponse;
    
    if (!m_requestExecutor->executePostRequest(QUrl(gettingUploadingServerRequestString, QUrl::TolerantMode), gettingUploadingServerPayload.toString(QUrl::FullyEncoded).toUtf8(), gettingUploadingServerResponse))
        return (m_lastError = err);
    
    if (gettingUploadingServerResponse.contains("error"))     return (m_lastError = err);
    if (!gettingUploadingServerResponse.contains("response")) return (m_lastError = err);
    
    QJsonObject uploadingServerJsonData{gettingUploadingServerResponse["response"].toObject()};
    
    if (uploadingServerJsonData.isEmpty())               return (m_lastError = err);
    if (!uploadingServerJsonData.contains("upload_url")) return (m_lastError = err);
    
    QUrl uploadingServerUrl{uploadingServerJsonData["upload_url"].toString()};
    
    if (uploadingServerUrl.isEmpty()) return (m_lastError = err);
    
    // uploading to gotten server:
    
    QString uploadingRequest{uploadingServerUrl.toString()};
    QHttpMultiPart uploadingPayload{QHttpMultiPart::FormDataType};
    
    QHttpPart attachmentPayloadPart{};
    
    attachmentPayloadPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"" + attachmentContentToUpload->getFileName() + "\""));
    attachmentPayloadPart.setBody(attachmentContentToUpload->getByteArray());
    
    uploadingPayload.append(attachmentPayloadPart);
    
    QJsonObject uploadingResponse;
    
    if (!m_requestExecutor->executePostRequestUsingMultipart(QUrl(uploadingRequest, QUrl::TolerantMode), &uploadingPayload, uploadingResponse))
        return (m_lastError = err);
    
    if (uploadingResponse.isEmpty())
        return (m_lastError = err);
    
    // saving by new request:
    
    QString savingUploadingRequest{NetworkSettings::getUrlBySocialNetwork(networkSettings->getSocialNetwork())};
    QUrlQuery savingUploadingPayload{};
    
    savingUploadingRequest += "/docs.save";
    
    if (!uploadingResponse.contains("file"))            return (m_lastError = err);
    if (uploadingResponse["file"].toString().isEmpty()) return (m_lastError = err);
     
    savingUploadingPayload.addQueryItem("file", uploadingResponse["file"].toString());
    savingUploadingPayload.addQueryItem("access_token", QString::fromUtf8(networkSettings->getToken()));
    savingUploadingPayload.addQueryItem("v", networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_API_VERSION));
    
    QJsonObject savingUploadedServerResponse;
    
    if (!m_requestExecutor->executePostRequest(QUrl(savingUploadingRequest, QUrl::TolerantMode), savingUploadingPayload.toString(QUrl::FullyEncoded).toUtf8(), savingUploadedServerResponse))
        return (m_lastError = err);
    
    if (savingUploadedServerResponse.contains("error"))     return (m_lastError = err);
    if (!savingUploadedServerResponse.contains("response")) return (m_lastError = err);
    
    std::vector<std::shared_ptr<AttachmentEntityBase>> uploadedAttachments{};
    
    if (!m_attachmentParser->jsonToAttachments(savingUploadedServerResponse["response"], RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_UPLOADED + RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_DOCUMENT_PROCESSING, uploadedAttachments))
        return (m_lastError = err);
    
    if (uploadedAttachments.empty()) return (m_lastError = err);
    
    uploadedAttachment = std::move(uploadedAttachments.front());
    
    return Error{};
}

Error NetworkAttachmentFacadeVK::uploadImage(const std::unique_ptr<AttachmentContentBase> &attachmentContentToUpload,
                                             const RelatedToEntityProcessingBase::EntityProcessingFlagType attachmentUploadingFlag,
                                             std::shared_ptr<AttachmentEntityBase> &uploadedAttachment)
{
    Error err{"Image uploading error!"};
    
    if (attachmentContentToUpload->getByteArray().isEmpty())
        return (m_lastError = err);
    
    // getting uploading server:
    
    auto networkSettings = NetworkSettings::getInstance();
    
    QString   gettingUploadingServerRequestString{NetworkSettings::getUrlBySocialNetwork(networkSettings->getSocialNetwork()) + QString("/photos.getMessagesUploadServer")};
    QUrlQuery gettingUploadingServerPayload;
    
    gettingUploadingServerPayload.addQueryItem("access_token", QString::fromUtf8(networkSettings->getToken()));
    gettingUploadingServerPayload.addQueryItem("v", networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_API_VERSION));
    
    QJsonObject gettingUploadingServerResponse;
    
    if (!m_requestExecutor->executePostRequest(QUrl(gettingUploadingServerRequestString, QUrl::TolerantMode), gettingUploadingServerPayload.toString(QUrl::FullyEncoded).toUtf8(), gettingUploadingServerResponse))
        return (m_lastError = err);
    
    if (gettingUploadingServerResponse.contains("error"))     return (m_lastError = err);
    if (!gettingUploadingServerResponse.contains("response")) return (m_lastError = err);
    
    QJsonObject uploadingServerJsonData{gettingUploadingServerResponse["response"].toObject()};
    
    if (uploadingServerJsonData.isEmpty())               return (m_lastError = err);
    if (!uploadingServerJsonData.contains("upload_url")) return (m_lastError = err);
    
    QUrl uploadingServerUrl{uploadingServerJsonData["upload_url"].toString()};
    
    if (uploadingServerUrl.isEmpty()) return (m_lastError = err);
    
    // uploading to gotten server:
    
    QString uploadingRequest{uploadingServerUrl.toString()};
    QHttpMultiPart uploadingPayload{QHttpMultiPart::FormDataType};

    QHttpPart attachmentPayloadPart{};
    
    switch (AttachmentDefinerVK::getContentExtensionIdByExtension(AttachmentDefinerVK::getExtensionByFilename(attachmentContentToUpload->getFileName()))) {
        case AttachmentDefinerVK::ContentExtensionId::CEI_JPG: {
            attachmentPayloadPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
            
            break;
        }
        case AttachmentDefinerVK::ContentExtensionId::CEI_PNG: {
            attachmentPayloadPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
            
            break;
        }
        case AttachmentDefinerVK::ContentExtensionId::CEI_GIF: {
            attachmentPayloadPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/gif"));
            
            break;
        }
        
        default: return err;
    }
    
    QDataStream imageStream{(attachmentContentToUpload->getByteArray())};
    
    attachmentPayloadPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"photo\"; filename=\"" + attachmentContentToUpload->getFileName() + "\""));
    attachmentPayloadPart.setBodyDevice(imageStream.device());
    
    uploadingPayload.append(attachmentPayloadPart);
    
    QJsonObject uploadingResponse;
    
    if (!m_requestExecutor->executePostRequestUsingMultipart(QUrl(uploadingRequest, QUrl::TolerantMode), &uploadingPayload, uploadingResponse))
        return (m_lastError = err);
    
    if (uploadingResponse.isEmpty())
        return (m_lastError = err);
    
    if (!uploadingResponse.contains("photo")
     || !uploadingResponse.contains("server")
     || !uploadingResponse.contains("hash"))
    {
        return (m_lastError = err);
    }
    
    if (!uploadingResponse["server"].toVariant().canConvert<uint64_t>()
     || !uploadingResponse["photo"].isString()
     || !uploadingResponse["hash"].isString())
    {
        return (m_lastError = err);
    }
    
    // saving by new request:
    
    QString savingUploadingRequest{NetworkSettings::getUrlBySocialNetwork(networkSettings->getSocialNetwork()) +  "/photos.saveMessagesPhoto"};
    QUrlQuery savingUploadingPayload{};
    
    savingUploadingPayload.addQueryItem("photo",        uploadingResponse["photo"].toString());
    savingUploadingPayload.addQueryItem("server",       QString::number(uploadingResponse["server"].toVariant().toULongLong()));
    savingUploadingPayload.addQueryItem("hash",         uploadingResponse["hash"].toString());
    savingUploadingPayload.addQueryItem("access_token", QString::fromUtf8(networkSettings->getToken()));
    savingUploadingPayload.addQueryItem("v",            networkSettings->getAdditionalPropValue(NetworkFacadeVK::C_API_VERSION));
    
    QJsonObject savingUploadedServerResponse;
    
    if (!m_requestExecutor->executePostRequest(QUrl(savingUploadingRequest, QUrl::TolerantMode), savingUploadingPayload.toString(QUrl::FullyEncoded).toUtf8(), savingUploadedServerResponse))
        return (m_lastError = err);
    
    if (savingUploadedServerResponse.contains("error"))     return (m_lastError = err);
    if (!savingUploadedServerResponse.contains("response")) return (m_lastError = err);
    
    MessageEntityBase::AttachmentsList uploadedAttachments{};
    
    if (!m_attachmentParser->jsonToAttachments(savingUploadedServerResponse["response"], RelatedToAttachmentsProcessingVK::AttachmentOriginFlag::AOF_UPLOADED + RelatedToAttachmentsProcessingVK::AttachmentProcessingWayFlag::AUF_PHOTO_PROCESSING, uploadedAttachments))
        return (m_lastError = err);
    
    if (uploadedAttachments.empty()) return (m_lastError = err);
    
    uploadedAttachment = uploadedAttachments.front();
    
    return Error{};
}
