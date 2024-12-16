#ifndef GRPCCLIENT_H
#define GRPCCLIENT_H
#include <QObject>
#include "api_client.grpc.qpb.h"
#include <QGrpcCallReply>
#include <QGrpcHttp2Channel>
#include <QTimer>
#include <QThread>
#include <QNetworkInterface>
#include <QDateTime>
/// @brief Implements MaintainingApi::Ping client
class GRPCClient : public QObject
{
    Q_OBJECT
    //gRPC service status 
    Q_PROPERTY(bool online READ online NOTIFY onlineChanged)
    //gRPC server URL
    Q_PROPERTY(QString serverUrl MEMBER m_serverUrl CONSTANT)
    //Client state
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    //List ping timestamp
    Q_PROPERTY(QString timestamp READ timestamp NOTIFY timestampChanged)

public:
    explicit GRPCClient(QUrl url, QString clinetIP, QObject *parent = nullptr);
    bool online() const { return m_online; }
    bool running() const { return m_running; }
    QString timestamp() const { return m_timestamp.toLocalTime().toString("hh:mm:ss"); }

public slots:
    void setRunning(bool on);
    void processResponse(const QGrpcStatus &status);
    void ping();

protected:
    void updateStatus(bool online);
    std::shared_ptr<QGrpcHttp2Channel> m_channel;
    MaintainingApi::Client m_client;
    std::unique_ptr<QGrpcCallReply> m_reply;
    QString m_clientIP;
    QDateTime m_timestamp;
    bool m_running{false};
    QString m_serverUrl;
    bool m_online{false};
    int m_failureCounter{0};
    int m_failureCountLimit{3};
    int m_pingInterval_ms{5000};
    QTimer *m_timer;

signals:
    void onlineChanged(bool);
    void runningChanged(bool);
    void timestampChanged();
};
#endif