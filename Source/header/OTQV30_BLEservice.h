#ifndef _OTQV2_SERVICE_H_
#define _OTQV2_SERVICE_H_


#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "struct.h"

#ifdef DEBUG
#define BLE_Interval_Slot 30
#define BLE_Interval_PerSlot 10
#define BLE_Interval_TimeSec 300
#else
#define BLE_Interval_Slot 60
#define BLE_Interval_PerSlot 10
#define BLE_Interval_TimeSec 600
#endif

#define BLE_OTQV2_BLE_OBSERVER_PRIO       2

#define BLE_OTQV2_DEF(_name)                      \
static ble_OTQV2_t _name;                         \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                   \
                     BLE_OTQV2_BLE_OBSERVER_PRIO, \
                     ble_OTQV2_on_ble_evt, &_name)




// OTQV2_SERVICE_UUID  "02001200-4202-eab5-ed11-8ae162414ee9"
//#define OTQV2_SERVICE_UUID_BASE {0xE9, 0x4E, 0x41, 0x62, 0xE1, 0x8A, 0x11, 0xED, 0xB5, 0xEA, 0x02, 0x42, 0x00, 0x12, 0x00, 0x02}
extern uint8 OTQV2_SERVICE_UUID_BASE[];
#define OTQV2_SERVICE_UUID  0x1200
#define OTQV2_SERVICE_CHAR_UUID 0x1201
#define OTQV2_SERVICE_CHAR_READ_UUID 0x1202
#define OTQV2_SERVICE_CHAR_WRITE_UUID 0x1203

/**@brief Connector Service event type. */
typedef enum
{
    BLE_OTQV2_EVT_NOTIFICATION_ENABLED,                              /**< OTQV2 value notification enabled event. */
    BLE_OTQV2_EVT_NOTIFICATION_DISABLED,                             /**< OTQV2 value notification disabled event. */
    BLE_OTQV2_EVT_DISCONNECTED,
    BLE_OTQV2_EVT_CONNECTED
} ble_OTQV2_evt_type_t;

/**@brief OTQV2 Service event. */
typedef struct
{
    ble_OTQV2_evt_type_t evt_type;                                  /**< Type of event. */
} ble_OTQV2_evt_t;

// Forward declaration of the ble_connector_t type.
typedef struct ble_OTQV2_s ble_OTQV2_t;


typedef void (*ble_OTQV2_evt_handler_t) (ble_OTQV2_t * p_OTQV2, ble_OTQV2_evt_t * p_evt);


typedef struct
{
    ble_OTQV2_evt_handler_t              evt_handler;                     /**< Event handler to be called for handling events in the Connector Service. */
    ble_srv_cccd_security_mode_t         custom_value_char_attr_md;       /**< Initial security level for Custom characteristics attribute */
} ble_OTQV2_init_t;

/**@brief Connector Service structure. This contains various status information for the service. */
struct ble_OTQV2_s
{
    ble_OTQV2_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the Connector Service. */
    uint16_t                        service_handle;                 /**< Handle of Connector Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t        OTQV2_value_handles;           /**< Handles related to the Connector Value characteristic. */
    ble_gatts_char_handles_t        OTQV2_read_handles;           /**< Handles related to the Connector Value characteristic. */
    ble_gatts_char_handles_t        OTQV2_write_handles;           /**< Handles related to the Connector Value characteristic. */
    uint16_t                        report_info_handle;
    uint16_t                        conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t                         uuid_type; 
};

void ble_OTQV2_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context);
stBleSettingReadData OTQV2_BLE_BuildSettingResponse(void);
uint32_t OTQV2_service_init(ble_OTQV2_t * p_OTQV2, const ble_OTQV2_init_t * p_OTQV2_init);
uint32_t ble_OTQV2_value_update(ble_OTQV2_t * p_OTQV2, uint8_t * value, uint16_t size);
void OTQV2_BLE_SetMainStructure(stMain *p);
void OTQV2_BLE_ApplySettingData(stMain *pMain);
uint8_t random_vector_generate(uint8_t * p_buff, uint8_t size);

#endif /* _OTQV2_SERVICE_H_ */

