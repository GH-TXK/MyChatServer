#pragma once
#include <QSet>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtEndian>
#include <QMap>
#include <QString>

struct UserConnection {
    int id;
    QString name;
    QTcpSocket* socket;
};


class ChatServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit ChatServer(QObject *parent = nullptr);
    bool start(quint16 port);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    void removeClient(QTcpSocket *s);
    // void extracted(QByteArray &frame);
    void broadCast(const QByteArray &payload);
    int nextsocketid;
    QMap<int, UserConnection> users;
};
