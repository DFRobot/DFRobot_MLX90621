#ifndef __DFROBOT_MLX90621_H__
#define __DFROBOT_MLX90621_H__

#include <Arduino.h>
#include <Wire.h>

#define Rate_512Hz    0b0000
#define Rate_256Hz    0b0110
#define Rate_128Hz    0b0111
#define Rate_64Hz     0b1000
#define Rate_32Hz     0b1001
#define Rate_16Hz     0b1010
#define Rate_8Hz      0b1011
#define Rate_4Hz      0b1100
#define Rate_2Hz      0b1101
#define Rate_1Hz      0b1110

#define Res_15bit     0b00
#define Res_16bit     0b01
#define Res_17bit     0b10
#define Res_18bit     0b11

#define VTH_L         0xDA
#define VTH_H         0xDB
#define KT1_L         0xDC
#define KT1_H         0xDD
#define KT2_L         0xDE
#define KT2_H         0xDF
#define KT_SCALE      0xD2

#define CAL_ACOMMON_L 0xD0
#define CAL_ACOMMON_H 0xD1
#define CAL_ACP_L     0xD3
#define CAL_ACP_H     0xD4
#define CAL_BCP       0xD5
#define CAL_alphaCP_L 0xD6
#define CAL_alphaCP_H 0xD7
#define CAL_TGC       0xD8
#define CAL_AI_SCALE  0xD9
#define CAL_BI_SCALE  0xD9

#define CAL_A0_L      0xE0
#define CAL_A0_H      0xE1
#define CAL_A0_SCALE  0xE2
#define CAL_DE_SCALE  0xE3
#define CAL_EMIS_L    0xE4
#define CAL_EMIS_H    0xE5
#define CAL_KSTA_L    0xE6
#define CAL_KSTA_H    0xE7

class DFRobot_MLX90621
{
public:
    void      initMLX90621(void);
    void      readEEPROM(void);
    void      readIR(void);
    void      getPTAT(void);
    void      getCPIX(void);
    void      writeTrimming(void);
    void      makeConfig(void);
    void      setRefreshRate(uint8_t refrate = Rate_1Hz);
    void      setResolution(uint8_t res = Res_18bit);
    void      getAmbient_T(void);
    void      measureTemperature(void);
    float     getAmbientTemperature(void);
    float     getTemperature(uint8_t i,uint8_t j);
    float     getMaxTemperature(void);
    float     getMinTemperature(void);
    uint16_t  readConfig(void);
    int16_t   calculate_16(uint8_t highByte, uint8_t lowByte);
    int8_t    calculate_8(uint8_t byte);
    bool      checkConfig(void);
private:
    byte     refreshRate;
    byte     resolution;
    uint16_t ptat;
    uint8_t  eepromData [256];
    int16_t  cpix;
    int16_t  irData[64]; 
    float    Ambient_T;
    float    temperatures[64];
    float    resolution_comp;
    float    maxTemp;
    float    minTemp;
};

#endif