#include "AttachmentDefinerBase.h"

//const QHash<AttachmentDefinerBase::ContentExtensionId, QString> &AttachmentDefinerBase::getContentExtensionHash()
//{
//    static QHash<AttachmentDefinerBase::ContentExtensionId, QString> contentExtensionIdExtensionStringHash = {
//        {}
//    };
    
//    return contentExtensionIdExtensionStringHash;
//}

const QHash<AttachmentDefinerBase::ContentExtensionId, QString> &AttachmentDefinerBase::getContentExtensionHash()
{
    static QHash<ContentExtensionId, QString> contentExtensionIdExtensionStringHash = {
        {CEI_ENCRYPTED, NetworkSettings::C_ENCODED_ATTACHMENT_EXTENSION},
        {CEI_JPG,       "jpg"},
        {CEI_PNG,       "png"},
        {CEI_GIF,       "gif"},
        {CEI_TXT,       "txt"},
        {CEI_DOC,       "doc"},
        {CEI_DOCX,      "docx"}
    };
    
    return contentExtensionIdExtensionStringHash;
}

QMetaType::Type AttachmentDefinerBase::getMetaTypeByFilename(const QString &filename)
{
    if (filename.isEmpty()) return QMetaType::Type::UnknownType;
    
    QString extension{getExtensionByFilename(filename)};
    
    static const QHash<QString, QMetaType::Type> metaTypeExtensionHash = {
        {QString("jpg"),                                           QMetaType::Type::QImage},
        {QString("png"),                                           QMetaType::Type::QImage},
        {QString("gif"),                                           QMetaType::Type::QImage},
        {QString("txt"),                                           NetworkSettings::C_DOCUMENT_ATTACHMENT_METATYPE},
        {QString("doc"),                                           NetworkSettings::C_DOCUMENT_ATTACHMENT_METATYPE},
        {QString("docx"),                                          NetworkSettings::C_DOCUMENT_ATTACHMENT_METATYPE},
        {QString(NetworkSettings::C_ENCODED_ATTACHMENT_EXTENSION), NetworkSettings::C_ENCODED_ATTACHMENT_METATYPE}
    };
    
    return (metaTypeExtensionHash.contains(extension)
            ? metaTypeExtensionHash[extension]
              : QMetaType::Type::UnknownType);
}

AttachmentDefinerBase::ContentExtensionId AttachmentDefinerBase::getContentExtensionIdByFilename(const QString &filename)
{
    if (filename.isEmpty()) return AttachmentDefinerBase::ContentExtensionId::CEI_INVALID;
    
    return getContentExtensionIdByExtension(getExtensionByFilename(filename));
}

AttachmentDefinerBase::ContentExtensionId AttachmentDefinerBase::getContentExtensionIdByExtension(const QString &extension)
{
    if (extension.isEmpty()) return AttachmentDefinerBase::ContentExtensionId::CEI_INVALID;
    
    const auto &contentExtensionHash{getContentExtensionHash()};
    auto contentExtensionIdIter = std::find(contentExtensionHash.begin(), contentExtensionHash.end(), extension);
    
    return (contentExtensionIdIter ==  contentExtensionHash.end()
            ? AttachmentDefinerBase::ContentExtensionId::CEI_INVALID
            : contentExtensionIdIter.key());
}

const QString AttachmentDefinerBase::getExtensionByContentExtensionId(const ContentExtensionId contentExtensionId)
{
    if (contentExtensionId == AttachmentDefinerBase::ContentExtensionId::CEI_INVALID) return QString{};
    
    const auto &contentExtensionHash{getContentExtensionHash()};
    
    return (contentExtensionHash.contains(contentExtensionId)
            ? contentExtensionHash[contentExtensionId]
            : QString{});
}

const QString AttachmentDefinerBase::getExtensionByFilename(const QString &filename)
{
    if (filename.isEmpty()) return QString{};
    
    QStringList filenameParts{filename.split('.')};
    
    if (filenameParts.length() <= 1) return QString{};
    
    return filenameParts[1];
}
