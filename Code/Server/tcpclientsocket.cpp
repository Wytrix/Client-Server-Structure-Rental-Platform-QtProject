#include "tcpclientsocket.h"

TcpClientSocket::TcpClientSocket(QObject *parent)
{
    setParent(parent);
    ID = "";
    connect(this, &TcpClientSocket::readyRead, this, &TcpClientSocket::receivedata);
    connect(this, &TcpClientSocket::disconnected, this, &TcpClientSocket::slotclientdisconnected);
}

void TcpClientSocket::initDB(QSqlDatabase &db)
{
    if(QSqlDatabase::contains("clientConnect"))
        db = QSqlDatabase::database("clientConnect");
    else
        db = QSqlDatabase::addDatabase("QSQLITE", "clientConnect");
    db.setDatabaseName("database.db");
    db.open();
}

void TcpClientSocket::fileReply()
{
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_5_11);
    if(byteRcv <= sizeof(qint64) * 2){
        if((bytesAvailable() >= sizeof(qint64) * 2) && (fileNameSize == 0)){
            in>>totByte>>fileNameSize;
            byteRcv += sizeof(qint64) * 2;
        }
        if((bytesAvailable() >= fileNameSize) && (fileNameSize != 0)){
            in>>fileName;
            byteRcv += fileNameSize;
            QString dir = QCoreApplication::applicationDirPath()+"/UserFiles/"+ID;
            QDir DIR;
            if (!DIR.exists(dir))
            {
                bool res = DIR.mkpath(dir);
                qDebug() << "新建目录是否成功" << res;
            }
            localFile = new QFile(dir+"/"+fileName);
            if(!localFile->open(QFile::WriteOnly)){
                qDebug()<<"server: open file error!";
                return ;
            }
        }
        else return ;
    }
    if(byteRcv < totByte){
        byteRcv += bytesAvailable();
        inBlock = readAll();
        picBlock += inBlock;
        localFile->write(inBlock);
        inBlock.resize(0);
    }
    if(byteRcv == totByte){
        localFile->close();
        disconnect(this,&TcpClientSocket::readyRead,this,&TcpClientSocket::fileReply);
        connect(this, &TcpClientSocket::readyRead, this, &TcpClientSocket::receivedata);
        QSqlDatabase db;
        initDB(db);
        QSqlQuery query(db);
        query.exec(QString("select * from item where id = %1").arg(itemID));
        query.next();
        query.prepare(QString("update item set pic = ? where id = ?"));
        query.addBindValue(picBlock);
        query.addBindValue(itemID);
        query.exec();
        sendMessage(12,"ok");
    }
}

void TcpClientSocket::fileToClient()
{
    connect(this,&TcpClientSocket::bytesWritten,this,&TcpClientSocket::sendFile);
    qDebug()<<"Yes";
    byteSend = 0;
    picFile = new QFile(fileName);
    if(!picFile->open(QFile::ReadOnly)){
        qDebug()<<"client:open file error!";
        QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
        sendOut.setVersion(QDataStream::Qt_5_11);
        return ;
    }
    totalSize = picFile->size();
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_11);
    QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/') - 1);
    sendOut<<qint64(0)<<qint64(0)<<currentFileName;
    totalSize += outBlock.size();
    sendOut.device()->seek(0);
    sendOut<<totalSize<<qint64(outBlock.size() - sizeof(qint64) * 2);
    byteRes = totalSize - write(outBlock);
    outBlock.resize(0);
}

void TcpClientSocket::sendFile(qint64 numByte)
{
    byteSend += (int)numByte;
    if(byteRes > 0){
        outBlock = picFile->read(qMin(byteRes,payloadSize));
        byteRes -= (int)write(outBlock);
        outBlock.resize(0);
    }
    else {
        picFile->close();
    }
    if(byteSend == totalSize){
        picFile->close();
        disconnect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(sendFile(qint64)));
    }
}


void TcpClientSocket::receivedata()
{
    readMessage(readType,readMsg);
    emit updateserver(ID+":"+QString::number(readType)+"//"+readMsg);
    switch(readType){
    case 1:
        loginReply();
        break;
    case 3:
        zhuceReply();
        break;
    case 5:
        gerenReply();
        break;
    case 7:
        xiugaiReply();
        break;
    case 9:
        updateReply();
        break;
    case 11:
        fabuReply();
        break;
    case 13:
        chakanReply(QString("select * from item order by time desc"));
        break;
    case 15:
        deltailReply();
        break;
    case 19:
        releasedReply();
        break;
    case 21:
        deleteItemReply();
        break;
    case 23:
        messageReply();
        break;
    case 25:
        talkReply();
        break;
    case 27:
        upMessageReply();
        break;
    case 485:
        sendPic1();
        break;
    case 486:
        sendPic2();
        break;
    case 487:
        sendPic3();
        break;
    case 488:
        downloadNewClientReply();
        break;
    }
}

void TcpClientSocket::downloadNewClientReply()
{
    fileName = QString("client_%1.rar").arg(version);
    localFile = new QFile(fileName);
    if(!localFile->open(QFile::ReadWrite)){
        qDebug()<<"server: open file error!";
        return ;
    }
    if(localFile->size() == 0)localFile->write(0);
    delete localFile;
    localFile = NULL;
    fileToClient();
}

void TcpClientSocket::upMessageReply()
{
    QString user = readMsg.section("::==",0,0);
    QString msg = readMsg.section("::==",1,1);
    QSqlDatabase db;
    initDB(db);
    QSqlQuery query(db);
    query.exec("select max(id) from mesg");
    query.next();
    mesgID = query.value(0).toInt();
    query.exec(QString("insert into mesg (id, receiver, sender, message, time, read) values(%1,'%2','%3','%4','%5',%6)").arg(++mesgID).arg(user).arg(ID).arg(msg).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss")).arg(0));
    qDebug()<<user<<msg;
    emit upMessageByServer(user,msg);
}

void TcpClientSocket::talkReply()
{
    QString user = readMsg;
    QSqlDatabase db;
    initDB(db);
    QSqlQuery query(db);
    query.exec(QString("select * from mesg where (receiver = '%1' AND sender = '%2') OR (receiver = '%3' AND sender = '%4') order by time desc").arg(ID).arg(user).arg(user).arg(ID));
    QString send[5],message[5];
    QString reply = "";
    reply += "ok";
    for(int i=0;i<5;i++){
        if(query.next()){
            send[i] = query.value(2).toString();
            message[i] = query.value(3).toString();
        }
        else {
            send[i] = "";
            message[i] = "";
        }
        reply+=("::=="+send[i]+"::=="+message[i]);
    }
    sendMessage(26,reply);
}

void TcpClientSocket::messageReply()
{
    int messagePage = readMsg.toInt();
    QSqlDatabase db;
    initDB(db);
    QSqlQuery query(db);
    query.exec(QString("select * from mesg where receiver = '%1' order by time desc").arg(ID));
    for(int i=1;i<=messagePage * 3;i++)
        query.next();
    QString sender[3],message[3],time[3];
    for(int i = 0; i <= 2; i++)
    {
        if(query.next()){
            sender[i] = query.value(2).toString();
            message[i] = query.value(3).toString();
            time[i] = query.value(4).toString();
        }
        else {
            sender[i] = "";
            message[i] = "";
            time[i] = "";
        }
    }
    sendMessage(24,sender[0]+"::=="+message[0]+"::=="+time[0]+"::=="+sender[1]+"::=="+message[1]+"::=="+time[1]+"::=="+sender[2]+"::=="+message[2]+"::=="+time[2]);
}

void TcpClientSocket::deleteItemReply()
{
    int id = readMsg.toInt();
    QSqlDatabase db;
    initDB(db);
    QSqlQuery query(db);
    query.prepare(QString("delete from item where id = ?"));
    query.addBindValue(id);
    if(query.exec())
    sendMessage(22,"ok");
    else sendMessage(22,"error");
}

void TcpClientSocket::releasedReply()
{
    chakanReply(QString("select * from item where user = '%1' order by time desc").arg(ID));
}

void TcpClientSocket::deltailReply()
{
    qint32 id = readMsg.toInt();
    QSqlDatabase db;
    initDB(db);
    QSqlQuery query(db);
    query.exec(QString("select * from item where id = %1").arg(id));
    if(query.next())
    sendMessage(16,"ok::=="+query.value(1).toString()+"::=="+query.value(2).toString()+"::=="+query.value(3).toString()+"::=="+query.value(4).toString()+"::=="+query.value(5).toString()+"::=="+query.value(6).toString()+"::=="+query.value(7).toString());
    else sendMessage(16,"not found");
}

void TcpClientSocket::sendPic1()
{
    QString dir = QCoreApplication::applicationDirPath()+"/User/"+ID;
    QDir DIR;
    if (!DIR.exists(dir))
    {
        bool res = DIR.mkpath(dir);
        qDebug() << "新建目录是否成功" << res;
    }
    fileName = QString("%1/1.jpg").arg(dir);
    localFile = new QFile(fileName);
    if(!localFile->open(QFile::WriteOnly)){
        qDebug()<<"server: open file error!";
        return ;
    }
    localFile->write(picData[0]);
    qDebug()<<localFile->size();
    delete localFile;
    localFile = NULL;
    fileToClient();
}
void TcpClientSocket::sendPic2()
{
    QString dir = QCoreApplication::applicationDirPath()+"/User/"+ID;
    QDir DIR;
    if (!DIR.exists(dir))
    {
        bool res = DIR.mkpath(dir);
        qDebug() << "新建目录是否成功" << res;
    }
    fileName = QString("%1/2.jpg").arg(dir);
    localFile = new QFile(fileName);
    if(!localFile->open(QFile::WriteOnly)){
        qDebug()<<"server: open file error!";
        return ;
    }
    localFile->write(picData[1]);
    qDebug()<<localFile->size();
    delete localFile;
    localFile = NULL;
    fileToClient();
}
void TcpClientSocket::sendPic3()
{
    QString dir = QCoreApplication::applicationDirPath()+"/User/"+ID;
    QDir DIR;
    if (!DIR.exists(dir))
    {
        bool res = DIR.mkpath(dir);
        qDebug() << "新建目录是否成功" << res;
    }
    fileName = QString("%1/3.jpg").arg(dir);
    localFile = new QFile(fileName);
    if(!localFile->open(QFile::WriteOnly)){
        qDebug()<<"server: open file error!";
        return ;
    }
    localFile->write(picData[2]);
    qDebug()<<localFile->size();
    delete localFile;
    localFile = NULL;
    fileToClient();
}
void TcpClientSocket::chakanReply(QString cmd)
{
    int chakanPage = readMsg.toInt();
    QSqlDatabase db;
    initDB(db);
    QSqlQuery query(db);
    query.exec(cmd);
    for(int i=1;i<=chakanPage * 3;i++)
        query.next();
    chakanNum = 0;
    QString id[3],account[3],title[3],addr[3],size[3],louceng[3],price[3],ps[3],fileMsg[3];
    for(int i=0;i<=2;i++)
    {
        if(query.next())
        {
            qint32 temp = query.value(0).toInt();
            id[i] = QString::number(temp);
            qDebug()<<i<<id[i];
            account[i] = query.value(1).toString();
            title[i] = query.value(2).toString();
            addr[i] = query.value(3).toString();
            size[i] = query.value(4).toString();
            louceng[i] = query.value(5).toString();
            price[i] = query.value(6).toString();
            ps[i] = query.value(7).toString();
            if(query.value(9).toByteArray().isEmpty()){
                picData[i].resize(0);
                fileMsg[i] = "true";
            }
            else {
                picData[i] = query.value(9).toByteArray();
                fileMsg[i] = "true";
            }
        }
        else {
            picData[i].resize(0);
            fileMsg[i] = "false";
        }
    }
    sendMessage(14,id[0]+"::=="+account[0]+"::=="+title[0]+"::=="+addr[0]+"::=="+size[0]+"::=="+louceng[0]+"::=="+price[0]+"::=="+ps[0]+"::=="+fileMsg[0]+"::=="+id[1]+"::=="+account[1]+"::=="+title[1]+"::=="+addr[1]+"::=="+size[1]+"::=="+louceng[1]+"::=="+price[1]+"::=="+ps[1]+"::=="+fileMsg[1]+"::=="+id[2]+"::=="+account[2]+"::=="+title[2]+"::=="+addr[2]+"::=="+size[2]+"::=="+louceng[2]+"::=="+price[2]+"::=="+ps[2]+"::=="+fileMsg[2]);
}

void TcpClientSocket::fabuReply()
{
    QString account = readMsg.section("::==",0,0);
    QString title = readMsg.section("::==",1,1);
    QString addr = readMsg.section("::==",2,2);
    QString size = readMsg.section("::==",3,3);
    QString louceng = readMsg.section("::==",4,4);
    QString price = readMsg.section("::==",5,5);
    QString ps = readMsg.section("::==",6,6);
    QString fileMsg = readMsg.section("::==",7,7);
    QSqlDatabase db;
    initDB(db);
    QSqlQuery query(db);
    query.exec("select max(id) from item");
    query.next();
    itemID = query.value(0).toInt();
    qDebug()<<"itemID:"<<itemID;
    if(account != ID)
        sendMessage(12,"id error");
    else{
        query.prepare("insert into item values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        query.addBindValue(++itemID);
        query.addBindValue(account);
        query.addBindValue(title);
        query.addBindValue(addr);
        query.addBindValue(size);
        query.addBindValue(louceng);
        query.addBindValue(price);
        query.addBindValue(ps);
        query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"));
        query.addBindValue(0);
        query.exec();
        if(fileMsg == ""){
            sendMessage(12,"ok");
        }
        else if(fileMsg == "ok"){
            sendMessage(484,"ok");
            totByte = 0;
            byteRcv = 0;
            fileNameSize = 0;
            picBlock.resize(0);
            disconnect(this, &TcpClientSocket::readyRead, this, &TcpClientSocket::receivedata);
            connect(this,&TcpClientSocket::readyRead,this,&TcpClientSocket::fileReply);
        }
    }
}

void TcpClientSocket::updateReply()
{
    if(readMsg == version){
        sendMessage(10,"new");
    }
    else sendMessage(10,"old");
}

void TcpClientSocket::xiugaiReply()
{
    QString mima = readMsg.section("::==",0,0);
    QString phone = readMsg.section("::==",1,1);
    QString mail = readMsg.section("::==",2,2);
    QSqlDatabase db;
    initDB(db);
    QSqlQuery query(db);
    query.exec(QString("select * from user where id = '%1'").arg(ID));
    query.next();
    query.exec(QString("update user set password = '%1' where id = '%2'").arg(mima).arg(ID));
    query.exec(QString("update user set phone = '%1' where id = '%2'").arg(phone).arg(ID));
    query.exec(QString("update user set mail = '%1' where id = '%2'").arg(mail).arg(ID));
    if(true)sendMessage(8,"ok");
}

void TcpClientSocket::gerenReply()
{
    QString id = readMsg.section("::==",0,0);
    QString queryUser = readMsg.section("::==",1,1);
    if(id == ID)
    {
        QSqlDatabase db;
        initDB(db);
        QSqlQuery query(db);
        query.exec(QString("select * from user where id = '%1'").arg(queryUser));
        if(ID == queryUser){
            if(query.next())
            sendMessage(6,"ok::=="+query.value(1).toString()+"::=="+query.value(2).toString()+"::=="+query.value(3).toString());
            else sendMessage(6,"ID not exist");
        }
        else {
            if(query.next())
            sendMessage(18,"ok::=="+query.value(2).toString()+"::=="+query.value(3).toString());
            else sendMessage(18,"ID not exist");
        }
    }
    else sendMessage(6,"ID error");
}

void TcpClientSocket::loginReply()
{
    QString account = readMsg.section("::==",0,0);
    QString password = readMsg.section("::==",1,1);
    QSqlDatabase db;
    initDB(db);
    QSqlQuery query(db);
    query.exec(QString("select * from user where id = '%1'").arg(account));
    ID = "";
    while(query.next()){
        if(query.value(0).toString() == account && query.value(1).toString() == password)
            ID = account;
    }
    if(ID != "")
        sendMessage(2,QString("ok::==%1").arg(account));
    else sendMessage(2,"account or password error");
}

void TcpClientSocket::zhuceReply()
{
    QString account = readMsg.section("::==",0,0);
    QString password = readMsg.section("::==",1,1);
    QString phone = readMsg.section("::==",2,2);
    QString mail = readMsg.section("::==",3,3);
    QSqlDatabase db;
    initDB(db);
    QSqlQuery query(db);
    query.exec(QString("select * from user where id = '%1'").arg(account));
    if(true){
        bool exist = false;
        while(query.next()){
            if(query.value(0).toString() == account)
                exist = true;
        }
        if(!exist)
        {
            query.prepare("insert into user values (?, ?, ?, ?)");
            query.addBindValue(account);
            query.addBindValue(password);
            query.addBindValue(phone);
            query.addBindValue(mail);
            query.exec();
            query.exec("select max(id) from mesg");
            query.next();
            int mesgID = query.value(0).toInt();
            query.exec(QString("insert into mesg values (%1, '%2', '%3', '%4', '%5', %6)").arg(++mesgID).arg(account).arg("admin").arg("欢迎使用").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss")).arg(0));
            sendMessage(4,"ok");
        }
        else sendMessage(4,"id exist");
    }
}

void TcpClientSocket::readMessage(quint16 &readType,QString &readMsg)
{
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_5_11);
    in>>readType;
    in>>readMsg;
}

void TcpClientSocket::sendMessage(quint16 type,QString msg)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_11);
    out<<(quint16)type<<msg;
    this->write(block);
}
void TcpClientSocket::slotclientdisconnected()
{
    emit clientdisconnected(this->socketDescriptor());
}
