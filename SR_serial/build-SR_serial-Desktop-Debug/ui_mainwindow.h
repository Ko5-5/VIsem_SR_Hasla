/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *passwordLabel;
    QTextEdit *passwordEdit;
    QLabel *przyciskLabel;
    QTextEdit *buttonEdit;
    QLabel *inSerialPortLabel;
    QTextEdit *serialEdit;
    QPushButton *sendButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(479, 226);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayoutWidget = new QWidget(centralWidget);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(30, 10, 421, 101));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        passwordLabel = new QLabel(gridLayoutWidget);
        passwordLabel->setObjectName(QStringLiteral("passwordLabel"));

        gridLayout->addWidget(passwordLabel, 3, 0, 1, 1);

        passwordEdit = new QTextEdit(gridLayoutWidget);
        passwordEdit->setObjectName(QStringLiteral("passwordEdit"));
        passwordEdit->setEnabled(true);

        gridLayout->addWidget(passwordEdit, 3, 1, 1, 1);

        przyciskLabel = new QLabel(gridLayoutWidget);
        przyciskLabel->setObjectName(QStringLiteral("przyciskLabel"));

        gridLayout->addWidget(przyciskLabel, 2, 0, 1, 1);

        buttonEdit = new QTextEdit(gridLayoutWidget);
        buttonEdit->setObjectName(QStringLiteral("buttonEdit"));

        gridLayout->addWidget(buttonEdit, 2, 1, 1, 1);

        inSerialPortLabel = new QLabel(gridLayoutWidget);
        inSerialPortLabel->setObjectName(QStringLiteral("inSerialPortLabel"));

        gridLayout->addWidget(inSerialPortLabel, 0, 0, 1, 1);

        serialEdit = new QTextEdit(gridLayoutWidget);
        serialEdit->setObjectName(QStringLiteral("serialEdit"));
        serialEdit->setEnabled(false);

        gridLayout->addWidget(serialEdit, 0, 1, 1, 1);

        sendButton = new QPushButton(centralWidget);
        sendButton->setObjectName(QStringLiteral("sendButton"));
        sendButton->setGeometry(QRect(30, 130, 421, 31));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 479, 24));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        passwordLabel->setText(QApplication::translate("MainWindow", "Has\305\202o do zapisania:", Q_NULLPTR));
        przyciskLabel->setText(QApplication::translate("MainWindow", "Numer przycisku:", Q_NULLPTR));
        inSerialPortLabel->setText(QApplication::translate("MainWindow", "Port seryjny:", Q_NULLPTR));
        sendButton->setText(QApplication::translate("MainWindow", "Send", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
