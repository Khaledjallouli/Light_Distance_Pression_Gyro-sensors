#include "mainwindow.h"
#include "ui_mainwindow.h"

#define DEFAULT_NODE "/dev/pcanusb32"

//===============================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    QWidget *zoneCentrale = new QWidget;
    setCentralWidget(zoneCentrale);

    but_send = new QPushButton("Refresh");
    m_layout = new QGridLayout();

    QLabel *labeldistance = new QLabel;
    labeldistance->setText("Distance data");
    
    QLabel *labelBright = new QLabel;
    labelBright->setText("Brightness data");

                                                                                                                                QLabel *labelPressure = new QLabel;
                                                                                                                                labelPressure->setText("Pressure data");
                                                                                                                                QLabel *labelwindspeed = new QLabel;
                                                                                                                                labelwindspeed->setText("wind speed data");
                                                                                                                                QLabel *labelkmh = new QLabel;
                                                                                                                                 labelkmh->setText("Km/h");
                                                                                                                                 QLabel *labelkpa = new QLabel;
                                                                                                                                 labelkpa->setText("KPa");

    distanceLabel = new QLineEdit();
    brightness = new QLineEdit();
                                                                                                                                    pressure = new QLineEdit();
                                                                                                                                    windspeed = new QLineEdit();
  

QRadioButton *buttonDistance = new QRadioButton("Distance", this);
QRadioButton *buttonbrightness = new QRadioButton("Brightness", this);



    m_layout->addWidget(but_send,1,0);
    m_layout->addWidget(buttonDistance,1,1);
    m_layout->addWidget(buttonbrightness,1,2);


    m_layout->addWidget(labeldistance,2,0);


    m_layout->addWidget(distanceLabel,2,1);

    m_layout->addWidget(labelBright,3,0);

    m_layout->addWidget(brightness,3,1);

                                                                                                                                m_layout->addWidget(labelPressure,4,0);
                                                                                                                                m_layout->addWidget(labelkpa,4,2);

                                                                                                                                m_layout->addWidget(pressure,4,1);

                                                                                                                                m_layout->addWidget(labelwindspeed,5,0);
                                                                                                                                 m_layout->addWidget(labelkmh,5,2);

                                                                                                                                 m_layout->addWidget(windspeed,5,1);
    


    zoneCentrale->setLayout(m_layout);

    openCANPort();

    timer_tick = new QTimer();
    connect( timer_tick, SIGNAL(timeout()), this, SLOT(onTimer_Tick()));


    connect(but_send,SIGNAL(clicked()),this,SLOT(onButSendClicked()));
    
    connect(buttonDistance,SIGNAL(clicked( )), this, SLOT(distanceSwitch()));
    connect(buttonbrightness,SIGNAL(clicked( )), this, SLOT(brithnessSwitch()));

	
     timer_tick -> start(1); // in ms
}
//===============================================================

MainWindow::~MainWindow()
{
    delete ui;
}

//===============================================================
void MainWindow::openCANPort()
{
    const char *szDevNode = DEFAULT_NODE;

    h=LINUX_CAN_Open(szDevNode, O_RDWR);
    if (!h)
        {
            printf("can't open %s\n", szDevNode);
        }
    CAN_Init(h, CAN_BAUD_500K,  CAN_INIT_TYPE_ST);         // BTR0BTR1	bitrate code in hex (default=0x1c (500 kbps))
    CAN_Status(h); // Clear Status

}
//===============================================================
void MainWindow::onButSendClicked()
{
    sendCANMessage('R');

}
//===============================================================
void MainWindow::sendCANMessage(char info)
{
    TPCANMsg msgBuff;

    msgBuff.ID      =   0x4;
    msgBuff.MSGTYPE =   MSGTYPE_STANDARD;
    msgBuff.LEN     =   8;

    msgBuff.DATA[0] =   info;

 LINUX_CAN_Write_Timeout(h, &msgBuff,0);

}

//===============================================================
void MainWindow::receiveCANMessage()
{


    LINUX_CAN_Read_Timeout(h, &pMsgBuff,1); // Timeout=1ms --> Non Bloquant

// il faut recevoir les donnes de la distance et luminosité car mnt on recoit juste n'importe quoi haha





                        uint pression = 0;
                         uint VitVent =0;

                            if (pMsgBuff.Msg.ID == 0x56)   {
                                         VitVent = ((uint)pMsgBuff.Msg.DATA[0]);
                                                 windspeed -> setText(QString::number((uint)VitVent));
                                           }

                             if (pMsgBuff.Msg.ID == 0x57 )
                                    {

                                        pression = ((uint)pMsgBuff.Msg.DATA[0]);
                                                 pressure -> setText(QString::number((uint)pression));
                                                }
                             if (pMsgBuff.Msg.ID == 0x69 ||pMsgBuff.Msg.ID == 0x78 )
                                    {
                                 uint range = ((uint)pMsgBuff.Msg.DATA[1]); // rassembler les 4 parties dans une seule variable
                                range = range | ((uint)pMsgBuff.Msg.DATA[2])<<8;
                                range = range | ((uint)pMsgBuff.Msg.DATA[3])<<16;
                                range = range | ((uint)pMsgBuff.Msg.DATA[4])<<24;

                                switch (pMsgBuff.Msg.DATA[0]) {
                                        case 'L':
                                            brightness->setText(QString::number((uint)range));
                                            break;
                                        case 'D':
                                           distanceLabel->setText(QString::number((uint)range));

                                        break;

                                    default:
                                        break;
                                    }
                                                }






}
//===============================================================

void MainWindow::onTimer_Tick()
{
receiveCANMessage();
}




void MainWindow::distanceSwitch(){
   sendCANMessage('D');
}

void MainWindow:: brithnessSwitch(){
	sendCANMessage('L');
}
