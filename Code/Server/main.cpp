#include "server.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","mainConnect");
    db.setDatabaseName("database.db");
    if(!db.open())
        return 1;
    QSqlQuery query(db);
    query.exec("create table if not exists user (id varchar(20) primary key,password varchar(20),phone varchar(20),mail varchar(40))");
    //query.exec("drop table mesg");
    query.exec("create table if not exists item (id int primary key,user varchar(20),title varchar(40),addr varchar(40),size varchar(20),louceng varchar(20),price varchar(20),ps varchar(40),time varchar(20),pic blob)");
    query.exec("create table if not exists mesg (id int primary key,receiver varchar(20),sender varchar(20),message varchar(40),time varchar(20),read int)");
    //query.exec("drop table message");
    //query.exec("update user set mail = 'fox@hotmail.com' where id = 'admin'");
    //query.exec("create table if not exists item (id int primary key,password varchar(20),phone varchar(20),mail varchar(40))");
    //query.exec("create table if not exists message (id varchar(20) primary key,password varchar(20),phone varchar(20),mail varchar(40))");
    //QSqlDatabase::removeDatabase("mainConnect");db.close();
    */
    Server w;
    w.show();
    return a.exec();
}
