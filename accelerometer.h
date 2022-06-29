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

#include <stdio.h> 

#ifndef accelerometer_h
#define accelerometer_h
    #define ACC_ADDR 0x6B
    void Initaccelerometer(void);// initialize piera sensor 
    typedef struct accvalues_t {
        float temperature,pitchaxisx, rollaxisy, yawaxisz, xacc,yacc,zacc;
        int flag; //0:no new data available, 1: new data available
        uint8_t ID, tiltdetect;
    }ACCVALUES;
    void sendByte(uint8_t RegAddress, uint8_t toSend);
    uint8_t readByte(uint8_t RegAddress);
    int16_t ReadAndconcatenatevalues(uint8_t LowbyteAddr, uint8_t HighByteAddr);
    void getaccData (ACCVALUES *dev);
    float readFloatAccelX(); 
    float readFloatAccelY();
    float readFloatAccelZ();
    float calcAccel( int16_t input );
    float readFloatGyroX();
    float readFloatGyroZ();
    float calcGyro( int16_t input );
    uint8_t isTilt();
    
#endif

/* [] END OF FILE */
