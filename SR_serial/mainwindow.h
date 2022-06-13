#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString serialString;
    QString buttonString;
    QString passwordString;

    QSerialPort * serial;

private slots:
    void on_sendButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
