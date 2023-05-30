#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


QSerialPort *port;
private:
    Ui::MainWindow *ui;
public slots:
    void handle();
private slots:
    void on_textEdit_textChanged();
    void send();
    void on_SliderRed_valueChanged(int value);
    void on_pushButton_clicked(bool checked);
    void on_pushButton_clicked();
    void on_lineEdit_inputRejected();
    void on_SliderRed_actionTriggered(int action);
    void on_m_sendButton_clicked();
    void on_m_sendButton_clicked(bool checked);
};
#endif // MAINWINDOW_H
