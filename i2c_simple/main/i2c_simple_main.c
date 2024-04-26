#include <stdio.h>
#include <inttypes.h>
#include "esp_log.h"
#include "http_rest_json_client.h"
#include "http_rest_types.h"
#include <unistd.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "mpu6050.h"
#include <time.h>
#include "esp_wifi.h"
#include "esp_netif.h"
#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_http_client.h"


// static const char *TAG = "i2c-simple-example";
static mpu6050_handle_t mpu6050 = NULL;

#define I2C_MASTER_SCL_IO 1         /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO 0         /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM 0            /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 400000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

#define WIFI_SSID "OP512"
#define WIFI_PASSWORD "TextShow0030"
#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
#define CONFIG_EXAMPLE_HTTP_ENDPOINT "httpbin.org"
static const char *TAG = "HTTP_CLIENT";

static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO,
        .scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    // return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

static void i2c_sensor_mpu6050_init(void)
{
    i2c_master_init();
    mpu6050 = mpu6050_create(I2C_MASTER_NUM, MPU6050_I2C_ADDRESS);
    mpu6050_config(mpu6050, ACCE_FS_4G, GYRO_FS_500DPS);
    mpu6050_wake_up(mpu6050);
}

void app_main(void)
{
    uint8_t mpu6050_deviceid;

    i2c_sensor_mpu6050_init();
    ESP_LOGI(TAG, "I2C initialized successfully");
    mpu6050_get_deviceid(mpu6050, &mpu6050_deviceid);
    printf("Device id: %X\n", mpu6050_deviceid);

    

    for (int i = 0; i < 20; i++)
    {
        time_t start_time = time(NULL);
        while (difftime(time(NULL), start_time) < 4)
        {
            mpu6050_acce_value_t acce;
            mpu6050_gyro_value_t gyro;

            mpu6050_get_acce(mpu6050, &acce);
            mpu6050_get_gyro(mpu6050, &gyro);
            
            

            printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", acce.acce_x, acce.acce_y, acce.acce_z, gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);
            sleep(0.5);
        }
        printf("xxx\n");
        sleep(2);
    }
    // // Declare local_response_buffer with size (MAX_HTTP_OUTPUT_BUFFER + 1) to prevent out of bound access when
    // // it is used by functions like strlen(). The buffer should only be used upto size MAX_HTTP_OUTPUT_BUFFER
    // char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};   // Buffer to store response of http request
    // int content_length = 0;
    // esp_http_client_config_t config = {
    //     .url = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/get",
    // };
    // esp_http_client_handle_t client = esp_http_client_init(&config);

    // // GET Request
    // esp_http_client_set_method(client, HTTP_METHOD_GET);
    // esp_err_t err = esp_http_client_open(client, 0);
    // if (err != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    // } else {
    //     content_length = esp_http_client_fetch_headers(client);
    //     if (content_length < 0) {
    //         ESP_LOGE(TAG, "HTTP client fetch headers failed");
    //     } else {
    //         int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
    //         if (data_read >= 0) {
    //             ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
    //             esp_http_client_get_status_code(client),
    //             esp_http_client_get_content_length(client));
    //             ESP_LOG_BUFFER_HEX(TAG, output_buffer, data_read);
    //         } else {
    //             ESP_LOGE(TAG, "Failed to read response");
    //         }
    //     }
    // }
    // esp_http_client_close(client);


    mpu6050_delete(mpu6050);
    i2c_driver_delete(I2C_MASTER_NUM);
    ESP_LOGI(TAG, "I2C de-initialized successfully");
  }
