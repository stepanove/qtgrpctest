#include "grpcclient.h"

GRPCClient::GRPCClient(QUrl url, QString clinetIP, QObject *parent) : QObject(parent)
{
    m_clientIP = clinetIP;
    m_serverUrl = url.toString();
    m_channel = std::make_shared<QGrpcHttp2Channel>(url);
    m_client.attachChannel(m_channel);
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(m_pingInterval_ms);
    QObject::connect(m_timer, &QTimer::timeout, this, &GRPCClient::ping);
}

void GRPCClient::setRunning(bool on)
{
    if (m_running == on)
        return;
    m_running = on;
    m_failureCounter = 0;
    on ? ping() : m_timer->stop();
    emit runningChanged(m_running);
}

void GRPCClient::processResponse(const QGrpcStatus &status)
{
    if (status.isOk())
    {
        m_failureCounter = 0;
        updateStatus(true);
    }
    else
    {
        if (++m_failureCounter >= m_failureCountLimit)
        {
            updateStatus(false);
            setRunning(false);
        }
    }
}

void GRPCClient::ping()
{
    if (m_reply && !m_reply.get()->isFinished())
    {
        qDebug() << Q_FUNC_INFO << "Previous operation hasn't finished yet, this should be handled somehow";
    }
    PingRequest request;
    request.setClientIp(m_clientIP);
    m_timestamp = QDateTime::currentDateTime();
    emit timestampChanged();
    m_reply = m_client.Ping(request);
    QObject::connect(m_reply.get(), &QGrpcCallReply::finished, this, &GRPCClient::processResponse);
    m_timer->start();
}
void GRPCClient::updateStatus(bool online)
{
    if (m_online == online)
        return;
    m_online = online;
    emit onlineChanged(m_online);
}