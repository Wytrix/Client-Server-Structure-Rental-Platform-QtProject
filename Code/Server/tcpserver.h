#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "header.h"
#include "tcpclientsocket.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    TcpServer(QObject *parent = 0, int port = 0);
    QList<TcpClientSocket*> tcpclientsocketlist;
    
protected:
    void incomingConnection(int socketDescriptor);

protected slots:
    void slotupdateserver(QString);
    void slotclientdisconnect(int);
    void sendMessageToClient(QString user,QString msg);

signals:
    void updateserver(QString);
};

#endif // TCPSERVER_H
