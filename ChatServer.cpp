#include "Chatserver.h"
#include <QDebug>

ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent), nextsocketid(1) {}



void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* socket = new QTcpSocket(this);
    if(!socket->setSocketDescriptor(socketDescriptor))
    {
        socket->deleteLater();
        return;
    }

    int id  = nextsocketid++;
    UserConnection user{id, QString("User%1").arg(id), socket};
    users[id] = user;

    qDebug() << "新用户连接: ID =" << id
             << " 名字 =" << user.name
             << " 地址 =" << socket->peerAddress().toString()
             << " 端口 =" << socket->peerPort();

    connect(socket, &QTcpSocket::readyRead, this, [this, id]{
        static thread_local QByteArray buffer;
        buffer += users[id].socket->readAll();
        while(buffer.size() >= 4){
            quint32 len = qFromBigEndian<quint32>(
                reinterpret_cast<const uchar*>(buffer.constData()));
            if (buffer.size() -4 < static_cast<int>(len))
                break;
            QByteArray payload = buffer.mid(4, len);
            buffer.remove(0,4 + len);
            broadCast(payload);
        }
    });
}

void ChatServer::broadCast(const QByteArray &payload)
{
    QByteArray frame;
    frame.resize(4 + payload.size());
    qToBigEndian<quint32>(payload.size(), reinterpret_cast<uchar *>(frame.data()));
    memcpy(frame.data() + 4, payload.constData(),payload.size());
    for(auto c :users){
        if(c.socket->state() == QAbstractSocket::ConnectedState)
            c.socket->write(frame);
    }
}



bool ChatServer::start(quint16 port)
{
    return listen(QHostAddress::Any, port);
}

