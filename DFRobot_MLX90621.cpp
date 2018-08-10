#include "DFRobot_MLX90621.h"

void DFRobot_MLX90621::initMLX90621()
{
    Wire.begin();
    Wire.setClock(400000L);
    delay(5);
    setRefreshRate(Rate_8Hz);
    setResolution(Res_17bit);
    readEEPROM();
    writeTrimming();
    makeConfig();
}

void DFRobot_MLX90621::setRefreshRate(uint8_t refrate)
{
    refreshRate = refrate & 0b1111;
}

void DFRobot_MLX90621::setResolution(uint8_t res)
{
    resolution      = res & 0b11;
    resolution_comp = pow(2.0, (3 - resolution));
}

void DFRobot_MLX90621::readEEPROM()
{
    for(int j=0;j<256;j+=32){
        Wire.beginTransmission(0x50);
        Wire.write(j);
        Wire.endTransmission(false);
        Wire.requestFrom(0x50, 32);
        for(int i = 0; i < 32; i++){
            eepromData[j+i] = (uint8_t)Wire.read();
        }
    }
}

void DFRobot_MLX90621::readIR()
{
    for(int j=0;j<64;j+=16){
        Wire.beginTransmission(0x60);
        Wire.write(0x02);
        Wire.write(j);
        Wire.write(0x01);
        Wire.write(0x20);
        Wire.endTransmission(false);
        Wire.requestFrom(0x60, 32);
        for(int i=0;i<16;i++){
            uint8_t pixelDataLow  = (uint8_t) Wire.read();
            uint8_t pixelDataHigh = (uint8_t) Wire.read();
            irData[j+i] = calculate_16(pixelDataHigh, pixelDataLow);
        }
    }
}

void DFRobot_MLX90621::getPTAT()
{
    Wire.beginTransmission(0x60);
    Wire.write(0x02);
    Wire.write(0x40);
    Wire.write(0x00);
    Wire.write(0x01);
    Wire.endTransmission(false);
    Wire.requestFrom(0x60, 2);
    byte ptatLow  = Wire.read();
    byte ptatHigh = Wire.read();
    ptat = (ptatHigh * 256) + ptatLow;
} 

void DFRobot_MLX90621::getCPIX()
{
    Wire.beginTransmission(0x60);
    Wire.write(0x02);
    Wire.write(0x41);
    Wire.write(0x00);
    Wire.write(0x01);
    Wire.endTransmission(false);
    Wire.requestFrom(0x60, 2);
    byte cpixLow  = Wire.read();
    byte cpixHigh = Wire.read();
    cpix = calculate_16(cpixHigh, cpixLow);
}

void DFRobot_MLX90621::writeTrimming()
{
    Wire.beginTransmission(0x60);
    Wire.write(0x04);
    Wire.write((byte) eepromData[0xF7] - 0xAA);
    Wire.write(eepromData[0xF7]);
    Wire.write(0x56);
    Wire.write(0x00);
    Wire.endTransmission();
}

void DFRobot_MLX90621::makeConfig()
{
    byte cfg_LSB         = refreshRate;
    byte defaultConfig_H = 0b01000110;
    bitWrite(cfg_LSB, 5, (resolution >> 1) & 1);
    bitWrite(cfg_LSB, 4, (resolution >> 0) & 1);
    Wire.beginTransmission(0x60);
    Wire.write(0x03);
    Wire.write((byte) cfg_LSB - 0x55);
    Wire.write(cfg_LSB);
    Wire.write(defaultConfig_H - 0x55);
    Wire.write(defaultConfig_H);
    Wire.endTransmission();
    uint16_t config_actual   = readConfig();
    uint8_t  real_resolution = (config_actual & 0x30) >> 4;
    setResolution(real_resolution);
}

uint16_t DFRobot_MLX90621::readConfig()
{
    Wire.beginTransmission(0x60);
    Wire.write(0x02);
    Wire.write(0x92);
    Wire.write(0x00);
    Wire.write(0x01);
    Wire.endTransmission(false);
    Wire.requestFrom(0x60,2);
    byte configLow  = Wire.read();
    byte configHigh = Wire.read();
    uint16_t config = ((uint16_t)(configHigh << 8) | configLow);
    return config;
}

bool DFRobot_MLX90621::checkConfig()
{
    bool check = !((readConfig() & 0x0400) >> 10);
    return check;
}

void DFRobot_MLX90621::getAmbient_T()
{
    getPTAT();
    int16_t  kt1_scale , kt2_scale;
    float    vth , kt1 , kt2;
    kt1_scale = (int16_t) (eepromData[KT_SCALE] & 0xF0) >> 4;
    kt2_scale = (int16_t) (eepromData[KT_SCALE] & 0x0F) + 10;
    vth       = (float) calculate_16(eepromData[VTH_H], eepromData[VTH_L]);
    vth       = vth / resolution_comp;
    kt1       = (float) calculate_16(eepromData[KT1_H], eepromData[KT1_L]);
    kt1      /= (pow(2, kt1_scale) * resolution_comp);
    kt2       = (float) calculate_16(eepromData[KT2_H], eepromData[KT2_L]);
    kt2      /= (pow(2, kt2_scale) * resolution_comp);
    Ambient_T = ((-kt1 + sqrt(sq(kt1) - (4 * kt2 * (vth - (float) ptat))))/ (2 * kt2)) + 25.0;
}

int16_t DFRobot_MLX90621::calculate_16(uint8_t highByte, uint8_t lowByte){
    uint16_t combine = 256 * highByte + lowByte;
    if (combine > 32767){
        return (int16_t) (combine - 65536);
    }else{
        return (int16_t) combine;
    }
}

int8_t DFRobot_MLX90621::calculate_8(uint8_t byte)
{
    if(byte > 127){
        return (int8_t) byte - 256;
    }else{
        return (int8_t) byte;
    }
}

void DFRobot_MLX90621::measureTemperature()
{
    getAmbient_T();
    getCPIX();
    readIR();
    int16_t  a_common, ai_scale, bi_scale;
    float    alpha_cp, a_cp, b_cp, tgc, vcp_comp, tak4 ,a_ij, b_ij, vir_off, vir_tgc , alpha_ij, alpha_comp, vir_comp, temperature;
    a_common = calculate_16(eepromData[CAL_ACOMMON_H], eepromData[CAL_ACOMMON_L]);
    ai_scale = (int16_t)(eepromData[CAL_AI_SCALE] & 0xF0) >> 4;
    bi_scale = (int16_t) eepromData[CAL_BI_SCALE] & 0x0F;
    alpha_cp = ((eepromData[CAL_alphaCP_H] << 8) | eepromData[CAL_alphaCP_L]) / (pow(2.0, eepromData[CAL_A0_SCALE]) * resolution_comp);
    a_cp     = (float) calculate_16(eepromData[CAL_ACP_H], eepromData[CAL_ACP_L]) / resolution_comp;
    b_cp     = (float) calculate_8(eepromData[CAL_BCP]) / (pow(2.0, (float)bi_scale) * resolution_comp);
    tgc      = (float) calculate_8(eepromData[CAL_TGC]) / 32.0;
    vcp_comp = (float) cpix - (a_cp + b_cp * (Ambient_T - 25.0));
    tak4     = pow((float) Ambient_T + 273.15, 4.0);
    minTemp  = NULL, maxTemp = NULL;
    for (int i = 0; i < 64; i++) {
        a_ij            = ((float) a_common + eepromData[i] * pow(2.0, ai_scale)) / resolution_comp;
        b_ij            = (float) calculate_8(eepromData[0x40 + i]) / (pow(2.0, bi_scale) * resolution_comp);
        vir_off         = (float) irData[i] - (a_ij + b_ij * (Ambient_T - 25.0));
        vir_tgc         = (float) vir_off - tgc * vcp_comp;
        alpha_ij        = ((float)((eepromData[CAL_A0_H] << 8) | eepromData[CAL_A0_L]) / pow(2.0, (float) eepromData[CAL_A0_SCALE]));
        alpha_ij       += ((float) eepromData[0x80 + i] / pow(2.0, (float) eepromData[CAL_DE_SCALE]));
        alpha_ij        = alpha_ij / resolution_comp;
        alpha_comp      = (alpha_ij - tgc * alpha_cp);
        vir_comp        = vir_tgc;
        temperature     = pow((vir_comp / alpha_comp) + tak4, 1.0 / 4.0) - 273.15;
        temperatures[i] = temperature;
        if (minTemp == NULL || temperature < minTemp) {
            minTemp = temperature;
        }
        if (maxTemp == NULL || temperature > maxTemp) {
            maxTemp = temperature;
        }
    }
}

float DFRobot_MLX90621::getAmbientTemperature()
{
    return Ambient_T;
}

float DFRobot_MLX90621::getTemperature(uint8_t i,uint8_t j)
{
    return temperatures[(i-1)*4+j-1];
}

float DFRobot_MLX90621::getMaxTemperature()
{
    return maxTemp;
}

float DFRobot_MLX90621::getMinTemperature()
{
    return minTemp;
}
