#include<Arduino.h>
#include <HardwareSerial.h>    //导入ESP32串口操作库,使用这个库我们可以把串口映射到其他的引脚上使用
#include<Wire.h>
////////////////////////////////////指令集///////////////////////////////////////////////
const byte SYSCMD[4][8] = {
  {0x01, 0x06, 0x00, 0x20, 0x00, 0x8C, 0x89, 0xA5},//01 06 00 20 00 8C 89 A5设置offcet校准距离140mm
  {0x01, 0x06, 0x00, 0x21, 0x00, 0x64, 0xD8, 0x2B},//01 06 00 21 00 64 D8 2B设置xtalk 校准距离100mm
  {0x01, 0x06, 0x00, 0x06, 0x00, 0x01, 0xA8, 0x0B},//01 06 00 06 00 01 A8 0B加载校准
  {0x01, 0x06, 0x00, 0x01, 0x10, 0x00, 0xD5, 0xCA},//01 06 00 01 10 00 D5 CA 测距模块重启
};
const byte distanceCMD[2][8] = {
  {0x01, 0x06, 0x00, 0x04, 0x00, 0x00, 0xC8, 0x0B},//01 06 00 04 00 00 C8 0B TOF400测距量程设置为1.3m  /   TOF200测距量程设置为默认
  {0x01, 0x06, 0x00, 0x04, 0x00, 0x01, 0x09, 0xCB},//01 06 00 04 00 01 09 CB TOF400测距量程设置为4.0m  /   TOF200测距量程设置为高精度
};
const byte timeCMD[1][8] = {
  {0x01, 0x06, 0x00, 0x05, 0x01, 0xF4, 0x99, 0xDC},//01 06 00 05 01 F4 99 DC连续输出及输出时间间隔设置,此处设置为500MS
};
///////////////////////////////////////////////////////////////////////////////////////

//我们测试需要用到的串口，ESP32一共有3个串口，串口0应该是用于程序烧写和打印输出调试用的，所以我们只用1和2
HardwareSerial MySerial_stm32(1);  
unsigned short i;
char temp;
//这个函数适用于清理串口读取缓存区的缓存，其实也就是用读一个删除一个的方式清理，我还会打印出有没有进行清理，清理了哪些内容
void clear_usart_buffer(){
  i = MySerial_stm32.available();
  if(i != 0){
    Serial.print("清空串口接收区的缓存......");
    Serial.println(MySerial_stm32.available());
    while(i--)
      MySerial_stm32.read();    //读取串口接收回来的数据但是不做处理只给与打印
    }
  else
    Serial.println("串口接收区的缓存为空!!!");
  }
   
/*    关于串口读取还有一些别的函数，用的不多但是找的资料给大家罗列在这里
 *     
 *     int peek(void);  //返回接收缓存中第一个字节数据，但并不从中删除它；
 *     void flush(void);  //等待串口收发完毕；
 *     下面是两个发送的，我没怎么用过，我一般直接使用println来发送
 *     size_t write(uint8_t);写数据到TX FIFO，在发送FIFO中的数据会自动输出到TX端口上；该方法有很多重载，可以用来发送字符串、长整型、整形；如果TX FIFO已满，则该方法将阻塞；
 *     size_t write(const uint8_t *buffer, size_t size);写数据到TX FIFO，如果发送FIFO已满，则该方法将阻塞；
 *     size_t setRxBufferSize(size_t);  //设置接收缓存大小（默认为256字节）；ESP32默认有128字节的硬件RX FIFO，在RX FIFO收到数据后会移送到上面的接收缓存中；这个挺重要的但是我们直接使用默认的就好
*/
void read_usart(){
  i = MySerial_stm32.available();  //返回目前串口接收区内的已经接受的数据量
  if(i != 0){
    Serial.print("串口接收到的数据量为:");
    Serial.println(MySerial_stm32.available());
    while(i--){
      temp = MySerial_stm32.read();   //读取一个数据并且将它从缓存区删除
      Serial.print(temp);    //读取串口接收回来的数据但是不做处理只给与打印
      }
    Serial.println("");
    //data_analyse();    //至关重要的一步，也就是把读取回来的数据进行分步截取直接拿到我们想要的数据，我下一篇博文会讲如何自己写这个函数
    }
  else
    Serial.println("串口接收区没有数据！！！");
  }
void modRST()//测距模块重启
{
  for (int i = 0; i < 8; i++)
    MySerial_stm32.write(SYSCMD[3][i]);
}
//*********测距时间间隔函数
void outputTIME500MS()//输出时间间隔500ms
{
  for (int i = 0; i < 8; i++)
    MySerial_stm32.write(timeCMD[0][i]);
}
//*************模式选择****************************//
void shortdistance()//短距离
{
  for (int i = 0; i < 8; i++)
    MySerial_stm32.write(distanceCMD[0][i]);
}

void longdistance()//长距离
{
  for (int i = 0; i < 8; i++)
    MySerial_stm32.write(distanceCMD[1][i]);
}



void setup() {
  Serial.begin(115200);
  //串口的开启，这里还可以传一些别的参数，但是我们只传入下面四个最重要的：波特率，默认SERIAL_8N1为8位数据位、无校验、1位停止位，后面两个分别为 RXD,TXD 引脚
  MySerial_stm32.begin(115200, SERIAL_8N1, 19, 23);
  //MySerial_stm32.updateBaudRate(9600);  //重新设置波特率；  
  //MySerial_stm32.end();  //失能串口，释放占用的资源
  clear_usart_buffer();  //清空接收区缓存函数


  // shortdistance();
  longdistance();

  delay(2000);
  
  outputTIME500MS();//设置输出间隔时间500MS
  delay(2000);
  
  modRST();//测距模块重启
  delay(2000);
}

void loop()
{
  
  if (MySerial_stm32.available() > 6)
  {
    Serial.print("CMD: ");
    char a = MySerial_stm32.read();
    if(a != 0x01)
      return;
    byte Buf[6];
    MySerial_stm32.readBytes(Buf, 6);
    for (int i = 0; i < 6; i++)
    {     
      if (Buf[i] < 0x10)
        Serial.print("0x0");
      else
        Serial.print("0x");
      Serial.print(Buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    if (Buf[2] == 0xFF)
    {
      Serial.print("Distance:");
      Serial.println("invalid");
      return;
    }
    
    long distance = Buf[2] * 256 + Buf[3];
    Serial.print("Distance:");
    Serial.print(distance);
    Serial.print("mm");
    Serial.println();
    
  }
}
