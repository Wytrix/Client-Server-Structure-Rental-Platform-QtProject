#ifndef SERVER_H
#define SERVER_H

#include "header.h"
#include "tcpserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QMainWindow
{
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);
    ~Server();

private:
    Ui::Server *ui;
    int port;
    TcpServer *tcpServer;
    void initDB(QSqlDatabase &db);

protected slots:
    void slotupdateserver(QString);

private slots:
    void on_pushButton_clicked();
};
#endif // SERVER_H
