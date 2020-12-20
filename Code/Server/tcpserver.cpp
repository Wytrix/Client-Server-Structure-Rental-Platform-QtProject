#include "tcpserver.h"
#include <QHostAddress>

TcpServer::TcpServer(QObject *parent, int port):
    QTcpServer(parent)
{
    listen(QHostAddress::Any, port);
}

void TcpServer::incomingConnection(int socketDescriptor)
{
    TcpClientSocket *tcpclientsocket = new TcpClientSocket(this);
    tcpclientsocket->setSocketDescriptor(socketDescriptor);
    tcpclientsocketlist.append(tcpclientsocket);
    connect(tcpclientsocket, &TcpClientSocket::updateserver, this, &TcpServer::slotupdateserver);
    connect(tcpclientsocket, &TcpClientSocket::clientdisconnected, this, &TcpServer::slotclientdisconnect);
    connect(tcpclientsocket, &TcpClientSocket::upMessageByServer, this, &TcpServer::sendMessageToClient);
}

void TcpServer::sendMessageToClient(QString user, QString msg)
{
    int size = tcpclientsocketlist.count();
    for(int i=0;i<size;i++){
        TcpClientSocket *item = tcpclientsocketlist.at(i);
        if(item->ID == user)
            item->sendMessage(28,msg);
    }
}

void TcpServer::slotupdateserver(QString msg){
    emit updateserver(msg);
    /*for(int i = 0; i < tcpclientsocketlist.count(); i++)
    {
        QTcpSocket *item = tcpclientsocketlist.at(i);
        item->write(msg.toUtf8().data());
    }*/
}

void TcpServer::slotclientdisconnect(int descriptor){
    for(int i = 0; i < tcpclientsocketlist.count(); i++)
    {
        QTcpSocket *item = tcpclientsocketlist.at(i);
        if(item->socketDescriptor() == descriptor)
        {
            emit updateserver(tcpclientsocketlist.at(i)->ID+":断开连接");
            tcpclientsocketlist.removeAt(i);
            return;
        }
    }
    return;
}
