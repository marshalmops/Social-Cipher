#include "NetworkRequestExecutorImpl.h"

NetworkRequestExecutorImpl::NetworkRequestExecutorImpl(QObject *parent)
    : m_networkManager     {},
      m_currentNetworkReply{nullptr}
{
    if (parent) m_networkManager.moveToThread(parent->thread());
}

bool NetworkRequestExecutorImpl::executeGetRequest(const QUrl &url,
                                                   QJsonObject &jsonResponse,
                                                   const uint32_t timeout)
{
    auto netSettings  = NetworkSettings::getInstance();

    QNetworkRequest request{url};
    
    request.setTransferTimeout(timeout);
    
    auto reply = m_networkManager.get(request);
    
    QByteArray responseBytes;
    
    if (!getResponse(reply, responseBytes)) return false;
    
    jsonResponse = QJsonDocument::fromJson(responseBytes).object();
    
    return true;
}

bool NetworkRequestExecutorImpl::executePostRequest(const QUrl &url,
                                                    const QByteArray &postPayload,
                                                          QJsonObject &jsonResponse,
                                                    const uint32_t timeout)
{
    auto netSettings  = NetworkSettings::getInstance();

    QNetworkRequest request{url};
    
    request.setHeader(QNetworkRequest::ContentTypeHeader, C_DEFAULT_PAYLOAD_CONTENT_TYPE);
    request.setTransferTimeout(timeout);
    
    auto reply = m_networkManager.post(request, postPayload);
    
    QByteArray responseBytes;
    
    if (!getResponse(reply, responseBytes)) return false;
    
    jsonResponse = QJsonDocument::fromJson(responseBytes).object();
    
    return true;
}

bool NetworkRequestExecutorImpl::executePostRequestUsingMultipart(const QUrl &url,
                                                                  QHttpMultiPart *multipartPayload, 
                                                                  QJsonObject &jsonResponse)
{
    auto netSettings  = NetworkSettings::getInstance();

    QNetworkRequest request{url};
    
    auto reply = m_networkManager.post(request, multipartPayload);
    
    QByteArray responseBytes;
    
    if (!getResponse(reply, responseBytes)) return false;
    
    jsonResponse = QJsonDocument::fromJson(responseBytes).object();
    
    return true;
}

bool NetworkRequestExecutorImpl::downloadByLink(const QUrl &url, 
                                                QByteArray &downloadedBytes)
{
    QNetworkRequest request{url};
    
    auto reply = m_networkManager.get(request);
    
    if (!getResponse(reply, downloadedBytes, false)) return false;
    
    return true;
}

void NetworkRequestExecutorImpl::abortCurrentRequest()
{
    m_currentNetworkReply->abort();
}

bool NetworkRequestExecutorImpl::getResponse(QNetworkReply *const reply,
                                             QByteArray &responseBytes,
                                             bool toJson)
{
    m_currentNetworkReply = reply;
    
    QEventLoop eventLoop;
    
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    
    eventLoop.exec();
    
    auto err = reply->error();
    
    if (err != QNetworkReply::NoError) {
        qInfo() << QTime::currentTime() << ": " << err;
        
        if (err != QNetworkReply::OperationCanceledError) return false; // timeout case
        
    } else {
        auto rawReplyBody = reply->readAll();
        
        if (toJson) {
            QJsonObject jsonResponse;
            
            if (!jsonFromBytes(rawReplyBody, jsonResponse))
                return false;
            
            responseBytes = (QJsonDocument{jsonResponse}.toJson());
        } else
            responseBytes = rawReplyBody;
    }
    
    QObject::disconnect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    
    reply->deleteLater();
    
    return true;
}
