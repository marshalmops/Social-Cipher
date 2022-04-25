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
    
    if (!getResponse(reply, jsonResponse)) return false;
    
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
    
    if (!getResponse(reply, jsonResponse)) return false;
    
    return true;
}

void NetworkRequestExecutorImpl::abortCurrentRequest()
{
    m_currentNetworkReply->abort();
}

//void NetworkRequestExecutorImpl::prepare()
//{
//    m_networkManager.moveToThread(QThread::currentThread());
//}

bool NetworkRequestExecutorImpl::getResponse(QNetworkReply *const reply,
                                             QJsonObject &jsonResponse)
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
        
        if (!jsonFromBytes(rawReplyBody, jsonResponse)) return false;
    }
    
    QObject::disconnect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    
    reply->deleteLater();
    
    return true;
}
