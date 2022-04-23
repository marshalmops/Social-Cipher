#include "NetworkSettings.h"

std::shared_ptr<NetworkSettings> NetworkSettings::m_settings = std::make_shared<NetworkSettings>(NetworkSettings());

const QString NetworkSettings::getUrlBySocialNetwork(const SocialNetwork socialNetwork)
{
    if (socialNetwork == SocialNetwork::SNT_INVALID) return QString();
    
    static const QHash<SocialNetwork, QString> socialNetworksUrls = {
        {SocialNetwork::SNT_VK, "https://api.vk.com/method"}
    };
    
    return socialNetworksUrls[socialNetwork];
}

std::shared_ptr<NetworkSettings> NetworkSettings::getInstance()
{
    return m_settings;
}

NetworkSettings::SocialNetwork NetworkSettings::getSocialNetwork() const
{
    return m_socialNetwork;
}

void NetworkSettings::setSocialNetwork(const SocialNetwork socialNetwork)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    
    m_socialNetwork = socialNetwork;
}

NetworkSettings::AuthToken NetworkSettings::getToken() const
{
    return m_token;
}

bool NetworkSettings::setToken(const AuthToken &token)
{
    if (token.isEmpty()) return false;
    
    std::unique_lock<std::mutex> lock(m_mutex);
    
    m_token = token;
    
    return true;
}

NetworkSettings::PeerId NetworkSettings::getLocalPeerId() const
{
    return m_localPeerId;
}

void NetworkSettings::setLocalPeerId(const PeerId peerId)
{
    m_localPeerId = peerId;
}

QString NetworkSettings::getAdditionalPropValue(const QString &propName) const
{
    std::unique_lock<std::mutex> lock(m_mutex);
    
    return m_additionalProps[propName];
}

void NetworkSettings::setAdditionalPropValue(const QString &propName, 
                                             const QString &propValue)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    
    m_additionalProps[propName] = propValue;
}

bool NetworkSettings::isNetworkLayerCreated() const
{
    return m_isNetworkLayerCreated;
}

void NetworkSettings::setNetworkLayerCreatedFlag()
{
    m_isNetworkLayerCreated = true;
}

QString NetworkSettings::getSettingsPath() const
{
    return getSettingsDir() + '/' + C_FILENAME;
}

bool NetworkSettings::save() const
{
//    auto settingsPath = getSettingsPath();
    
//    if (settingsPath.isEmpty()) return false;
    
//    QSettings settings{settingsPath, QSettings::NativeFormat};
    
    std::unique_lock<std::mutex> lock(m_mutex);
    
    std::unique_ptr<QSettings> settings;
    
    if (!getQtSettingsObject(settings)) return false;
    
    settings->setValue(C_SOCIAL_NETWORK_PROP_NAME, static_cast<SocialNetworkType>(m_socialNetwork));
    settings->setValue(C_AUTH_TOKEN_PROP_NAME,     m_token);
    settings->setValue(C_LOCAL_PEER_ID_PROP_NAME,  m_localPeerId);
    
//    foreach (const auto &propName, m_additionalProps)
//        settings.setValue(propName, m_additionalProps[propName]);
    
    settings->sync();
    
    return true;
}

bool NetworkSettings::load()
{
//    auto settingsPath = getSettingsPath();
    
//    if (settingsPath.isEmpty()) return false;
    
//    QSettings settings{settingsPath, QSettings::NativeFormat};
    
    std::unique_ptr<QSettings> settings;
    
    if (!getQtSettingsObject(settings)) return false;
    
    SocialNetwork socialNetworkBuffer;
    AuthToken     authTokenBuffer;
    PeerId        localPeerIdBuffer;
    
    bool isLocalPeerIdConvOK{false};
    
    socialNetworkBuffer = static_cast<SocialNetwork>(settings->value(C_SOCIAL_NETWORK_PROP_NAME).toUInt());
    authTokenBuffer     = settings->value(C_AUTH_TOKEN_PROP_NAME).toByteArray();
    localPeerIdBuffer   = settings->value(C_LOCAL_PEER_ID_PROP_NAME).toLongLong(&isLocalPeerIdConvOK);
    
    if (socialNetworkBuffer == SocialNetwork::SNT_INVALID
     || authTokenBuffer.isEmpty()
     || !isLocalPeerIdConvOK)
    {
        return false;
    }
    
    m_socialNetwork = socialNetworkBuffer;
    m_token         = authTokenBuffer;
    m_localPeerId   = localPeerIdBuffer;
    
    return true;
}

void NetworkSettings::setDefaults()
{
    m_token         = AuthToken{};
    m_socialNetwork = SocialNetwork::SNT_VK;
}

//bool NetworkSettings::fromFile()
//{
//    QFile settingsFile(C_FILENAME);
    
//    if (!settingsFile.open(QFile::OpenModeFlag::ReadOnly))
//        return false;
    
//    // main props reading:
    
//    QByteArray readingBuffer;
//    auto       socialNetworkTypeLength {sizeof(SocialNetworkType)};
    
//    if (!settingsFile.read(readingBuffer.data(), socialNetworkTypeLength))
//        return false;
    
//    SocialNetworkType socialNetwork;
    
//    if (!numbFromChars(readingBuffer.data(), socialNetwork))
//        return false;
  
//    readingBuffer.clear();
    
//    AuthToken token;
    
//    if ((token = settingsFile.readLine()).isEmpty())
//        return false;
    
//    readingBuffer.clear();
    
//    // additional props reading:
    
//    AdditionalProps additionalProps;
    
//    while (settingsFile.bytesAvailable() > 0) {
//        readingBuffer = settingsFile.readLine();
        
//        auto dividerPos = readingBuffer.indexOf(C_PROPS_DIVIDER);
        
//        if (dividerPos <= 0) return false;
        
//        QString propName {readingBuffer.mid(0, dividerPos)},
//                propValue{readingBuffer.mid(dividerPos + 1)};
        
//        additionalProps[propName] = propValue;
        
//        readingBuffer.clear();
//    }
    
//    // final assignment:
    
//    m_socialNetwork   = static_cast<SocialNetwork>(socialNetwork);
//    m_token           = std::move(token);
//    m_additionalProps = std::move(additionalProps);
    
//    return true;
//}

//bool NetworkSettings::toFile() const
//{
//    std::unique_lock<std::mutex> lock(m_mutex);
    
//    QFile settingsFile(C_FILENAME);
    
//    if (!settingsFile.open(QFile::OpenModeFlag::WriteOnly | QFile::OpenModeFlag::ExistingOnly))
//        return false;
    
//    std::unique_ptr<char> socialNetworkTypeChars;
    
//    if (!numbToChars(static_cast<SocialNetworkType>(m_socialNetwork), socialNetworkTypeChars))
//        return false;
    
//    if (socialNetworkTypeChars.get()) return false;
    
//    settingsFile.write(socialNetworkTypeChars.get());
//    settingsFile.write(m_token + C_ROWS_DIVIDER);
    
//    foreach (const auto &propName, m_additionalProps) {
//        QByteArray propBytes;
        
//        propBytes += propName; 
//        propBytes += C_PROPS_DIVIDER;
//        propBytes += m_additionalProps[propName];
        
//        settingsFile.write(propBytes + C_ROWS_DIVIDER);
//    }
    
//    return true;
//}

void NetworkSettings::clearAdditionalProps()
{
    m_additionalProps.clear();
}

NetworkSettings::NetworkSettings()
    : m_socialNetwork        {SocialNetwork::SNT_INVALID},
      m_token                {},
      m_isNetworkLayerCreated{false}
{
    
}

NetworkSettings::NetworkSettings(NetworkSettings &&networkSettings)
    : m_mutex{}
{
    m_token                 = networkSettings.m_token;
    m_socialNetwork         = networkSettings.m_socialNetwork;
    m_additionalProps       = std::move(networkSettings.m_additionalProps);
    m_isNetworkLayerCreated = networkSettings.m_isNetworkLayerCreated;
}

const NetworkSettings::CheckingType NetworkSettings::getCheckingTypeBySocialNetwork(const SocialNetwork socialNetwork)
{
    if (socialNetwork == SocialNetwork::SNT_INVALID) return CheckingType::CT_INVALID;
    
    static const QHash<SocialNetwork, CheckingType> socialNetworksCheckerTypes = {
        {SocialNetwork::SNT_VK, CheckingType::CT_EVENTIONAL}
    };
    
    return socialNetworksCheckerTypes[socialNetwork];
}
