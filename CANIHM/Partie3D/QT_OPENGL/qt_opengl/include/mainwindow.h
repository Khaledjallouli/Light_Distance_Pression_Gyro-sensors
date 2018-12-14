#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include <QtCore>
#include <QDebug>

#include <QString>
#include <QMessageBox>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QTimer>

#include <libpcan.h>        // /usr/include/libpcan.h
#include <fcntl.h>    // O_RDWR
#include <signal.h>
#include <unistd.h>   // exit
 #include <QRadioButton> 

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void openCANPort();
    void sendCANMessage(char);
    void receiveCANMessage();

public slots:

    void onButSendClicked();
         void onTimer_Tick();
void distanceSwitch();
 void  brithnessSwitch();

private:

    HANDLE h;
    TPCANRdMsg pMsgBuff;

    Ui::MainWindow  *ui;
     QGridLayout     *m_layout;
     QPushButton     *but_send;
     QLineEdit       *distanceLabel, *brightness, *pressure,*windspeed;
    // QAction *_distanceAction, *_brightnessAction;
    QRadioButton *buttonDistance,*buttonbrightness;
    QTimer *timer_tick;







};

#endif // MAINWINDOW_H
