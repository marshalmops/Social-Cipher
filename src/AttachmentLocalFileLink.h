#ifndef ATTACHMENTLOCALFILELINK_H
#define ATTACHMENTLOCALFILELINK_H

#include <QUrl>
#include <QMetaType>

class AttachmentLocalFileLink
{
    Q_GADGET
    Q_PROPERTY(QString localFileLink READ getLocalFilePathString)
    Q_PROPERTY(QMetaType::Type metaType READ getMetaType)
    
public:
    AttachmentLocalFileLink() = default;
    AttachmentLocalFileLink(const QUrl &fileLink,
                            const QMetaType::Type metaType);
    AttachmentLocalFileLink(const AttachmentLocalFileLink &) = default;
    
    QUrl            getLocalFileLink() const;
    QMetaType::Type getMetaType     () const;
    
    QString getLocalFilePathString() const;
    
    bool isValid() const;
    
private:
    QUrl            m_localFileLink;
    QMetaType::Type m_metaType;
};

Q_DECLARE_METATYPE(AttachmentLocalFileLink)

#endif // ATTACHMENTLOCALFILELINK_H
