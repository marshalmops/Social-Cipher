#ifndef NETWORKREQUESTEXECUTORIMPL_H
#define NETWORKREQUESTEXECUTORIMPL_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QThread>

#include "NetworkRequestExecutorInterface.h"

class NetworkRequestExecutorImpl : public NetworkRequestExecutorInterface
{
public:
    NetworkRequestExecutorImpl(QObject *parent = nullptr);
    
    virtual bool executeGetRequest(const QUrl &url,
                                   QJsonObject &jsonResponse,
                                   const uint32_t timeout) override;
    
    virtual bool executePostRequest(const QUrl &url,
                                    const QByteArray &postPayload,
                                    QJsonObject &jsonResponse,
                                    const uint32_t timeout) override;
    
    virtual bool executePostRequestUsingMultipart(const QUrl &url,
                                                  QHttpMultiPart *multipartPayload,
                                                  QJsonObject &jsonResponse) override;
    
    
    virtual bool downloadByLink(const QUrl &url, 
                                QByteArray &downloadedBytes) override;
    
    virtual void abortCurrentRequest() override;
//    virtual void prepare() override;
    
protected:
    bool getResponse(QNetworkReply * const reply,
                     QByteArray &jsonResponse,
                     bool toJson = true);
    
private:
    QNetworkAccessManager m_networkManager;
    
    QNetworkReply *m_currentNetworkReply;
};

#endif // NETWORKREQUESTEXECUTORIMPL_H
