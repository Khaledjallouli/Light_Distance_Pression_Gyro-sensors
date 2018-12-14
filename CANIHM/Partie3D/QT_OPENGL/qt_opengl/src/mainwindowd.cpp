#include "mainwindowd.h"

#define DEFAULT_NODE "/dev/pcanusb32"
//=============================================================================================
// Constructor of the main window
// Create window properties, menu etc ...
//=============================================================================================
MainWindowd::MainWindowd(QWidget *parent,int w, int h)
    : QMainWindow(parent)
{        
    // Set the window size
    this->resize(w,h);
    this->setWindowTitle("Object viewer");

    // Create a layout in the main window
    centralWidget = new QWidget(this);
    gridLayoutWidget = new QWidget(centralWidget);
    gridLayoutWidget->setGeometry(QRect(0, 0, this->width(), this->height()));
    gridLayout = new QGridLayout(gridLayoutWidget);

    // Create the openGL display for the map
    Object_GL = new ObjectOpenGL(gridLayoutWidget);
    Object_GL->setObjectName(QString::fromUtf8("ObjectOpenGL"));
    Object_GL->setGeometry(QRect(0, 0, this->width(), this->height()));

    // Insert the Open Gl display into the layout
    gridLayout->addWidget(Object_GL, 0, 0, 1, 1);
    setCentralWidget(centralWidget);

    // Create the menubar
    QMenu *FileMenu = menuBar()->addMenu("&File");
    FileMenu->addSeparator();
    FileMenu->addAction("Quit", qApp, SLOT (quit()), QKeySequence(tr("Ctrl+Q")));

    // Add menu items
    QMenu *ViewMenu = menuBar()->addMenu("&View");
    ViewMenu->addAction("Front view",       Object_GL, SLOT (FrontView()),  QKeySequence(tr("Ctrl+f")));
    ViewMenu->addAction("Rear view",        Object_GL, SLOT (RearView()),   QKeySequence(tr("Ctrl+e")));
    ViewMenu->addAction("Left view",        Object_GL, SLOT (LeftView()),   QKeySequence(tr("Ctrl+l")));
    ViewMenu->addAction("Right view",       Object_GL, SLOT (RightView()),  QKeySequence(tr("Ctrl+r")));
    ViewMenu->addAction("Top view",         Object_GL, SLOT (TopView()),    QKeySequence(tr("Ctrl+t")));
    ViewMenu->addAction("Bottom view",      Object_GL, SLOT (BottomView()), QKeySequence(tr("Ctrl+b")));
    FileMenu->addSeparator();
    ViewMenu->addAction("Isometric",        Object_GL, SLOT (IsometricView()), QKeySequence(tr("Ctrl+i")));


    openCANPort();

    // Timer (used for repainting the GL Window every 50 ms)
    timerDisplay = new QTimer();
    timerDisplay->connect(timerDisplay, SIGNAL(timeout()),this, SLOT(onTimer_UpdateDisplay()));
    timerDisplay->start(50);

    timerMvt = new QTimer();
    timerMvt->connect(timerMvt, SIGNAL(timeout()),this, SLOT(onTimer_Mvt()));
    timerMvt->start(1);


}
//=============================================================================================
MainWindowd::~MainWindowd()
{}
//=============================================================================================
// On resize event, the items in the window are resized
//=============================================================================================
void MainWindowd::resizeEvent(QResizeEvent *)
{
    Object_GL->resize(centralWidget->width(),centralWidget->height());
    gridLayoutWidget->setGeometry(QRect(0, 0, centralWidget->width(), centralWidget->height()));
}
//=============================================================================================
// Timer event : repain the Open Gl window
//=============================================================================================
void MainWindowd::onTimer_UpdateDisplay()
{
    Object_GL->updateGL();
}
//=============================================================================================
void MainWindowd::onTimer_Mvt(){
    receiveCANMessage();

    //Object_GL->updateGL();
}


void MainWindowd::openCANPort(){
    const char *szDevNode = DEFAULT_NODE;

       h=LINUX_CAN_Open(szDevNode, O_RDWR);
       if (!h)
           {
               printf("can't open %s\n", szDevNode);
           }
       CAN_Init(h, CAN_BAUD_500K,  CAN_INIT_TYPE_ST);         // BTR0BTR1	bitrate code in hex (default=0x1c (500 kbps))
       CAN_Status(h); // Clear Status

}
void MainWindowd::receiveCANMessage(){
int16_t msg1=0,msg2=0,msg3=0;
    LINUX_CAN_Read_Timeout(h, &pMsgBuff,1);
 if (pMsgBuff.Msg.ID == 0x70)   {

    msg1=(int16_t)pMsgBuff.Msg.DATA[0]<<8 ;
    msg1 |= (uint)pMsgBuff.Msg.DATA[1];
    msg1float = (float) (msg1/ 10.0);


    msg2=(int16_t)pMsgBuff.Msg.DATA[2]<<8 ;
    msg2 |= (uint)pMsgBuff.Msg.DATA[3];
    msg2float = (float) (msg2/ 10.0);


    msg3=(int16_t)pMsgBuff.Msg.DATA[4]<<8 ;
    msg3 |= (uint)pMsgBuff.Msg.DATA[5];
    msg3float = (float) (msg3/ 10.0);

    Object_GL->setAngles(msg1float,msg2float,msg3float);
}
}
