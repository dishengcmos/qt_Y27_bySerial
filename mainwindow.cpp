#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDebug>

uint8_t G_mark1=0xff;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->SliderGreen,SIGNAL(valueChanged(int)),  //关联 SliderGreen 的valueChanged()
                     this,SLOT(on_SliderRed_valueChanged(int)));

    QObject::connect(ui->SliderBlue,SIGNAL(valueChanged(int)), //关联 SliderBlue的valueChanged()
                     this,SLOT(on_SliderRed_valueChanged(int)));

    QList<QSerialPortInfo> list;
    connect(ui->m_sendButton,SIGNAL(clicked()),this,SLOT(send()));
    list = QSerialPortInfo::availablePorts();
    for (int i = 0; i < list.size(); i++)
    {
        qDebug() << list.at(i).portName() ;//串口名字
        qDebug() << list.at(i).description();//串口描述
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
    QByteArray readnum=port->readAll().toHex();
    //ui->m_textEdit->append(port->readAll().toHex());
    ui->m_textEdit->append(readnum);

    bool ok;
    int val=readnum.toInt(&ok,16);//以十六进制数读入

    QByteArray str=QByteArray::number(val,10);// 显示为10进制字符串
   // qDebug()<<val;
    ui->progBarV->setValue(val);
    ui->SliderV->setValue(val);
}

void MainWindow::send()
{
//  port->write("1234");
    QString rednum;
    rednum=ui->SliderRed->value();
    //qDebug()<<ui->m_inputEdit->toPlainText();
    port->write( rednum.toUtf8());
    //port->write(ui->m_inputEdit->toPlainText().toUtf8());
}


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

    /**LED控制数据发送**/
    unsigned char LedContraldata[11]={0xfd,0xfd,0X01,0X06, 0x01, 0x03,0x03,0x03,0xff, 0x00, 0x00};
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

/**************************LED灯选择控制*********************************************/
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
