#ifndef DISCOVERY_CLIENT_H
#define DISCOVERY_CLIENT_H
#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QUrl>
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QNetworkInterface>
/// @brief Discovery client listens for broadcast datagrams with gRPC server URL
class DiscoveryClient : public QObject
{
    Q_OBJECT
public:
    DiscoveryClient(QObject *parent = nullptr) : QObject(parent)
    {
        QObject::connect(&m_socket, &QUdpSocket::readyRead, this, &DiscoveryClient::readDatagram);
    }
public slots:
    void start(int port)
    {
        m_socket.bind(port, QAbstractSocket::ReuseAddressHint);
    }
    void stop()
    {
        m_socket.close();
        m_discoveredUrls.clear();
    }
    QList<QUrl> discoveredUrls() const
    {
        return m_discoveredUrls;
    }
protected slots:
    void readDatagram()
    {
        while (m_socket.hasPendingDatagrams())
        {
            QNetworkDatagram datagram = m_socket.receiveDatagram(1024);
            if (datagram.isValid())
            {
                QUrl url(datagram.data());
                if (url.isValid())
                {
                    if (!m_discoveredUrls.contains(url))
                    {
                        // Try to guess the relevant IP by using the first adrress of the interface that received the datagram
                        // Probaly it makes no sense in this particular case, but...
                        QString clientIP = QNetworkInterface::interfaceFromIndex(datagram.interfaceIndex()).allAddresses().first().toString();
                        m_discoveredUrls.append(url);
                        emit newServerDiscovered(url, clientIP);
                        qDebug() << "New URL" << url << "All URLs" << m_discoveredUrls << "Client IP" << clientIP;
                    }
                }
            }
        }
    }

protected:
    QUdpSocket m_socket;
    QList<QUrl> m_discoveredUrls;
    QUrl m_url;
signals:
    void newServerDiscovered(QUrl url, QString clientIP);
};

#endif