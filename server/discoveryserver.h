#ifndef DISCOVERY_SERVER_H
#define DISCOVERY_SERVER_H
#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QUrl>
#include <QElapsedTimer>

/// @brief The discovery server broadcasts datagrams with gRPC server adress. Advertisement is suspended if there are active clients.
class DiscoveryServer : public QObject
{
    Q_OBJECT
public:
    DiscoveryServer(QObject *parent = nullptr) : QObject(parent)
    {
        m_broadcastTimer.setInterval(m_broadcastInterval_ms);
        QObject::connect(&m_broadcastTimer, &QTimer::timeout, this, &DiscoveryServer::broadcast);
        m_broadcastTimer.setSingleShot(true);

        m_restartTimer.setInterval(m_suspendTime_ms);
        QObject::connect(&m_restartTimer, &QTimer::timeout, this, &DiscoveryServer::broadcast);
        m_restartTimer.setSingleShot(true);
    }
public slots:
    void start(int port, QUrl advertisementUrl)
    {
        m_port = port;
        m_url = advertisementUrl;
        broadcast();
    }
    void stop()
    {
        m_broadcastTimer.stop();
        m_restartTimer.stop();
    }
    void suspend()
    {
        //Stop broadcasting and resume after m_suspendTime_ms (if not called again)
        m_broadcastTimer.stop();
        m_restartTimer.start();
    }
protected slots:
    void broadcast()
    {
        emit log("Broadcasting");
        m_socket.writeDatagram(m_url.toString().toLocal8Bit(), QHostAddress::Broadcast, m_port);
        m_broadcastTimer.start();
    }

protected:
    QUdpSocket m_socket;
    QTimer m_broadcastTimer;
    int m_port{10001};
    int m_suspendTime_ms{15000};
    int m_broadcastInterval_ms{1000};
    QTimer m_restartTimer;
    QUrl m_url;
signals:
    void log(QString);
};

#endif