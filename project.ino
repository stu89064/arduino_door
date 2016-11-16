/*配置腳位
     RFID模組   蜂鳴器   繼電器   測距器  |RFID
0                                         |
1                                         |VCC  -3.3V
2----------------------------------TRIG   |RST  -5
3--------------------------S              |GND  -GND
4                                         |MISO -12
5------RST                                |MOSI -11
6----------------------------------ECHO   |SCK  -13
7                                         |NSS  -10
8-----------------S                       |
9                                         |
10-----NSS                                |
11-----MOSI                               |
12-----MISO                               |
13-----SCK                                |
                                          |
3.3V---VSS                                |
5V-------------------------+              |
GND----GND--------GND-----GND             |
*/
#include <SPI.h>
#include <Freq.h>
#include <Melody.h>
#include <Rfid.h>
#include <Ultra.h>

uchar serNum[5];

void setup()
{
    Serial.begin(57600);//RFID
    SPI.begin();
    
    pinMode(trigPin, OUTPUT);//測距器
    pinMode(echoPin, INPUT);
 
    pinMode(chipSelectPin,OUTPUT); //PIN 10
    digitalWrite(chipSelectPin, LOW); //ACTIVE
    pinMode(NRSTPD,OUTPUT); //PIN 5
 
    MFRC522_Init();
    pinMode(3,OUTPUT); //繼電器PIN3
    digitalWrite(3,LOW);
}

void loop()
{
	sensor();
	uchar status;
	uchar str[MAX_LEN];

	status = MFRC522_Request(PICC_REQIDL, str);
	if (status != MI_OK)
	{
			return;
	}
	status = MFRC522_Anticoll(str);
	if (status == MI_OK)
	{
		memcpy(serNum, str, 5);
		uchar* id = serNum;
		if(1)
		//卡號儲存在id[0]-id[3]四個數值裡，寫規則判斷
		{
			Serial.println("SUCCESS! Door opened for 5 seconds.");
			opendoor();
        }
        else
		{
			Serial.println("FAILED");
			Serial.print("The card ID is: ");
			memcpy(serNum, str, 5);
			ShowCardID(serNum);
			uchar* id = serNum;
			play(melody_wrong, noteDurations_default, sizeof(melody_default) / sizeof(int));
		}
	}
    //MFRC522_Halt();
}

void ShowCardID(uchar *id)
{
	int IDlen=4;
	for(int i=0; i<IDlen; i++)
	{
    	Serial.print(0x0F & (id[i]>>4), HEX);
        Serial.print(0x0F & id[i],HEX);
    }
    Serial.println("");
}

 
void Write_MFRC522(uchar addr, uchar val)
{
    digitalWrite(chipSelectPin, LOW);
    //address format：0XXXXXX0
    SPI.transfer((addr<<1)&0x7E);
    SPI.transfer(val);
    digitalWrite(chipSelectPin, HIGH);
}

uchar Read_MFRC522(uchar addr)
{
    uchar val;
    digitalWrite(chipSelectPin, LOW);
    SPI.transfer(((addr<<1)&0x7E) | 0x80);
    val =SPI.transfer(0x00);
    digitalWrite(chipSelectPin, HIGH);
    return val;
}

void SetBitMask(uchar reg, uchar mask)
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp | mask); // set bit mask
}

void ClearBitMask(uchar reg, uchar mask)
{
	uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & (~mask)); // clear bit mask
}

void AntennaOn(void)
{
    uchar temp;
    temp = Read_MFRC522(TxControlReg);
    if (!(temp & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}

void AntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}

void MFRC522_Reset(void)
{
    Write_MFRC522(CommandReg, PCD_RESETPHASE);
}

void MFRC522_Init(void)
{
    digitalWrite(NRSTPD,HIGH);
    MFRC522_Reset();
    Write_MFRC522(TModeReg, 0x8D); //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    Write_MFRC522(TPrescalerReg, 0x3E); //TModeReg[3..0] + TPrescalerReg
    Write_MFRC522(TReloadRegL, 30);
    Write_MFRC522(TReloadRegH, 0);
    Write_MFRC522(TxAutoReg, 0x40); //100%ASK
    Write_MFRC522(ModeReg, 0x3D); //CRC initilizate value 0x6363
    AntennaOn(); //turn on antenna
}

uchar MFRC522_Request(uchar reqMode, uchar *TagType)
{
    uchar status;
    uint backBits; //the data bits that received
    Write_MFRC522(BitFramingReg, 0x07); //TxLastBists = BitFramingReg[2..0] ???
    TagType[0] = reqMode;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
    if ((status != MI_OK) || (backBits != 0x10))
    {
        status = MI_ERR;
    }
    return status;
}

uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen)
{
    uchar status = MI_ERR;
    uchar irqEn = 0x00;
    uchar waitIRq = 0x00;
    uchar lastBits;
    uchar n;
    uint i;
    switch (command)
    {
        case PCD_AUTHENT: //verify card password
        {
            irqEn = 0x12;
            waitIRq = 0x10;
            break;
        }
        case PCD_TRANSCEIVE: //send data in the FIFO
        {
            irqEn = 0x77;
            waitIRq = 0x30;
            break;
        }
        default:
            break;
    }
    Write_MFRC522(CommIEnReg, irqEn|0x80); //Allow interruption
    ClearBitMask(CommIrqReg, 0x80); //Clear all the interrupt bits
    SetBitMask(FIFOLevelReg, 0x80); //FlushBuffer=1, FIFO initilizate
    Write_MFRC522(CommandReg, PCD_IDLE); //NO action;cancel current command ???
    for (i=0; i<sendLen; i++)
    {
        Write_MFRC522(FIFODataReg, sendData[i]);
    }
    //procceed it
    Write_MFRC522(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg, 0x80); //StartSend=1,transmission of data starts
    }
    i = 2000; //i should adjust according the clock, the maxium the waiting time should be 25 ms???
    do
    {
        n = Read_MFRC522(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));
 
    ClearBitMask(BitFramingReg, 0x80); //StartSend=0
 
    if (i != 0)
    {
        if(!(Read_MFRC522(ErrorReg) & 0x1B)) //BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR; //??
            }
 
            if (command == PCD_TRANSCEIVE)
            {
                n = Read_MFRC522(FIFOLevelReg);
                lastBits = Read_MFRC522(ControlReg) & 0x07;
                if (lastBits)
                {
                    *backLen = (n-1)*8 + lastBits;
                }
                else
                {
                    *backLen = n*8;
                }
 
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAX_LEN)
                {
                    n = MAX_LEN;
                }
 
                //read the data from FIFO
                for (i=0; i<n; i++)
                {
                    backData[i] = Read_MFRC522(FIFODataReg);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }
    }
    //SetBitMask(ControlReg,0x80); //timer stops
    //Write_MFRC522(CommandReg, PCD_IDLE);
    return status;
}

uchar MFRC522_Anticoll(uchar *serNum)
{
    uchar status;
    uchar i;
    uchar serNumCheck=0;
    uint unLen;
    Write_MFRC522(BitFramingReg, 0x00); //TxLastBists = BitFramingReg[2..0]
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
    if (status == MI_OK)
    {
        //Verify card serial number
        for (i=0; i<4; i++)
        {
            serNumCheck ^= serNum[i];
        }
        if (serNumCheck != serNum[i])
        {
            status = MI_ERR;
        }
    }
    //SetBitMask(CollReg, 0x80); //ValuesAfterColl=1
    return status;
}

void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData)
{
    uchar i, n;
    ClearBitMask(DivIrqReg, 0x04); //CRCIrq = 0
    SetBitMask(FIFOLevelReg, 0x80); //Clear FIFO pointer
    for (i=0; i<len; i++)
    {
        Write_MFRC522(FIFODataReg, *(pIndata+i));
    }
    Write_MFRC522(CommandReg, PCD_CALCCRC);
    //waite CRC caculation to finish
    i = 0xFF;
    do
    {
        n = Read_MFRC522(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04)); //CRCIrq = 1
    pOutData[0] = Read_MFRC522(CRCResultRegL);
    pOutData[1] = Read_MFRC522(CRCResultRegM);
}

uchar MFRC522_Write(uchar blockAddr, uchar *writeData)
{
    uchar status;
    uint recvBits;
    uchar i;
    uchar buff[18];
    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    CalulateCRC(buff, 2, &buff[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }
    if (status == MI_OK)
    {
        for (i=0; i<16; i++) //Write 16 bytes data into FIFO
        {
            buff[i] = *(writeData+i);
        }
        CalulateCRC(buff, 16, &buff[16]);
        status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
 
        if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }
    return status;
}

void MFRC522_Halt(void)
{
    uchar status;
    uint unLen;
    uchar buff[4];
    buff[0] = PICC_HALT;
    buff[1] = 0;
    CalulateCRC(buff, 2, &buff[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}

void sensor()
{
	float duration, distances;
	int intervaltime=500;
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(1000);
	digitalWrite(trigPin, LOW);
	duration = pulseIn(echoPin, HIGH);
	distances = (duration/2) / 29.1;
	if( distances < 10 && distances != 0)
	{
		Serial.println("Internal sensor activated!    Door opened for 5 seconds.");
		digitalWrite(3,HIGH);
		play(melody_default, noteDurations_default, sizeof(melody_default) / sizeof(int));
		delay(5000);
		digitalWrite(3,LOW);
		Serial.println("                              Door locked due to timeout.");
	}
}

void opendoor()
{
	digitalWrite(3,HIGH);
	play(melody_default, noteDurations_default, sizeof(melody_default) / sizeof(int));
	delay(5000);
	digitalWrite(3,LOW);
	Serial.println("                              Door locked due to timeout.");
}

