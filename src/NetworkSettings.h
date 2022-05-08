#ifndef NETWORKSETTINGS_H
#define NETWORKSETTINGS_H

#include <QByteArray>
#include <QHash>
#include <mutex>

#include "SettingsBase.h"

class NetworkSettings : public SettingsBase
{
    NetworkSettings();
    
public:
    constexpr static const char* C_FILENAME = "networkSettings.ini";
    
    constexpr static const char* C_SOCIAL_NETWORK_PROP_NAME = "socialNetwork";
    constexpr static const char* C_AUTH_TOKEN_PROP_NAME     = "authToken";
    constexpr static const char* C_LOCAL_PEER_ID_PROP_NAME  = "localPeerId";
    
    constexpr static const char*           C_ENCODED_ATTACHMENT_EXTENSION = "enc";
    constexpr static const QMetaType::Type C_ENCODED_ATTACHMENT_METATYPE  = QMetaType::Type::QByteArray;
    constexpr static const QMetaType::Type C_DOCUMENT_ATTACHMENT_METATYPE = static_cast<QMetaType::Type>(QMetaType::Type::User + 1);
    
    using SocialNetworkType = uint8_t;
    using AuthToken         = QByteArray;
    using PeerId            = int64_t;
    using AdditionalProps   = QHash<QString, QString>;
    
    enum CheckingType : uint8_t {
        CT_INVALID = 0,
        CT_TRADITIONAL,
        CT_EVENTIONAL
    };
    
    enum class SocialNetwork : SocialNetworkType {
        SNT_INVALID = 0,
        SNT_VK
    };
    
    NetworkSettings(NetworkSettings &&);
    
    static const CheckingType getCheckingTypeBySocialNetwork(const SocialNetwork socialNetwork);
    static const QString      getUrlBySocialNetwork         (const SocialNetwork socialNetwork);
    
    static std::shared_ptr<NetworkSettings> getInstance();
    
    SocialNetwork getSocialNetwork() const;
    void          setSocialNetwork(const SocialNetwork socialNetwork);
    AuthToken getToken() const;
    bool      setToken(const AuthToken &token);
    PeerId    getLocalPeerId() const;
    void      setLocalPeerId(const PeerId peerId);
    
    QString getAdditionalPropValue(const QString &propName) const;
    void    setAdditionalPropValue(const QString &propName, 
                                   const QString &propValue);
    
    bool isNetworkLayerCreated     () const;
    void setNetworkLayerCreatedFlag();
    
    virtual QString getSettingsPath() const override;
    
    virtual bool save       () const override;
    virtual bool load       ()       override;
    virtual void setDefaults()       override;
    
    void clearAdditionalProps();
    
private:
    static std::shared_ptr<NetworkSettings> m_settings;
    
    SocialNetwork m_socialNetwork;
    AuthToken     m_token;
    PeerId        m_localPeerId;
    
    AdditionalProps m_additionalProps;
    
    mutable std::mutex m_mutex;
    
    bool m_isNetworkLayerCreated;
};

#endif // NETWORKSETTINGS_H
