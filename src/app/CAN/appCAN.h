#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "sensor_dbc.h"
#include "main_dbc.h"


#define CAN_SEC FDCAN1

void CAN_error_handler();
bool CAN_init();
bool CAN_tx();
void CAN_rx();
void CAN_send_input_data();

typedef struct 
{
    struct sensor_dbc_dash_inputs_d inputs; 

    struct sensor_dbc_vc_endurance_info_t vc_endurance_info;
    struct main_dbc_vc_status_d vc_status;

    uint64_t vc_fault_vector;
    uint64_t pdu_fault_vector;
    uint64_t hvbms_fault_vector;
    uint64_t lvbms_fault_vector;

    struct main_dbc_bms_status_t hvbms_status;
    struct main_dbc_bms_cell_overview_t hvbms_cell_overview;

    struct sensor_dbc_lvbms_voltages_t lvbms_voltages;
    struct sensor_dbc_lvbms_temperatures_t lvbms_temperatures;
    struct sensor_dbc_lvbms_pack_data_t lvbms_pack_data;

    struct sensor_dbc_vc_motor_temps_t motor_temps;
    struct sensor_dbc_vc_inverter_temps_t inverter_temps;
} SEC_BUS;

extern SEC_BUS sec_bus;
