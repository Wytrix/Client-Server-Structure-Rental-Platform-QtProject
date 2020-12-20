#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Header.h"
#include "button.h"

class QGraphicsScene;
class QKeyEvent;
class QGraphicsView;
class Button;
class QGraphicsTextItem;
class QFile;

class Controller : public QObject
{
    Q_OBJECT
public:
    Controller(QGraphicsScene &scene, QGraphicsView &view, QObject *parent = 0);
    ~Controller();

private:
    //场景
    void loginScene();
    void mainScene();
    void chakanScene(int count);
    void detailScene();
    void fabuScene();
    void gerenScene();
    void settingScene();
    void messageScene(int count);
    void zhuceScene();
    void talkScene();
    void xiugaiScene();
    void userScene();
    void releasedScene(int count);
    void popWindow(QString s,bool b = true);
    void disappearPop();
    void deletePop();
    void openFile();
    void nextChakan();
    void preChakan();
    void nextReleased();
    void preReleased();
    void nextMessage();
    void preMessage();

    //按钮设置
    void setBackButton();
    void setQuitButton();
    void setNextButton();
    void setPreButton();
    void setBackToLoginButton();
    void setDetailBackButton();
    void setUserBackButton();

    //网络通信请求和响应
    void loginRequest();
    void loginReply();
    void zhuceRequest();
    void zhuceReply();
    void gerenRequest();
    void gerenReply();
    void xiugaiRequest();
    void xiugaiReply();
    void updateRequest();
    void updateReply();
    void fabuRequest();
    void fabuReply();
    void chakanRequest();
    void chakanReply();
    void detailRequest1();
    void detailRequest2();
    void detailRequest3();
    void detailReply();
    void userRequest();
    void userReply();
    void releasedRequest();
    void releasedReply();
    void deleteItemRequest();
    void deleteItemReply();
    void messageRequest();
    void messageReply();
    void talkRequest();
    void talkReply();
    void talkRequest1();
    void talkRequest2();
    void talkRequest3();
    void senderRequest1();
    void senderRequest2();
    void senderRequest3();
    void upMessageRequest();
    void upMessageReply();
    void downloadNewClientRequest();
    void downloadNewClientReply();

    //文件操作相关
    void fileReply();
    void addPic();
    void disappearPic();
    void showPic();
    void loadPic();
    void deletePic();
    void updateMessage(bool b,QString s);
    void userMessageAnimation();
    void myMessageAnimation();
    void notFinish();

    //动画组
    QPropertyAnimation *appearAnimation(Button *b,int tim = 800);
    QPropertyAnimation *disappearAnimation(Button *b);
    QPropertyAnimation *posAnimation(Button *b);

    QParallelAnimationGroup *Pgroup;
    QSequentialAnimationGroup *Sgroup;
    QGraphicsScene &scene;
    QGraphicsView &view;
    QTimer timer;
    QTcpSocket *tcpSocket;
    quint16 readType;
    QString readMsg;
    QString ID;
    //发送图片
    QFile *picFile;
    QString fileName;
    QByteArray outBlock;
    qint64 byteSend;
    qint64 fileSize;
    qint64 totalSize;
    qint64 byteRes;
    //接收图片
    qint64 totByte;
    qint64 byteRcv;
    qint64 fileNameSize;
    QFile *localFile;
    QByteArray inBlock;
    int Page;
    int port;
    int chakanPage;
    int releasedPage;
    int picFileCounter;
    bool released;
    int messagePage;
    int userKey;
    int myKey;
    bool downloadFile;
    //common
    Button *quitButton;
    Button *backButton;
    Button *backToLoginButton;
    Button *nextButton;
    Button *preButton;
    Button *popButton;
    Button *detailBackButton;
    Button *userBackButton;
    Button *showPicButton;
    //loginScene
    Button *loginButton;
    Button *accountButton;
    Button *passwordButton;
    Button *forgetButton;
    Button *signButton;
    Button *accountLine;
    Button *passwordLine;
    //mainScene
    Button *chakanButton;
    Button *fabuButton;
    Button *gerenButton;
    Button *settingButton;
    Button *messageButton;
    //chakanScene && releasedScene
    Button *groundItem[3];
    Button *titleItem[3];
    Button *priceItem[3];
    Button *picItem[3];
    Button *mianjiTag[3];
    Button *loucengTag[3];
    Button *chakanYema;

    QString chakanID[3];
    QString fileMsg[3];
    QString title[3];
    QString size[3];
    QString louceng[3];
    QString price[3];
    int click;
    //fabuScene && detailScene
    Button *titleButon;
    Button *titleLine;
    Button *addrButton;
    Button *addrLine;
    Button *sizeButton;
    Button *sizeLine;
    Button *loucengButton;
    Button *loucengLine;
    Button *picButton;
    Button *picLine;
    Button *openPicLine;
    Button *priceButton;
    Button *priceLine;
    Button *psButton;
    Button *psLine;
    Button *userButton;
    Button *contactButton;
    Button *finishButton;
    Button *deleteButton;
    QString user;
    //gerenScene && zhuceScene && xiugaiScene && userScene
    Button *idButton;
    Button *idLine;
    Button *mimaButton;
    Button *mimaLine;
    Button *phoneButton;
    Button *phoneLine;
    Button *mailButton;
    Button *mailLine;
    Button *releasedButton;
    Button *zhuceButton;
    Button *xiugaiButton;
    Button *fangqiButton;
    Button *querenButton;
    //settingScene
    Button *backColorButton;
    Button *checkUpdateButton;
    Button *versionButton;
    //messageScene
    Button *messageBox[3];
    Button *sender[3];
    Button *timButton[3];
    QString sendUser[3];
    QString messageTime[3];
    QString message[3];
    bool inMessageScene;
    //talkScene
    Button *talkUserButton;
    Button *userMessageButton[11];
    Button *myMessageButton[11];
    Button *shuruButton;
    Button *fasongButton;
    bool inTalkScene;

private slots:
    void newConnect();
    void sendMessage(quint16, QString);
    void readMessage(quint16&,QString&);
    void sendFile(qint64);
    void fileReceive();
    void picFileCount();
    void displayError(QAbstractSocket::SocketError);
    void receivedata();
signals:
    void picFileReceive();


};

#endif // CONTROLLER_H
