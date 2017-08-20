#include "deska.h"

/*
 uint32_t APB1_ClockTime =0;
int main(void)
{ 
  int i;
  InitIOPort(GPIOB,14,portOUTPUT);//LED
  InitIOPort(GPIOA,11,portALTIN); //CAN Rx
  InitIOPort(GPIOA,12,portALTOUT);//CAN Tx
  
  RCC->APB1ENR|=RCC_APB1ENR_CAN1EN;
  RCC->APB1RSTR|=RCC_APB1RSTR_CAN1RST;
  RCC->APB1RSTR&=~RCC_APB1RSTR_CAN1RST;
  
  CAN1->MCR&=~CAN_MCR_SLEEP;
  CAN1->MCR|=CAN_MCR_INRQ;          //Enter initialization mode
  while(!(CAN1->MSR&CAN_MSR_INAK)); //Wait for hardware confirmed the initialization
  APB1_ClockTime=GetTimerClock(2);
  CAN1->BTR&=0x00000000;            //Nulovani registru
  CAN1->BTR|=(0x3<<24)|(0x2<<20)|(0x3<<16)|17;  //SJW,TS2,TS1,BRP toto nastaveni je pro 250kbps pro abp1=36000000
  CAN1->MCR&=~CAN_MCR_INRQ;       //exit initialization mode
  while(CAN1->MSR&CAN_MSR_INAK);
  
 CAN1->FMR&=0x00000000;
  
  while(1)
  {
   if(CAN1->TSR&CAN_TSR_TME0)    //pokud je transmit mailbox prázdný
  {
    
   CAN1->sTxMailBox[0].TDTR|=1; //data length 
   CAN1->sTxMailBox[0] .TDLR=0x41; //data 'A'
  CAN1->sTxMailBox[0].TIR&=0x00000000;
  CAN1->sTxMailBox[0].TIR|=(0x001<<21)|CAN_TI0R_TXRQ;   //ID =1 transmit rqst
  
    TOGGLE_IO(GPIOB,14);
    for(i=0;i<1000000;i++);
  }
   if(CAN1->TSR&CAN_TSR_TME1)    //pokud je transmit mailbox prázdný
  {
    
   CAN1->sTxMailBox[1].TDTR|=1; //data length 
   CAN1->sTxMailBox[1] .TDLR=0x00000042; //data 'B'
  CAN1->sTxMailBox[1].TIR&=0x00000000;
  CAN1->sTxMailBox[1].TIR|=(0x001<<21)|CAN_TI0R_TXRQ;   //ID =1 transmit rqst
  
    TOGGLE_IO(GPIOB,14);
    for(i=0;i<1000000;i++);
  }}
  
}
*/
uint8_t mesg=0;

uint8_t dat[8];

uint32_t StId;  

  uint32_t ExId; 

  uint8_t IDEi;    

  uint8_t RTRi;     

  uint8_t DLCi;    
  
  uint8_t FMIi; 
  
  int irqRx1=0;
  int irqRx0=0;
  int irqSce=0;

CanRxMsg can_st_rx, *ukRx;
CanTxMsg can_st_tx, *ukTx;
   void CAN1_RX1_IRQHandler(void)  //obsluha  prerušení  pro príjem dat po CAN1
 {
 
 }


 void USB_LP_CAN1_RX0_IRQHandler(void)  //obsluha  prerušení  pro príjem dat po CAN1
 {
  irqRx0++; //pocitadlo vstupu do prerusení
   
   CAN_Receive(CAN1,0,ukRx);    //Vyctení dat z CAN do struktury CanRxMsg
  
    StId= can_st_rx.StdId;       //zapis vyctenych parametru do vlastních promených
    ExId=  can_st_rx.ExtId;
    IDEi=  can_st_rx.IDE;
    RTRi=  can_st_rx.RTR;
    DLCi=  can_st_rx.DLC;
    dat[0]=  can_st_rx.Data[0];
    dat[1]=  can_st_rx.Data[1];
    dat[2]=  can_st_rx.Data[2];
    dat[3]=  can_st_rx.Data[3];
    dat[4]=  can_st_rx.Data[4];
    dat[5]=  can_st_rx.Data[5];
    dat[6]=  can_st_rx.Data[6];
    dat[7]=  can_st_rx.Data[7];
    FMIi=  can_st_rx.FMI;

 }   
  
  void USB_HP_CAN1_TX_IRQHandler(void)
  {
    CAN_CancelTransmit(CAN1,mesg);
  }

 
 bool CAN1_Transmit(uint32_t ID, bool Ext, bool DATAframe,uint8_t DATA[8], uint8_t pocet_dat)
  {
    uint8_t msg;
  can_st_tx.StdId=0x7ff&ID;            //naplnené struktury pro vysílání
  can_st_tx.ExtId=0x1fffffff&ID;       //dle použité ID st nebo ext se vyplní jen jedno pole
    if(Ext)
      can_st_tx.IDE=CAN_Id_Extended;
    else    
      can_st_tx.IDE=CAN_Id_Standard;
    if(DATAframe)
  can_st_tx.RTR=CAN_RTR_DATA;
    else
   can_st_tx.RTR=CAN_RTR_REMOTE; 
    
  can_st_tx.DLC=0x09&pocet_dat;
  can_st_tx.Data[0]=DATA[0];
  can_st_tx.Data[1]=DATA[1];
  can_st_tx.Data[2]=DATA[2];
  can_st_tx.Data[3]=DATA[3];
  can_st_tx.Data[4]=DATA[4];
  can_st_tx.Data[5]=DATA[5];
  can_st_tx.Data[6]=DATA[6];
  can_st_tx.Data[7]=DATA[7];
  
  
  
  msg=CAN_Transmit(CAN1,ukTx);      //Odeslání dat po CAN1
  while(!(CAN_TransmitStatus(CAN1,msg)==CAN_TxStatus_Ok));  //Cekaní na dokoncení vysílání po CAN
  CAN_CancelTransmit(CAN1,msg);     //Uvolnení transmit boxu
    
  return true;
  }

 
int main(void)
{
  int i;
  int x=0;
  CAN_InitTypeDef can_st_in, *ukIni;
  CAN_FilterInitTypeDef can_st_fi , *ukFi;
  CanTxMsg can_st_tx, *ukTx;
  
  ukIni=&can_st_in;   //ukazatel na strukturu pro innicializaci
  ukFi=&can_st_fi;    //ukazatel na strukturu filtru
  ukRx=&can_st_rx;    //ukazatel na strukturu príjmu
  ukTx=&can_st_tx;    //ukazatel na strukturu vysílání
  
  NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);   //Povolení prerušení pro príjem z CAN1
  NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
  NVIC_EnableIRQ(CAN1_RX1_IRQn);
  InitIOPort(GPIOB,14,portOUTPUT);        //LED
  InitIOPort(GPIOA,11,portALTIN);         //CAN Rx
  InitIOPort(GPIOA,12,portALTOUT);        //CAN Tx
  
  RCC->APB1ENR|=RCC_APB1ENR_CAN1EN;       //povolení hodin CAN1
  RCC->APB1RSTR|=RCC_APB1RSTR_CAN1RST;    //reset CAN1
  RCC->APB1RSTR&=~RCC_APB1RSTR_CAN1RST;   //zrusení resetu CAN1
   
  can_st_in.CAN_Prescaler=18;             //naplnení struktury pro inicializaci
  can_st_in.CAN_Mode=CAN_Mode_Normal;
  can_st_in.CAN_SJW=3;
  can_st_in.CAN_BS1=CAN_BS1_4tq;
  can_st_in.CAN_BS2=CAN_BS2_3tq;
  can_st_in.CAN_TTCM=ENABLE;
  can_st_in.CAN_ABOM=ENABLE;
  can_st_in.CAN_AWUM=ENABLE;
  can_st_in.CAN_NART=DISABLE;
  can_st_in.CAN_RFLM=DISABLE;
  can_st_in.CAN_TXFP=DISABLE;
  
  CAN_Init(CAN1,ukIni);          //Inicializace CAN1
  
  CAN_OperatingModeRequest(CAN1,CAN_OperatingMode_Initialization);  //Další inicializace
  CAN_DBGFreeze(CAN1,DISABLE);
  CAN_OperatingModeRequest(CAN1,CAN_OperatingMode_Normal);    //Konec inicializace
  
  
  can_st_tx.StdId=0x456;            //naplnené struktury pro vysílání
  can_st_tx.ExtId=0x12345678;       //dle použité ID st nebo ext se vyplní jen jedno pole
  can_st_tx.IDE=CAN_Id_Extended;
  can_st_tx.RTR=CAN_RTR_DATA;
  can_st_tx.DLC=8;
  can_st_tx.Data[0]=0x41;
  can_st_tx.Data[1]=0x42;
  can_st_tx.Data[2]=0x43;
  can_st_tx.Data[3]=0x44;
  can_st_tx.Data[4]=0x45;
  can_st_tx.Data[5]=0x46;
  can_st_tx.Data[6]=0x47;
  can_st_tx.Data[7]=0x48;
  
  can_st_fi.CAN_FilterIdHigh=0x0480;    //naplnení struktury filtru a masky
  can_st_fi.CAN_FilterIdLow=0x0000;
  can_st_fi.CAN_FilterMaskIdHigh=0x1E00;
  can_st_fi.CAN_FilterMaskIdLow=0x0000;
  can_st_fi.CAN_FilterFIFOAssignment=CAN_FIFO0;
  can_st_fi.CAN_FilterMode=CAN_FilterMode_IdMask;
  can_st_fi.CAN_FilterNumber=0;
  can_st_fi.CAN_FilterScale=CAN_FilterScale_32bit;
  can_st_fi.CAN_FilterActivation=ENABLE;
 
/*
  StId= can_st_rx.StdId;      //vyctení prijmutých dat ze struktury
  ExId=  can_st_rx.ExtId;
  IDEi=  can_st_rx.IDE;
  RTRi=  can_st_rx.RTR;
  DLCi=  can_st_rx.DLC;
  dat[0]=  can_st_rx.Data[0];
  dat[1]=  can_st_rx.Data[1];
  dat[2]=  can_st_rx.Data[2];
  dat[3]=  can_st_rx.Data[3];
  dat[4]=  can_st_rx.Data[4];
  dat[5]=  can_st_rx.Data[5];
  dat[6]=  can_st_rx.Data[6];
  dat[7]=  can_st_rx.Data[7];
  FMIi=  can_st_rx.FMI;
*/

 // CAN_FilterInit(ukFi);                       //Inicializace filtru
  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);      //Nastavení interuptu fifo 0
  CAN_ITConfig(CAN1,CAN_IT_FMP1,ENABLE);      //Nastavení interuptu fifo 1
  CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);
  
  CAN_WakeUp(CAN1);      //Probuzení CAN1
  
while(1)
{ 
  /*uint8_t dat[8];
  dat[0]=0x00;
  dat[1]=0x01;
  dat[2]=0x02;
  dat[3]=0x03;
  dat[4]=0x04;
  dat[5]=0x05;
  dat[6]=0x06;
  dat[7]=0x07;
  */
 // CAN1_Transmit(0x12345678,true,true,dat,8);
  uint8_t msg;
  mesg=CAN_Transmit(CAN1,ukTx);      //Odeslání dat po CAN1
 // while(!(CAN_TransmitStatus(CAN1,msg)==CAN_TxStatus_Ok));  //Cekaní na dokoncení vysílání po CAN
  //CAN_CancelTransmit(CAN1,msg);     //Uvolnení transmit boxu
    
  TOGGLE_IO(GPIOB,14);  //zmena stavu LED
  
 // while((CAN1->MSR&CAN_MSR_RXM)==CAN_MSR_RXM);
 //CAN_Receive(CAN1,0,ukRx);
  //dat[1]=(uint8_t)CAN_RDL0R_DATA1;
  
    for(i=0;i<4000000;i++); //Cekací smycka
  if(dat[1]==0x01)
    for(i=0;i<8;i++)
      dat[i]=0x00;
  x++;
  if(x==5)
    while(1)
    {}
  
}


}
