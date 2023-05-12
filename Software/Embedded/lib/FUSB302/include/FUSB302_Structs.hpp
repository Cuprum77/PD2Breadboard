#pragma once

typedef struct
{
    unsigned short outputVoltage;
    unsigned short outputCurrent;
    unsigned short pps_type_flag;
    unsigned short pps_operating_mode;
} FUSB302_Status_t;

typedef struct
{
    unsigned short type;
    unsigned short spec_rev;
    unsigned short id;
    unsigned short number_of_objects;
} FUSB302_Message_Header_t;


typedef struct
{
    const char* name;
    unsigned char id;
    unsigned char spec_rev;
    unsigned char number_of_objects;
    unsigned char extended;
} FUSB302_Message_Info_t;

typedef struct
{
    unsigned int power_source;
    unsigned short min_voltage;
    unsigned short max_voltage;
    unsigned short max_current;
    unsigned short max_power;
} FUSB302_Power_Info_t;

struct FUSB302_Message_State_t;
typedef struct 
{
    const struct FUSB302_Message_State_t* state;
    unsigned short tx_message_header;
    unsigned short rx_message_header;
    unsigned char message_id;

    unsigned short PPS_voltage;
    unsigned short PPS_current;
    unsigned short PPS_status_data_block[4];

    unsigned char power_option;
    unsigned int power_data_object[7];
    unsigned char power_data_object_count;
    unsigned char power_data_object_position;
} FUSB302_Protocol_t;
