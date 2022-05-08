#ifndef NETWORKREQUESTEXECUTORINTERFACE_H
#define NETWORKREQUESTEXECUTORINTERFACE_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QUrl>
#include <QHttpMultiPart>

#include "NetworkSettings.h"

class NetworkRequestExecutorInterface
{
public:
    constexpr static const char*    C_DEFAULT_PAYLOAD_CONTENT_TYPE = "application/x-www-form-urlencoded";
    constexpr static const char*    C_DEFAULT_UPLOAD_CONTENT_TYPE  = "multipart/form-data";
    constexpr static const uint32_t C_DEFAULT_TIMEOUT              = 30000;
    
    NetworkRequestExecutorInterface();
    
    virtual bool executeGetRequest(const QUrl &url,
                                   QJsonObject &jsonResponse,
                                   const uint32_t timeout = C_DEFAULT_TIMEOUT) = 0;
    
    virtual bool executePostRequest(const QUrl &url,
                                    const QByteArray &postPayload,
                                    QJsonObject &jsonResponse,
                                    const uint32_t timeout = C_DEFAULT_TIMEOUT) = 0;
    
    virtual bool executePostRequestUsingMultipart(const QUrl &url,
                                                  QHttpMultiPart *multipartPayload,
                                                  QJsonObject &jsonResponse) = 0;
    
    virtual bool downloadByLink(const QUrl &url,
                                QByteArray &downloadedBytes) = 0;
    
    virtual void abortCurrentRequest() = 0;
    
//    virtual void prepare() = 0;
    
protected:
    bool jsonFromBytes(const QByteArray &bytes,
                       QJsonObject &jsonObject);
};

#endif // NETWORKREQUESTEXECUTORINTERFACE_H
