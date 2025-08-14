#include "pcf8575.h"
#include <string.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PCF8575_I2C_DEFAULT_FREQUENCY 400000
#define PCF8575_TIMEOUT_MS 100
#define TAG "PCF8575"

#define BITMASK_SET_PIN(mask, pin, state) \
    ((mask) = ((mask) & ~(pin)) | ((state) ? (pin) : 0))

esp_err_t pcf8575_init(pcf8575_handle_t **out_handle,
                       i2c_master_bus_handle_t *i2c_bus,
                       uint8_t address)
{
    if (out_handle == NULL || i2c_bus == NULL || address < PCF8575_DEFAULT_ADDRESS) {
        return ESP_ERR_INVALID_ARG;
    }

    pcf8575_handle_t *handle = calloc(1, sizeof(pcf8575_handle_t));
    if (handle == NULL) {
        return ESP_ERR_NO_MEM;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = address,
        .scl_speed_hz    = PCF8575_I2C_DEFAULT_FREQUENCY,
    };

    esp_err_t ret = i2c_master_bus_add_device(*i2c_bus, &dev_cfg, &handle->i2c_handle);
    if (ret != ESP_OK) {
        free(handle);
        return ret;
    }

    handle->pins_selected_mode = 0xFFFF; // Inputs by default
    handle->pins_set = 0xFFFF;           // All HIGH initially

    ret = pcf8575_write_port(handle);
    if (ret != ESP_OK) {
        i2c_master_bus_rm_device(handle->i2c_handle);
        free(handle);
        return ret;
    }

    *out_handle = handle;
    return ESP_OK;
}

esp_err_t pcf8575_read_port(pcf8575_handle_t *handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t data[2] = {0};
    esp_err_t ret = i2c_master_receive(handle->i2c_handle, data, sizeof(data),
                                       pdMS_TO_TICKS(PCF8575_TIMEOUT_MS));
    if (ret == ESP_OK) {
        handle->pins_state = data[0] | (data[1]<<8);
    } else {
        ESP_LOGE(TAG, "I2C read failed: %s", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t pcf8575_write_port(pcf8575_handle_t *handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    handle->pins_selected_mode = handle->pins_set;
    uint8_t data[2] = {
        (uint8_t)((handle->pins_set>>8) & 0x00FF),
        (uint8_t)((handle->pins_set) & 0x00FF)
    };

    esp_err_t ret = i2c_master_transmit(handle->i2c_handle, data, sizeof(data),
                                        pdMS_TO_TICKS(PCF8575_TIMEOUT_MS));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C write failed: %s", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t pcf8575_set_pin(pcf8575_handle_t *handle, pcf8575_pin_t pin, bool state)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    BITMASK_SET_PIN(handle->pins_selected_mode, pin, state);
    uint16_t old_mask = handle->pins_set;
    BITMASK_SET_PIN(handle->pins_set, pin, state);

    if (handle->pins_set == old_mask) {
        return ESP_OK; // No change
    }

    return pcf8575_write_port(handle);
}

esp_err_t pcf8575_toggle_pin(pcf8575_handle_t *handle, pcf8575_pin_t pin)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    handle->pins_set ^= pin;
    handle->pins_selected_mode ^= pin;
    return pcf8575_write_port(handle);
}

void pcf8575_deinit(pcf8575_handle_t **handle)
{
    if (handle == NULL || *handle == NULL) {
        return;
    }

    i2c_master_bus_rm_device((*handle)->i2c_handle);
    free(*handle);
    *handle = NULL;
}
