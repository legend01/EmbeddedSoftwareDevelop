#include "APP_I2S.h"  
#include "tim.h"

//I2S2��ʼ��
//I2S_Standard:I2S��׼,�������ã�I2S_STANDARD_PHILIPS/I2S_STANDARD_MSB/
//				       I2S_STANDARD_LSB/I2S_STANDARD_PCM_SHORT/I2S_STANDARD_PCM_LONG
//I2S_Mode:I2S����ģʽ,�������ã�I2S_MODE_SLAVE_TX/I2S_MODE_SLAVE_RX/I2S_MODE_MASTER_TX/I2S_MODE_MASTER_RX
//I2S_Clock_Polarity:ʱ�ӵ�ƽ����������Ϊ:I2S_CPOL_LOW/I2S_CPOL_HIGH
//I2S_DataFormat:���ݳ���,�������ã�I2S_DATAFORMAT_16B/I2S_DATAFORMAT_16B_EXTENDED/I2S_DATAFORMAT_24B/I2S_DATAFORMAT_32B
void I2S2_Init(u32 I2S_Standard,u32 I2S_Mode,u32 I2S_Clock_Polarity,u32 I2S_DataFormat)
{
	hi2s2.Instance=SPI2;
	hi2s2.Init.Mode=I2S_Mode;					//IISģʽ
	hi2s2.Init.Standard=I2S_Standard;			//IIS��׼
	hi2s2.Init.DataFormat=I2S_DataFormat;		//IIS���ݳ���
	hi2s2.Init.MCLKOutput=I2S_MCLKOUTPUT_ENABLE;	//��ʱ�����ʹ��
	hi2s2.Init.AudioFreq=I2S_AUDIOFREQ_DEFAULT;	//IISƵ������
	hi2s2.Init.CPOL=I2S_Clock_Polarity;			//����״̬ʱ�ӵ�ƽ
	hi2s2.Init.ClockSource=I2S_CLOCK_PLL;		//IISʱ��ԴΪPLL
	hi2s2.Init.FullDuplexMode=I2S_FULLDUPLEXMODE_ENABLE;	//IISȫ˫��
	HAL_I2S_Init(&hi2s2); 
	
	SPI2->CR2|=1<<1;									//SPI2 TX DMA����ʹ��.
	I2S2ext->CR2|=1<<0;									//I2S2ext RX DMA����ʹ��.
	__HAL_I2S_ENABLE(&hi2s2);					//ʹ��I2S2	
	__HAL_I2SEXT_ENABLE(&hi2s2);					//ʹ��I2S2ext	
}

//�����ʼ��㹫ʽ:Fs=I2SxCLK/[256*(2*I2SDIV+ODD)]
//I2SxCLK=(HSE/pllm)*PLLI2SN/PLLI2SR
//һ��HSE=8Mhz 
//pllm:��Sys_Clock_Set���õ�ʱ��ȷ����һ����8
//PLLI2SN:һ����192~432 
//PLLI2SR:2~7
//I2SDIV:2~255
//ODD:0/1
//I2S��Ƶϵ����@pllm=8,HSE=8Mhz,��vco����Ƶ��Ϊ1Mhz
//���ʽ:������/10,PLLI2SN,PLLI2SR,I2SDIV,ODD
const u16 I2S_PSC_TBL[][5]=
{
	{800 ,256,5,12,1},		//8Khz������
	{1102,429,4,19,0},		//11.025Khz������ 
	{1600,213,2,13,0},		//16Khz������
	{2205,429,4, 9,1},		//22.05Khz������
	{3200,213,2, 6,1},		//32Khz������
	{4410,271,2, 6,0},		//44.1Khz������
	{4800,258,3, 3,1},		//48Khz������
	{8820,316,2, 3,1},		//88.2Khz������
	{9600,344,2, 3,1},  	//96Khz������
	{17640,361,2,2,0},  	//176.4Khz������ 
	{19200,393,2,2,0},  	//192Khz������
};

//����I2S��DMA����,HAL��û���ṩ�˺���
//���������Ҫ�Լ������Ĵ�����дһ��
void I2S_DMA_Enable(void)
{
    u32 tempreg=0;
    tempreg=SPI2->CR2;    	//�ȶ�����ǰ������			
	tempreg|=1<<1;			//ʹ��DMA
	SPI2->CR2=tempreg;		//д��CR1�Ĵ�����
}

//����SAIA�Ĳ�����(@MCKEN)
//samplerate:������,��λ:Hz
//����ֵ:0,���óɹ�;1,�޷�����.
u8 I2S2_SampleRate_Set(u32 samplerate)
{   
    u8 i=0; 
	u32 tempreg=0;
    RCC_PeriphCLKInitTypeDef RCCI2S2_ClkInitSture;  
	
	for(i=0;i<(sizeof(I2S_PSC_TBL)/10);i++)//�����Ĳ������Ƿ����֧��
	{
		if((samplerate/10)==I2S_PSC_TBL[i][0])break;
	}
    if(i==(sizeof(I2S_PSC_TBL)/10))return 1;//�ѱ���Ҳ�Ҳ���
	
    RCCI2S2_ClkInitSture.PeriphClockSelection=RCC_PERIPHCLK_I2S;	//����ʱ��Դѡ�� 
    RCCI2S2_ClkInitSture.PLLI2S.PLLI2SN=(u32)I2S_PSC_TBL[i][1];    	//����PLLI2SN
    RCCI2S2_ClkInitSture.PLLI2S.PLLI2SR=(u32)I2S_PSC_TBL[i][2];    	//����PLLI2SR
    HAL_RCCEx_PeriphCLKConfig(&RCCI2S2_ClkInitSture);             	//����ʱ��
	
	RCC->CR|=1<<26;					//����I2Sʱ��
	while((RCC->CR&1<<27)==0);		//�ȴ�I2Sʱ�ӿ����ɹ�. 
	tempreg=I2S_PSC_TBL[i][3]<<0;	//����I2SDIV
	tempreg|=I2S_PSC_TBL[i][4]<<8;	//����ODDλ
	tempreg|=1<<9;					//ʹ��MCKOEλ,���MCK
	SPI2->I2SPR=tempreg;			//����I2SPR�Ĵ��� 
	return 0;
} 

//I2S2 TX DMA����
//����Ϊ˫����ģʽ,������DMA��������ж�
//buf0:M0AR��ַ.
//buf1:M1AR��ַ.
//num:ÿ�δ���������
void I2S2_TX_DMA_Init(u8* buf0,u8 *buf1,u16 num)
{  
    __HAL_RCC_DMA1_CLK_ENABLE();                                    		//ʹ��DMA1ʱ��
    __HAL_LINKDMA(&hi2s2,hdmatx,hdma_spi2_tx);         		//��DMA��I2S��ϵ����
	
    hdma_spi2_tx.Instance=DMA1_Stream4;                       		//DMA1������4                     
    hdma_spi2_tx.Init.Channel=DMA_CHANNEL_0;                  		//ͨ��0
    hdma_spi2_tx.Init.Direction=DMA_MEMORY_TO_PERIPH;         		//�洢��������ģʽ
    hdma_spi2_tx.Init.PeriphInc=DMA_PINC_DISABLE;             		//���������ģʽ
    hdma_spi2_tx.Init.MemInc=DMA_MINC_ENABLE;                 		//�洢������ģʽ
    hdma_spi2_tx.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD;   	//�������ݳ���:16λ
    hdma_spi2_tx.Init.MemDataAlignment=DMA_MDATAALIGN_HALFWORD;    	//�洢�����ݳ���:16λ
    hdma_spi2_tx.Init.Mode=DMA_CIRCULAR;                      		//ʹ��ѭ��ģʽ 
    hdma_spi2_tx.Init.Priority=DMA_PRIORITY_HIGH;             		//�����ȼ�
    hdma_spi2_tx.Init.FIFOMode=DMA_FIFOMODE_DISABLE;          		//��ʹ��FIFO
    hdma_spi2_tx.Init.MemBurst=DMA_MBURST_SINGLE;             		//�洢������ͻ������
    hdma_spi2_tx.Init.PeriphBurst=DMA_PBURST_SINGLE;          		//����ͻ�����δ��� 
    HAL_DMA_DeInit(&hdma_spi2_tx);                            		//�������ǰ������
    HAL_DMA_Init(&hdma_spi2_tx);	                            		//��ʼ��DMA

    HAL_DMAEx_MultiBufferStart(&hdma_spi2_tx,(u32)buf0,(u32)&SPI2->DR,(u32)buf1,num);//����˫����
    __HAL_DMA_DISABLE(&hdma_spi2_tx);                         		//�ȹر�DMA 
    SysDelay_us(10);                                                   		//10us��ʱ����ֹ-O2�Ż������� 	
    __HAL_DMA_ENABLE_IT(&hdma_spi2_tx,DMA_IT_TC);             		//������������ж�
    __HAL_DMA_CLEAR_FLAG(&hdma_spi2_tx,DMA_FLAG_TCIF0_4);     		//���DMA��������жϱ�־λ
    HAL_NVIC_SetPriority(DMA1_Stream4_IRQn,5,0);                    		//DMA�ж����ȼ�
    HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
} 

//I2S2ext RX DMA����
//����Ϊ˫����ģʽ,������DMA��������ж�
//buf0:M0AR��ַ.
//buf1:M1AR��ַ.
//num:ÿ�δ���������
void I2S2ext_RX_DMA_Init(u8* buf0,u8 *buf1,u16 num)
{  
	__HAL_RCC_DMA1_CLK_ENABLE();                                    		//ʹ��DMA1ʱ��
    __HAL_LINKDMA(&hi2s2,hdmarx,hdma_i2s2_ext_rx);         		//��DMA��I2S��ϵ����
	
    hdma_i2s2_ext_rx.Instance=DMA1_Stream3;                       		//DMA1������3                    
    hdma_i2s2_ext_rx.Init.Channel=DMA_CHANNEL_3;                  		//ͨ��3
    hdma_i2s2_ext_rx.Init.Direction=DMA_PERIPH_TO_MEMORY;         		//���赽�洢��ģʽ
    hdma_i2s2_ext_rx.Init.PeriphInc=DMA_PINC_DISABLE;             		//���������ģʽ
    hdma_i2s2_ext_rx.Init.MemInc=DMA_MINC_ENABLE;                 		//�洢������ģʽ
    hdma_i2s2_ext_rx.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD;   	//�������ݳ���:16λ
    hdma_i2s2_ext_rx.Init.MemDataAlignment=DMA_MDATAALIGN_HALFWORD;    	//�洢�����ݳ���:16λ
    hdma_i2s2_ext_rx.Init.Mode=DMA_CIRCULAR;                      		//ʹ��ѭ��ģʽ 
    hdma_i2s2_ext_rx.Init.Priority=DMA_PRIORITY_MEDIUM;             		//�е����ȼ�
    hdma_i2s2_ext_rx.Init.FIFOMode=DMA_FIFOMODE_DISABLE;          		//��ʹ��FIFO
    hdma_i2s2_ext_rx.Init.MemBurst=DMA_MBURST_SINGLE;             		//�洢������ͻ������
    hdma_i2s2_ext_rx.Init.PeriphBurst=DMA_PBURST_SINGLE;          		//����ͻ�����δ��� 
    HAL_DMA_DeInit(&hdma_i2s2_ext_rx);                            		//�������ǰ������
    HAL_DMA_Init(&hdma_i2s2_ext_rx);	                            		//��ʼ��DMA

    HAL_DMAEx_MultiBufferStart(&hdma_i2s2_ext_rx,(u32)&I2S2ext->DR,(u32)buf0,(u32)buf1,num);//����˫����
    __HAL_DMA_DISABLE(&hdma_i2s2_ext_rx);                         		//�ȹر�DMA 
    SysDelay_us(10);                                                   		//10us��ʱ����ֹ-O2�Ż������� 	
    __HAL_DMA_ENABLE_IT(&hdma_i2s2_ext_rx,DMA_IT_TC);             		//������������ж�
    __HAL_DMA_CLEAR_FLAG(&hdma_i2s2_ext_rx,DMA_FLAG_TCIF3_7);     		//���DMA��������жϱ�־λ
	
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn,5,0);   
    HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);	
} 

//I2S DMA�ص�����ָ��
void (*i2s_tx_callback)(void);	//TX�ص����� 
void (*i2s_rx_callback)(void);	//RX�ص�����

//DMA1_Stream4�жϷ�����
void DMA1_Stream4_IRQHandler_callback(void)
{  
    if(__HAL_DMA_GET_FLAG(&hdma_spi2_tx,DMA_FLAG_TCIF0_4)!=RESET) //DMA�������
    {
        __HAL_DMA_CLEAR_FLAG(&hdma_spi2_tx,DMA_FLAG_TCIF0_4);     //���DMA��������жϱ�־λ
        i2s_tx_callback();	//ִ�лص�����,��ȡ���ݵȲ����������洦��  
    } 
} 

//DMA1_Stream3�жϷ�����
void DMA1_Stream3_IRQHandler_callback(void)
{ 
    if(__HAL_DMA_GET_FLAG(&hdma_i2s2_ext_rx,DMA_FLAG_TCIF3_7)!=RESET) //DMA�������
    {
        __HAL_DMA_CLEAR_FLAG(&hdma_i2s2_ext_rx,DMA_FLAG_TCIF3_7);     //���DMA��������жϱ�־λ
		if(i2s_rx_callback!=NULL) i2s_rx_callback();	//ִ�лص�����,��ȡ���ݵȲ����������洦��  
    } 											 
} 

//I2S��ʼ����
void I2S_Play_Start(void)
{   	
	__HAL_DMA_ENABLE(&hdma_spi2_tx);//����DMA TX����  			
}

//�ر�I2S����
void I2S_Play_Stop(void)
{ 
	__HAL_DMA_DISABLE(&hdma_spi2_tx);//��������;		 	 
} 

//I2S��ʼ¼��
void I2S_Rec_Start(void)
{ 
	__HAL_DMA_ENABLE(&hdma_i2s2_ext_rx); //����DMA RX����  				
}

//�ر�I2S¼��
void I2S_Rec_Stop(void)
{   
	__HAL_DMA_DISABLE(&hdma_i2s2_ext_rx); //����¼��				 
}





