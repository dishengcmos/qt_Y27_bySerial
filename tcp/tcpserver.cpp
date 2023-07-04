#include "tcpserver.h"
#include <QDebug>

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{
    m_server = new QTcpServer(this);
    dataHandle = nullptr;
    //新连接信号和槽
    connect(m_server,SIGNAL(newConnection()),this,SLOT(doProcessNewConnect()));
}

// 创建开启一个服务器
// 参数：侦听的ip，端口
// 返回：true创建成功，false创建失败
bool TcpServer::openServer(QString ip, uint16_t port)
{
    return m_server->listen(QHostAddress(ip), port);
}

void TcpServer::closeServer()
{
    m_server->close();
}

//处理有client的socket连接
void TcpServer::doProcessNewConnect()
{
    QTcpSocket *t_socket = m_server->nextPendingConnection();//获取客户端描述符
    m_socketArray.push_back(t_socket);
    qDebug()<<"connect: "<<t_socket->localAddress();
    t_socket->write("welcome");

    connect(t_socket, SIGNAL(readyRead()), this, SLOT(clientHandle())); /* 接受客户端数据的槽函数 */
    connect(t_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &TcpServer::clientErrorHandle); /* 错误反馈 */
    connect(t_socket, SIGNAL(disconnected()), this, SLOT(clientDisconnectHandle())); /* 断开反馈 */
}

//客户端发来的数据处理
void TcpServer::clientHandle()
{
    QTcpSocket *t_socket = qobject_cast<QTcpSocket *>(sender());
    QByteArray data = t_socket->readAll();
    if (dataHandle){
        dataHandle(t_socket, data);
    }
    else
    {
        emit clientStrFinish(data);
    }
//    if(t_socket == m_socketArray.back()){
//        qDebug()<<"1";
//    }
//    else{
//        qDebug()<<"2";
//    }
    QString s = data;
    qDebug()<<s;
}
//客户端出错(一般是断连)
void TcpServer::clientErrorHandle(void)
{
    qDebug()<<"client error";
}
//客户端断连
void TcpServer::clientDisconnectHandle()
{
    QTcpSocket *t_socket = qobject_cast<QTcpSocket *>(sender());
    for(int i = 0; i < m_socketArray.size(); i++) {
        if (m_socketArray.at(i) == t_socket) {
            m_socketArray.remove(i);
            qDebug()<<"remoted";
        }
    }
    qDebug()<<"disconnected";
}
