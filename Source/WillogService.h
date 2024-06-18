
#ifndef WILLOGSERVICE_H_
#define WILLOGSERVICE_H_

#include "struct.h"

#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"
#include "ble_dfu.h"
#include "ble_nus.h"
#include "nrf_ble_qwr.h"
#include "nrf_bootloader_info.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_pwr_mgmt.h"
void OTQ_EPDTestCode(stMain *pMain);
void OTQ_EnterBootLoader(void);
void OTQ_LED_TestLoop(stMain *pMain);
void OTQV2_LEDIndicatorHandler(stMain *pMain);


void OTQV2_Service_StartLogging(stMain *pMain);
void OTQV2_Service_FactoryReset(stMain *pMain);
void OTQV2_Service_CustomerReset(stMain *pMain);
void OTQV2_Service_RestoreParameter(stMain *pMain,stLogData *pLogData);

#endif