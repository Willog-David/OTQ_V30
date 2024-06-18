
#include "OTQV2_NFC.h"
#include "nfc_t2t_lib.h"
#include "nfc_launchapp_msg.h"
#include "nfc_launchapp_rec.h"
#include "nfc_text_rec.h"
#include "nfc_ndef_msg.h"
#include "ble_gap.h"


/** @snippet [NFC Launch App usage_0] */
/* nRF Toolbox Android application package name */
//static const uint8_t m_android_package_name[] = {'n', 'o', '.', 'n', 'o', 'r', 'd', 'i', 'c', 's',
                                                 //'e', 'm', 'i', '.', 'a', 'n', 'd', 'r', 'o', 'i',
                                                 //'d', '.', 'n', 'r', 'f', 't', 'o', 'o', 'l', 'b',
                                                 //'o', 'x'};

static const uint8_t m_android_package_name[] = {"io.willog.rms"};
																							 

/* nRF Toolbox application ID for Windows phone */
static const uint8_t m_windows_application_id[] = {'{', 'e', '1', '2', 'd', '2', 'd', 'a', '7', '-',
                                                   '4', '8', '8', '5', '-', '4', '0', '0', 'f', '-',
                                                   'b', 'c', 'd', '4', '-', '6', 'c', 'b', 'd', '5',
                                                   'b', '8', 'c', 'f', '6', '2', 'c', '}'};
static const uint8_t en_code[] = {'e', 'n'};
uint8_t m_ndef_msg_buf[290];
uint8_t NFC_EN_Payload[260];//[960];

#define MAX_REC_COUNT      3     /**< Maximum records count. */

/** @snippet [NFC Launch App usage_0] */

/**
 * @brief Callback function for handling NFC events.
 */
static void nfc_callback(void * p_context, nfc_t2t_event_t event, const uint8_t * p_data, size_t data_length)
{
    (void)p_context;
		stMain *pMain = (stMain*)p_context;

    switch (event)
    {
        case NFC_T2T_EVENT_FIELD_ON:
						pMain->Flags.NFCFieldOnFlag = 1;
            break;

        case NFC_T2T_EVENT_FIELD_OFF:
						pMain->Flags.NFCFieldOnFlag = 0;
            break;

				case NFC_T2T_EVENT_DATA_READ:
						pMain->Flags.NFCFieldOnFlag = 1;
						break;
						
        default:
            break;
    }
}

ret_code_t TAG_MAC_encode(stMain *pMain,uint8 *pBuffer,uint32 *pLen)
{
uint32_t err_code;
uint8 NFCTag_MAC[260];

memset(NFCTag_MAC,0x00,260);

NFCTag_MAC[0] = pMain->BLE.MAC[5];
NFCTag_MAC[1] = pMain->BLE.MAC[4];
NFCTag_MAC[2] = pMain->BLE.MAC[3];
NFCTag_MAC[3] = pMain->BLE.MAC[2];
NFCTag_MAC[4] = pMain->BLE.MAC[1];
NFCTag_MAC[5] = pMain->BLE.MAC[0];

	NFCTag_MAC[255]=0x00;
	NFCTag_MAC[256]=pMain->Sensor.SensorIntervalSec / 60;//ADVER_Interval;
	NFCTag_MAC[257]=23;
	NFCTag_MAC[258]=19;
#if 0

    /* Create NFC NDEF message description, capacity - 2 records */
    NFC_NDEF_MSG_DEF(nfc_launchapp_msg, 1);

    /* Create NFC NDEF Android Application Record description */
    NFC_NDEF_ANDROID_LAUNCHAPP_RECORD_DESC_DEF(nfc_and_launchapp_rec,
                                               m_android_package_name,
                                               sizeof(m_android_package_name));
        /* Add Android Application Record as second record to message */
        err_code = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_launchapp_msg),
                                           &NFC_NDEF_ANDROID_LAUNCHAPP_RECORD_DESC(nfc_and_launchapp_rec));
        VERIFY_SUCCESS(err_code);

    VERIFY_FALSE(NFC_NDEF_MSG(nfc_launchapp_msg).record_count == 0,
                              NRF_ERROR_INVALID_PARAM);

    err_code = nfc_ndef_msg_encode(&NFC_NDEF_MSG(nfc_launchapp_msg),
                                   pBuffer,
                                   pLen);
#endif																	 
    /** @snippet [NFC text usage_3] */
    NFC_NDEF_MSG_DEF(nfc_text_msg, MAX_REC_COUNT);
    NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_en_text_rec,
                                  UTF_8,
                                  en_code,
                                  sizeof(en_code),
                                  NFCTag_MAC,
                                  260
																	);
																	/* Add text records to NDEF text message */
																	
																	/* Create NFC NDEF message description, capacity - MAX_REC_COUNT records */
																	
    /** @snippet [NFC text usage_4] */
    err_code = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
                                       &NFC_NDEF_TEXT_RECORD_DESC(nfc_en_text_rec));
    VERIFY_SUCCESS(err_code);
																	
    /** @snippet [NFC text usage_5] */
    err_code = nfc_ndef_msg_encode(&NFC_NDEF_MSG(nfc_text_msg),
                                   pBuffer,
                                   pLen);
    return err_code;
    /** @snippet [NFC text usage_5] */
}
uint32_t len;


static ret_code_t nfc_address_encode(stMain *pMain,uint8_t * p_buffer, uint32_t * p_len)
{
    ret_code_t err_code;
    ble_gap_addr_t device_addr;
    uint8_t mac_address[23];
		uint8_t DataIndex[6];

    // Get BLE address.
    err_code = sd_ble_gap_addr_get(&device_addr);
    VERIFY_SUCCESS(err_code);

    sprintf(mac_address, "%02X:%02X:%02X:%02X:%02X:%02X,%05d", 
			device_addr.addr[5], device_addr.addr[4], device_addr.addr[3], device_addr.addr[2], device_addr.addr[1], device_addr.addr[0],pMain->Sensor.DataNumber - 1);
			NRF_LOG_INFO("MAC : %s", mac_address);

    sprintf(DataIndex, "%05d", 
			pMain->Sensor.DataNumber - 1);			// 실제 기록된 데이터 인덱스 전달
			NRF_LOG_INFO("MAC : %s", DataIndex);

    /* Create NFC NDEF message description, capacity - MAX_REC_COUNT records */
    /** @snippet [NFC text usage_3] */
    NFC_NDEF_MSG_DEF(nfc_text_msg, 1);

    NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_addr_text_rec,
                                  UTF_8,
                                  en_code,
                                  sizeof(en_code),
                                  mac_address,
                                  sizeof(mac_address));

    NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_index_text_rec,
                                  UTF_8,
                                  en_code,
                                  sizeof(en_code),
                                  DataIndex,
                                  sizeof(DataIndex));
																	
    err_code = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
                                       &NFC_NDEF_TEXT_RECORD_DESC(nfc_addr_text_rec));
    //err_code = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
                                       //&NFC_NDEF_TEXT_RECORD_DESC(nfc_index_text_rec));

																			 
   err_code = nfc_ndef_msg_encode(&NFC_NDEF_MSG(nfc_text_msg),
                                  p_buffer,
                                  p_len);
   return err_code;
}

static ret_code_t nfc_index_encode(stMain *pMain,uint8_t * p_buffer, uint32_t * p_len)
{
    ret_code_t err_code;
    ble_gap_addr_t device_addr;
    uint8_t mac_address[23];
		uint8_t DataIndex[6];

    // Get BLE address.
    err_code = sd_ble_gap_addr_get(&device_addr);
    VERIFY_SUCCESS(err_code);

    sprintf(mac_address, "%02X:%02X:%02X:%02X:%02X:%02X,%5d", 
			device_addr.addr[5], device_addr.addr[4], device_addr.addr[3], device_addr.addr[2], device_addr.addr[1], device_addr.addr[0] , DataIndex);
			NRF_LOG_INFO("MAC : %s", mac_address);

    sprintf(DataIndex, ",%5d", 
			pMain->Sensor.DataNumber);
			NRF_LOG_INFO("index : %d", DataIndex);

    /* Create NFC NDEF message description, capacity - MAX_REC_COUNT records */
    /** @snippet [NFC text usage_3] */
    NFC_NDEF_MSG_DEF(nfc_text_msg, 1);

    NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_addr_text_rec,
                                  UTF_8,
                                  en_code,
                                  sizeof(en_code),
                                  mac_address,
                                  sizeof(mac_address));

    NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_index_text_rec,
                                  UTF_8,
                                  en_code,
                                  sizeof(en_code),
                                  DataIndex,
                                  sizeof(DataIndex));
																	
    err_code = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
                                       &NFC_NDEF_TEXT_RECORD_DESC(nfc_addr_text_rec));
    //err_code = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
                                       //&NFC_NDEF_TEXT_RECORD_DESC(nfc_index_text_rec));

																			 
   err_code = nfc_ndef_msg_encode(&NFC_NDEF_MSG(nfc_text_msg),
                                  p_buffer,
                                  p_len);
   return err_code;
}
 
void OTQV2_NFC_Init(stMain *pMain)
{
#if 0
uint32_t err_code;

    /* Set up NFC */
    err_code = nfc_t2t_setup(nfc_callback, NULL);
    APP_ERROR_CHECK(err_code);

    /** @snippet [NFC Launch App usage_2] */
    /*  Provide information about available buffer size to encoding function. */
    len = sizeof(m_ndef_msg_buf);

    /* Encode launchapp message into buffer */
/*    err_code = nfc_launchapp_msg_encode(m_android_package_name,
                                        sizeof(m_android_package_name),
                                        //m_windows_application_id,
                                        //sizeof(m_windows_application_id),
                                        NULL,
                                        NULL,
                                        m_ndef_msg_buf,
                                        &len);

    APP_ERROR_CHECK(err_code);
		*/
    /** @snippet [NFC Launch App usage_2] */
		
		TAG_MAC_encode(pMain,m_ndef_msg_buf,&len);

    /* Set created message as the NFC payload */
    err_code = nfc_t2t_payload_set(m_ndef_msg_buf, len);
    APP_ERROR_CHECK(err_code);

    /* Start sensing NFC field */
    err_code = nfc_t2t_emulation_start();
    APP_ERROR_CHECK(err_code);
		
		#else    
		len = sizeof(m_ndef_msg_buf);
    ret_code_t err_code;
    
    /* Set up NFC */
    err_code = nfc_t2t_setup(nfc_callback, pMain);
    APP_ERROR_CHECK(err_code);


//    /* Encode welcome message */
//    err_code = welcome_msg_encode(m_ndef_msg_buf, &len);
//    APP_ERROR_CHECK(err_code);

    err_code = nfc_address_encode(pMain,m_ndef_msg_buf, &len);
    APP_ERROR_CHECK(err_code);


    /* Set created message as the NFC payload */
    err_code = nfc_t2t_payload_set(m_ndef_msg_buf, len);
    APP_ERROR_CHECK(err_code);

    /* Start sensing NFC field */
    //err_code = nfc_t2t_emulation_start();
    //APP_ERROR_CHECK(err_code);
		#endif
}

void OTQV2_NFC_Update(stMain *pMain)
{
		len = sizeof(m_ndef_msg_buf);
    ret_code_t err_code;

//    /* Encode welcome message */
//    err_code = welcome_msg_encode(m_ndef_msg_buf, &len);
//    APP_ERROR_CHECK(err_code);

    err_code = nfc_address_encode(pMain,m_ndef_msg_buf, &len);
    APP_ERROR_CHECK(err_code);


    /* Set created message as the NFC payload */
    err_code = nfc_t2t_payload_set(m_ndef_msg_buf, len);
    APP_ERROR_CHECK(err_code);

    /* Start sensing NFC field */
    //err_code = nfc_t2t_emulation_start();
    //APP_ERROR_CHECK(err_code);

}
	
static uint8 NFCActivateFlag = 0;

void OTQV2_NFC_UpdatePayload(void)
{
uint32 err_code = 0;
m_ndef_msg_buf[strlen(m_ndef_msg_buf)] = ',';
err_code = nfc_t2t_payload_set(m_ndef_msg_buf, len);
APP_ERROR_CHECK(err_code);
}

void OTQV2_NFC_Start(stMain *pMain)
{
	ret_code_t err_code;

	if(!NFCActivateFlag)
		{
		err_code = nfc_t2t_emulation_start();
		APP_ERROR_CHECK(err_code);
		}
	NFCActivateFlag = 1;
}

void OTQV2_NFC_Stop(stMain *pMain)
{
	ret_code_t err_code;

	if(NFCActivateFlag)
		{
		err_code = nfc_t2t_emulation_stop();
		APP_ERROR_CHECK(err_code);
		}
	
	NFCActivateFlag = 0;
}