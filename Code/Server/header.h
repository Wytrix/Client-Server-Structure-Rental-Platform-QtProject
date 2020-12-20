#ifndef HEADER_H
#define HEADER_H

#include <QTcpSocket>
#include <QDataStream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFile>
#include <QFileDialog>
#include <QTcpServer>
#include <QObject>
#include <QList>
#include <QMainWindow>
#include <QAbstractSocket>
#include <QDateTime>
#include <QCoreApplication>

const QString version = "0.0.1";
const qint64 payloadSize = 64 * 1024;

#endif // HEADER_H
