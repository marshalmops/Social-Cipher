#include "AttachmentManagerBase.h"

AttachmentManagerBase::AttachmentManagerBase(std::unique_ptr<NetworkAttachmentFacadeInterface> &attachmentsFacade)
    : m_attachmentsFacade{attachmentsFacade.release()}
{
    
}

Error AttachmentManagerBase::changeAttachmentContentById(const EntityInterface::EntityId attachmentId,
                                                         const std::unique_ptr<AttachmentContentBase> &attachmentContent,
                                                         std::unique_ptr<AttachmentLocalFileLink> &changedAttachment)
{
    Error err{"Requested attachment content hasn't be found!"};
    
    if (attachmentContent->isValid() || !m_attachmentsHash.contains(attachmentId))
        return err;
    
    // getting attachment link:
    
    auto attachmentLocalFileLink{m_attachmentsHash[attachmentId]};
    
    QFile file{attachmentLocalFileLink.getLocalFileLink().path()};
    
    if (!file.remove()) 
        return Error{"Cannot delete a previous attachment file!", true};
    
    if ((err = storeAttachmentContentInFileById(attachmentId, attachmentContent, changedAttachment)).isValid())
        return err;
    
    return Error{};
}

Error AttachmentManagerBase::getAttachmentBytesByLocalPath(const QUrl &filePath,
                                                           QByteArray &fileBytes)
{
    Error err{"Cannot load attachment from local file!", true};
    
    if (filePath.isEmpty()) return err;
    
    QFile fileToLoad{filePath.path()};
    
    if (!fileToLoad.open(QIODevice::ReadOnly)) return err;
    
    QByteArray readBytes{fileToLoad.readAll()};
    
    if (readBytes.isEmpty()) return err;
    
    fileBytes = std::move(readBytes);
    
    return Error{};
}

Error AttachmentManagerBase::getAttachmentContentByLocalPath(const QUrl &filePath,
                                                             std::unique_ptr<AttachmentContentBase> &attachmentContent)
{
    if (AttachmentDefinerBase::getMetaTypeByFilename(filePath.fileName()) == QMetaType::Type::UnknownType)
        return Error{"Invalid attachment extension!"};
    
    Error err{"Cannot get attachment content from " + filePath.path()};
    
    if (filePath.isEmpty()) return err;
    
    QFile fileToLoad{filePath.path()};
    
    if (!fileToLoad.open(QIODevice::ReadOnly)) return err;
    
    QByteArray readBytes{fileToLoad.readAll()};
    
    if (readBytes.isEmpty()) return err;
    
    AttachmentContentBase newAttachmentContent{filePath.fileName(),
                                               std::move(readBytes)};
    
    if (!newAttachmentContent.isValid()) return err;
    
    attachmentContent = std::make_unique<AttachmentContentBase>(std::move(newAttachmentContent));
    
    return Error{};
}

Error AttachmentManagerBase::storeAttachmentContentInFileById(const EntityInterface::EntityId attachmentId,
                                                              const std::unique_ptr<AttachmentContentBase> &attachmentContentToStore, 
                                                              std::unique_ptr<AttachmentLocalFileLink> &storedAttachment)
{
    Error err{"Cannot store attachment content!", true};
    
    if (!attachmentContentToStore->isValid()) return err;
    
    QString newFilePath{QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/" + attachmentContentToStore->getFileName()};
    
    QFile newFile{newFilePath};
    
    if (!newFile.open(QIODevice::WriteOnly))                         return err;
    if (newFile.write(attachmentContentToStore->getByteArray()) < 0) return err;
    
    newFile.close();
    
    QUrl localFileUrl{};
    
    localFileUrl.setPath(newFilePath);
    
    AttachmentLocalFileLink newAttachmentFileLocalLink{localFileUrl, AttachmentDefinerBase::getMetaTypeByFilename(localFileUrl.fileName())};
    
    if (!newAttachmentFileLocalLink.isValid()) return err;
    
    m_attachmentsHash[attachmentId] = newAttachmentFileLocalLink;
    
    storedAttachment = std::make_unique<AttachmentLocalFileLink>(std::move(newAttachmentFileLocalLink));
    
    return Error{};
}

Error AttachmentManagerBase::storeAttachmentLocalFileUrlById(const EntityInterface::EntityId attachmentId,
                                                             const QUrl &attachmentLocalUrl,
                                                             std::unique_ptr<AttachmentLocalFileLink> &storedAttachment)
{
    Error err{"Cannot store attachment by local file!", true};
    
    if (attachmentLocalUrl.isEmpty()) return err;
    
    AttachmentLocalFileLink newAttachmentFileLocalLink{attachmentLocalUrl, AttachmentDefinerBase::getMetaTypeByFilename(attachmentLocalUrl.fileName())};
    
    if (!newAttachmentFileLocalLink.isValid()) return err;
    
    m_attachmentsHash[attachmentId] = newAttachmentFileLocalLink;
    
    storedAttachment = std::make_unique<AttachmentLocalFileLink>(std::move(newAttachmentFileLocalLink));
    
    return Error{};
}

Error AttachmentManagerBase::loadAttachmentContentById(const EntityInterface::EntityId attachmentId,
                                                       std::unique_ptr<AttachmentContentBase> &storedAttachment)
{
    Error err{"File related to provided ID cannot be loaded!", true};
    
    if (!m_attachmentsHash.contains(attachmentId)) return err;
    
    const auto &attachmentLocalFileLink{m_attachmentsHash[attachmentId]};
    
    QFile attachmentLocalFile{attachmentLocalFileLink.getLocalFileLink().path()};
    QByteArray attachmentBytes{};
    
    if (!attachmentLocalFile.open(QIODevice::ReadOnly))              return err;
    if ((attachmentBytes = attachmentLocalFile.readAll()).isEmpty()) return err;
    
    AttachmentContentBase newAttachmentContent{attachmentLocalFileLink.getLocalFileLink().fileName(),
                                              std::move(attachmentBytes)};
    
    if (!newAttachmentContent.isValid()) return err;
    
    storedAttachment = std::make_unique<AttachmentContentBase>(std::move(newAttachmentContent));
    
    return Error{};
}

Error AttachmentManagerBase::getAttachmentLocalFileLinkById(const EntityInterface::EntityId attachmentId,
                                                            std::unique_ptr<AttachmentLocalFileLink> &storedAttachment)
{
    Error err{"Requested local file cannot be found!", true};
    
    if (!m_attachmentsHash.contains(attachmentId)) return err;
    
    storedAttachment = std::make_unique<AttachmentLocalFileLink>(m_attachmentsHash[attachmentId]);
    
    return Error{};
}
