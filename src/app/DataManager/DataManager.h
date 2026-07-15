#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "main_dbc.h"

#define VC_FAULT_COUNT      23
#define PDU_FAULT_COUNT     32
#define HVBMS_FAULT_COUNT   13
#define LVBMS_FAULT_COUNT   13

#define VEHICLE_STATES          7

#define NOT_READY_STATE         0
#define INVERTERS_POWERED_STATE 1
#define PRECHARGING_STATE       2
#define WAIT_STATE              3
#define STANDBY_STATE           4
#define READY_TO_DRIVE_STATE    5
#define SHUTDOWN_STATE          6


typedef struct
{
    struct main_dbc_vc_status_t vc_status;

    uint64_t vc_fault_vector;
    uint64_t pdu_fault_vector;
    uint64_t hvbms_fault_vector;
    uint64_t lvbms_fault_vector;

    int32_t endurance_delta;

    uint16_t hvbms_min_cell;
    uint16_t hvbms_max_temp;
    uint16_t hvbms_pack_v;

    uint16_t lvbms_min_cell;
    uint16_t lvbms_max_temp;
    uint16_t lvbms_pack_v;

    uint16_t max_motor_temp;
    uint16_t fr_motor_temp;
    uint16_t fl_motor_temp;
    uint16_t rr_motor_temp;
    uint16_t rl_motor_temp;

    uint16_t avg_inv_temp;
    uint16_t max_inv_temp;

    bool vc_controls_level_flag;
    bool endurance_flag;
    bool vc_status_flag;
    bool fault_flag;
    
    bool min_cell_flag;
    bool max_temp_flag;
    bool pack_v_flag;
    bool motor_temp_flag;
    bool inv_temp_flag;

    bool lvbms_flag;
    bool hvbms_flag;
    bool motor_flag;
    bool inverter_flag;

    bool screen_flag;

} debug_dashboard_data_t;

extern debug_dashboard_data_t dash_data;


extern const char* VC_STATUS_MESSAGES[];
extern const char* VC_FAULT_MESSAGES[];
extern const char* VC_CONTROLS_LEVELS[];
extern const char* PDU_FAULT_MESSAGES[];
extern const char* HVBMS_FAULT_MESSAGES[];
extern const char* LVBMS_FAULT_MESSAGES[];

void DataManager_update_data(void);
void DataManager_update_lvbms(void);
void DataManager_update_hvbms(void);
void DataManager_update_motors(void);
void DataManager_update_inverters(void);
void DataManager_update_vc_status(void);
void DataManager_update_faults(void);
void DataManager_update_controls_level(void);
void DataManager_update_endurance(void);
