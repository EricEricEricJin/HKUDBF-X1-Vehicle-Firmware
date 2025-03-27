#include "sensor_task.h"
#include "board.h"

#include "cmsis_os.h"
#include "communicate.h"

#include "protocol.h"

#include "log.h"

#include "gps.h"
#include "board.h"

struct bmp280 bmp280 = {
    .i2c_dev = {
        .i2c = &hi2c2,
        .dev_addr = 0x77 << 1,
    }};

struct jy901 jy901 = {
    .i2c_dev = {
        .i2c = &hi2c1,
        .dev_addr = 0x50 << 1,
    }};

inline int16_t export_pressure(float pressure) { return (int16_t)((pressure - 1e5) * 10.0f); }
inline int16_t export_temperature(float temperature) { return (int16_t)(temperature * 100.0f); }

struct jy901_data_decoded jy901_data_decoded;
struct jy901_data_raw jy901_data_raw;
struct bmp280_data bmp280_data;

osMutexId_t sensor_mux_id;

const osMutexAttr_t sensor_mutex_attr = {
    .name = "SENSOR_MUTEX",
};

struct gps gps;
struct gps_pvt_data gps_pvt_data;


uint32_t usart2_callback(uint8_t *buf, uint16_t len)
{
    gps_rx_data_handle(&gps, buf, len);
    return 0;
}

__NO_RETURN void sensor_task(void *args)
{
    // sensor_mux_id = osMutexNew(&sensor_mutex_attr);

    // Initialize BMP280
    int ret = bmp280_init(&bmp280);
    log_i("bmp280 init = %d", ret);
    osDelay(100);

    // Initialize JY901
    ret = jy901_init(&jy901, 0, 0);
    log_i("jy901 init = %d", ret);
    osDelay(100);

    // Initialize GPS
    // gps_init(&gps);
    // usart2_rx_callback_register(usart2_callback);
    // osDelay(100);

    TickType_t waketime = osKernelSysTick();
    while (1)
    {
        jy901_update(&jy901);
        bmp280_update(&bmp280);

        // gps_unpack_fifo_data(&gps);


        osMutexAcquire(sensor_mux_id, osWaitForever);
        jy901_decode(&jy901.raw_data, &jy901_data_decoded);
        memcpy(&jy901_data_raw, &jy901.raw_data, sizeof(struct jy901_data_raw));
        memcpy(&bmp280_data, &bmp280.data, sizeof(struct bmp280_data));
        osMutexRelease(sensor_mux_id);
        
        adc_convert();
        // log_i("Lock Done!");
        
        // waketime += 20;
        // osDelayUntil(waketime);
        osDelay(20);
        // log_i("Wait Done!");
    }
}

void get_decoded_sensor_data(sensor_data_decoded_t data, uint32_t timeout)
{
    osMutexAcquire(sensor_mux_id, timeout);
    memcpy(&data->jy901_data, &jy901_data_decoded, sizeof(struct jy901_data_decoded));
    memcpy(&data->bmp280_data, &bmp280.data, sizeof(struct bmp280_data));
    osMutexRelease(sensor_mux_id);
}

#define BAT_CELL_NUM (2)
#define CELL_VOLT_MIN (3000)
#define CELL_VOLT_MAX (4200)

void get_export_sensor_data(sensor_data_export_t data, uint32_t timeout)
{
    osMutexAcquire(sensor_mux_id, timeout);
    data->a_x = jy901.raw_data.a_x;
    data->a_y = jy901.raw_data.a_y;
    data->a_z = jy901.raw_data.a_z;
    data->w_x = jy901.raw_data.w_x;
    data->w_y = jy901.raw_data.w_y;
    data->w_z = jy901.raw_data.w_z;
    data->b_x = jy901.raw_data.b_x;
    data->b_y = jy901.raw_data.b_y;
    data->b_z = jy901.raw_data.b_z;
    data->roll = jy901.raw_data.roll;
    data->pitch = jy901.raw_data.pitch;
    data->yaw = jy901.raw_data.yaw;
    data->pressure = export_pressure(bmp280_data.pressure);
    data->temperature = export_temperature(bmp280_data.temperature);
    // log_i("temp = %f", bmp280_data.temperature);
    
    if (GET_DET_SW1() == DET_SW_DETACHED)
        SET_BOARD_LED_ON();
    else
        SET_BOARD_LED_OFF();   
    
    osMutexRelease(sensor_mux_id);
    data->volt_bat = adc_get_mv();
    // gps_get_pvt_data(&gps, &gps_pvt_data);
    // log_i("lat=%d, lon=%d, alt=%d", gps_pvt_data.lat, gps_pvt_data.lon, gps_pvt_data.height);
    // log_i("v=%d", data->volt_bat);
}
