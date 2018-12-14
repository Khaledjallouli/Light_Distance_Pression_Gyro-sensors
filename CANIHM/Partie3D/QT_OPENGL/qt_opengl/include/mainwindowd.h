#ifndef MAINWINDOWD_H
#define MAINWINDOWD_H

#include <iostream>
#include <QtWidgets/QMainWindow>
#include <qgridlayout.h>
#include <objectgl.h>
#include <QMenuBar>
#include <QMessageBox>
#include <QtCore>
#include <QTimer>
#include <QDebug>
#include <libpcan.h>        // /usr/include/libpcan.h
#include <fcntl.h>    // O_RDWR
#include <signal.h>
#include <unistd.h>   // exit

class MainWindowd : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor and destructor
    MainWindowd(QWidget *parent = 0, int w=600, int h=400);
    ~MainWindowd();
    void openCANPort();
    void receiveCANMessage();

    bool                    connect();

protected slots:
    // Redraw the scene
    void                    onTimer_UpdateDisplay();
    void onTimer_Mvt();

protected:

    // Overload of the resize event
    void                    resizeEvent(QResizeEvent *);

private:

    // Layout of the window
    QGridLayout             *gridLayout;
    QWidget                 *gridLayoutWidget;

    // Central widget (where the openGL window is drawn)
    QWidget                 *centralWidget;

    // OpenGL object
    ObjectOpenGL            *Object_GL;
    QTimer *timerDisplay, *timerMvt;
int ID;
    float msg1float,msg2float,msg3float;
    HANDLE h;
    TPCANRdMsg pMsgBuff;
};

#endif // MAINWINDOWD_H
