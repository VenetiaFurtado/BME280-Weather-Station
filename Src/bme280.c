#include "spi.h"
#include "bme280.h"


// BME280 Register Addresses
#define BME280_REG_CHIP_ID      0xD0
#define BME280_REG_RESET        0xE0
#define BME280_REG_CTRL_HUM     0xF2
#define BME280_REG_STATUS       0xF3
#define BME280_REG_CTRL_MEAS    0xF4
#define BME280_REG_CONFIG       0xF5
#define BME280_REG_PRESS_MSB    0xF7
#define BME280_REG_TEMP_MSB     0xFA
#define BME280_REG_HUM_MSB      0xFD

// Calibration data registers
#define BME280_REG_CALIB_00     0x88
#define BME280_REG_CALIB_26     0xE1

// Expected chip ID
#define BME280_CHIP_ID          0x60


// BME280 calibration parameters
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} BME280_CalibData;



// Global calibration data
BME280_CalibData calib;
int32_t t_fine;  // Used for temperature compensation

void BME280_WriteReg(const uint8_t reg, const uint8_t value) {
    SPI_Write(reg & 0x7F, value);  // Write: clear MSB
}

uint8_t BME280_ReadReg(const uint8_t reg) {    
    return SPI_Read(reg);
}

void BME280_ReadRegs(uint8_t reg, uint8_t *buffer, uint8_t len) {    
    for (uint8_t i = 0; i < len; i++) {
        buffer[i] = BME280_ReadReg(reg++);
    }
}

// ========== BME280 Initialization ==========
uint8_t BME280_Init(void) {
    uint8_t chip_id;
    uint8_t calib_data[32];
    
    // Check chip ID
    chip_id = BME280_ReadReg(BME280_REG_CHIP_ID);
    if (chip_id != BME280_CHIP_ID) {
        return 0;  // Wrong chip ID
    }
    
    // Soft reset
    BME280_WriteReg(BME280_REG_RESET, 0xB6);
    
    // Wait for reset to complete
    for (volatile int i = 0; i < 100000; i++);
    
    // Read calibration data (Temperature & Pressure)
    BME280_ReadRegs(BME280_REG_CALIB_00, calib_data, 26);
    
    calib.dig_T1 = (calib_data[1] << 8) | calib_data[0];
    calib.dig_T2 = (calib_data[3] << 8) | calib_data[2];
    calib.dig_T3 = (calib_data[5] << 8) | calib_data[4];
    
    calib.dig_P1 = (calib_data[7] << 8) | calib_data[6];
    calib.dig_P2 = (calib_data[9] << 8) | calib_data[8];
    calib.dig_P3 = (calib_data[11] << 8) | calib_data[10];
    calib.dig_P4 = (calib_data[13] << 8) | calib_data[12];
    calib.dig_P5 = (calib_data[15] << 8) | calib_data[14];
    calib.dig_P6 = (calib_data[17] << 8) | calib_data[16];
    calib.dig_P7 = (calib_data[19] << 8) | calib_data[18];
    calib.dig_P8 = (calib_data[21] << 8) | calib_data[20];
    calib.dig_P9 = (calib_data[23] << 8) | calib_data[22];
    
    calib.dig_H1 = calib_data[25];
    
    // Read calibration data (Humidity)
    BME280_ReadRegs(BME280_REG_CALIB_26, calib_data, 7);
    
    calib.dig_H2 = (calib_data[1] << 8) | calib_data[0];
    calib.dig_H3 = calib_data[2];
    calib.dig_H4 = (calib_data[3] << 4) | (calib_data[4] & 0x0F);
    calib.dig_H5 = (calib_data[5] << 4) | (calib_data[4] >> 4);
    calib.dig_H6 = calib_data[6];
    
    // Configure sensor
    // Humidity oversampling x1
    BME280_WriteReg(BME280_REG_CTRL_HUM, 0x01);
    
    // Temperature oversampling x1, Pressure oversampling x1, Normal mode
    BME280_WriteReg(BME280_REG_CTRL_MEAS, 0x27);
    
    // Standby time 0.5ms, filter off
    BME280_WriteReg(BME280_REG_CONFIG, 0x00);
    
    return 1;  // Success
}

// ========== BME280 Compensation Functions ==========
int32_t BME280_CompensateTemp(int32_t adc_T) {
    int32_t var1, var2, T;
    
    var1 = ((((adc_T >> 3) - ((int32_t)calib.dig_T1 << 1))) * 
            ((int32_t)calib.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)calib.dig_T1)) * 
            ((adc_T >> 4) - ((int32_t)calib.dig_T1))) >> 12) * 
            ((int32_t)calib.dig_T3)) >> 14;
    
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    
    return T;  // Temperature in 0.01°C
}


uint32_t BME280_CompensatePressure(int32_t adc_P) {
    int64_t var1, var2, p;
    
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)calib.dig_P3) >> 8) + 
           ((var1 * (int64_t)calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calib.dig_P1) >> 33;
    
    if (var1 == 0) {
        return 0;  // Avoid division by zero
    }
    
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)calib.dig_P7) << 4);
    
    return (uint32_t)p;  // Pressure in Pa/256
}

uint32_t BME280_CompensateHumidity(int32_t adc_H) {
    int32_t v_x1_u32r;
    
    v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)calib.dig_H4) << 20) - 
                (((int32_t)calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) * 
                (((((((v_x1_u32r * ((int32_t)calib.dig_H6)) >> 10) * 
                (((v_x1_u32r * ((int32_t)calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + 
                ((int32_t)2097152)) * ((int32_t)calib.dig_H2) + 8192) >> 14));
    
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * 
                ((int32_t)calib.dig_H1)) >> 4));
    
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    
    return (uint32_t)(v_x1_u32r >> 12);  // Humidity in %/1024
}

// ========== BME280 Read All Measurements ==========
void BME280_ReadAll(BME280_Data *data) {
    uint8_t raw_data[8];
    int32_t adc_T, adc_P, adc_H;
    
    // Read all sensor data (0xF7 to 0xFE)
    BME280_ReadRegs(BME280_REG_PRESS_MSB, raw_data, 8);
    
    // Parse raw data
    adc_P = ((uint32_t)raw_data[0] << 12) | ((uint32_t)raw_data[1] << 4) | 
            ((uint32_t)raw_data[2] >> 4);
    adc_T = ((uint32_t)raw_data[3] << 12) | ((uint32_t)raw_data[4] << 4) | 
            ((uint32_t)raw_data[5] >> 4);
    adc_H = ((uint32_t)raw_data[6] << 8) | (uint32_t)raw_data[7];
    
    // Compensate and convert to float
    data->temperature = BME280_CompensateTemp(adc_T) / 100.0f;  // °C
    data->pressure = BME280_CompensatePressure(adc_P) / 25600.0f;  // hPa
    data->humidity = BME280_CompensateHumidity(adc_H) / 1024.0f;  // %
}


