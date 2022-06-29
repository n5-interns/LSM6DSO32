/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "accelerometer.h"



void Initaccelerometer(void){
    I2CM_Start();
    CyDelay(10);
    uint8_t toSend;
    
    //enable tilt detection
    sendByte(0x01, 0x80);  //enable access to embedded functions registers
    sendByte(0x04, 0x10);  //enable tilt detection
    sendByte(0x17, 0x80);  //enable page wr
    sendByte(0x01, 0x00);  // disable access to embedded functions registers
    //////////////
    //turn on gyro and acc
    sendByte(0x10,0xA8); //6.6 kHz, 8g acc
    sendByte(0x11,0xA4); //6.6 kHz, 500 dps gyro
    //////////////
    toSend = readByte(0x12);
    toSend &= 0b10111111;
    toSend |= 0b01000000;
    sendByte(0x12,toSend); //enable BDU
    

}

void sendByte(uint8_t RegAddress, uint8_t toSend){
    
    
    I2CM_Start();
    CyDelayUs(100);
    I2CM_MasterSendStart( ACC_ADDR, 0x00);
    CyDelayUs(100);
    I2CM_MasterWriteByte(RegAddress);
    CyDelayUs(100);
    I2CM_MasterWriteByte(toSend);
    CyDelayUs(100); 
    I2CM_MasterSendStop();

}

uint8_t readByte(uint8_t RegAddress){
    
    uint8_t receivedByte;
    I2CM_Start();
    CyDelayUs(100);
    I2CM_MasterSendStart( ACC_ADDR, 0x00);
    CyDelayUs(100);
    I2CM_MasterWriteByte(RegAddress);
    CyDelayUs(100);
    I2CM_MasterSendRestart( ACC_ADDR, 0x01);
    CyDelayUs(100);
    receivedByte = I2CM_MasterReadByte(I2CM_NAK_DATA);
    CyDelayUs(100);
    I2CM_MasterSendStop();
    
    return receivedByte;
}

int16_t ReadAndconcatenatevalues(uint8_t LowbyteAddr, uint8_t HighByteAddr){
    
    volatile int16_t aux;
    volatile int8_t LSB, MSB;
    LSB = readByte(LowbyteAddr);
    MSB = readByte(HighByteAddr);
    
    aux = MSB;
    aux = (aux << 8)&0xff00;
    aux += (LSB&0xff);
    volatile float test;
    test = calcAccel(aux);
    
    return aux;
}


float readFloatAccelX() {
	float output = calcAccel(ReadAndconcatenatevalues(0x28,0x29));
	return output;
}



float readFloatAccelY()
{
	float output = calcAccel(ReadAndconcatenatevalues(0x2A,0x2B));
	return output;
}


float readFloatAccelZ()
{
	float output = calcAccel(ReadAndconcatenatevalues(0x2C,0x2D));
	return output;
}

float calcAccel( int16_t input )
{
  uint8_t accelRange; 
  float output;

  accelRange = readByte(0x10);
  accelRange = accelRange & 0x0C;
  
    switch( accelRange ){
      case 0x00:// Register value 0: 4g
        output = ((float)input * (.122)) / 1000;
        break;
      case 0x04: //Register value 1 : 32g
        output = ((float)input * (.976)) / 1000;
        break;
      case 0x08: //Register value 2 : 8g
        output = ((float)input * (.244)) / 1000;
        break;
      case 0x0C://Register value 3: 16g
        output = ((float)input * (.488)) / 1000;
        break;
    }


	return output;
}

float readFloatGyroX() {

	float output = calcGyro(ReadAndconcatenatevalues(0x22,0x23));
	return output;
}

float readFloatGyroY() {
  
	float output = calcGyro(ReadAndconcatenatevalues(0x24,0x25));
	return output;
}

float readFloatGyroZ() {

	float output = calcGyro(ReadAndconcatenatevalues(0x26,0x27));
	return output;

}

float calcGyro( int16_t input ) {

  uint8_t gyroRange;  
  
  float output; 

  gyroRange = readByte(0x11) ;
  gyroRange = gyroRange & 0x0C;

 
    switch( gyroRange ){
      case 0x00:
        output = ((float)input * 8.75)/1000;
        break;
      case 0x04:
        output = ((float)input * 17.50)/1000;
        break;
      case 0x08:
        output = ((float)input * 35)/1000;
        break;
      case 0x0C:
        output = ((float)input * 70)/1000;
        break;
    }
  

  return output;
}

uint8_t isTilt(){
    
    uint8_t response,aux;
        
   
    sendByte(0x01,0x80);
    aux = readByte(0x12);
    sendByte(0x01,0x00);
    
    
    if (aux == 0x10){
        return 0x01;
    }
    else{
        return 0x00;
    }
}

void getaccData (ACCVALUES *dev){
    
    volatile uint8_t status,mask,aux;
    int16_t value;
    
    //get ID
    dev->ID = readByte(0x0F);
    if (dev->ID != 0x6C){
        exit(0);
    }
    
    
    //read acc
    dev->xacc = readFloatAccelX();
    dev->yacc = readFloatAccelY();
    dev->zacc = readFloatAccelZ();
    
    //read gyro
    dev->pitchaxisx = readFloatGyroX();
    dev->rollaxisy = readFloatGyroY();
    dev->yawaxisz = readFloatGyroZ();
    
    //read temp 
    status = readByte(0x1E);
    mask = status & 0x04;
    if (mask == 0x04){
        dev->flag = 1; //new data available
        //reading temperature
        value = ReadAndconcatenatevalues(0x20,0x21);
        dev->temperature = (float)(value/256.0 + 25);
    }
    else if (status == 0x00){
        dev->flag = 0;
      
    }
    
    //read tilt
    dev->tiltdetect = isTilt();
         
    CyDelay(1000);
    
}



