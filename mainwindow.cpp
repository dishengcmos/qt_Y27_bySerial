﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QtNetwork>
#include <QTableWidgetItem>

uint8_t G_mark1=0xff;
unsigned char LedContraldata[11]={0xfd,0xfd,0X01,0X06, 0x01, 0x03,0x03,0x03,0xff, 0x00, 0x00};

QString MainWindow::getLocalIP()
{//获取本机IPv4地址
    QString hostName=QHostInfo::localHostName();//本地主机名
    QHostInfo   hostInfo=QHostInfo::fromName(hostName);
    QString   localIP="";

    QList<QHostAddress> addList=hostInfo.addresses();//

    if (!addList.isEmpty())
    for (int i=0;i<addList.count();i++)
    {
        QHostAddress aHost=addList.at(i);
        if (QAbstractSocket::IPv4Protocol==aHost.protocol())
        {
            localIP=aHost.toString();
            break;
        }
    }
    return localIP;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->SliderGreen,SIGNAL(valueChanged(int)),  //关联 SliderGreen 的valueChanged()
                     this,SLOT(on_SliderRed_valueChanged(int)));

    QObject::connect(ui->SliderBlue,SIGNAL(valueChanged(int)), //关联 SliderBlue的valueChanged()
                     this,SLOT(on_SliderRed_valueChanged(int)));

    LabListen=new QLabel("监听状态:");
    LabListen->setMinimumWidth(150);

    LabSocketState=new QLabel("Socket状态：");//
    LabSocketState->setMinimumWidth(200);

    QString localIP=getLocalIP();//本机IP
    this->setWindowTitle(this->windowTitle()+"----本机IP："+localIP);
    ui->comboIP->addItem(localIP);
    tcpServer=new QTcpServer(this);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(onNewConnection()));

    QList<QSerialPortInfo> list;
    connect(ui->m_sendButton,SIGNAL(clicked()),this,SLOT(send()));
    list = QSerialPortInfo::availablePorts();
    for (int i = 0; i < list.size(); i++)
    {
        qDebug() << list.at(i).portName() ;     //串口名字
        qDebug() << list.at(i).description();   //串口描述
        qDebug() << "#########";

        port = new QSerialPort(list.at(i).portName());//通过名字打开，可通过上面的list里查找
        /*根据需求对串口进行进行配置*/
        port->setBaudRate(QSerialPort::Baud115200);//配置波特率位115200

        port->setDataBits(QSerialPort::Data8);//配置数据格式为8bit
        port->setParity(QSerialPort::NoParity);//无校验位
        port->setStopBits(QSerialPort::OneStop);//一个停止位
        port->setFlowControl(QSerialPort::NoFlowControl);//不使用流控制
        /*打开设备*/
        if (!port->open(QIODevice::ReadWrite))
        {
             qDebug() << "serial open failed";
        }
        else
        {
             qDebug() << "serial open success";
             /*绑定接受槽函数，但收到信息是将调用MainSerialRecvMsgEvent函数*/
             connect(port, &QSerialPort::readyRead, this, &MainWindow::handle);
        }
        break;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handle()
{
    static uint8_t recieveData[8];
    static uint8_t step=0x01;
    QByteArray readnum=port->readAll();
    qDebug()<<"size:"<<readnum.length();

    for(uint8_t i=0;i<readnum.length();i++)
    {
        qDebug()<<(int)(readnum[i]);
        recieveData[i]=(int)(readnum[i]);
    }

    ui->m_textEdit->append(readnum);

    switch(step)
    {
    case 0x01:
        if((recieveData[0]==0xfc)&&(recieveData[1]==0xfc))
            step=0x02;
        else
            step=0x01;
        break;
    case 0x02:
        if(recieveData[2]==0x01)
            step=0x03;
        else
            step=0x01;
        break;

    case 0x03:
    {
        uint16_t datah,datal;
        uint16_t data=0x00;
        datah=recieveData[4];
        datal=recieveData[5];
        data=(datah<<8)|datal;

        ui->progBarV->setValue(data);
        ui->SliderV->setValue(data);
        break;
    }
    default:
        break;
    }

    //bool ok;
    //int val=readnum.toInt(&ok,16);            //以十六进制数读入
    //QByteArray str=QByteArray::number(val,10);//显示为10进制字符串
    //ui->m_textEdit->append(str);
    //ui->progBarV->setValue(val);
    //ui->SliderV->setValue(val);
}

void MainWindow::send()
{
    //port->write("1234");
    QString rednum;
    rednum=ui->SliderRed->value();
    //qDebug()<<ui->m_inputEdit->toPlainText();
    port->write( rednum.toUtf8());
    //port->write(ui->m_inputEdit->toPlainText().toUtf8());
}

/**********************************wifi*************************************/

void MainWindow::onNewConnection()
{
//  ui->plainTextEdit->appendPlainText("有新连接");
    tcpSocket = tcpServer->nextPendingConnection(); //创建socket

    connect(tcpSocket, SIGNAL(connected()),
            this, SLOT(onClientConnected()));
    onClientConnected();//

    connect(tcpSocket, SIGNAL(disconnected()),
            this, SLOT(onClientDisconnected()));

    connect(tcpSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this,SLOT(onSocketStateChange(QAbstractSocket::SocketState)));
    onSocketStateChange(tcpSocket->state());

    connect(tcpSocket,SIGNAL(readyRead()),
            this,SLOT(onSocketReadyRead()));
}

void MainWindow::onSocketStateChange(QAbstractSocket::SocketState socketState)
{//socket状态变化时
    switch(socketState)
    {
    case QAbstractSocket::UnconnectedState:
        LabSocketState->setText("scoket状态：UnconnectedState");
        break;
    case QAbstractSocket::HostLookupState:
        LabSocketState->setText("scoket状态：HostLookupState");
        break;
    case QAbstractSocket::ConnectingState:
        LabSocketState->setText("scoket状态：ConnectingState");
        break;

    case QAbstractSocket::ConnectedState:
        LabSocketState->setText("scoket状态：ConnectedState");
        break;

    case QAbstractSocket::BoundState:
        LabSocketState->setText("scoket状态：BoundState");
        break;

    case QAbstractSocket::ClosingState:
        LabSocketState->setText("scoket状态：ClosingState");
        break;

    case QAbstractSocket::ListeningState:
        LabSocketState->setText("scoket状态：ListeningState");
    }
}

void MainWindow::onClientConnected()
{
    // 客户端接入时
    ui->plainTextEdit->appendPlainText("**client socket connected");
    ui->plainTextEdit->appendPlainText("**peer address:"+
                                   tcpSocket->peerAddress().toString());
    ui->plainTextEdit->appendPlainText("**peer port:"+
                                   QString::number(tcpSocket->peerPort()));
}

void MainWindow::onSocketReadyRead()
{//读取缓冲区行文本
//    QStringList   lines;
    while(tcpSocket->canReadLine())
        ui->plainTextEdit->appendPlainText("[in] "+tcpSocket->readLine());
//      lines.append(clientConnection->readLine());
}

void MainWindow::on_turnonwifi_clicked()
{
    //开始监听
    QString     IP=ui->comboIP->currentText();//IP地址
    quint16     port=ui->spinPort->value();//端口
    QHostAddress    addr(IP);
    tcpServer->listen(addr,port);//
    //  tcpServer->listen(QHostAddress::LocalHost,port);// Equivalent to QHostAddress("127.0.0.1").
    ui->plainTextEdit->appendPlainText("**开始监听...");
    ui->plainTextEdit->appendPlainText("**服务器地址："
                       +tcpServer->serverAddress().toString());
    ui->plainTextEdit->appendPlainText("**服务器端口："
                       +QString::number(tcpServer->serverPort()));


    LabListen->setText("监听状态：正在监听");
}


void MainWindow::on_sendwifi_clicked()
{
    // 发送一行字符串，以换行符结束
    QString  msg=ui->lineEdit->text();
    ui->plainTextEdit->appendPlainText("[out] "+msg);
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();

    QByteArray  str=msg.toUtf8();
    str.append('\n');//添加一个换行符
    tcpSocket->write(str);
}

/******
void MainWindow::onClientDisconnected()
{//客户端断开连接时
    ui->plainTextEdit->appendPlainText("**client socket disconnected");
    tcpSocket->deleteLater();
    //    deleteLater();//QObject::deleteLater();
}
******/
/*********
void MainWindow::on_actStop_triggered()
{//停止监听
    if (tcpServer->isListening()) //tcpServer正在监听
    {
        tcpServer->close();//停止监听
        ui->actStart->setEnabled(true);
        ui->actStop->setEnabled(false);
        LabListen->setText("监听状态：已停止监听");
    }
}

void MainWindow::on_actClear_triggered()
{
    ui->plainTextEdit->clear();
}
*********/

/**************************************************************/
void MainWindow::on_SliderRed_valueChanged(int value)
{
    //拖动Red、Green、Blue 颜色滑动条
    Q_UNUSED(value);
    QColor  color;
    int R=ui->SliderRed->value();  //读取SliderRed的当前值
    int G=ui->SliderGreen->value();//读取 SliderGreen 的当前值
    int B=ui->SliderBlue->value();//读取 SliderBlue 的当前值

    color.setRgb(R,G,B,255); //使用QColor的setRgb()函数 获得颜色

    QPalette pal=ui->textEdit_2->palette();//获取textEdit原有的 palette

    pal.setColor(QPalette::Base,color); //设置palette的基色（即背景色）

    ui->textEdit_2->setPalette(pal);//设置为textEdit的palette,改变textEdit的底色

    /****LED控制数据发送****/
    QByteArray rednum;
    rednum.resize(11);
    for(uint8_t i=0;i<=10;i++)
    {
        rednum[i]= LedContraldata[i];
    }
    rednum[5]= ui->SliderRed->value();
    rednum[6]= ui->SliderGreen->value();
    rednum[7]= ui->SliderBlue->value();
    rednum[8]= G_mark1;
    for(uint8_t i=0;i<=7;i++)
    {
        rednum[10]=rednum[10]+ rednum[2+i];
    }
    port->write( rednum);
    qDebug()<<rednum;
    ui->m_outputEdit->append(rednum.toHex());
}

/********************************LED灯选择控制*******************************************/
void MainWindow::on_checkBox_1_clicked(bool checked)
{
    if(checked==true)
        G_mark1|=0x01;
    else
        G_mark1&=0xfe;
}

void MainWindow::on_checkBox_2_clicked(bool checked)
{
    if(checked==true)
        G_mark1|=0x02;
    else
        G_mark1&=0xfd;
}

void MainWindow::on_checkBox_3_clicked(bool checked)
{
    if(checked==true)
        G_mark1|=0x04;
    else
        G_mark1&=0xfb;
}

void MainWindow::on_checkBox_4_clicked(bool checked)
{
    if(checked==true)
        G_mark1|=0x08;
    else
        G_mark1&=0xf7;
}

void MainWindow::on_checkBox_5_clicked(bool checked)
{
    if(checked==true)
        G_mark1|=0x10;
    else
        G_mark1&=0xef;
}

void MainWindow::on_checkBox_6_clicked(bool checked)
{
    if(checked==true)
        G_mark1|=0x20;
    else
        G_mark1&=0xdf;
}

void MainWindow::on_checkBox_7_clicked(bool checked)
{
    if(checked==true)
        G_mark1|=0x40;
    else
        G_mark1&=0xbf;
}

void MainWindow::on_checkBox_8_clicked(bool checked)
{
    if(checked==true)
        G_mark1|=0x80;
    else
        G_mark1&=0x7f;
}

void MainWindow::createItemsARow(int rowNo,QString Name,QString Sex,int shakeNum,int tofHigh,bool isPM,int score)
{ //为一行的单元格创建 Items
    QTableWidgetItem    *item;
    QString str;
    uint StudID=201605000; //学号基数

//姓名
    //新建一个Item，设置单元格type为自定义的MainWindow::ctName
    item=new  QTableWidgetItem(Name,MainWindow::ctName);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter); //文本对齐格式
    StudID  +=rowNo; //学号=基数+ 行号
    item->setData(Qt::UserRole,QVariant(StudID));  //设置studID为data
    ui->tableInfo->setItem(rowNo,MainWindow::colName,item); //为单元格设置Item

//性别
    QIcon   icon;
    if (Sex=="男")
        icon.addFile(":/images/icons/boy.ico");
    else
        icon.addFile(":/images/icons/girl.ico");
    item=new  QTableWidgetItem(Sex,MainWindow::ctSex); //新建一个Item，设置单元格type为自定义的 MainWindow::ctSex
    item->setIcon(icon);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//为单元格设置Item
    ui->tableInfo->setItem(rowNo,MainWindow::colSex,item);//为单元格设置Item

//出生日期
    str.setNum(shakeNum);//
    item=new  QTableWidgetItem(str,MainWindow::ctBirth);//新建一个Item，设置单元格type为自定义的 MainWindow::ctBirth
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter); //文本对齐格式
    ui->tableInfo->setItem(rowNo,MainWindow::colBirth,item);//为单元格设置Item

//民族
    str.setNum(tofHigh);//
    item=new  QTableWidgetItem(str,MainWindow::ctNation); //新建一个Item，设置单元格type为自定义的 MainWindow::ctNation
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//文本对齐格式
    ui->tableInfo->setItem(rowNo,MainWindow::colNation,item);//为单元格设置Item

//是否党员
    item=new  QTableWidgetItem("布尔",MainWindow::ctPartyM);//新建一个Item，设置单元格type为自定义的 MainWindow::ctPartyM
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//文本对齐格式
    if (isPM)
        item->setCheckState(Qt::Checked);
    else
        item->setCheckState(Qt::Unchecked);
//    item->setBackgroundColor(Qt::yellow);     // setBackgroundColor()函数已经deprecated
    item->setBackground(QBrush(Qt::yellow));    //Qt::green  lightGray  yellow
    ui->tableInfo->setItem(rowNo,MainWindow::colPartyM,item);//为单元格设置Item

//分数
    str.setNum(score);
    item=new  QTableWidgetItem(str,MainWindow::ctScore);//新建一个Item，设置单元格type为自定义的 MainWindow::ctPartyM
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//文本对齐格式
    ui->tableInfo->setItem(rowNo,MainWindow::colScore,item);//为单元格设置Item
}

void MainWindow::on_btnIniData_clicked()
{ //初始化表格内容
    QString strName,strSex;
    bool    isParty=false;


    ui->tableInfo->clearContents();//只清除工作区，不清除表头

    int Rows=ui->tableInfo->rowCount(); //数据区行数，
    for (int i=0;i<Rows;i++) //数据区第1行的编号为0，所以范围是0~rowCount()-1
    {
        strName=QString::asprintf("模块%d",i); //模块名
        if ((i % 2)==0) //分奇数，偶数行设置性别，及其图标
            strSex="开";
        else
            strSex="关";

        createItemsARow(i, strName, strSex, 0,0,isParty,70);// 为某一行创建 items

        isParty =!isParty;
    }
}

void MainWindow::on_btnSetRows_clicked()
{
    //设置行数,设置的行数为数据区的行数，不含表头
    //如设置10行，数据区有10行，但是访问行号为0~9
       ui->tableInfo->setRowCount(ui->spinRowCount->value());//设置数据区行数
       ui->tableInfo->setAlternatingRowColors(ui->chkBoxRowColor->isChecked()); //设置交替行背景颜色
}


void MainWindow::on_btnInsertRow_clicked()
{ //插入一行
    int curRow=ui->tableInfo->currentRow();//当前行号

    ui->tableInfo->insertRow(curRow); //插入一行，但不会自动为单元格创建item
    //createItemsARow(curRow, "新学生", "男",0,0,true,60 ); //为某一行创建items
}

void MainWindow::on_btnAppendRow_clicked()
{ //添加一行
    int curRow=ui->tableInfo->rowCount();//当前行号
    ui->tableInfo->insertRow(curRow);//在表格尾部添加一行
   // createItemsARow(curRow, "新生", "女", 0,0,false,50 ); //为某一行创建items
}

void MainWindow::on_btnDelCurRow_clicked()
{//删除当前行及其items
    int curRow=ui->tableInfo->currentRow();//当前行号
    ui->tableInfo->removeRow(curRow); //删除当前行及其items
}
