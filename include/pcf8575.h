#pragma once
#include <stdint.h>
#include "driver/i2c_master.h"
#define PCF8575_DEFAULT_ADDRESS 0x20

typedef enum {
    PCF8575_P10 = 1 << 0, 
    PCF8575_P11 = 1 << 1, 
    PCF8575_P12 = 1 << 2, 
    PCF8575_P13 = 1 << 3, 
    PCF8575_P14 = 1 << 4, 
    PCF8575_P15 = 1 << 5, 
    PCF8575_P16 = 1 << 6, 
    PCF8575_P17 = 1 << 7,  
    PCF8575_P0 = 1 << 8, 
    PCF8575_P1 = 1 << 9, 
    PCF8575_P2 = 1 << 10, 
    PCF8575_P3 = 1 << 11, 
    PCF8575_P4 = 1 << 12, 
    PCF8575_P5 = 1 << 13, 
    PCF8575_P6 = 1 << 14, 
    PCF8575_P7 = 1 << 15 
}pcf8575_pin_t; 

/**
* @brief struct for pcf8575
*/
    typedef struct { 
/**
* @param pins_selected_mode selected pin io function must be 1 for input
*/   
        uint16_t pins_selected_mode; 
/**
* @param pins_state logical value of each pin form register
*/ 
        uint16_t pins_state; 
/**
* @param pins_set value to be set or is set on pins
*/
        uint16_t pins_set; 
/**
* @param i2c_config pcf8575 config
*/
        i2c_device_config_t i2c_config;
/**
* @param i2c_handle handle of i2c for pcf8575 to be assigned to 
*/ 
        i2c_master_dev_handle_t i2c_handle; 
    } pcf8575_handle_t; 

/** 
* @brief Initialize the PCF8575 device handle 
* @param handle Pointer to pointer that will hold the allocated handle 
* @param i2c_bus Pointer to the initialized I2C bus handle 
* @param i2c_address I2C 7-bit address of the PCF8575
* @return esp_err_t ESP_OK on success
*/ 

esp_err_t pcf8575_init(pcf8575_handle_t **handle, i2c_master_bus_handle_t *i2c_bus, uint8_t i2c_address);

/** 
* @brief Read the current port state from the PCF8575
* @param handle Pointer to PCF8575 device handle
* @return esp_err_t ESP_OK on success
*/

esp_err_t pcf8575_read_port(pcf8575_handle_t *handle);
/**
* @brief Write set pins to the PCF8575 port form handle 
* @param handle Pointer to PCF8575 device handle 
* @return esp_err_t ESP_OK on success 
*/ 

esp_err_t pcf8575_write_port(pcf8575_handle_t *handle); 
/** 
* @brief Write set pins to the PCF8575 port form handle 
* @param handle Pointer to PCF8575 device handle 
* @param pcf8575_pin_t Pin selected 
* @param state state of pin 
* @return esp_err_t ESP_OK on success 
*/ 

esp_err_t pcf8575_set_pin(pcf8575_handle_t *handle, pcf8575_pin_t pin, bool state);
/** 
* @brief toggle pcf8575 pin state 
* @param handle Pointer to PCF8575 device handle 
* @param pcf8575_pin_t Pin selected 
* @return esp_err_t ESP_OK on success
*/ 
esp_err_t pcf8575_toggle_pin(pcf8575_handle_t *handle, pcf8575_pin_t pin);

/**
* @brief Deinitialize and free the PCF8575 handle 
* @param handle Pointer to PCF8575 device handle pointer 
*/ void pcf8575_deinit(pcf8575_handle_t **handle);
