#include "controller.h"
#include <QTimer>

Controller::Controller(QGraphicsScene &scene, QGraphicsView &view, QObject *parent) :
    QObject(parent),
    scene(scene),
    view(view)
{
    timer.start( 1000/30 );

    scene.installEventFilter(this);
    port = 6666;
    newConnect();
    connect(&timer, SIGNAL(timeout()),&scene, SLOT(advance()));
    connect(this,&Controller::picFileReceive,this,&Controller::picFileCount);
    loginScene();
}

void Controller::picFileCount()
{
    picFileCounter++;
    qDebug()<<picFileCounter;
    if(picFileCounter>3)
    {
        deletePop();
        int count = 0;
        for(int i=0;i<=2;i++)
            if(fileMsg[i] == "true")
                count++;
        if(!released)chakanScene(count);
        else releasedScene(count);
        chakanYema->setText(QString("%1").arg(QString::number((released ? releasedPage : chakanPage)+1)));
        for(int i=0;i<=2;i++){
            if(fileMsg[i] == "true"){
                titleItem[i]->setText(title[i]);
                priceItem[i]->setText(QString("%1元/月").arg(price[i]));
                mianjiTag[i]->setText(QString("面积：%1").arg(size[i]));
                loucengTag[i]->setText(QString("楼层：%1").arg(louceng[i]));
            }
        }
        for(int i=0;i<=2;i++){
            QString Name = QString("%1.jpg").arg(QString::number(i+1));
            QImage *localFile = new QImage(Name);
            if(localFile->sizeInBytes() != 0)
            picItem[i]->setImage(Name);
            delete localFile;
            localFile = NULL;
        }

        return ;
    }
    sendMessage(484+picFileCounter,"ok");
    totByte = 0;
    byteRcv = 0;
    fileNameSize = 0;
    disconnect(tcpSocket, &QTcpSocket::readyRead, this, &Controller::receivedata);
    connect(tcpSocket,&QTcpSocket::readyRead,this,&Controller::fileReceive);
}
void Controller::newConnect()
{
    tcpSocket = new QTcpSocket(this);
    tcpSocket->abort();
    tcpSocket->connectToHost("localhost", port);
    connect(tcpSocket,&QTcpSocket::readyRead,this,&Controller::receivedata);
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
}

void Controller::receivedata()
{
    readMessage(readType,readMsg);
    switch(readType){
    case 2:
        loginReply();
        break;
    case 4:
        zhuceReply();
        break;
    case 6:
        gerenReply();
        break;
    case 8:
        xiugaiReply();
        break;
    case 10:
        updateReply();
        break;
    case 12:
        fabuReply();
        break;
    case 14:
        chakanReply();
        break;
    case 16:
        detailReply();
        break;
    case 18:
        userReply();
        break;
    case 20:
        releasedReply();
        break;
    case 22:
        deleteItemReply();
        break;
    case 24:
        messageReply();
        break;
    case 26:
        talkReply();
        break;
    case 28:
        upMessageReply();
        break;
    case 484:
        fileReply();
        break;
    }
}

void Controller::loginRequest()
{
    if(accountLine->text->toPlainText() == "")
        popWindow("账号不能为空");
    else if(passwordLine->text->toPlainText() == "")
        popWindow("密码不能为空");
    else {
        sendMessage(1,accountLine->text->toPlainText()+"::=="+passwordLine->text->toPlainText());
        popWindow("登陆中...",false);
    }
}

void Controller::zhuceRequest()
{
    if(idLine->text->toPlainText() == "")
        popWindow("账号不能为空");
    else if(mimaLine->text->toPlainText() == "")
        popWindow("密码不能为空");
    else {
        sendMessage(3,idLine->text->toPlainText()+"::=="+mimaLine->text->toPlainText()+"::=="+phoneLine->text->toPlainText()+"::=="+mailLine->text->toPlainText());
        popWindow("请稍后...",false);
    }
}

void Controller::gerenRequest()
{
    sendMessage(5,ID+"::=="+ID);
}

void Controller::userRequest()
{
    sendMessage(5,ID+"::=="+user);
}

void Controller::xiugaiRequest()
{
    if(mimaLine->text->toPlainText() == "")
        popWindow("密码不能为空");
    else {
        sendMessage(7,mimaLine->text->toPlainText()+"::=="+phoneLine->text->toPlainText()+"::=="+mailLine->text->toPlainText());
    }
}
void Controller::xiugaiReply()
{
    if(readMsg == "ok"){
        popWindow("修改成功");
    }
    else {
        popWindow("修改失败");
    }
}

void Controller::updateRequest()
{
    sendMessage(9,version);
}

void Controller::fabuRequest()
{
    if(titleLine->text->toPlainText() == "") popWindow("标题不能为空");
    else if(addrLine->text->toPlainText() == "") popWindow("地址不能为空");
    else if(sizeLine->text->toPlainText() == "") popWindow("面积不能为空");
    else if(loucengLine->text->toPlainText() == "") popWindow("楼层不能为空");
    else if(priceLine->text->toPlainText() == "") popWindow("价格不能为空");
    else {

        QString fileString;
        fileSize = 0;
        if(fileName.isEmpty())fileString = "";
        else {
            fileString = "::==ok";
            picFile = new QFile(fileName);
            fileSize = picFile->size();
            picFile->close();
        }
        if(fileSize <= 256 * 1024){
            popWindow("发布中...",false);
            sendMessage(11,ID+"::=="+titleLine->text->toPlainText()+"::=="+addrLine->text->toPlainText()+"::=="+sizeLine->text->toPlainText()+"::=="+loucengLine->text->toPlainText()+"::=="+priceLine->text->toPlainText()+"::=="+psLine->text->toPlainText()+fileString);
            }
        else popWindow("文件过大(图片不超过256KB)");
    }
}

void Controller::chakanRequest()
{
    sendMessage(13,QString::number(chakanPage));
    popWindow("请稍后...",false);
}

void Controller::detailRequest1(){
    click = 1;
    sendMessage(15,chakanID[0]);
    popWindow("请稍后...",false);
}
void Controller::detailRequest2(){
    click = 2;
    sendMessage(15,chakanID[1]);
    popWindow("请稍后...",false);
}
void Controller::detailRequest3(){
    click = 3;
    sendMessage(15,chakanID[2]);
    popWindow("请稍后...",false);
}

void Controller::releasedRequest()
{
    released = true;
    sendMessage(19,QString::number(releasedPage));
    popWindow("请稍后...",false);
}

void Controller::deleteItemRequest()
{
    sendMessage(21,chakanID[click-1]);
    popWindow("请稍后...",false);
}

void Controller::messageRequest()
{
    sendMessage(23,QString::number(messagePage));
    popWindow("请稍后...",false);
}

void Controller::talkRequest()
{
    sendMessage(25,user);
    popWindow("请稍后...",false);
}
void Controller::talkRequest1(){
    user = sendUser[0];
    sendMessage(25,user);
    popWindow("请稍后...",false);
}
void Controller::talkRequest2(){
    user = sendUser[1];
    sendMessage(25,user);
    popWindow("请稍后...",false);
}
void Controller::talkRequest3(){
    user = sendUser[2];
    sendMessage(25,user);
    popWindow("请稍后...",false);
}
void Controller::senderRequest1(){
    user = sendUser[0];
    userRequest();
}
void Controller::senderRequest2(){
    user = sendUser[1];
    userRequest();
}
void Controller::senderRequest3(){
    user = sendUser[2];
    userRequest();
}

void Controller::upMessageRequest()
{
    if(shuruButton->text->toPlainText() != ""){
        updateMessage(true,shuruButton->text->toPlainText());
        sendMessage(27,user+"::=="+shuruButton->text->toPlainText());
        shuruButton->text->setPlainText("");
    }
}

void Controller::downloadNewClientRequest()
{
    sendMessage(488,"ok");
    totByte = 0;
    byteRcv = 0;
    fileNameSize = 0;
    downloadFile = true;
    disconnect(tcpSocket, &QTcpSocket::readyRead, this, &Controller::receivedata);
    connect(tcpSocket,&QTcpSocket::readyRead,this,&Controller::fileReceive);
}

void Controller::upMessageReply()
{
    if(inTalkScene){
        updateMessage(false,readMsg);
    }
}
void Controller::talkReply()
{
    userKey = 5, myKey = 5;
    deletePop();
    QString send[5],getMessage[5];
    QString State = readMsg.section("::==",0,0);
    for(int i=0;i<5;i++){
        send[i] = readMsg.section("::==",1+i*2,1+i*2);
        getMessage[i] = readMsg.section("::==",2+i*2,2+i*2);
    }
    talkScene();
    talkUserButton->setText(tr("与%1的会话").arg(user));
    for(int i=0;i<5;i++){
        if(send[i] == user){
            userMessageButton[i]->setText(getMessage[i]);
            scene.addItem(userMessageButton[i]);
        }
        else if(send[i] == ID){
            myMessageButton[i]->setText(getMessage[i]);
            scene.addItem(myMessageButton[i]);
        }
    }
    Pgroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void Controller::messageReply()
{
    deletePop();
    sendUser[0] = readMsg.section("::==",0,0);
    message[0] = readMsg.section("::==",1,1);
    messageTime[0] = readMsg.section("::==",2,2);
    sendUser[1] = readMsg.section("::==",3,3);
    message[1] = readMsg.section("::==",4,4);
    messageTime[1] = readMsg.section("::==",5,5);
    sendUser[2] = readMsg.section("::==",6,6);
    message[2] = readMsg.section("::==",7,7);
    messageTime[2] = readMsg.section("::==",8,8);
    int count = 0;
    for(int i=0;i<=2;i++)
    {
        if(sendUser[i] != "")
            count ++;
    }
    messageScene(count);
    for(int i=0;i<count;i++)
    {
        sender[i]->setText(sendUser[i]);
        messageBox[i]->setText(message[i]);
        int yyyy = QString(messageTime[i].section("-",0,0)).toInt();
        int MM = QString(messageTime[i].section("-",1,1)).toInt();
        int dd = QString(messageTime[i].section("-",2,2)).toInt();
        int hh = QString(messageTime[i].section("-",3,3)).toInt();
        int mm = QString(messageTime[i].section("-",4,4)).toInt();
        int yyyyNow = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss").section("-",0,0)).toInt();
        int MMNOw = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss").section("-",1,1)).toInt();
        int ddNow = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss").section("-",2,2)).toInt();
        int hhNow = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss").section("-",3,3)).toInt();
        int mmNow = QString(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss").section("-",4,4)).toInt();
        if(yyyy == yyyyNow && MM == MMNOw && dd == ddNow){
            if(hh == hhNow && mm == mmNow)
                timButton[i]->setText("刚刚");
            else if(hhNow * 60 + mmNow - (hh * 60 +mm) < 60)
                timButton[i]->setText(QString("%1分钟前").arg(hhNow * 60 + mmNow - (hh * 60 +mm)));
            else timButton[i]->setText(QString("%1小时前").arg((hhNow * 60 + mmNow - (hh * 60 +mm))/60));
        }
        else if(yyyy == yyyyNow)
            timButton[i]->setText(QString("%1月%2日 %3:%4").arg(MM).arg(dd).arg(hh).arg(mm));
        else timButton[i]->setText(QString("%1年%2月%3日 %4:%5").arg(yyyy).arg(MM).arg(dd).arg(hh).arg(mm));
    }
}

void Controller::deleteItemReply()
{
    qDebug()<<"reply";
    deletePop();
    if(readMsg == "ok")
    {
        popWindow("删除成功");
        if(released)releasedRequest();
        else chakanRequest();
    }
    else popWindow("删除失败");
}
void Controller::detailReply()
{
    QString State = readMsg.section("::==",0,0);
    user = readMsg.section("::==",1,1);
    QString title = readMsg.section("::==",2,2);
    QString addr = readMsg.section("::==",3,3);
    QString size = readMsg.section("::==",4,4);
    QString louceng = readMsg.section("::==",5,5);
    QString price = readMsg.section("::==",6,6);
    QString ps = readMsg.section("::==",7,7);
    if(State == "ok"){
        deletePop();
        detailScene();
        titleLine->setText(title);
        addrLine->setText(addr);
        sizeLine->setText(size);
        loucengLine->setText(louceng);
        priceLine->setText(price);
        psLine->setText(ps);
        userButton->setText(QString("ID：%1").arg(user));
        connect(contactButton,&Button::ButtonIsReleased,this,&Controller::talkRequest);
        if(user == ID){
            scene.removeItem(userButton);
            scene.removeItem(contactButton);
            scene.addItem(deleteButton);
            connect(deleteButton,&Button::ButtonIsReleased,this,&Controller::deleteItemRequest);
        }
    }
    else {
        deletePop();
        popWindow("发生错误，此条信息可能已被删除");
    }
}
void Controller::fileReply()
{
    connect(tcpSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(sendFile(qint64)));
    byteSend = 0;
    picFile = new QFile(fileName);
    if(!picFile->open(QFile::ReadOnly)){
        qDebug()<<"client:open file error!";
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
    byteRes = totalSize - tcpSocket->write(outBlock);
    outBlock.resize(0);
}

void Controller::loginReply()
{
    QString msg = readMsg.section("::==",0,0);
    QString account = readMsg.section("::==",1,1);
    if(msg == "ok"){
        deletePop();
        ID = account;
        mainScene();
    }
    else if(msg == "account or password error")
    {
        deletePop();
        popWindow("用户名或密码错误");
    }
}

void Controller::zhuceReply()
{
    if(readMsg == "ok"){
        deletePop();
        popWindow("注册成功");
    }
    else if(readMsg == "id exist"){
        deletePop();
        popWindow("账号已存在");
    }
    else {
        deletePop();
        popWindow("注册失败");
    }
}

void Controller::gerenReply()
{
    QString State = readMsg.section("::==",0,0);
    QString mima = readMsg.section("::==",1,1);
    QString phone = readMsg.section("::==",2,2);
    QString mail = readMsg.section("::==",3,3);
    if(State == "ok"){
        gerenScene();
        idLine->setText(ID);
        phoneLine->setText(phone);
        mailLine->setText(mail);
    }
    else if(State == "id not exist")
        popWindow("账号不存在");
    else if(State == "ID error"){
        popWindow("账号信息错误");
    }
}

void Controller::userReply()
{
    QString State = readMsg.section("::==",0,0);
    QString phone = readMsg.section("::==",1,1);
    QString mail = readMsg.section("::==",2,2);
    if(State == "ok"){
        userScene();
        idLine->setText(user);
        phoneLine->setText(phone);
        mailLine->setText(mail);
    }
    else if(State == "id not exist")
        popWindow("账号不存在");
    else{
        popWindow("发生错误");
    }
}

void Controller::updateReply()
{
    if(readMsg == "new"){
        popWindow("当前为最新版本");
    }
    else {
        popWindow("有新版本可用，正在下载中...",false);
        downloadNewClientRequest();
    }
}

void Controller::fabuReply()
{
    deletePop();
    if(readMsg == "id error")
        popWindow("账号信息错误");
    else if(readMsg == "ok")
        popWindow("发布成功");
}

void Controller::chakanReply()
{
    QString account[3];
    QString addr[3];
    QString ps[3];
    for(int i=0;i<=2;i++)
    {
        chakanID[i] = readMsg.section("::==",i*9,i*9);
        account[i] = readMsg.section("::==",i*9+1,i*9+1);
        title[i] = readMsg.section("::==",i*9+2,i*9+2);
        addr[i] = readMsg.section("::==",i*9+3,i*9+3);
        size[i] = readMsg.section("::==",i*9+4,i*9+4);
        louceng[i] = readMsg.section("::==",i*9+5,i*9+5);
        price[i] = readMsg.section("::==",i*9+6,i*9+6);
        ps[i] = readMsg.section("::==",i*9+7,i*9+7);
        fileMsg[i] = readMsg.section("::==",i*9+8,i*9+8);
    }
    picFileCounter = 0;
    if(picFileCounter<3) emit picFileReceive();
}

void Controller::releasedReply()
{
    chakanReply();
}

void Controller::loginScene()
{
    scene.clear();
    popButton = NULL;
    ID = "";
    released = false;
    downloadFile = false;
    chakanPage = 0;
    releasedPage = 0;
    loginButton = new Button(tr("登录"),0,70,620,80);
    accountButton = new Button(tr("账号"),-256,-160,2 * EDIT_LINE_HEIGHT,EDIT_LINE_HEIGHT);
    passwordButton = new Button(tr("密码"),-256,-70,2 * EDIT_LINE_HEIGHT,EDIT_LINE_HEIGHT);
    forgetButton = new Button(tr("忘记密码"),-150,200,180,EDIT_LINE_HEIGHT);
    signButton = new Button(tr("注册账号"),150,200,180,EDIT_LINE_HEIGHT);
    accountLine = new Button(tr(""),70,-160,480,EDIT_LINE_HEIGHT);
    passwordLine = new Button(tr(""),70,-70,480,EDIT_LINE_HEIGHT);
    accountLine->setEdit();
    passwordLine->setEdit();
    loginButton->setBackColor(PaleTurquoise);
    loginButton->setLineColor(RoyalBlue);
    accountButton->setBackColor(LightPink);
    accountButton->setLineColor(Crimson);
    passwordButton->setBackColor(LightSkyBlue);
    passwordButton->setLineColor(DoderBlue);
    forgetButton->setBackColor(NavajoWhite);
    forgetButton->setLineColor(Coral);
    signButton->setBackColor(PaleGreen);
    signButton->setLineColor(ForestGreen);
    loginButton->setRandom(true);
    loginButton->setFontSize(40);
    accountButton->setFontSize(20);
    passwordButton->setFontSize(20);
    forgetButton->setFontSize(24);
    signButton->setFontSize(24);
    scene.addItem(loginButton);
    scene.addItem(accountButton);
    scene.addItem(passwordButton);
    scene.addItem(forgetButton);
    scene.addItem(signButton);
    scene.addItem(accountLine);
    scene.addItem(passwordLine);
    Pgroup = new QParallelAnimationGroup(this);
    Pgroup->addAnimation(appearAnimation(loginButton));
    Pgroup->addAnimation(appearAnimation(accountButton));
    Pgroup->addAnimation(appearAnimation(passwordButton));
    Pgroup->addAnimation(appearAnimation(forgetButton));
    Pgroup->addAnimation(appearAnimation(signButton));
    Pgroup->addAnimation(appearAnimation(accountLine));
    Pgroup->addAnimation(appearAnimation(passwordLine));
    Pgroup->start(QAbstractAnimation::DeleteWhenStopped);
    /*
    Pgroup->addAnimation(disappearAnimation(loginButton));
    Pgroup->addAnimation(disappearAnimation(accountButton));
    Pgroup->addAnimation(disappearAnimation(passwordButton));
    Pgroup->addAnimation(disappearAnimation(forgetButton));
    Pgroup->addAnimation(disappearAnimation(signButton));
    Pgroup->addAnimation(disappearAnimation(accountLine));
    Pgroup->addAnimation(disappearAnimation(passwordLine));*/
    connect(loginButton,&Button::ButtonIsReleased,this,&Controller::loginRequest);
   // connect(loginButton,&Button::ButtonIsReleased,this,&Controller::mainScene);
    connect(signButton,&Button::ButtonIsReleased,this,&Controller::zhuceScene);
    connect(forgetButton,&Button::ButtonIsReleased,this,&Controller::notFinish);
}

void Controller::mainScene()
{
    scene.clear();
    popButton = NULL;
    released = false;
    inTalkScene = false;
    inMessageScene = false;
    downloadFile = false;
    for(int i=0;i<11;i++){
        myMessageButton[i] = NULL;
        userMessageButton[i] = NULL;
    }
    chakanButton = new Button(tr("查看租房信息"),0,-144,480,100);
    fabuButton = new Button(tr("发布租房信息"),0,0,480,100);
    gerenButton = new Button(tr("个人信息"),0,144,480,100);
    settingButton = new Button(tr("设置"),-450,230,100,80);
    messageButton = new Button(tr("消息"),450,230,100,80);
    chakanButton->setBackColor(Plum);
    chakanButton->setLineColor(HotPink);
    fabuButton->setBackColor(Khaki);
    fabuButton->setLineColor(Orange);
    gerenButton->setBackColor(PaleTurquoise);
    gerenButton->setLineColor(SteelBlue);
    settingButton->setBackColor(Silver);
    settingButton->setLineColor(DimGray);
    messageButton->setBackColor(Wheat);
    messageButton->setLineColor(DarkOrange);
    chakanButton->setFontSize(40);
    fabuButton->setFontSize(40);
    gerenButton->setFontSize(40);
    settingButton->setFontSize(28);
    messageButton->setFontSize(28);
    scene.addItem(chakanButton);
    scene.addItem(fabuButton);
    scene.addItem(gerenButton);
    scene.addItem(settingButton);
    scene.addItem(messageButton);
    setBackToLoginButton();
    Pgroup = new QParallelAnimationGroup(this);
    Pgroup->addAnimation(appearAnimation(chakanButton));
    Pgroup->addAnimation(appearAnimation(fabuButton));
    Pgroup->addAnimation(appearAnimation(gerenButton));
    Pgroup->addAnimation(appearAnimation(settingButton));
    Pgroup->addAnimation(appearAnimation(messageButton));
    Pgroup->addAnimation(appearAnimation(backToLoginButton));
    Pgroup->start(QAbstractAnimation::DeleteWhenStopped);
    connect(backToLoginButton,&Button::ButtonIsReleased,this,&Controller::loginScene);
    connect(chakanButton,&Button::ButtonIsReleased,this,&Controller::chakanRequest);
    connect(fabuButton,&Button::ButtonIsReleased,this,&Controller::fabuScene);
    connect(gerenButton,&Button::ButtonIsReleased,this,&Controller::gerenRequest);
    connect(settingButton,&Button::ButtonIsReleased,this,&Controller::settingScene);
    connect(messageButton,&Button::ButtonIsReleased,this,&Controller::messageRequest);
}

void Controller::chakanScene(int count)
{
    scene.clear();
    popButton = NULL;
    groundItem[0] = new Button(tr(""),0,-167,760,122);
    groundItem[1] = new Button(tr(""),0,25,760,122);
    groundItem[2] = new Button(tr(""),0,217,760,122);
    titleItem[0] = new Button(tr(""),0,-253,760,50);
    titleItem[1] = new Button(tr(""),0,-61,760,50);
    titleItem[2] = new Button(tr(""),0,131,760,50);
    picItem[0] = new Button(tr("No image"),-264,-167,192,108);
    picItem[1] = new Button(tr("No image"),-264,25,192,108);
    picItem[2] = new Button(tr("No image"),-264,217,192,108);
    priceItem[0] = new Button(tr(""),285,-167,150,50);
    priceItem[1] = new Button(tr(""),285,25,150,50);
    priceItem[2] = new Button(tr(""),285,217,150,50);
    mianjiTag[0] = new Button(tr(""),-65,-167,150,50);
    mianjiTag[1] = new Button(tr(""),-65,25,150,50);
    mianjiTag[2] = new Button(tr(""),-65,217,150,50);
    loucengTag[0] = new Button(tr(""),105,-167,150,50);
    loucengTag[1] = new Button(tr(""),105,25,150,50);
    loucengTag[2] = new Button(tr(""),105,217,150,50);
    chakanYema = new Button(tr("0"),-450,230,100,80,28,7);
    Pgroup = new QParallelAnimationGroup(this);
    for(int i=0;i<count;i++)
    {
        groundItem[i]->setFontSize(14);
        groundItem[i]->setBackColor(LightPink);
        groundItem[i]->setLineColor(Indigo);
        titleItem[i]->setBackColor(PaleGodenrod);
        titleItem[i]->setLineColor(DimGray);
        titleItem[i]->setFontSize(20);
        picItem[i]->setEnglish(true);
        picItem[i]->setBackColor(White);
        picItem[i]->setLineColor(Black);
        picItem[i]->setFontSize(20);
        priceItem[i]->setBackColor(NavajoWhite);
        priceItem[i]->setLineColor(DarkOrange);
        priceItem[i]->setFontSize(20);
        mianjiTag[i]->setBackColor(LightSkyBlue);
        mianjiTag[i]->setLineColor(RoyalBlue);
        mianjiTag[i]->setFontSize(20);
        loucengTag[i]->setBackColor(LightSalmon);
        loucengTag[i]->setLineColor(Crimson);
        loucengTag[i]->setFontSize(20);
        scene.addItem(titleItem[i]);
        scene.addItem(groundItem[i]);
        //scene.addItem(picItem[i]);
        scene.addItem(priceItem[i]);
        scene.addItem(mianjiTag[i]);
        scene.addItem(loucengTag[i]);
        Pgroup->addAnimation(appearAnimation(titleItem[i]));
        Pgroup->addAnimation(appearAnimation(groundItem[i]));
        //Pgroup->addAnimation(appearAnimation(picItem[i]));
        Pgroup->addAnimation(appearAnimation(priceItem[i]));
        Pgroup->addAnimation(appearAnimation(mianjiTag[i]));
        Pgroup->addAnimation(appearAnimation(loucengTag[i]));
    }
    setBackButton();
    setNextButton();
    setPreButton();
    scene.addItem(chakanYema);
    Pgroup->addAnimation(appearAnimation(backButton));
    Pgroup->addAnimation(appearAnimation(nextButton));
    Pgroup->addAnimation(appearAnimation(preButton));
    Pgroup->addAnimation(appearAnimation(chakanYema));
    Pgroup->start(QAbstractAnimation::DeleteWhenStopped);
    connect(backButton,&Button::ButtonIsReleased,this,&Controller::mainScene);
    if(released)connect(nextButton,&Button::ButtonIsReleased,this,&Controller::nextReleased);
    else connect(nextButton,&Button::ButtonIsReleased,this,&Controller::nextChakan);
    if(released)connect(preButton,&Button::ButtonIsReleased,this,&Controller::preReleased);
    else connect(preButton,&Button::ButtonIsReleased,this,&Controller::preChakan);
    connect(groundItem[0],&Button::ButtonIsReleased,this,&Controller::detailRequest1);
    connect(picItem[0],&Button::ButtonIsReleased,this,&Controller::detailRequest1);
    connect(titleItem[0],&Button::ButtonIsReleased,this,&Controller::detailRequest1);
    connect(groundItem[1],&Button::ButtonIsReleased,this,&Controller::detailRequest2);
    connect(picItem[1],&Button::ButtonIsReleased,this,&Controller::detailRequest2);
    connect(titleItem[1],&Button::ButtonIsReleased,this,&Controller::detailRequest2);
    connect(groundItem[2],&Button::ButtonIsReleased,this,&Controller::detailRequest3);
    connect(picItem[2],&Button::ButtonIsReleased,this,&Controller::detailRequest3);
    connect(titleItem[2],&Button::ButtonIsReleased,this,&Controller::detailRequest3);
    connect(Pgroup,&QParallelAnimationGroup::finished,this,&Controller::addPic);
}

void Controller::releasedScene(int count)
{
    scene.clear();
    popButton = NULL;
    chakanScene(count);
}

void Controller::detailScene()
{
    scene.clear();
    popButton = NULL;
    titleButon = new Button(tr("标题"),-330,-240,100,EDIT_LINE_HEIGHT,20);
    titleLine = new Button(tr(""),60,-240,640,EDIT_LINE_HEIGHT);
    addrButton = new Button(tr("地址"),-330,-144,100,EDIT_LINE_HEIGHT,20);
    addrLine = new Button(tr(""),60,-144,640,EDIT_LINE_HEIGHT);
    sizeButton = new Button(tr("面积"),-330,-48,100,EDIT_LINE_HEIGHT,20);
    sizeLine = new Button(tr(""),-140,-48,240,EDIT_LINE_HEIGHT);
    loucengButton = new Button(tr("楼层"),70,-48,100,EDIT_LINE_HEIGHT,20);
    loucengLine = new Button(tr(""),260,-48,240,EDIT_LINE_HEIGHT);
    picButton = new Button(tr("图片"),-330,48,100,EDIT_LINE_HEIGHT,20);
    picLine = new Button(tr("查看"),-140,48,240,EDIT_LINE_HEIGHT,20);
    priceButton = new Button(tr("价格"),70,48,100,EDIT_LINE_HEIGHT,20);
    priceLine = new Button(tr(""),260,48,240,EDIT_LINE_HEIGHT);
    psButton = new Button(tr("备注"),-330,144,100,EDIT_LINE_HEIGHT,20);
    psLine = new Button(tr(""),60,144,640,EDIT_LINE_HEIGHT);
    userButton = new Button(tr(""),-200,240,360,EDIT_LINE_HEIGHT,20);
    contactButton = new Button(tr("联系"),200,240,360,EDIT_LINE_HEIGHT,20);
    deleteButton = new Button(tr("删除"),200,240,360,EDIT_LINE_HEIGHT,20,1);
    scene.addItem(titleButon);
    scene.addItem(titleLine);
    scene.addItem(addrButton);
    scene.addItem(addrLine);
    scene.addItem(sizeButton);
    scene.addItem(sizeLine);
    scene.addItem(loucengButton);
    scene.addItem(loucengLine);
    scene.addItem(picButton);
    scene.addItem(picLine);
    scene.addItem(priceButton);
    scene.addItem(priceLine);
    scene.addItem(psButton);
    scene.addItem(psLine);
    scene.addItem(userButton);
    scene.addItem(contactButton);
    setDetailBackButton();
    Pgroup = new QParallelAnimationGroup(this);
    Pgroup->addAnimation(appearAnimation(titleButon));
    Pgroup->addAnimation(appearAnimation(titleLine));
    Pgroup->addAnimation(appearAnimation(addrButton));
    Pgroup->addAnimation(appearAnimation(addrLine));
    Pgroup->addAnimation(appearAnimation(sizeButton));
    Pgroup->addAnimation(appearAnimation(sizeLine));
    Pgroup->addAnimation(appearAnimation(loucengButton));
    Pgroup->addAnimation(appearAnimation(loucengLine));
    Pgroup->addAnimation(appearAnimation(picButton));
    Pgroup->addAnimation(appearAnimation(picLine));
    Pgroup->addAnimation(appearAnimation(priceButton));
    Pgroup->addAnimation(appearAnimation(priceLine));
    Pgroup->addAnimation(appearAnimation(psButton));
    Pgroup->addAnimation(appearAnimation(psLine));
    Pgroup->addAnimation(appearAnimation(userButton));
    Pgroup->addAnimation(appearAnimation(contactButton));
    Pgroup->addAnimation(appearAnimation(deleteButton));
    Pgroup->addAnimation(appearAnimation(detailBackButton));
    Pgroup->start(QAbstractAnimation::DeleteWhenStopped);
    connect(picLine,&Button::ButtonIsReleased,this,&Controller::showPic);
    if(released)connect(detailBackButton,&Button::ButtonIsReleased,this,&Controller::releasedRequest);
    else connect(detailBackButton,&Button::ButtonIsReleased,this,&Controller::chakanRequest);
    connect(userButton,&Button::ButtonIsReleased,this,&Controller::userRequest);
}

void Controller::fabuScene()
{
    scene.clear();
    popButton = NULL;
    detailScene();
    setBackButton();
    fileName = "";
    scene.removeItem(userButton);
    scene.removeItem(contactButton);
    scene.removeItem(picLine);
    scene.removeItem(detailBackButton);

    finishButton = new Button(tr("发布"),200,240,360,EDIT_LINE_HEIGHT,20);
    openPicLine = new Button(tr("打开"),-140,48,240,EDIT_LINE_HEIGHT);
    scene.addItem(finishButton);
    scene.addItem(openPicLine);
    Pgroup = new QParallelAnimationGroup(this);
    Pgroup->addAnimation(appearAnimation(finishButton));
    Pgroup->addAnimation(appearAnimation(openPicLine));
    Pgroup->addAnimation(appearAnimation(backButton));
    Pgroup->start(QAbstractAnimation::DeleteWhenStopped);
    titleLine->setEdit();
    addrLine->setEdit();
    sizeLine->setEdit();
    loucengLine->setEdit();
    priceLine->setEdit();
    psLine->setEdit();
    connect(backButton,&Button::ButtonIsReleased,this,&Controller::mainScene);
    connect(finishButton,&Button::ButtonIsReleased,this,&Controller::fabuRequest);
    connect(openPicLine,&Button::ButtonIsReleased,this,&Controller::openFile);
}

void Controller::gerenScene()
{
    scene.clear();
    popButton = NULL;
    idButton = new Button(tr("账号"),-330,-240,100,EDIT_LINE_HEIGHT,20);
    idLine = new Button(tr(""),60,-240,640,EDIT_LINE_HEIGHT,24);

    phoneButton = new Button(tr("电话"),-330,-48,100,EDIT_LINE_HEIGHT,20);
    phoneLine = new Button(tr(""),60,-48,640,EDIT_LINE_HEIGHT,24);
    mailButton = new Button(tr("邮箱"),-330,48,100,EDIT_LINE_HEIGHT,20);
    mailLine = new Button(tr(""),60,48,640,EDIT_LINE_HEIGHT,24);

    releasedButton = new Button(tr("已发布"),-200,240,360,EDIT_LINE_HEIGHT,20);
    xiugaiButton = new Button(tr("修改"),200,240,360,EDIT_LINE_HEIGHT,20);
    scene.addItem(idButton);
    scene.addItem(idLine);
    scene.addItem(phoneButton);
    scene.addItem(phoneLine);
    scene.addItem(mailButton);
    scene.addItem(mailLine);
    scene.addItem(releasedButton);
    scene.addItem(xiugaiButton);

    setBackButton();
    Pgroup = new QParallelAnimationGroup(this);
    Pgroup->addAnimation(appearAnimation(idButton));
    Pgroup->addAnimation(appearAnimation(idLine));
    Pgroup->addAnimation(appearAnimation(phoneButton));
    Pgroup->addAnimation(appearAnimation(phoneLine));
    Pgroup->addAnimation(appearAnimation(mailButton));
    Pgroup->addAnimation(appearAnimation(mailLine));
    Pgroup->addAnimation(appearAnimation(releasedButton));
    Pgroup->addAnimation(appearAnimation(xiugaiButton));
    Pgroup->addAnimation(appearAnimation(backButton));
    Pgroup->start(QAbstractAnimation::DeleteWhenStopped);
    connect(backButton,&Button::ButtonIsReleased,this,&Controller::mainScene);
    connect(xiugaiButton,&Button::ButtonIsReleased,this,&Controller::xiugaiScene);
    connect(releasedButton,&Button::ButtonIsReleased,this,&Controller::releasedRequest);
}

void Controller::xiugaiScene()
{
    scene.clear();
    popButton = NULL;
    gerenScene();
    idLine->setText(ID);
    mimaButton = new Button(tr("密码"),-330,-144,100,EDIT_LINE_HEIGHT,20);
    mimaLine = new Button(tr(""),60,-144,640,EDIT_LINE_HEIGHT,24);
    fangqiButton = new Button(tr("放弃修改"),-200,240,360,EDIT_LINE_HEIGHT,20);
    querenButton = new Button(tr("确认修改"),200,240,360,EDIT_LINE_HEIGHT,20);
    scene.addItem(mimaButton);
    scene.addItem(mimaLine);
    scene.addItem(fangqiButton);
    scene.addItem(querenButton);
    scene.removeItem(releasedButton);
    scene.removeItem(xiugaiButton);
    mimaLine->setEdit();
    phoneLine->setEdit();
    mailLine->setEdit();
    Pgroup = new QParallelAnimationGroup(this);
    Pgroup->addAnimation(appearAnimation(mimaButton));
    Pgroup->addAnimation(appearAnimation(mimaLine));
    Pgroup->addAnimation(appearAnimation(fangqiButton));
    Pgroup->addAnimation(appearAnimation(querenButton));
    Pgroup->start(QAbstractAnimation::DeleteWhenStopped);
    connect(fangqiButton,&Button::ButtonIsReleased,this,&Controller::gerenRequest);
    connect(querenButton,&Button::ButtonIsReleased,this,&Controller::xiugaiRequest);
}

void Controller::userScene()
{
    scene.clear();
    popButton = NULL;
    gerenScene();
    scene.removeItem(releasedButton);
    scene.removeItem(xiugaiButton);
    scene.removeItem(backButton);
    setUserBackButton();
    if(!inMessageScene){
        switch(click){
        case 1:
            connect(userBackButton,&Button::ButtonIsReleased,this,&Controller::detailRequest1);
            break;
        case 2:
            connect(userBackButton,&Button::ButtonIsReleased,this,&Controller::detailRequest2);
            break;
        case 3:
            connect(userBackButton,&Button::ButtonIsReleased,this,&Controller::detailRequest3);
            break;
        }
    }
    else connect(userBackButton,&Button::ButtonIsReleased,this,&Controller::messageRequest);
}

void Controller::settingScene()
{
    scene.clear();
    popButton = NULL;
    backColorButton = new Button(tr("更改背景颜色"),0,-100,480,100);
    checkUpdateButton = new Button(tr("检查更新"),0,100,480,100);
    versionButton = new Button(tr("Version %1").arg(version),350,230,300,80);
    backColorButton->setBackColor(Auqamarin);
    backColorButton->setLineColor(ForestGreen);
    checkUpdateButton->setBackColor(LightSkyBlue);
    checkUpdateButton->setLineColor(RoyalBlue);
    versionButton->setBackColor(LightSteelBlue);
    versionButton->setLineColor(LightSlateGray);
    backColorButton->setFontSize(40);
    checkUpdateButton->setFontSize(40);
    versionButton->setFontSize(28);
    scene.addItem(backColorButton);
    scene.addItem(checkUpdateButton);
    scene.addItem(versionButton);
    setBackButton();
    Pgroup = new QParallelAnimationGroup(this);
    Pgroup->addAnimation(appearAnimation(backColorButton));
    Pgroup->addAnimation(appearAnimation(checkUpdateButton));
    Pgroup->addAnimation(appearAnimation(versionButton));
    Pgroup->addAnimation(appearAnimation(backButton));
    Pgroup->start(QAbstractAnimation::DeleteWhenStopped);
    connect(backButton,&Button::ButtonIsReleased,this,&Controller::mainScene);
    connect(checkUpdateButton,&Button::ButtonIsReleased,this,&Controller::updateRequest);
    connect(backColorButton,&Button::ButtonIsReleased,this,&Controller::notFinish);
}

void Controller::messageScene(int count)
{
    scene.clear();
    popButton = NULL;
    inMessageScene = true;
    messageBox[0] = new Button(tr(""),0,-167,760,122);
    messageBox[1] = new Button(tr(""),0,25,760,122);
    messageBox[2] = new Button(tr(""),0,217,760,122);
    sender[0] = new Button(tr(""),-230,-253,300,50);
    sender[1] = new Button(tr(""),-230,-61,300,50);
    sender[2] = new Button(tr(""),-230,131,300,50);
    timButton[0] = new Button(tr(""),230,-253,300,50,20,3);
    timButton[1] = new Button(tr(""),230,-61,300,50,20,3);
    timButton[2] = new Button(tr(""),230,131,300,50,20,3);
    chakanYema = new Button(tr("0"),-450,230,100,80,28,7);
    chakanYema->setText(QString::number(messagePage+1));
    Pgroup = new QParallelAnimationGroup(this);
    for(int i=0;i<count;i++)
    {
        messageBox[i]->setFontSize(20);
        messageBox[i]->setBackColor(LightSkyBlue);
        messageBox[i]->setLineColor(DoderBlue);
        sender[i]->setBackColor(PaleGodenrod);
        sender[i]->setLineColor(DimGray);
        sender[i]->setFontSize(20);
        scene.addItem(sender[i]);
        scene.addItem(messageBox[i]);
        scene.addItem(timButton[i]);
        Pgroup->addAnimation(appearAnimation(sender[i]));
        Pgroup->addAnimation(appearAnimation(timButton[i]));
        Pgroup->addAnimation(appearAnimation(messageBox[i]));
    }
    setBackButton();
    setNextButton();
    setPreButton();
    scene.addItem(chakanYema);
    Pgroup->addAnimation(appearAnimation(backButton));
    Pgroup->addAnimation(appearAnimation(nextButton));
    Pgroup->addAnimation(appearAnimation(preButton));
    Pgroup->addAnimation(appearAnimation(chakanYema));
    Pgroup->start(QAbstractAnimation::DeleteWhenStopped);
    connect(sender[0],&Button::ButtonIsReleased,this,&Controller::senderRequest1);
    connect(sender[1],&Button::ButtonIsReleased,this,&Controller::senderRequest2);
    connect(sender[2],&Button::ButtonIsReleased,this,&Controller::senderRequest3);
    connect(messageBox[0],&Button::ButtonIsReleased,this,&Controller::talkRequest1);
    connect(messageBox[1],&Button::ButtonIsReleased,this,&Controller::talkRequest2);
    connect(messageBox[2],&Button::ButtonIsReleased,this,&Controller::talkRequest3);
    connect(backButton,&Button::ButtonIsReleased,this,&Controller::mainScene);
    connect(nextButton,&Button::ButtonIsReleased,this,&Controller::nextMessage);
    connect(preButton,&Button::ButtonIsReleased,this,&Controller::preMessage);
}

void Controller::zhuceScene()
{
    scene.clear();
    popButton = NULL;
    gerenScene();
    mimaButton = new Button(tr("密码"),-330,-144,100,EDIT_LINE_HEIGHT,20);
    mimaLine = new Button(tr(""),60,-144,640,EDIT_LINE_HEIGHT);
    zhuceButton = new Button(tr("注册"),200,240,360,EDIT_LINE_HEIGHT,20);
    scene.addItem(mimaButton);
    scene.addItem(mimaLine);
    scene.addItem(zhuceButton);
    scene.removeItem(releasedButton);
    scene.removeItem(xiugaiButton);
    scene.removeItem(backButton);
    setBackToLoginButton();
    idLine->setEdit();
    mimaLine->setEdit();
    phoneLine->setEdit();
    mailLine->setEdit();
    Pgroup = new QParallelAnimationGroup(this);
    Pgroup->addAnimation(appearAnimation(mimaButton));
    Pgroup->addAnimation(appearAnimation(mimaLine));
    Pgroup->addAnimation(appearAnimation(zhuceButton));
    Pgroup->addAnimation(appearAnimation(backToLoginButton));
    Pgroup->start(QAbstractAnimation::DeleteWhenStopped);
    connect(backToLoginButton,&Button::ButtonIsReleased,this,&Controller::loginScene);
    connect(zhuceButton,&Button::ButtonIsReleased,this,&Controller::zhuceRequest);
}

void Controller::talkScene()
{
    scene.clear();
    popButton = NULL;
    inTalkScene = true;
    talkUserButton = new Button(tr(""),0,-258,760,40,20,2);
    shuruButton = new Button("",-50,222+EDIT_LINE_HEIGHT/2,660,EDIT_LINE_HEIGHT);
    shuruButton->setEdit();
    fasongButton = new Button(tr("发送"),330,222+EDIT_LINE_HEIGHT/2,100,EDIT_LINE_HEIGHT,20,5);
    userMessageButton[4] = new Button("",-100,-188,560,80,20);
    userMessageButton[3] = new Button("",-100,-98,560,80,20);
    userMessageButton[2] = new Button("",-100,-8,560,80,20);
    userMessageButton[1] = new Button("",-100,82,560,80,20);
    userMessageButton[0] = new Button("",-100,172,560,80,20);
    myMessageButton[4] = new Button("",100,-188,560,80,20,3);
    myMessageButton[3] = new Button("",100,-98,560,80,20,3);
    myMessageButton[2] = new Button("",100,-8,560,80,20,3);
    myMessageButton[1] = new Button("",100,82,560,80,20,3);
    myMessageButton[0] = new Button("",100,172,560,80,20,3);
    setBackButton();
    scene.addItem(talkUserButton);
    scene.addItem(shuruButton);
    scene.addItem(fasongButton);
    Pgroup = new QParallelAnimationGroup(this);
    Pgroup->addAnimation(appearAnimation(talkUserButton));
    Pgroup->addAnimation(appearAnimation(shuruButton));
    Pgroup->addAnimation(appearAnimation(fasongButton));
    Pgroup->addAnimation(appearAnimation(backButton));
    for(int i=0;i<5;i++)
    {
        Pgroup->addAnimation(appearAnimation(userMessageButton[i]));
        Pgroup->addAnimation(appearAnimation(myMessageButton[i]));
    }
    Pgroup->start(QAbstractAnimation::DeleteWhenStopped);
    connect(fasongButton,&Button::ButtonIsReleased,this,&Controller::upMessageRequest);
    connect(backButton,&Button::ButtonIsReleased,this,&Controller::mainScene);
}

void Controller::sendMessage(quint16 type,QString msg)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_11);
    out<<(quint16)type<<msg;
    tcpSocket->write(block);
}

void Controller::readMessage(quint16 &readType,QString &readMsg)
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_11);
    in>>readType;
    in>>readMsg;
}

void Controller::sendFile(qint64 numByte)
{
    byteSend += (int)numByte;
    if(byteRes > 0){
        outBlock = picFile->read(qMin(byteRes,payloadSize));
        byteRes -= (int)tcpSocket->write(outBlock);
        outBlock.resize(0);
    }
    else {
        picFile->close();
    }
    if(byteSend == totalSize){
        picFile->close();
        disconnect(tcpSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(sendFile(qint64)));
    }
}

void Controller::fileReceive()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_11);
    qDebug()<<"fileReceive";
    if(byteRcv <= sizeof(qint64) * 2){
        if((tcpSocket->bytesAvailable() >= sizeof(qint64) * 2) && (fileNameSize == 0)){
            in>>totByte>>fileNameSize;
            byteRcv += sizeof(qint64) * 2;
        }
        if((tcpSocket->bytesAvailable() >= fileNameSize) && (fileNameSize != 0)){
            in>>fileName;
            byteRcv += fileNameSize;
            localFile = new QFile(fileName);
            if(!localFile->open(QFile::WriteOnly)){
                qDebug()<<"server: open file error!";
                return ;
            }
        }
        else return ;
    }
    if(byteRcv < totByte){
        byteRcv += tcpSocket->bytesAvailable();
        inBlock = tcpSocket->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }
    if(byteRcv == totByte){
        localFile->close();
        disconnect(tcpSocket,&QTcpSocket::readyRead,this,&Controller::fileReceive);
        connect(tcpSocket, &QTcpSocket::readyRead, this, &Controller::receivedata);
        if(!downloadFile)emit picFileReceive();
        else {
            deletePop();
            popWindow("更新文件下载成功");
        }
    }
}

void Controller::displayError(QTcpSocket::SocketError)
{
    qDebug()<<tcpSocket->errorString();
    tcpSocket->close();
    popWindow("连接错误",false);
}

void Controller::popWindow(QString s,bool removable)
{
    if(popButton == NULL)
    {
        popButton = new Button(s,0,0,640,360,28);
        popButton->setRandom(true);
        QPropertyAnimation *animation = new QPropertyAnimation(popButton, "scale");
        animation->setDuration(800);
        animation->setKeyValueAt(0,0);
        animation->setKeyValueAt(1,1);
        animation->setEasingCurve(QEasingCurve::OutBounce);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        scene.addItem(popButton);
        if(removable)connect(popButton,&Button::ButtonIsReleased,this,&Controller::disappearPop);
    }
}

void Controller::disappearPop()
{
    QPropertyAnimation *animation = new QPropertyAnimation(popButton, "scale");
    animation->setDuration(800);
    animation->setKeyValueAt(0,1);
    animation->setKeyValueAt(1,0);
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    connect(animation,&QAbstractAnimation::finished,this,&Controller::deletePop);
}

void Controller::deletePop()
{
    scene.removeItem(popButton);
    delete popButton;
    popButton = NULL;
}

void Controller::openFile()
{
    QWidget *widget = new QWidget;
    fileName = QFileDialog::getOpenFileName(widget,tr("Open File"),"",tr("Images (*.png *.jpg *.jpeg)"));
    if(fileName.isEmpty())
    {
        popWindow("打开失败");
        openPicLine->setText("打开");
        openPicLine->update();
    }
    else{
        QString currentFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/') - 1);
        openPicLine->setText(currentFileName);
        openPicLine->update();
    }
    qDebug()<<fileName;
    delete widget;
    widget = NULL;
}

QPropertyAnimation* Controller::appearAnimation(Button *b,int tim)
{
    QPropertyAnimation *animation = new QPropertyAnimation(b, "scale");
    animation->setDuration(tim);
    animation->setKeyValueAt(0,0);
    animation->setKeyValueAt(1,1);
    animation->setEasingCurve(QEasingCurve::OutExpo);
    return animation;
}

QPropertyAnimation* Controller::disappearAnimation(Button *b)
{
    QPropertyAnimation *animation = new QPropertyAnimation(b, "scale");
    animation->setDuration(800);
    animation->setKeyValueAt(0,1);
    animation->setKeyValueAt(1,0);
    animation->setEasingCurve(QEasingCurve::OutExpo);
    return animation;
}

QPropertyAnimation* Controller::posAnimation(Button *b)
{
    QPropertyAnimation *animation = new QPropertyAnimation(b, "pos");
    animation->setDuration(400);
    animation->setKeyValueAt(0,QPoint(b->x(),b->y()));
    animation->setKeyValueAt(1,QPoint(b->x(),b->y()-90));
    animation->setEasingCurve(QEasingCurve::OutExpo);
    return animation;
}

void Controller::myMessageAnimation(){
    appearAnimation(myMessageButton[(myKey-1+11)%11])->start(QAbstractAnimation::DeleteWhenStopped);
    scene.addItem(myMessageButton[(myKey-1+11)%11]);
}
void Controller::userMessageAnimation(){
    appearAnimation(userMessageButton[(userKey-1+11)%11])->start(QAbstractAnimation::DeleteWhenStopped);
    scene.addItem(userMessageButton[(userKey-1+11)%11]);
}
void Controller::updateMessage(bool b,QString s)
{
    Pgroup = new QParallelAnimationGroup(this);
    for(int i=0;i<11;i++){
        if(myMessageButton[i] != NULL)Pgroup->addAnimation(posAnimation(myMessageButton[i]));
        if(userMessageButton[i] != NULL)Pgroup->addAnimation(posAnimation(userMessageButton[i]));
    }
    for(int i=0;i<11;i++){

        if(myMessageButton[i] != NULL){
            if(myMessageButton[i]->y()<=-180){
                scene.removeItem(myMessageButton[i]);
                myMessageButton[i] = NULL;
            }
        }
        if(userMessageButton[i] != NULL){
            if(userMessageButton[i]->y()<=-180){
                scene.removeItem(userMessageButton[i]);
                userMessageButton[i] = NULL;
            }
        }
    }
    if(!b){
        if(userMessageButton[userKey] == NULL)
            userMessageButton[userKey] = new Button("",-100,172,560,80,20);
        else userMessageButton[userKey]->setPos(-100,172);
        userMessageButton[userKey]->setText(s);
        connect(Pgroup,&QAbstractAnimation::finished,this,&Controller::userMessageAnimation);
        userKey=(userKey+1)%11;
    }
    else{
         if(myMessageButton[myKey] == NULL)
             myMessageButton[myKey] = new Button("",100,172,560,80,20,3);
         else myMessageButton[myKey]->setPos(100,172);
         myMessageButton[myKey]->setText(s);
         connect(Pgroup,&QAbstractAnimation::finished,this,&Controller::myMessageAnimation);
         myKey=(myKey+1)%11;
    }
    Pgroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void Controller::setBackButton()
{
    backButton = new Button(tr("返回"),-450,-230,100,80);
    backButton->setBackColor(LightCyan);
    backButton->setLineColor(DarkTurquoise);
    backButton->setFontSize(28);
    scene.addItem(backButton);
}

void Controller::setDetailBackButton()
{
    detailBackButton = new Button(tr("返回"),-450,-230,100,80);
    detailBackButton->setBackColor(LightCyan);
    detailBackButton->setLineColor(DarkTurquoise);
    detailBackButton->setFontSize(28);
    scene.addItem(detailBackButton);
}

void Controller::setUserBackButton()
{
    userBackButton = new Button(tr("返回"),-450,-230,100,80);
    userBackButton->setBackColor(LightCyan);
    userBackButton->setLineColor(DarkTurquoise);
    userBackButton->setFontSize(28);
    scene.addItem(userBackButton);
}

void Controller::setQuitButton()
{
    quitButton = new Button(tr("退出"),450,-230,100,80);
    quitButton->setBackColor(LightCoral);
    quitButton->setLineColor(Crimson);
    quitButton->setFontSize(28);
    scene.addItem(quitButton);
}

void Controller::setPreButton()
{
    preButton = new Button(tr("上一页"),-450,0,100,80);
    preButton->setBackColor(LightYellow);
    preButton->setLineColor(Gold);
    preButton->setFontSize(28);
    scene.addItem(preButton);
}

void Controller::setNextButton()
{
    nextButton = new Button(tr("下一页"),450,0,100,80);
    nextButton->setBackColor(PowDerBlue);
    nextButton->setLineColor(SlateBlue);
    nextButton->setFontSize(28);
    scene.addItem(nextButton);
}

void Controller::setBackToLoginButton()
{
    backToLoginButton = new Button(tr("登录界面"),-450,-230,100,80);
    backToLoginButton->setBackColor(LightCyan);
    backToLoginButton->setLineColor(DarkTurquoise);
    backToLoginButton->setFontSize(24);
    scene.addItem(backToLoginButton);
}

void Controller::notFinish()
{
    popWindow("功能尚未开放");
}

void Controller::addPic()
{
    if(fileMsg[0] == "true")scene.addItem(picItem[0]);
    if(fileMsg[1] == "true")scene.addItem(picItem[1]);
    if(fileMsg[2] == "true")scene.addItem(picItem[2]);
}

void Controller::loadPic()
{
    showPicButton->setImage(QString("%1.jpg").arg(click));
}
void Controller::showPic()
{
    if(showPicButton == NULL)
    {
        showPicButton = new Button("",0,0,760,428);
        loadPic();
        QPropertyAnimation *animation = new QPropertyAnimation(showPicButton, "scale");
        animation->setDuration(800);
        animation->setKeyValueAt(0,0);
        animation->setKeyValueAt(1,1);
        animation->setEasingCurve(QEasingCurve::OutExpo);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        scene.addItem(showPicButton);
        //connect(animation,&QPropertyAnimation::finished,this,&Controller::loadPic);
        connect(showPicButton,&Button::ButtonIsReleased,this,&Controller::disappearPic);
    }
}
void Controller::disappearPic()
{
    QPropertyAnimation *animation = new QPropertyAnimation(showPicButton, "scale");
    animation->setDuration(800);
    animation->setKeyValueAt(0,1);
    animation->setKeyValueAt(1,0);
    animation->setEasingCurve(QEasingCurve::OutExpo);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    connect(animation,&QAbstractAnimation::finished,this,&Controller::deletePic);
}

void Controller::deletePic()
{
    scene.removeItem(showPicButton);
    delete showPicButton;
    showPicButton = NULL;
}

void Controller::nextChakan()
{
    chakanPage++;
    chakanRequest();
}
void Controller::preChakan()
{
    if(chakanPage>0)
    {
        chakanPage--;
        chakanRequest();
    }
}
void Controller::nextReleased()
{
    releasedPage++;
    releasedRequest();
}
void Controller::preReleased()
{
    if(releasedPage>0)
    {
        releasedPage--;
        releasedRequest();
    }
}
void Controller::nextMessage()
{
    messagePage++;
    messageRequest();
}
void Controller::preMessage()
{
    if(messagePage>0)
    {
        messagePage--;
        messageRequest();
    }
}

Controller::~Controller()
{

}
