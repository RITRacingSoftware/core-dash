#include "DataManager.h"
#include "appCAN.h"
#include <stdbool.h>


dashboard_data_t dash_data = {0};

const char* VC_STATUS_MESSAGES[] = 
{
    "NOT READY",
    "INVERTERS POWER",
    "PRECHARGING",
    "WAIT",
    "STANDBY",
    "READY TO DRIVE",
    "SHUTDOWN"
};

const char* VC_FAULT_MESSAGES[] = 
{
    "PDU SHUTDOWN",
    "BMS FAULT",
    "ACCEL A IRR",
    "ACCEL B IRR",
    "APPS DISAGREE",
    "DOUBLE PEDAL",
    "FSSDB LOST",
    "FBP IRR",
    "RBP IRR",
    "STR ANG IRR",
    "RSSDB LOST",
    "RR ERROR",
    "RL ERROR",
    "FR ERROR",
    "FL ERROR",
    "PRECHARGE TIMEOUT",
    "SOFT DOUBLE PEDAL",
    "VN NO LOCK",
    "VN IRR",
    "RUNAWAY",
    "CURRENT IRR",
    "VN LOST",
    "DS TIMEOUT"
};

const char* PDU_FAULT_MESSAGES[] =
{
    "OC BMS",
    "OC INTERLOCK",
    "OC IMD",
    "OC INVERTERS",
    "OC SDC",
    "OC RED SSI",
    "OC GREEN SSI",
    "OC DRIVERLESS",
    "OC RTDS",
    "OC RSSDB",
    "OC PITOT TUBE",
    "OC LEFT RAD FAN",
    "OC EM",
    "OC FSSDB",
    "OC",
    "OC WATER PUMP 1",
    "OC TPMS",
    "OC WATER PUMP 2",
    "OC EHVD",
    "OC RML",
    "OC BRAKE LIGHT",
    "OC RIGHT RAD FAN",
    "OC DATALOGGER",
    "OC DSSI",
    "OC DRS",
    "RES OC",
    "OC START BUTTON",
    "OC ACC FAN",
    "OC DASH OC",
    "OC WATER PUMP 3",
    "OC VCU",
    "OC EKEY"
};


const char* HVBMS_FAULT_MESSAGES[] = 
{
    "PDU SHUTDOWN",
    "M17 ERROR",
    "ADES ERROR",
    "CELL IRR",
    "CHIP IRR",
    "TEMP IRR",
    "OUT OF JUICE",
    "CELL DIFF",
    "CHARGER ERROR",
    "OVERTEMP",
    "BALANCING ERROR",
    "EEPROM ERROR",
    "CURRENT IRR"
};

const char* LVBMS_FAULT_MESSAGES[] =
{
    "SHUTDOWN",
    "M17 ERROR",
    "ADES ERROR",
    "CELL IRR",
    "CHIP IRR",
    "TEMP IRR",
    "OUT OF JUICE",
    "CELL DIFF",
    "CHARGER ERROR",
    "OVERTEMP",
    "BALANCING ERROR",
    "OVERCURRENT",
    "CURRENT IRR"
};

static uint16_t calculate_lvbms_min_volt()
{
    uint16_t min_volt = sec_bus.lvbms_voltages.lvbms_voltages_0;

    if (sec_bus.lvbms_voltages.lvbms_voltages_1 < min_volt) min_volt = sec_bus.lvbms_voltages.lvbms_voltages_1;
    if (sec_bus.lvbms_voltages.lvbms_voltages_2 < min_volt) min_volt = sec_bus.lvbms_voltages.lvbms_voltages_2;
    if (sec_bus.lvbms_voltages.lvbms_voltages_3 < min_volt) min_volt = sec_bus.lvbms_voltages.lvbms_voltages_3; 
    if (sec_bus.lvbms_voltages.lvbms_voltages_4 < min_volt) min_volt = sec_bus.lvbms_voltages.lvbms_voltages_4;
    if (sec_bus.lvbms_voltages.lvbms_voltages_5 < min_volt) min_volt = sec_bus.lvbms_voltages.lvbms_voltages_5; 

    return min_volt;
}

static uint16_t calculate_lvbms_max_temp()
{
    uint16_t max_temp = sec_bus.lvbms_temperatures.lvbms_temperatures_0;

    if (sec_bus.lvbms_temperatures.lvbms_temperatures_1 > max_temp) max_temp = sec_bus.lvbms_temperatures.lvbms_temperatures_1;
    if (sec_bus.lvbms_temperatures.lvbms_temperatures_2 > max_temp) max_temp = sec_bus.lvbms_temperatures.lvbms_temperatures_2;
    if (sec_bus.lvbms_temperatures.lvbms_temperatures_3 > max_temp) max_temp = sec_bus.lvbms_temperatures.lvbms_temperatures_3;
    if (sec_bus.lvbms_temperatures.lvbms_temperatures_4 > max_temp) max_temp = sec_bus.lvbms_temperatures.lvbms_temperatures_4; 
    if (sec_bus.lvbms_temperatures.lvbms_temperatures_5 > max_temp) max_temp = sec_bus.lvbms_temperatures.lvbms_temperatures_5; 

    return max_temp;
}

static uint16_t calculate_avg_inverter_temp()
{
    return ((sec_bus.inverter_temps.vc_rr_inverter_temp +
             sec_bus.inverter_temps.vc_rl_inverter_temp +
             sec_bus.inverter_temps.vc_fr_inverter_temp +
             sec_bus.inverter_temps.vc_fl_inverter_temp) / 4);
}

static uint16_t calculate_max_inverter_temp()
{
    uint16_t max_temp = sec_bus.inverter_temps.vc_rr_inverter_temp;

    if (sec_bus.inverter_temps.vc_rl_inverter_temp > max_temp) max_temp = sec_bus.inverter_temps.vc_rl_inverter_temp;
    if (sec_bus.inverter_temps.vc_fr_inverter_temp > max_temp) max_temp = sec_bus.inverter_temps.vc_fr_inverter_temp;
    if (sec_bus.inverter_temps.vc_fl_inverter_temp > max_temp) max_temp = sec_bus.inverter_temps.vc_fl_inverter_temp;

    return max_temp;
}


void DataManager_update_data()
{
    DataManager_update_lvbms();
    DataManager_update_hvbms();
    DataManager_update_motors();
    DataManager_update_inverters();
    DataManager_update_faults();
    DataManager_update_vc_status();
}


void DataManager_update_lvbms()
{
    uint16_t new_min_volt = calculate_lvbms_min_volt();
    uint16_t new_max_temp = calculate_lvbms_max_temp();

    if (dash_data.lvbms_min_cell != new_min_volt) 
    {
        dash_data.lvbms_min_cell = new_min_volt;
        dash_data.lvbms_flag = true;
    }

    if (dash_data.lvbms_max_temp != new_max_temp)
    {
        dash_data.lvbms_max_temp = new_max_temp;
        dash_data.lvbms_flag = true;
    }

    if (dash_data.lvbms_pack_v != sec_bus.lvbms_pack_data.lvbms_chip_voltage)
    {
        dash_data.lvbms_pack_v = sec_bus.lvbms_pack_data.lvbms_chip_voltage;
        dash_data.lvbms_flag = true;
    }
}


void DataManager_update_hvbms()
{
    if (dash_data.hvbms_min_cell != sec_bus.hvbms_cell_overview.bms_overview_volt_min)
    {
        dash_data.hvbms_min_cell = sec_bus.hvbms_cell_overview.bms_overview_volt_min;
        dash_data.hvbms_flag = true;
    }

    if (dash_data.hvbms_max_temp != sec_bus.hvbms_cell_overview.bms_overview_temp_max)
    {
        dash_data.hvbms_max_temp = sec_bus.hvbms_cell_overview.bms_overview_temp_max;
        dash_data.hvbms_flag = true;
    }

    if (dash_data.hvbms_pack_v != sec_bus.hvbms_status.bms_status_pack_voltage)
    {
        dash_data.hvbms_pack_v = sec_bus.hvbms_status.bms_status_pack_voltage;
        dash_data.hvbms_flag = true;
    }

    if (dash_data.hvbms_soc != sec_bus.hvbms_status.bms_status_soc)
    {
        dash_data.hvbms_soc = sec_bus.hvbms_status.bms_status_soc;
        dash_data.soc_flag = true;
    }
}


void DataManager_update_motors()
{
    if (dash_data.rr_motor_temp != sec_bus.motor_temps.vc_rr_motor_temp)
    {
        dash_data.rr_motor_temp = sec_bus.motor_temps.vc_rr_motor_temp;
        dash_data.motor_flag = true;
    }

    if (dash_data.rl_motor_temp != sec_bus.motor_temps.vc_rl_motor_temp)
    {
        dash_data.rl_motor_temp = sec_bus.motor_temps.vc_rl_motor_temp;
        dash_data.motor_flag = true;
    }

    if (dash_data.fr_motor_temp != sec_bus.motor_temps.vc_fr_motor_temp)
    {
        dash_data.fr_motor_temp = sec_bus.motor_temps.vc_fr_motor_temp;
        dash_data.motor_flag = true;
    }

    if (dash_data.fl_motor_temp != sec_bus.motor_temps.vc_fl_motor_temp)
    {
        dash_data.fl_motor_temp = sec_bus.motor_temps.vc_fl_motor_temp;
        dash_data.motor_flag = true;
    }
}


void DataManager_update_inverters()
{
    uint16_t new_avg_temp = calculate_avg_inverter_temp();
    uint16_t new_max_temp = calculate_max_inverter_temp();

    if (dash_data.avg_inv_temp != new_avg_temp)
    {
        dash_data.avg_inv_temp = new_avg_temp;
        dash_data.inverter_flag = true;
    }

    if (dash_data.max_inv_temp != new_max_temp)
    {
        dash_data.max_inv_temp = new_max_temp;
        dash_data.inverter_flag = true;
    }
}


void DataManager_update_faults()
{
    if (dash_data.vc_fault_vector != sec_bus.vc_fault_vector       ||
        dash_data.pdu_fault_vector != sec_bus.pdu_fault_vector     ||
        dash_data.hvbms_fault_vector != sec_bus.hvbms_fault_vector ||
        dash_data.lvbms_fault_vector != sec_bus.lvbms_fault_vector) 
    {
        dash_data.vc_fault_vector = sec_bus.vc_fault_vector;
        dash_data.pdu_fault_vector = sec_bus.pdu_fault_vector;
        dash_data.hvbms_fault_vector = sec_bus.hvbms_fault_vector;
        dash_data.lvbms_fault_vector = sec_bus.lvbms_fault_vector;
        dash_data.fault_flag = true;
    }
}


void DataManager_update_vc_status()
{
    if (dash_data.vc_status.vc_status_vehicle_state != sec_bus.vc_status.vc_status_vehicle_state)
    {
        dash_data.vc_status.vc_status_vehicle_state = sec_bus.vc_status.vc_status_vehicle_state;
        dash_data.vc_status_flag = true;
    }
}
