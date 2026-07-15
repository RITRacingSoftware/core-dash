#include "appCAN.h"

#include <stdbool.h>
#include <string.h>

#include "core.h"

#include "config.h"
#include "appGPIO.h"

#define PDU_FAULT_VECTOR_FRAME_ID 55

SEC_BUS sec_bus;

void CAN_error_handler() {}

bool CAN_init()
{
    core_timestamp_init();
    if (!core_CAN_init(CAN_SEC, 1000000)) return false;
    if (!core_CAN_add_filter(CAN_SEC, false, 0x00, 0x7FF)) return false;
    
    return true;
}

bool CAN_tx()
{
    if (!core_CAN_send_from_tx_queue_task(CAN_SEC)) return false;
    return true;
}

void CAN_send_input_data()
{
    uint64_t msg = 0;

    sec_bus.inputs.dash_button1 = GPIO_get_right_paddle_state();
    sec_bus.inputs.dash_button2 = GPIO_get_enc1_pb_state();
    sensor_dbc_dash_inputs_full_encode((uint8_t *)&msg, &sec_bus.inputs, 8);
    core_CAN_add_message_to_tx_queue(CAN_SEC, SENSOR_DBC_DASH_INPUTS_FRAME_ID, 8, msg);
}

void CAN_rx() 
{
    struct {core_CAN_head_t head; uint8_t data[64];} can_msg;
    if (core_CAN_receive_from_msgbuf(CAN_SEC, (uint8_t*)&can_msg, 100)) {
        // rprintf("MESSAGE ID: %u \n", can_msg.head.id);
        switch (can_msg.head.id) {
            case MAIN_DBC_VC_STATUS_FRAME_ID :
                    main_dbc_vc_status_full_decode(&sec_bus.vc_status, can_msg.data, can_msg.head.length);
                    break;

            case MAIN_DBC_VC_FAULT_VECTOR_FRAME_ID :
                    memcpy(&(sec_bus.vc_fault_vector), can_msg.data, 8);
                    break;

            case MAIN_DBC_BMS_STATUS_FRAME_ID :
                    main_dbc_bms_status_unpack(&sec_bus.hvbms_status, can_msg.data, can_msg.head.length);
                    break;

            case MAIN_DBC_BMS_CELL_OVERVIEW_FRAME_ID :
                    main_dbc_bms_cell_overview_unpack(&sec_bus.hvbms_cell_overview, can_msg.data, can_msg.head.length);
                    break;

            case MAIN_DBC_BMS_FAULT_VECTOR_FRAME_ID :
                    memcpy(&(sec_bus.hvbms_fault_vector), can_msg.data, 8);
                    break;

            case SENSOR_DBC_LVBMS_VOLTAGES_FRAME_ID :
                    sensor_dbc_lvbms_voltages_unpack(&sec_bus.lvbms_voltages, can_msg.data, can_msg.head.length);
                    break;

            case SENSOR_DBC_LVBMS_TEMPERATURES_FRAME_ID :
                    sensor_dbc_lvbms_temperatures_unpack(&sec_bus.lvbms_temperatures, can_msg.data, can_msg.head.length);
                    break;

            case SENSOR_DBC_LVBMS_PACK_DATA_FRAME_ID :
                    sensor_dbc_lvbms_pack_data_unpack(&sec_bus.lvbms_pack_data, can_msg.data, can_msg.head.length);
                    break;

            case SENSOR_DBC_LVBMS_FAULT_VECTOR_FRAME_ID :
                    memcpy(&(sec_bus.lvbms_fault_vector), can_msg.data, 8);
                    break;

            case SENSOR_DBC_VC_MOTOR_TEMPS_FRAME_ID :
                    sensor_dbc_vc_motor_temps_unpack(&sec_bus.motor_temps, can_msg.data, can_msg.head.length);
                    // rprintf("RR TEMP : %u\n", sec_bus.motor_temps.vc_rr_motor_temp);
                    // rprintf("RL TEMP : %u\n", sec_bus.motor_temps.vc_rl_motor_temp);
                    // rprintf("FR TEMP : %u\n", sec_bus.motor_temps.vc_fr_motor_temp);
                    // rprintf("FL TEMP : %u\n", sec_bus.motor_temps.vc_fl_motor_temp);
                    break;

            case SENSOR_DBC_VC_INVERTER_TEMPS_FRAME_ID :
                    sensor_dbc_vc_inverter_temps_unpack(&sec_bus.inverter_temps, can_msg.data, can_msg.head.length);
                    break;

            case PDU_FAULT_VECTOR_FRAME_ID :
                    memcpy(&(sec_bus.pdu_fault_vector), can_msg.data, 8);
                    break;

            case SENSOR_DBC_VC_ENDURANCE_INFO_FRAME_ID :
                    sensor_dbc_vc_endurance_info_unpack(&sec_bus.vc_endurance_info, can_msg.data, can_msg.head.length);
                    break;

            default: return;
        }
    }
}

