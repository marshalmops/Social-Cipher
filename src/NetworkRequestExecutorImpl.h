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
//    virtual void prepare() override;
    
protected:
    bool getResponse(QNetworkReply * const reply,
                     QJsonObject &jsonResponse);
    
private:
    QNetworkAccessManager m_networkManager;
    
};

#endif // NETWORKREQUESTEXECUTORIMPL_H
