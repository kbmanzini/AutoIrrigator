#include <SoftwareSerial.h>
#include <string.h>

SoftwareSerial mySerial(3,2);   //软串口，RX--3，TX--2

char buf[100]={0};
#define ACK_ERROR 1
#define ACK_OK 0

//发送AT指令函数
//commnd ---AT指令
//ack ---关键字
//time ---执行的间隔时长
uint8_t Send_Commnd(char* commnd ,uint8_t*ack,  unsigned long time)
{
  memset(buf,0,sizeof(buf));
  while(*commnd)  //一个一个字母发送
  {
    mySerial.print(*commnd);
    commnd++;
  }
  mySerial.readBytes(buf,sizeof(buf));
  Serial.println(buf);  //串口打印调试

  delay(time);
 
  if(strstr((char *)buf,(char *)ack) == NULL) //判断ESP01S打印的内容中是否有 ack 关键字
  {
    return ACK_ERROR;   
  }
  else
  {
    return ACK_OK;
  }     
}

//向服务器发送数据
//string ---发送的数据
void Send(char *string)
{
  uint64_t len; //数据的长度

  len= strlen(string);

  char buf[255];
  sprintf(buf, "AT+CIPSEND=%d\r\n",len);  //将数据的长度与AT指令打包

  while(Send_Commnd(buf,(uint8_t*)"OK",1000)!= ACK_OK); //向ESP01S发送AT指令

  delay(5);

  mySerial.println(string); //发送数据

}

//ESP01S连接网络，服务器
void wifi_init(void)
{
  while(Send_Commnd((char*)"AT\r\n",(uint8_t*)"OK",500)!= ACK_OK);  //
  while(Send_Commnd((char*)"AT+RESTORE\r\n",(uint8_t*)"OK",500)!= ACK_OK);
  while(Send_Commnd((char*)"AT+CWMODE=1\r\n",(uint8_t*)"OK",1000)!= ACK_OK);
  while(Send_Commnd((char*)"AT+CWJAP=\"mmm\",\"123456879\"\r\n",(uint8_t*)"OK",1000)!= ACK_OK);
  //while(Send_Commnd((char*)"AT+CIFSR\r\n",(uint8_t*)"OK",(uint8_t)500)!= ACK_OK);
  while(Send_Commnd((char*)"AT+CIPSTART=\"TCP\",\"192.168.3.100\",777\r\n",(uint8_t*)"CONNECT",1000)!= ACK_OK);
  while(Send_Commnd((char*)"AT+CIPMODE=0\r\n",(uint8_t*)"OK",(uint8_t)1000)!= ACK_OK);
  //while(Send_Commnd((char*)"AT+CIPSEND\r\n",(uint8_t*)"OK",(uint8_t)1000)!= ACK_OK);
  //while(Send_Commnd((char*)"AT+CIPSEND=4\r\n",(uint8_t*)"OK",500)!= ACK_OK);
  //mySerial.print("1234");

  Send("123.123445f");

  Serial.println("wifi init ok\r\n");

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  mySerial.begin(115200);

  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  
  pinMode(A3,OUTPUT);
  pinMode(A4,OUTPUT);
  pinMode(A5,OUTPUT);

  wifi_init();

}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t updataTime[3];

  char channel_1[255];
  char channel_2[255];
  char channel_3[255];

  float GetA0=analogRead(A0);
  float GetA1=analogRead(A1);
  float GetA2=analogRead(A2);

  float GetA3=analogRead(A3);
  float GetA4=analogRead(A4);
  float GetA5=analogRead(A5);
  
  bool Status_1 = 1;
  bool Status_2 = 0;
  bool Status_3 = 0;

  if (millis() - updataTime[0] >= 3090)
  {
    updataTime[0] = millis();

    GetA0+=21.6;
    GetA1+=11.3;
    GetA2+=33.98;
    GetA3+=5.533;
    GetA4+=23.3;
    GetA5+=23.1;

    Status_1=!Status_1;

  }

  if (millis() - updataTime[1] >= 4550)
  {
    updataTime[1] = millis();

    GetA0+=2.6;
    GetA1+=3.3;
    GetA2+=11.98;
    GetA3+=14.533;
    GetA4+=4.3;
    GetA5+=7.1;

    Status_2=!Status_2;

  }

  if (millis() - updataTime[2] >= 6070)
  {
    updataTime[2] = millis();

    GetA0+=8.6;
    GetA1+=6.3;
    GetA2+=9.98;
    GetA3+=11.533;
    GetA4+=2.3;
    GetA5+=1.1;

    Status_3=!Status_3;

  }

  //将三个通道的数据打包
  sprintf(channel_1,"channel 1 temp is %d,humi is %d,the relay is %d",GetA0,GetA3,Status_1);
  sprintf(channel_2,"channel 2 temp is %d,humi is %d,the relay is %d",GetA1,GetA4,Status_2);
  sprintf(channel_3,"channel 3 temp is %d,humi is %d,the relay is %d",GetA2,GetA5,Status_3);

  Send(channel_1);
  Send(" ");
  Send("--------------------");
  Send(" ");
  Send(channel_2);
  Send(" ");
  Send("--------------------");
  Send(" ");
  Send(channel_3);
  Send(" ");
  Send("--------------------");
  Send(" ");
  delay(100);

}
