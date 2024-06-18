
#include "OTQV2_NFC.h"

//uint8_t m_ndef_msg_buf[290];//[990];
uint8_t en_payload[260];//[960];

#if 0
static ret_code_t TAG_msg_encode(uint8_t * p_buffer, uint32_t * p_len)
{
    /** @snippet [NFC text usage_2] */
    ret_code_t err_code;

    /* Create NFC NDEF text record description in English */
    NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_en_text_rec,
                                  UTF_8,
                                  en_code,
                                  sizeof(en_code),
                                  en_payload,
                                  sizeof(en_payload));
																	/* Add text records to NDEF text message */
																	
																	/* Create NFC NDEF message description, capacity - MAX_REC_COUNT records */
    /** @snippet [NFC text usage_3] */
    NFC_NDEF_MSG_DEF(nfc_text_msg, MAX_REC_COUNT);
																	
    /** @snippet [NFC text usage_4] */
    err_code = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
                                       &NFC_NDEF_TEXT_RECORD_DESC(nfc_en_text_rec));
    VERIFY_SUCCESS(err_code);
																	
    /** @snippet [NFC text usage_5] */
    err_code = nfc_ndef_msg_encode(&NFC_NDEF_MSG(nfc_text_msg),
                                   p_buffer,
                                   p_len);
    return err_code;
    /** @snippet [NFC text usage_5] */
}
#endif

static ret_code_t TAG_MACaddress_encode(stMain *pMain)
{
    /** @snippet [NFC text usage_2] */
    ret_code_t err_code;
		//uint8 Mac_Ascii[12];

    /* Create NFC NDEF text record description in English */
    NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_en_text_rec,
                                  UTF_8,
                                  en_code,
                                  sizeof(en_code),
                                  pMain->BLE.MAC,
                                  6);
																	/* Add text records to NDEF text message */
																	
																	/* Create NFC NDEF message description, capacity - MAX_REC_COUNT records */
    /** @snippet [NFC text usage_3] */
    NFC_NDEF_MSG_DEF(nfc_text_msg, MAX_REC_COUNT);
																	
    /** @snippet [NFC text usage_4] */
    err_code = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
                                       &NFC_NDEF_TEXT_RECORD_DESC(nfc_en_text_rec));
    VERIFY_SUCCESS(err_code);
																	
    /** @snippet [NFC text usage_5] */
    err_code = nfc_ndef_msg_encode(&NFC_NDEF_MSG(nfc_text_msg),
                                   p_buffer,
                                   p_len);
    return err_code;
    /** @snippet [NFC text usage_5] */
}


void OTQV2_NFC_Init(stMain *pMain)
{
    /* Set up NFC */
    err_code = nfc_t2t_setup(nfc_callback, NULL);
    APP_ERROR_CHECK(err_code);

    /** @snippet [NFC Launch App usage_2] */
    /*  Provide information about available buffer size to encoding function. */
    len = sizeof(m_ndef_msg_buf);

    /* Encode launchapp message into buffer */
    err_code = nfc_launchapp_msg_encode(m_android_package_name,
                                        sizeof(m_android_package_name),
                                        //m_windows_application_id,
                                        //sizeof(m_windows_application_id),
                                        NULL,
                                        NULL,
                                        m_ndef_msg_buf,
                                        &len);

    APP_ERROR_CHECK(err_code);
    /** @snippet [NFC Launch App usage_2] */

    /* Set created message as the NFC payload */
    err_code = nfc_t2t_payload_set(m_ndef_msg_buf, len);
    APP_ERROR_CHECK(err_code);

    /* Start sensing NFC field */
    err_code = nfc_t2t_emulation_start();
    APP_ERROR_CHECK(err_code);
}

