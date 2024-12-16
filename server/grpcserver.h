#ifndef GRPCSERVER_H
#define GRPCSERVER_H
#include <QObject>
#include <grpc++/grpc++.h>
#include <QDebug>
#include "api.grpc.pb.h"
#include <QThread>

/// @brief Implements MaintainingApi::Ping service 
class GRPCServer final : public QObject, MaintainingApi::Service
{
    Q_OBJECT
public:
    GRPCServer(QObject *parent = nullptr) : QObject(parent), MaintainingApi::Service()
    {
    }
    grpc::Status Ping(grpc::ServerContext *context,
                      const PingRequest *request,
                      PingResponse *response) override
    {
        emit newRequest(QString::fromStdString(request->clientip()));
        emit log(QString::fromStdString("Got ping from " + request->clientip()));
        return grpc::Status();
    }

public slots:
    bool start(int port)
    {
        //Build and start gRPC server
        std::string serverUri("127.0.0.1:" + std::to_string(port));
        grpc::ServerBuilder builder;
        builder.RegisterService(this);
        builder.AddListeningPort(serverUri, grpc::InsecureServerCredentials());

        m_server = builder.BuildAndStart();
        if (!m_server)
        {
            emit log("Creating grpc::Server failed");
            qDebug() << "Creating grpc::Server failed";
            emit error();
            return false;
        }
        emit log(QString::fromStdString("Server is listening on " + serverUri));
        return true;
    }
    void stop()
    {
        emit log("Stopping the server");
        if (m_server)
            m_server->Shutdown();
        else
            qDebug() << "server is not running";
    }
protected:
    std::unique_ptr<grpc::Server> m_server;

signals:
    void newRequest(QString ip);
    void log(QString);
    void error();
};
#endif // SERVER_RUNNER_H
