#include "grpcserver.h"

#include <QApplication>
#include <memory>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include "discoveryserver.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create a minimalistic GUI
    QWidget mainWindow;
    QVBoxLayout *layout = new QVBoxLayout(&mainWindow);

    QTextEdit *textEdit = new QTextEdit(&mainWindow);
    textEdit->setReadOnly(true);
    textEdit->document()->setMaximumBlockCount(1000); // Limit the log size

    QPushButton *runButton = new QPushButton("Run", &mainWindow);
    runButton->setCheckable(true);

    QSpinBox *portSpin = new QSpinBox(&mainWindow);
    portSpin->setRange(50000, 60000);
    portSpin->setValue(50051);

    layout->addWidget(textEdit);
    layout->addWidget(runButton);
    layout->addWidget(portSpin);

    GRPCServer server;

    DiscoveryServer discoveryServer;

    // Suspend discovery server broadcasting if there are active clients
    QObject::connect(&server, &GRPCServer::newRequest, &discoveryServer, &DiscoveryServer::suspend);

    QObject::connect(runButton, &QPushButton::toggled, [&server, &discoveryServer, portSpin, runButton](bool checked)
                     {
                        portSpin->setEnabled(!checked);
        if(checked)
        {
            if(server.start(portSpin->value()))
            {
                //Advertise localhost since no address is given. It won't work in a real network.
                discoveryServer.start(10001,QUrl(QString("http://127.0.0.1:%1").arg(portSpin->value())));
            }   
        }
        else
        {
            server.stop();
            discoveryServer.stop();
        } });

    // Connect logging signals
    QObject::connect(&discoveryServer, &DiscoveryServer::log, textEdit, &QTextEdit::append);
    QObject::connect(&server, &GRPCServer::log, textEdit, &QTextEdit::append);

    // Unchek "Run" button in case of an error
    QObject::connect(&server, &GRPCServer::error, [runButton]()
                     { runButton->setChecked(false); });

    mainWindow.show();
    return a.exec();
}
