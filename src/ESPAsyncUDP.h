#ifndef ESPASYNCUDP_H
#define ESPASYNCUDP_H

#include "IPAddress.h"
#include "Print.h"

extern "C" {
#include "osapi.h"
#include "ets_sys.h"
#include "user_interface.h"
}
#include <functional>

class AsyncUDP;
class AsyncUDPPacket;
class AsyncUDPMessage;
struct udp_pcb;
struct pbuf;

class AsyncUDPMessage : public Print
{
protected:
    uint8_t *_buffer;
    size_t _index;
    size_t _size;
public:
    AsyncUDPMessage(size_t size=1460);
    ~AsyncUDPMessage();
    size_t write(const uint8_t *data, size_t len);
    size_t write(uint8_t data);
    size_t space();
    uint8_t * data();
    size_t length();
    void flush();
    operator bool()
    {
        return _buffer != NULL;
    }
};

class AsyncUDPPacket : public Print
{
protected:
    AsyncUDP *_udp;
    ip_addr_t *_localIp;
    uint16_t _localPort;
    ip_addr_t *_remoteIp;
    uint16_t _remotePort;
    uint8_t *_data;
    size_t _len;
public:
    AsyncUDPPacket(AsyncUDP *udp, ip_addr_t *localIp, uint16_t localPort, ip_addr_t *remoteIp, uint16_t remotePort, uint8_t *data, size_t len);
    ~AsyncUDPPacket();

    uint8_t * data();
    size_t length();
    bool isBroadcast();
    bool isMulticast();

    IPAddress localIP();
    uint16_t localPort();
    IPAddress remoteIP();
    uint16_t remotePort();

    size_t send(AsyncUDPMessage &message);

    size_t write(const uint8_t *data, size_t len);
    size_t write(uint8_t data);
};

typedef std::function<void(AsyncUDPPacket& packet)> AuPacketHandlerFunction;
typedef std::function<void(void * arg, AsyncUDPPacket& packet)> AuPacketHandlerFunctionWithArg;

class AsyncUDP : public Print
{
protected:
    udp_pcb *_pcb;
    bool _connected;
    AuPacketHandlerFunction _handler;

    void _recv(udp_pcb *upcb, pbuf *pb, ip_addr_t *addr, u16_t port);
    static void _s_recv(void *arg, udp_pcb *upcb, pbuf *p, struct ip_addr *addr, uint16_t port);

public:
    AsyncUDP();
    ~AsyncUDP();

    void onPacket(AuPacketHandlerFunctionWithArg cb, void * arg=NULL);
    void onPacket(AuPacketHandlerFunction cb);

    bool listen(ip_addr_t *addr, uint16_t port);
    bool listen(const IPAddress addr, uint16_t port);
    bool listen(uint16_t port);

    bool listenMulticast(ip_addr_t *addr, uint16_t port, uint8_t ttl=1);
    bool listenMulticast(const IPAddress addr, uint16_t port, uint8_t ttl=1);

    bool connect(ip_addr_t *addr, uint16_t port);
    bool connect(const IPAddress addr, uint16_t port);

    void close();

    size_t writeTo(const uint8_t *data, size_t len, ip_addr_t *addr, uint16_t port);
    size_t writeTo(const uint8_t *data, size_t len, const IPAddress addr, uint16_t port);
    size_t write(const uint8_t *data, size_t len);
    size_t write(uint8_t data);

    size_t broadcastTo(uint8_t *data, size_t len, uint16_t port);
    size_t broadcastTo(const char * data, uint16_t port);
    size_t broadcast(uint8_t *data, size_t len);
    size_t broadcast(const char * data);

    size_t sendTo(AsyncUDPMessage &message, ip_addr_t *addr, uint16_t port);
    size_t sendTo(AsyncUDPMessage &message, const IPAddress addr, uint16_t port);
    size_t send(AsyncUDPMessage &message);

    size_t broadcastTo(AsyncUDPMessage &message, uint16_t port);
    size_t broadcast(AsyncUDPMessage &message);

    bool connected();
    operator bool();
};

#endif
