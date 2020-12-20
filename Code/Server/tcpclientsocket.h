#ifndef TCPCLIENTSOCKET_H
#define TCPCLIENTSOCKET_H

#include "header.h"

class TcpClientSocket : public QTcpSocket
{
    Q_OBJECT
public:
    TcpClientSocket(QObject *parent = 0);
    void readMessage(quint16 &readType,QString &readMsg);
    void sendMessage(quint16 type,QString msg);
    QString ID;

private:
    void loginReply();
    void zhuceReply();
    void gerenReply();
    void xiugaiReply();
    void updateReply();
    void fabuReply();
    void chakanReply(QString cmd);
    void deltailReply();
    void releasedReply();
    void deleteItemReply();
    void messageReply();
    void talkReply();
    void upMessageReply();
    void downloadNewClientReply();

    void fileToClient();
    void sendPic1();
    void sendPic2();
    void sendPic3();
    void initDB(QSqlDatabase &db);
    quint16 readType;
    QString readMsg;

    qint32 itemID;
    qint32 mesgID;
    //接收文件
    qint64 totByte;
    qint64 byteRcv;
    qint64 fileNameSize;
    QString fileName;
    QFile *localFile;
    QByteArray inBlock;
    QByteArray picBlock;
    //发送文件
    qint64 byteSend;
    qint64 fileSize;
    qint64 totalSize;
    qint64 byteRes;
    QByteArray outBlock;
    QByteArray picData[3];
    QFile *picFile;
    int chakanNum;
protected slots:
    void receivedata();
    void fileReply();
    void sendFile(qint64);
    void slotclientdisconnected();

signals:
    void updateserver(QString);
    void clientdisconnected(int);
    void upMessageByServer(QString user,QString msg);
};

#endif // TCPCLIENTSOCKET_H
