#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "grpcclient.h"
#include "discoveryclient.h"
#include <QVariant>
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    DiscoveryClient discoveryClient;

    QList<QObject *> clients;

    discoveryClient.start(10001);
    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []()
                     { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    QObject::connect(&discoveryClient, &DiscoveryClient::newServerDiscovered, &app, [&clients, &engine](QUrl url, QString clientIP)
                     {
        qDebug()<<"Got a new url"<<url;
        clients.append(new GRPCClient(url,clientIP));
        //Update context property because QML can't know that the list itself has changed,
        //but it can track clinets' properties.
        //This can be avoided by providing an item model instead of QList but this is good enough for this test case
        engine.rootContext()->setContextProperty("clientsModel", QVariant::fromValue(clients)); });

    engine.rootContext()->setContextProperty("clientsModel", QVariant::fromValue(clients));
    engine.loadFromModule("grpctest.client", "Main");

    return app.exec();
}
