#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "main_dbc.h"

#define VEHICLE_STATE_COUNT 7
#define VC_FAULT_COUNT      23
#define PDU_FAULT_COUNT     32
#define HVBMS_FAULT_COUNT   13
#define LVBMS_FAULT_COUNT   13

typedef struct
{
    struct main_dbc_vc_status_t vc_status;

    uint64_t vc_fault_vector;
    uint64_t pdu_fault_vector;
    uint64_t hvbms_fault_vector;
    uint64_t lvbms_fault_vector;

    uint16_t hvbms_min_cell;
    uint16_t hvbms_max_temp;
    uint16_t hvbms_pack_v;
    uint8_t hvbms_soc;

    uint16_t lvbms_min_cell;
    uint16_t lvbms_max_temp;
    uint16_t lvbms_pack_v;

    uint16_t fr_motor_temp;
    uint16_t fl_motor_temp;
    uint16_t rr_motor_temp;
    uint16_t rl_motor_temp;

    uint16_t avg_inv_temp;
    uint16_t max_inv_temp;

    bool vc_status_flag;
    bool fault_flag;
    bool soc_flag;


    bool lvbms_flag;
    bool hvbms_flag;
    bool motor_flag;
    bool inverter_flag;

} dashboard_data_t;


extern dashboard_data_t dash_data;

extern const char* VC_FAULT_MESSAGES[];
extern const char* VC_STATUS_MESSAGES[];
extern const char* PDU_FAULT_MESSAGES[];
extern const char* HVBMS_FAULT_MESSAGES[];
extern const char* LVBMS_FAULT_MESSAGES[];

void DataManager_update_data();
void DataManager_update_lvbms();
void DataManager_update_hvbms();
void DataManager_update_motors();
void DataManager_update_inverters();
void DataManager_update_faults();
void DataManager_update_vc_status();

