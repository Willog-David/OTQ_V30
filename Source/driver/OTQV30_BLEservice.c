#include "ble_srv_common.h"
#include "app_scheduler.h"
#include "app_timer.h"
#include "OTQV30_Timer.h"
#include "nrf_calendar.h"
#include "nrf_drv_rng.h"

#include "nrf_log.h"

#include "OTQV30_BLEservice.h"
#include "EEPROM.h"



stMain *pMain;
	ble_OTQV2_t *p_OTQV2Handler;

uint8 OTQV2_SERVICE_UUID_BASE[] =  {0xE9, 0x4E, 0x41, 0x62, 0xE1, 0x8A, 0x11, 0xED, 0xB5, 0xEA, 0x02, 0x42, 0x00, 0x12, 0x00, 0x02};
	
void OTQV2_BLE_SetMainStructure(stMain *p)
{
	pMain = p;
}

void ble_OTQV2_InitBleHandler(ble_OTQV2_t * p_OTQV2)
{
	p_OTQV2Handler = p_OTQV2;
}

/** @brief Function for getting vector of random numbers.
 *
 * @param[out] p_buff       Pointer to unit8_t buffer for storing the bytes.
 * @param[in]  length       Number of bytes to take from pool and place in p_buff.
 *
 * @retval     Number of bytes actually placed in p_buff.
 */

uint8_t random_vector_generate(uint8_t * p_buff, uint8_t size)
{
    uint32_t err_code;
    uint8_t  available;

    nrf_drv_rng_bytes_available(&available);
    uint8_t length = MIN(size, available);

    err_code = nrf_drv_rng_rand(p_buff, length);
    APP_ERROR_CHECK(err_code);

    return length;
}
	
uint8 OTQV2_BLE_GetTimeSlot(void)
{
	uint8 Length = 0;
	uint8 RNGBuf[64] = {0};
	Length = random_vector_generate(RNGBuf, 64);
	for(int i=0 ; i<Length ; i++)
		{
		if(RNGBuf[i] < BLE_Interval_Slot)
			{
			NRF_LOG_INFO("[%10d] RNG generation : %d, MAX : %d", getTickCount() , RNGBuf[i] , BLE_Interval_Slot);
			return RNGBuf[i];
			}
		}
return 0;
}

void OTQV2_BLE_ActiveModeApply(void)
{
	switch(pMain->DeviceConfig.BLEActive)
		{
			case DeviceConfig_BLEActive_Always:
			advertising_start(pMain);
			break;

			case DeviceConfig_BLEActive_NFC:
			break;

			case DeviceConfig_BLEActive_Interval:
			break;

			case DeviceConfig_BLEActive_Disabled:
			break;

			}
}

void OTQV2_BLE_TxPowerApply(void)
{
	switch(pMain->DeviceConfig.BLEPower)
		{
		case DeviceConfig_BLEPwr_0dBm:
			pMain->BLE.Power = BLE_TX_P00;
			break;
			
		case DeviceConfig_BLEPwr_1dBm:
			pMain->DeviceConfig.BLEPower = DeviceConfig_BLEPwr_0dBm;
			pMain->BLE.Power = BLE_TX_P00;
			break;
			
		case DeviceConfig_BLEPwr_2dBm:
			pMain->BLE.Power = BLE_TX_P20;
			break;
			
		case DeviceConfig_BLEPwr_3dBm:
			pMain->BLE.Power = BLE_TX_P30;
			break;
			
		case DeviceConfig_BLEPwr_4dBm:
			pMain->BLE.Power = BLE_TX_P40;
			break;
			
		case DeviceConfig_BLEPwr_5dBm:
			pMain->BLE.Power = BLE_TX_P50;
			break;
			
		case DeviceConfig_BLEPwr_6dBm:
			pMain->BLE.Power = BLE_TX_P60;
			break;
			
		case DeviceConfig_BLEPwr_7dBm:
			pMain->BLE.Power = BLE_TX_P70;
			break;
			
		case DeviceConfig_BLEPwr_8dBm:
			pMain->BLE.Power = BLE_TX_P80;
			break;

			}
		
if(pMain->DeviceConfig.BLEActive == DeviceConfig_BLEActive_Always)
	{
	advertising_stop();
	advertising_start(pMain);
	}
}


stBleSettingReadData OTQV2_BLE_BuildSettingResponse(void)
{
stBleSettingReadData ret = {0};
Key_GetIntervalCursor(pMain);
Key_GetDisplayIntervalCursor(pMain);

ret.SyntaxVer = BLEDataSyntaxVersion;
ret.Interval = pMain->DeviceConfig.Interval;
ret.DisplayInterval = pMain->DeviceConfig.DisplayInterval;
ret.DeviceType = 0xAF;
ret.FWName[0] = pMain->DevInfo.FWVer[0];
ret.FWName[1] = pMain->DevInfo.FWVer[1];
ret.FWName[2] = pMain->DevInfo.FWVer[2];
ret.FWName[3] = pMain->DevInfo.FWVer[3];
#ifdef DEBUG
ret.FWVer = //(pMain->DevInfo.FWVer[4] - 0x30) * 10000 +
						(pMain->DevInfo.FWVer[5] - 0x30) * 1000 +
						(pMain->DevInfo.FWVer[6] - 0x30) * 100 +
						(pMain->DevInfo.FWVer[7] - 0x30) * 10 +
						(pMain->DevInfo.FWVer[8] - 0x30) * 1;
#else
ret.FWVer = (pMain->DevInfo.FWVer[4] - 0x30) * 10000 +
						(pMain->DevInfo.FWVer[5] - 0x30) * 1000 +
						(pMain->DevInfo.FWVer[6] - 0x30) * 100 +
						(pMain->DevInfo.FWVer[7] - 0x30) * 10 +
						(pMain->DevInfo.FWVer[8] - 0x30) * 1;
#endif

#if (BLEDataSyntaxVersion >= 12)
ret.Serial = (pMain->DevInfo.Serial[0] - 0x30) * 10000000 +
					   (pMain->DevInfo.Serial[1] - 0x30) * 1000000 +
					   (pMain->DevInfo.Serial[2] - 0x30) * 100000 +
					   (pMain->DevInfo.Serial[3] - 0x30) * 10000 +
					   (pMain->DevInfo.Serial[4] - 0x30) * 1000 +
					   (pMain->DevInfo.Serial[5] - 0x30) * 100 +
					   (pMain->DevInfo.Serial[6] - 0x30) * 10 +
					   (pMain->DevInfo.Serial[7] - 0x30) * 1 ;
#else
ret.Serial_Major = pMain->DevInfo.RawSerial / 100000;
ret.Serial_Minor = pMain->DevInfo.RawSerial % 100000;
#endif
ret.BatteryRemain = Batt_GetVoltage_Percent(pMain);
ret.ProbeOffset = pMain->Calib.CalData.Offset.ProbeTemp;

	pMain->Device.Time.Calendar = *nrf_cal_get_time();    // get uptime
	pMain->Device.Time.UpTimeSec = mktime(&pMain->Device.Time.Calendar); // get uptime
ret.UpTimeSec = pMain->Device.Time.UpTimeSec;

ret.CalIndex = pMain->Calib.CalData.Index;
ret.LastIndex = pMain->Sensor.DataNumber-1;
if(ret.LastIndex > MaxEEPROMAddress)ret.FirstIndex = ret.LastIndex - MaxEEPROMAddress;
	else ret.FirstIndex = 0;

return ret;
}



void OTQV2_BLE_SetValue_ReadSequence()
{
	uint32 err_code = 0;
	stBleSettingReadData data = OTQV2_BLE_BuildSettingResponse();
	ble_gatts_value_t gatts_value;

	// Initialize value struct.
	memset(&gatts_value, 0, sizeof(gatts_value));

	gatts_value.len     = sizeof(stBleSettingReadData);
	gatts_value.offset  = 0;
	gatts_value.p_value = &data;
	
	err_code = sd_ble_gatts_value_set(p_OTQV2Handler->conn_handle,
																 p_OTQV2Handler->OTQV2_read_handles.value_handle,
																 &gatts_value);
	if (err_code != NRF_SUCCESS)
	{
			NRF_LOG_INFO("[%10d] response value update fail!!  error code : %x",getTickCount(),err_code);
	}
	else
		{
			NRF_LOG_INFO("[%10d] response value update success",getTickCount());
		}
}
	
void OTQV2_BLE_ApplySettingData(stMain *pMain)
{
uint32 err_code = 0;
stBleSettingWriteData *WriteData = (stBleSettingWriteData*)pMain->BLE.ReceiveData;
stBleSettingReadData data = OTQV2_BLE_BuildSettingResponse();

// Initialize value struct.
ble_gatts_value_t gatts_value;

memset(&gatts_value, 0, sizeof(gatts_value));

gatts_value.len     = sizeof(stBleSettingReadData);
gatts_value.offset  = 0;
gatts_value.p_value = &data;

err_code = sd_ble_gatts_value_set(p_OTQV2Handler->conn_handle,
															 p_OTQV2Handler->OTQV2_read_handles.value_handle,
															 &gatts_value);
if (err_code != NRF_SUCCESS)
{
		NRF_LOG_INFO("[%10d] response value update fail!!  error code : %x",getTickCount(),err_code);
}
else
	{
		NRF_LOG_INFO("[%10d] response value update success",getTickCount());
	}

ble_gatts_hvx_params_t hvx_params;

memset(&hvx_params, 0, sizeof(hvx_params));

//NRF_LOG_INFO("Length : %d", gatts_value.len);
//NRF_LOG_INFO("%02X %02X %02X %02X", gatts_value.p_value[0], gatts_value.p_value[1], gatts_value.p_value[2], gatts_value.p_value[3]);

hvx_params.handle = p_OTQV2Handler->OTQV2_write_handles.value_handle;
hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
hvx_params.offset = gatts_value.offset;
hvx_params.p_len  = &gatts_value.len;
hvx_params.p_data = gatts_value.p_value;


err_code = sd_ble_gatts_hvx(p_OTQV2Handler->conn_handle, &hvx_params);

if (err_code != NRF_SUCCESS)
	{
		NRF_LOG_INFO("[%10d] hvx fail!!  error code : %x",getTickCount(),err_code);
	}
else
	{
		NRF_LOG_DEBUG("sd_ble_gatts_hvx result: %x. len %d \r\n", err_code,*hvx_params.p_len); 
		NRF_LOG_INFO("[%10d] response value update success",getTickCount());
	}
OTQV2_Timer_AllStop(pMain);
OTQV2_Timer_Start(pMain);
OTQV2_BLE_SetValue_ReadSequence();
}


static void OTQV2_on_connect(ble_OTQV2_t * p_OTQV2, ble_evt_t const * p_ble_evt)
{
    ret_code_t err_code;
    ble_OTQV2_evt_t evt;
    p_OTQV2->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;   

}

static void OTQV2_on_disconnect(ble_OTQV2_t * p_OTQV2, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_OTQV2->conn_handle = BLE_CONN_HANDLE_INVALID;
    ble_OTQV2_evt_t evt;
//iron6_20230515      start_data_update(false);
    stop_update_timer();
}


static void OTQV2_on_write(ble_OTQV2_t * p_OTQV2, ble_evt_t const * p_ble_evt)				// 
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    ret_code_t err_code;
		NRF_LOG_INFO("[%10d] packet receive. CHAR UUID : %x" , getTickCount() , p_evt_write->uuid.uuid);
    if(p_evt_write->handle == p_OTQV2->OTQV2_value_handles.value_handle)
    {
			switch(p_evt_write->uuid.uuid)
				{
				case OTQV2_SERVICE_CHAR_UUID:					// data index query
					memcpy(pMain->BLE.ReceiveData,p_evt_write->data,p_evt_write->len);
					BLE_SetAddress(pMain);
					pMain->Flags.BLEGattHandler = OTQV2_SERVICE_CHAR_UUID;
					break;
				case OTQV2_SERVICE_CHAR_WRITE_UUID:		// setting command
				memcpy(pMain->BLE.ReceiveData,p_evt_write->data,p_evt_write->len);
					//OTQV2_BLE_ApplySettingData(pMain);
					break;	
				}
					NRF_LOG_INFO("recv : %s", p_evt_write->data);
    }
		else if(p_evt_write->handle == p_OTQV2->OTQV2_write_handles.value_handle)
					{
					memcpy(pMain->BLE.ReceiveData,p_evt_write->data,p_evt_write->len);
					//OTQV2_BLE_ApplySettingData(pMain);
					pMain->Flags.BLEGattHandler = OTQV2_SERVICE_CHAR_WRITE_UUID;
					pMain->Flags.BLEActiveFlag = 1;
					}
    else if (p_evt_write->handle == p_OTQV2->OTQV2_value_handles.cccd_handle)  // Notification Enable/Disable
    {
        NRF_LOG_INFO("Notification enable =  %d %d", p_evt_write->data[0], p_evt_write->data[1]);
        bool enabled = p_evt_write->data[0];
        if(enabled)
        {
            start_update_timer();
        }
        else
            stop_update_timer();
    }    
    
}

static void OTQV2_on_read_setting(ble_OTQV2_t * p_OTQV2, ble_evt_t const * p_ble_evt)				// 
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    ret_code_t err_code;

    if(p_evt_write->handle == p_OTQV2->OTQV2_value_handles.value_handle)
    {
			memcpy(pMain->BLE.ReceiveData,p_evt_write->data,p_evt_write->len);
			BLE_SetAddress(pMain);
			NRF_LOG_INFO("recv : %s", p_evt_write->data);
    }
    else if (p_evt_write->handle == p_OTQV2->OTQV2_value_handles.cccd_handle)  // Notification Enable/Disable
    {
        NRF_LOG_INFO("Notification enable =  %d %d", p_evt_write->data[0], p_evt_write->data[1]);
        bool enabled = p_evt_write->data[0];
        if(enabled)
        {
            start_update_timer();
        }
        else
            stop_update_timer();
    }    
    
}

void ble_OTQV2_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_OTQV2_t * p_OTQV2 = (ble_OTQV2_t *) p_context;
    
    //NRF_LOG_INFO("BLE event received. Event type = %d\r\n", p_ble_evt->header.evt_id); 
    if (p_OTQV2 == NULL || p_ble_evt == NULL)
    {
        return;
    }
    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            OTQV2_on_connect(p_OTQV2, p_ble_evt);
						OTQV2_BLE_SetValue_ReadSequence();
						pMain->Flags.BLEGetDataStatusFlag = 1;
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            OTQV2_on_disconnect(p_OTQV2, p_ble_evt);
            break;

        case BLE_GAP_EVT_SEC_REQUEST:
            NRF_LOG_INFO("[%10d] Sec req event detected. event id : 0x%x",getTickCount() , p_ble_evt->header.evt_id);
            break;
						
        case BLE_GAP_EVT_DATA_LENGTH_UPDATE:
            NRF_LOG_INFO("[%10d] Len update event detected. event id : 0x%x",getTickCount() , p_ble_evt->header.evt_id);
            break;

				case BLE_GAP_EVT_CONN_PARAM_UPDATE:
            NRF_LOG_INFO("[%10d] Con param update event detected. event id : 0x%x",getTickCount() , p_ble_evt->header.evt_id);
            break;
						
        case BLE_GATTS_EVT_WRITE:            
            OTQV2_on_write(p_OTQV2, p_ble_evt);
            break;
						
				case BLE_GATTC_EVT_HVX:
						OTQV2_on_read_setting(p_OTQV2, p_ble_evt);	
						break;	

				case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:
						NRF_LOG_INFO("[%10d] Exchange MTU event detected. event id : 0x%x",getTickCount() , p_ble_evt->header.evt_id);
						break;	
						
        default:
						NRF_LOG_INFO("[%10d] Unknown BLE event detected. event id : 0x%x",getTickCount() , p_ble_evt->header.evt_id);
            break;
    }
}

static uint32_t OTQV2_char_add(ble_OTQV2_t * p_OTQV2, const ble_OTQV2_init_t * p_OTQV2_init)
{
    ret_code_t err_code;

    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_gatts_attr_t    descr_params;    
    ble_uuid_t          ble_uuid;
    ble_uuid_t          ble_descp_uuid;
    
    ble_gatts_attr_md_t attr_md;

    // Add Custom Value characteristic
    memset(&cccd_md, 0, sizeof(cccd_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.write_perm = p_OTQV2_init->custom_value_char_attr_md.cccd_write_perm;
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 1; 
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md; 
    char_md.p_sccd_md         = NULL;
		
    ble_uuid.type = p_OTQV2->uuid_type;
    ble_uuid.uuid = OTQV2_SERVICE_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_OTQV2_init->custom_value_char_attr_md.read_perm;
    attr_md.write_perm = p_OTQV2_init->custom_value_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = 90;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = 90;
    attr_char_value.p_value = NULL;

    err_code = sd_ble_gatts_characteristic_add(p_OTQV2->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_OTQV2->OTQV2_value_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }


    return NRF_SUCCESS;    
    
}

static uint32_t OTQV2_char_add_2(ble_OTQV2_t * p_OTQV2, const ble_OTQV2_init_t * p_OTQV2_init)
{
    ret_code_t err_code;

    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_gatts_attr_t    descr_params;    
    ble_uuid_t          ble_uuid;
    ble_uuid_t          ble_descp_uuid;
    stBleSettingReadData ret = OTQV2_BLE_BuildSettingResponse();
		
    ble_gatts_attr_md_t attr_md;

    // Add Custom Value characteristic
    memset(&cccd_md, 0, sizeof(cccd_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.write_perm = p_OTQV2_init->custom_value_char_attr_md.cccd_write_perm;
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 1; 
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md; 
    char_md.p_sccd_md         = NULL;
		
    ble_uuid.type = p_OTQV2->uuid_type;
    ble_uuid.uuid = OTQV2_SERVICE_CHAR_READ_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_OTQV2_init->custom_value_char_attr_md.read_perm;
    attr_md.write_perm = p_OTQV2_init->custom_value_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(stBleSettingReadData);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(stBleSettingReadData);
    attr_char_value.p_value = &ret;

    err_code = sd_ble_gatts_characteristic_add(p_OTQV2->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_OTQV2->OTQV2_read_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;    
    
}


static uint32_t OTQV2_char_add_3(ble_OTQV2_t * p_OTQV2, const ble_OTQV2_init_t * p_OTQV2_init)
{
    ret_code_t err_code;

    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_gatts_attr_t    descr_params;    
    ble_uuid_t          ble_uuid;
    ble_uuid_t          ble_descp_uuid;
    
    ble_gatts_attr_md_t attr_md;

    // Add Custom Value characteristic
    memset(&cccd_md, 0, sizeof(cccd_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.write_perm = p_OTQV2_init->custom_value_char_attr_md.cccd_write_perm;
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 1; 
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md; 
    char_md.p_sccd_md         = NULL;
		
    ble_uuid.type = p_OTQV2->uuid_type;
    ble_uuid.uuid = OTQV2_SERVICE_CHAR_WRITE_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_OTQV2_init->custom_value_char_attr_md.read_perm;
    attr_md.write_perm = p_OTQV2_init->custom_value_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(stBleSettingWriteData);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(stBleSettingReadData);
    attr_char_value.p_value = NULL;

    err_code = sd_ble_gatts_characteristic_add(p_OTQV2->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_OTQV2->OTQV2_write_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;    
    
}



uint32_t ble_OTQV2_service_init(ble_OTQV2_t * p_OTQV2, const ble_OTQV2_init_t * p_OTQV2_init)
{
    if (p_OTQV2 == NULL || p_OTQV2_init == NULL)
    {
        return NRF_ERROR_NULL;
    }

		ble_OTQV2_init_t p_OTQV2_init_2 = {0};
		ble_OTQV2_init_t p_OTQV2_init_3 = {0};

		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&p_OTQV2_init_2.custom_value_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&p_OTQV2_init_2.custom_value_char_attr_md.read_perm);
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&p_OTQV2_init_2.custom_value_char_attr_md.write_perm);

		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&p_OTQV2_init_3.custom_value_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&p_OTQV2_init_3.custom_value_char_attr_md.read_perm);
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&p_OTQV2_init_3.custom_value_char_attr_md.write_perm);
		
    ret_code_t   err_code;
    ble_uuid_t   ble_uuid;

		// mac address write to UUID
		OTQV2_SERVICE_UUID_BASE[0] = pMain->BLE.MAC[0];
		OTQV2_SERVICE_UUID_BASE[1] = pMain->BLE.MAC[1];
		OTQV2_SERVICE_UUID_BASE[2] = pMain->BLE.MAC[2];
		OTQV2_SERVICE_UUID_BASE[3] = pMain->BLE.MAC[3];
		OTQV2_SERVICE_UUID_BASE[4] = pMain->BLE.MAC[4];
		OTQV2_SERVICE_UUID_BASE[5] = pMain->BLE.MAC[5];
		
    // Initialize service structure
    p_OTQV2->evt_handler               = p_OTQV2_init->evt_handler;
    p_OTQV2->conn_handle               = BLE_CONN_HANDLE_INVALID;

    // Add Custom Service UUID
    ble_uuid128_t base_uuid;
		memcpy(base_uuid.uuid128 , OTQV2_SERVICE_UUID_BASE , 16);
    err_code =  sd_ble_uuid_vs_add(&base_uuid, &p_OTQV2->uuid_type);


    ble_uuid.type = p_OTQV2->uuid_type;
    ble_uuid.uuid = OTQV2_SERVICE_UUID;

    // Add the Custom Service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_OTQV2->service_handle);
    APP_ERROR_CHECK(err_code);

    // Add OTQV2 info characteristic 0x1201
    err_code = OTQV2_char_add(p_OTQV2, p_OTQV2_init);
    APP_ERROR_CHECK(err_code);    

    // Add OTQV2 info characteristic 0x1202
    err_code = OTQV2_char_add_2(p_OTQV2, &p_OTQV2_init_2);
    APP_ERROR_CHECK(err_code);    
		
    // Add OTQV2 info characteristic 0x1203
    err_code = OTQV2_char_add_3(p_OTQV2, &p_OTQV2_init_3);
    APP_ERROR_CHECK(err_code);    		
    

    return NRF_SUCCESS;   
}

uint32_t ble_OTQV2_value_update(ble_OTQV2_t * p_OTQV2, uint8_t * value, uint16_t size)
{
    if (p_OTQV2 == NULL)
    {
        return NRF_ERROR_NULL;
    }

    if(p_OTQV2->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        NRF_LOG_DEBUG("Not connected state..");
        return NRF_ERROR_SOFTDEVICE_NOT_ENABLED;
    }

    uint32_t err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = size;
    gatts_value.offset  = 0;
    gatts_value.p_value = value;

    // Update database.
    err_code = sd_ble_gatts_value_set(p_OTQV2->conn_handle,
                                      p_OTQV2->OTQV2_value_handles.value_handle,
                                      &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
				NRF_LOG_DEBUG("value set error : %X",err_code);
        return err_code;
    }
    // Send value if connected and notifying.
    if ((p_OTQV2->conn_handle != BLE_CONN_HANDLE_INVALID)) 
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        //NRF_LOG_INFO("Length : %d", gatts_value.len);
        //NRF_LOG_INFO("%02X %02X %02X %02X", gatts_value.p_value[0], gatts_value.p_value[1], gatts_value.p_value[2], gatts_value.p_value[3]);

        hvx_params.handle = p_OTQV2->OTQV2_value_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = gatts_value.offset;
        hvx_params.p_len  = &gatts_value.len;
        hvx_params.p_data = gatts_value.p_value;


        err_code = sd_ble_gatts_hvx(p_OTQV2->conn_handle, &hvx_params);
        NRF_LOG_DEBUG("sd_ble_gatts_hvx result: %x. len %d \r\n", err_code,*hvx_params.p_len); 
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
        NRF_LOG_ERROR("sd_ble_gatts_hvx result: NRF_ERROR_INVALID_STATE. \r\n"); 
    }


    return err_code;

}







