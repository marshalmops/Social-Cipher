#include "NetworkRequestExecutorInterface.h"

NetworkRequestExecutorInterface::NetworkRequestExecutorInterface()
{
    
}

bool NetworkRequestExecutorInterface::jsonFromBytes(const QByteArray &bytes,
                                                    QJsonObject &jsonObject)
{
    if (bytes.isEmpty()) return false;
    
    QJsonParseError jsonErr;
    
    auto jsonDocument = QJsonDocument::fromJson(bytes, &jsonErr);
    
    if (jsonErr.error != QJsonParseError::NoError) return false;
    
    jsonObject = jsonDocument.object();
    
    return true;
}
