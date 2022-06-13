#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serial = new QSerialPort(this);

    bool isAvailable = false;
       QString portName;

       foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
           if(serialPortInfo.hasProductIdentifier()){
                   isAvailable = true;
                   portName = serialPortInfo.portName();
                   qDebug() << "Serial port name: " << portName << endl;
           }
       }

       if(isAvailable){
           qDebug() << "Found the port..." << endl;
           serial->setPortName(portName);
           serial->open(QSerialPort::ReadWrite);
           serial->setBaudRate(QSerialPort::Baud9600);
           serial->setDataBits(QSerialPort::Data8);
           serial->setFlowControl(QSerialPort::NoFlowControl);
           serial->setParity(QSerialPort::NoParity);
           serial->setStopBits(QSerialPort::OneStop);
       }else{
           qDebug() << "Couldn't find the correct port.\n";
       }
   }



MainWindow::~MainWindow()
{
    delete ui;
    serial->close();
}

void MainWindow::on_sendButton_clicked()
{
    buttonString = ui->buttonEdit->toPlainText();
    passwordString = ui->passwordEdit->toPlainText();
    QString temp = buttonString + " " + passwordString;
    serial->write(buttonString.toLatin1());

}
