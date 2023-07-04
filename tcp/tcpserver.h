#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);

    QTcpServer *m_server;
    QVector <QTcpSocket *> m_socketArray;

    void (* dataHandle)(QTcpSocket *socket, QByteArray data);
    bool openServer(QString ip, uint16_t port);
    void closeServer(void);
signals:
    void clientDataFinish(QByteArray data);
    void clientStrFinish(QString data);
public slots:
    void doProcessNewConnect(void);

    void clientHandle(void);
    void clientErrorHandle(void);
    void clientDisconnectHandle(void);
};

#endif // TCPSERVER_H
