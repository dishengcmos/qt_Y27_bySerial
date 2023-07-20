#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTcpServer>
#include <QLabel>
#include    <QTableWidgetItem>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

	
private:
    Ui::MainWindow *ui;

    //  自定义单元格Type的类型，在创建单元格的Item时使用
    enum    CellType{ctName=1000,ctSex,ctBirth,ctNation,ctPartyM,ctScore}; //各单元格的类型
    //  各字段在表格中的列号
    enum    FieldColNum{colName=0, colSex,colBirth,colNation,colScore,colPartyM};

    QLabel  *LabListen;     //状态栏标签
    QLabel  *LabSocketState;//状态栏标签

    QTcpServer *tcpServer;  //TCP服务器
    QTcpSocket *tcpSocket;  //TCP通讯的Socket

    QString getLocalIP();//获取本机IP地址
    void    createItemsARow(int rowNo,QString Name,QString Sex,int shakeNum,
                            int tofHigh,bool isPM,int score); //为某一行创建items
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSerialPort *port;
public slots:
    void handle();
private slots:
    //自定义槽函数
    void    onNewConnection();//QTcpServer的newConnection()信号
    void    onSocketStateChange(QAbstractSocket::SocketState socketState);
    void    onClientConnected(); //Client Socket connected
    void    onClientDisconnected();//Client Socket disconnected
    void    onSocketReadyRead();//读取socket传入的数据

    /*********************自定义槽函数***********************/

    void on_textEdit_textChanged();
    void send();
    void on_SliderRed_valueChanged(int value);
//  void on_pushButton_clicked(bool checked);
//  void on_pushButton_clicked();
    void on_lineEdit_inputRejected();
    void on_SliderRed_actionTriggered(int action);
    void on_m_sendButton_clicked();
    void on_m_sendButton_clicked(bool checked);
    void on_checkBox_stateChanged(int arg1);
    void on_checkBox_clicked();
//  void on_checkBox_clicked(bool checked);
    void on_checkBox_1_clicked(bool checked);
    void on_checkBox_2_clicked(bool checked);
    void on_checkBox_3_clicked(bool checked);
    void on_checkBox_4_clicked(bool checked);
    void on_checkBox_5_clicked(bool checked);
    void on_checkBox_6_clicked(bool checked);
    void on_checkBox_7_clicked(bool checked);
    void on_checkBox_8_clicked(bool checked);

    void on_turnonwifi_clicked();
    void on_sendwifi_clicked();
    void on_btnSetRows_clicked();

    void on_btnInsertRow_clicked();  //插入行 按键
    void on_btnAppendRow_clicked(); //添加行  按键
    void on_btnDelCurRow_clicked();     //删除当前行 按键
    void on_btnIniData_clicked();     //  按键

};
#endif // MAINWINDOW_H
