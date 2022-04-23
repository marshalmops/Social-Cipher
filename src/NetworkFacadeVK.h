#ifndef NETWORKFACADEVK_H
#define NETWORKFACADEVK_H


class NetworkFacadeVK
{
public:
    constexpr static const char* C_LONGPOLL_SERVER_PROP_NAME  = "longPollServer";
    constexpr static const char* C_LONGPOLL_KEY_PROP_NAME     = "longPollKey";
    constexpr static const char* C_LONGPOLL_WAIT_PROP_NAME    = "longPollWait";
    constexpr static const char* C_LONGPOLL_MODE_PROP_NAME    = "longPollMode";
    constexpr static const char* C_LONGPOLL_VERSION_PROP_NAME = "longPollVersion";
    constexpr static const char* C_LONGPOLL_TS_PROP_NAME      = "ts";
    
    constexpr static const char* C_API_VERSION = "v";
    
    NetworkFacadeVK() = default;
};

#endif // NETWORKFACADEVK_H
