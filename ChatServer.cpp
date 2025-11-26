#include "Chatserver.h"
#include <QDebug>

ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent), nextsocketid(1) {}



void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    int id  = nextsocketid++;
    UserConnection user{id, QString("User%1").arg(id), socket};
    users[id] = user;

    qDebug() << "新用户连接: ID =" << id
             << " 名字 =" << user.name
             << " 地址 =" << socket->peerAddress().toString()
             << " 端口 =" << socket->peerPort();

}



bool ChatServer::start(quint16 port)
{
    return listen(QHostAddress::Any, port);
}

