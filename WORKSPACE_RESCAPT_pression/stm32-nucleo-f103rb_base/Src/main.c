#include "main.h"
//====================================================================
#define VL6180X	0
#define MPU9250	0
#define MPL115A_ANEMO 1
//====================================================================
//			CAN ACCEPTANCE FILTER
//====================================================================
#define USE_FILTER	1
// Can accept until 4 Standard IDs
#define ID_1	0x01
#define ID_2	0x02
#define ID_3	0x03
#define ID_4	0x04
//====================================================================
extern void systemClock_Config(void);

void (*rxCompleteCallback) (void);
void can_callback(void);

CAN_Message      rxMsg;
CAN_Message      txMsg;
long int        counter = 0;

uint8_t* aTxBuffer[2];

extern float magCalibration[3];

void VL6180x_Init(void);
void VL6180x_Step(void);

int status;
int new_switch_state;
int switch_state = -1;

int count =0;
int Vit_Anemo = 0;
int periode=100;
int omega = 0;
uint8_t CPT[6]={0,0,0,0,0,0};
uint8_t coef[9]={0,0,0,0,0,0,0,0,0};
int pressure,temperature=0;
uint16_t padc, tadc =0;
float npadc, ntadc =0;
int16_t  a0_v,b0_v,b1_v,c12_v= 0;
float a0,b0,b1,c12=0;
int16_t Pcomp=0;


//====================================================================
// >>>>>>>>>>>>>>>>>>>>>>>>>> MAIN <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//====================================================================
int main(void)
{
	int count;

	HAL_Init();
	systemClock_Config();
    SysTick_Config(HAL_RCC_GetHCLKFreq() / 1000); //SysTick end of count event each 1ms
	uart2_Init();





#if VL6180X || MPU9250
	i2c1_Init();
#endif

	HAL_Delay(1000); // Wait

#if MPL115A_ANEMO
    spi1_Init();
    anemo_Timer1Init();
#endif

#if VL6180X
    VL6180x_Init();
#endif

#if MPU9250
    mpu9250_InitMPU9250();
    mpu9250_CalibrateMPU9250();
#if USE_MAGNETOMETER
    mpu9250_InitAK8963(magCalibration);
#endif
    uint8_t response=0;
	response =  mpu9250_WhoAmI();
	//term_printf("%d",response);
#endif


    can_Init();
    can_SetFreq(CAN_BAUDRATE); // CAN BAUDRATE : 500 MHz -- cf Inc/config.h
#if USE_FILTER
    can_Filter_list((ID_1<<21)|(ID_2<<5) , (ID_3<<21)|(ID_4<<5) , CANStandard, 0); // Accept until 4 Standard IDs
#else
    can_Filter_disable(); // Accept everybody
#endif
    can_IrqInit();
    can_IrqSet(&can_callback);

    txMsg.id=0x55;
    txMsg.data[0]=1;
    txMsg.data[1]=2;
    txMsg.len=2;
    txMsg.format=CANStandard;
    txMsg.type=CANData;

    can_Write(txMsg);


    // Décommenter pour utiliser ce Timer ; permet de déclencher une interruption toutes les N ms
    // Le programme d'interruption est dans tickTimer.c

    tickTimer_Init(periode); // period in ms



////////////////////// COEFFS	/////////////////////////////////////

    			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0); 		//		[CS=0]
        		spi1_Transfer(0x88);							//      [0x88]
        		coef[0] = spi1_Transfer(0x00);					//		[0x00]
        		spi1_Transfer(0x8A);							//      [0x8A]
        		coef[1] = spi1_Transfer(0x00);					//		[0x00]
        		spi1_Transfer(0x8C);							//      [0x8C]
        		coef[2] = spi1_Transfer(0x00);					//		[0x00]
        		spi1_Transfer(0x8E);							//      [0x8E]
        		coef[3] = spi1_Transfer(0x00);					//		[0x00]
        		spi1_Transfer(0x90);							//      [0x90]
        		coef[4] = spi1_Transfer(0x00);					//		[0x00]
        		spi1_Transfer(0x92);							//      [0x92]
        		coef[5] = spi1_Transfer(0x00);					//		[0x00]
        		spi1_Transfer(0x94);							//      [0x94]
        		coef[6] = spi1_Transfer(0x00);					//		[0x00]
        		spi1_Transfer(0x96);							//      [0x96]
        		coef[7] = spi1_Transfer(0x00);
        		coef[8] = spi1_Transfer(0x00);					//		[0x00]
        		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);


        		a0_v =  (( coef[0] << 8 )		|		coef[1] );
        		a0 = (float)(a0_v)/(pow(2,3));



        		b0_v =  (( coef[2] << 8 )		|		coef[3] );
        		b0 = (float)(b0_v)/(pow(2,13));


        		b1_v =  (( coef[4] << 8 )		|		coef[5] );
        		b1 = (float)(b1_v)/(pow(2,14));


        		c12_v = (( coef[6] << 6 )		|		coef[7] );
        		c12 = (float)(c12_v)/(pow(2,22));








    while (1) {


    	//////////////////////////////////////////////////////////////////////
    	//////				DEBUT PRESSION TEMPERATURE					//////
    	//////////////////////////////////////////////////////////////////////

    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0); 		//		[CS=0]
    		spi1_Transfer(0x24);							//      [0x24]
    		CPT[0] = spi1_Transfer(0x00);
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1); 		//		[CS=1]
    		HAL_Delay(3);									// attente de 3 ms


CAN_Message msg_Pression;

    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0); 		//		[CS=0]
    		spi1_Transfer(0x80);							//      [0x80]
    		CPT[1] = spi1_Transfer(0x00);					//		[0x00]
    		spi1_Transfer(0x82);							//      [0x82]
    		CPT[2] = spi1_Transfer(0x00);					//		[0x00]
    		spi1_Transfer(0x84);							//      [0x84]
    		CPT[3] = spi1_Transfer(0x00);					//		[0x00]
    		spi1_Transfer(0x86);							//      [0x86]
    		CPT[4] = spi1_Transfer(0x00);					//		[0x00]
    		CPT[5] = spi1_Transfer(0x00);					//		[0x00]
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1); 		//		[CS=1]


    		tadc = ((CPT[3] << 8)	|	CPT[4]);
    		ntadc = (float)(tadc >>6);
    		padc = ((CPT[1] << 8)	|	CPT[2]);
    		npadc = (float)(padc >>6);

    		Pcomp = a0 + ( b0 + c12*ntadc )*npadc +b1*ntadc;

    		pressure = Pcomp*((115.0-50.0)/1023.0)+50.0;
    		temperature = 30 + (ntadc-472)/(-5.35);





    		term_printf("Valeur de la pression = %d KPa\r\n",pressure);
    		term_printf("Valeur de la temperature = %d °C\r\n",ntadc);

    		msg_Pression.id=0x57;
    		msg_Pression.data[0] = pressure;
    		msg_Pression.len = 1;
    		msg_Pression.format=CANStandard;
    		msg_Pression.type=CANData;



    		        	    can_Write(msg_Pression);


    	//////////////////////////////////////////////////////////////////////
    	//////				FIN	 PRESSION TEMPERATURE					//////
    	//////////////////////////////////////////////////////////////////////











#if VL6180X
    VL6180x_Step();
#endif

#if MPU9250

#endif

    }
	return 0;
}


//====================================================================
//			CAN CALLBACK RECEPT
//====================================================================

void can_callback(void)
{


	CAN_Message msg_rcv;
	int i=0;

	can_Read(&msg_rcv);
	txMsg.id=0x55;			// Identifiant du message à envoyer

	for(i=0;i<8;i++)
	{
	txMsg.data[i]=msg_rcv.data[i]+1;
	}
	txMsg.len=8;			// Nombre d'octets à envoyer
	txMsg.format=CANStandard;
	txMsg.type=CANData;

	can_Write(txMsg);



}
//====================================================================
//			TIMER CALLBACK PERIOD
//====================================================================

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
//////////////////////////////////////////////////////////////////////
//////					DEBUT ANEMOMETRE						//////
//////////////////////////////////////////////////////////////////////

CAN_Message msg_VitVent;

	count = anemo_GetCount();
	omega = (count/4.0)/(periode/1000.0);    //(tr/s)
	Vit_Anemo = omega;
	term_printf("Vitesse de l`anemometre = %d Km/h\n\r",Vit_Anemo);

	msg_VitVent.id=0x56;
	msg_VitVent.data[0]=Vit_Anemo;
	msg_VitVent.len=1;
	msg_VitVent.format=CANStandard;
	msg_VitVent.type=CANData;
	can_Write(msg_VitVent);

	anemo_ResetCount();

//////////////////////////////////////////////////////////////////////
//////					FIN  	 ANEMOMETRE						//////
//////////////////////////////////////////////////////////////////////


}
//====================================================================

void VL6180x_Init(void)
{
	uint8_t id;
	State.mode = 1;

    XNUCLEO6180XA1_Init();
    HAL_Delay(500); // Wait
    // RESET
    XNUCLEO6180XA1_Reset(0);
    HAL_Delay(10);
    XNUCLEO6180XA1_Reset(1);
    HAL_Delay(1);

    HAL_Delay(10);
    VL6180x_WaitDeviceBooted(theVL6180xDev);
    id=VL6180x_Identification(theVL6180xDev);
    term_printf("id=%d, should be 180 (0xB4) \n\r", id);
    VL6180x_InitData(theVL6180xDev);

    State.InitScale=VL6180x_UpscaleGetScaling(theVL6180xDev);
    State.FilterEn=VL6180x_FilterGetState(theVL6180xDev);

     // Enable Dmax calculation only if value is displayed (to save computation power)
    VL6180x_DMaxSetState(theVL6180xDev, DMaxDispTime>0);

    switch_state=-1 ; // force what read from switch to set new working mode
    State.mode = AlrmStart;
}
//====================================================================
void VL6180x_Step(void)
{
    DISP_ExecLoopBody();

    new_switch_state = XNUCLEO6180XA1_GetSwitch();
    if (new_switch_state != switch_state) {
        switch_state=new_switch_state;
        status = VL6180x_Prepare(theVL6180xDev);
        // Increase convergence time to the max (this is because proximity config of API is used)
        VL6180x_RangeSetMaxConvergenceTime(theVL6180xDev, 63);
        if (status) {
            AbortErr("ErIn");
        }
        else{
            if (switch_state == SWITCH_VAL_RANGING) {
                VL6180x_SetupGPIO1(theVL6180xDev, GPIOx_SELECT_GPIO_INTERRUPT_OUTPUT, INTR_POL_HIGH);
                VL6180x_ClearAllInterrupt(theVL6180xDev);
                State.ScaleSwapCnt=0;
                DoScalingSwap( State.InitScale);
            } else {
                 State.mode = RunAlsPoll;
                 InitAlsMode();
            }
        }
    }

    switch (State.mode) {
    case RunRangePoll:
        RangeState();
        break;

    case RunAlsPoll:
        AlsState();
        break;

    case InitErr:
        TimeStarted = g_TickCnt;
        State.mode = WaitForReset;
        break;

    case AlrmStart:
       GoToAlaramState();
       break;

    case AlrmRun:
        AlarmState();
        break;

    case FromSwitch:
        // force reading swicth as re-init selected mode
        switch_state=!XNUCLEO6180XA1_GetSwitch();
        break;

    case ScaleSwap:

        if (g_TickCnt - TimeStarted >= ScaleDispTime) {
            State.mode = RunRangePoll;
            TimeStarted=g_TickCnt; /* reset as used for --- to er display */
        }
        else
        {
        	DISP_ExecLoopBody();
        }
        break;

    default: {
    	 DISP_ExecLoopBody();
          if (g_TickCnt - TimeStarted >= 5000) {
              NVIC_SystemReset();
          }
    }
    }
}
