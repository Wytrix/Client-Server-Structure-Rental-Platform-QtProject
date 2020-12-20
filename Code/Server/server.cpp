#include "server.h"
#include "ui_server.h"
#include <QtNetwork>

Server::Server(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    port = 6666;
    tcpServer = new TcpServer(this,port);
    connect(tcpServer,&TcpServer::updateserver,this,&Server::slotupdateserver);
    QSqlDatabase db;
    initDB(db);
    QSqlQuery query(db);
    query.exec("create table if not exists user (id varchar(20) primary key,password varchar(20),phone varchar(20),mail varchar(40))");
    query.exec("create table if not exists item (id int primary key,user varchar(20),title varchar(40),addr varchar(40),size varchar(20),louceng varchar(20),price varchar(20),ps varchar(40),time varchar(20),pic blob)");
    query.exec("create table if not exists mesg (id int primary key,receiver varchar(20),sender varchar(20),message varchar(40),time varchar(20),read int)");
}

void Server::slotupdateserver(QString msg)
{
    ui->textBrowser->append(msg);
}
Server::~Server()
{
    delete ui;
}

void Server::initDB(QSqlDatabase &db)
{
    if(QSqlDatabase::contains("mainConnect"))
        db = QSqlDatabase::database("mainConnect");
    else
        db = QSqlDatabase::addDatabase("QSQLITE", "mainConnect");
    db.setDatabaseName("database.db");
    db.open();
}

void Server::on_pushButton_clicked()
{
    QSqlDatabase db;
    initDB(db);
    QSqlQuery query(db);
    QString cmd = ui->lineEdit->text();
    QString State;
    if(query.exec(cmd))State = "true";
    else State = "fasle";
    slotupdateserver("system:"+State+"  "+cmd);
}
