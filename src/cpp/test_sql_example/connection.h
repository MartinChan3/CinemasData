#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QVariant>

static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../cinemas.db");
    if (!db.open())
        return false;
    QSqlQuery query;
    query.exec("select name from sqlite_master where type='table'");
    while (query.next())
    {
        if (QString("cinemas") == query.value(0).toString())
            return true;   //检测到有cinema表
    }

    return false;
}

#endif // CONNECTION_H
