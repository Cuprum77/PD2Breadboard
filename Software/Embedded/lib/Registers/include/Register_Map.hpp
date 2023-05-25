#pragma once

/*
    Default values for the system registers
*/

#define Device_Ping_Default 0x45U
#define Device_Target_Voltage_Default 0x1388U
#define Device_Target_Current_Default 0x1f4U

/*
    Default values for the INA219
*/

#define Shunt_Calibration_Default 0x00U
#define Bus_ADC_Config_Default 0x14U
#define Shunt_ADC_Config_Default 0x14U

/*
    Default values for the display
*/

#define Display_Brightness_Default 0x64U
#define Display_Brightness_Limit_Default 0x64U
#define Display_Background_Color_Default 0x00U
#define Display_Text_Color_Default __UINT32_MAX__

/*
    Default values for the programmable fuse
*/

#define PFuse_Warning_Current_Default 0x3e8
#define PFuse_Trip_Current_Default 0xbb8

/*
    0x00 through 0x0f are reserved for device control
    0x10 through 0x1f are reserved for the INA219
    0x20 through 0x2f are reserved for the display
    0x30 through 0x3f are reserved for the programmable fuse
    0x40 through 0x5f are reserved for the FUSB302
*/

typedef enum : unsigned int
{
    Device_Ping                 = 0x00,
    Device_Reset                = 0x05,
    Device_Reboot_Bootloader    = 0x06,
    Device_Self_Test            = 0x07,
    Device_Self_Test_Result     = 0x08,
    Device_Target_Voltage       = 0x09,
    Device_Target_Current       = 0x0A,
    Git_Hash                    = 0x0F,

    Bus_Voltage                 = 0x10,
    Shunt_Voltage               = 0x11,
    Current                     = 0x12,
    Power                       = 0x13,
    Shunt_Calibration           = 0x14,
    Bus_ADC_Config              = 0x15,
    Shunt_ADC_Config            = 0x16,

    Display_Brightness          = 0x20,
    Display_Brightness_Limit    = 0x21,
    Display_Background_Color    = 0x22,
    Display_Text_Color          = 0x23,

    PFuse_Status                = 0x30,
    PFuse_Warning_Current       = 0x31,
    PFuse_Trip_Current          = 0x32,

    USB_PD_Status               = 0x40,
    USB_PD_IsPD                 = 0x41,
    USB_PD_HasPPS               = 0x42,
    USB_PD_Voltage              = 0x43,
    USB_PD_Current              = 0x44,
    USB_PD_Voltage_Step         = 0x45,
    USB_PD_Current_Step         = 0x46,
    USB_PD_Voltage_Min          = 0x47,
    USB_PD_Voltage_Max          = 0x48,
    USB_PD_Current_Min          = 0x49,
    USB_PD_Current_Max          = 0x4A,
    USB_PD_Supply_Type          = 0x4B,
    USB_PD_Dual_Role            = 0x4C,
    USB_PD_COM_Capable          = 0x4D,    
} Register_Address;

enum RegisterType
{
    Default,
    ReadOnly,
    WriteOnly,
};

struct Register
{
private:
    bool _readOnly = false;
    bool _writeOnly = false;
    unsigned int _defaultValue = 0x0;

public:
    unsigned int value = 0x0;

    Register(RegisterType type, unsigned int defaultValue = __UINT32_MAX__)
    {
        _readOnly = (type & 0x1) ? true : false;
        _writeOnly = ((type >> 0x1) & 0x1 && !_readOnly) ? true : false;
        _defaultValue = defaultValue;
        value = defaultValue;
    }

    bool isReadOnly() { return _readOnly; }
    bool isWriteOnly() { return _writeOnly; }
    void reset() { value = _defaultValue; }
};

/**
 * @brief A register array is a register that can be read from or written to.
 * @note This contains a 32 word array of unsigned chars.
*/
struct RegisterArray
{
private:
    bool _readOnly = false;
    bool _writeOnly = false;

public:
    unsigned int array[0x20] = {0x0};

    RegisterArray(RegisterType type)
    {
        _readOnly = (type & 0x1) ? true : false;
        _writeOnly = ((type >> 0x1) & 0x1 && !_readOnly) ? true : false;
    }

    bool isReadOnly() { return _readOnly; }
    bool isWriteOnly() { return _writeOnly; }
};

struct RegisterMap
{
    Register Device_Ping                    = Register(RegisterType::ReadOnly, Device_Ping_Default);
    Register Device_Reset                   = Register(RegisterType::WriteOnly);
    Register Device_Reboot_Bootloader       = Register(RegisterType::WriteOnly);
    Register Device_Self_Test               = Register(RegisterType::WriteOnly);
    RegisterArray Device_Self_Test_Result   = RegisterArray(RegisterType::ReadOnly);
    Register Device_Target_Voltage          = Register(RegisterType::Default, Device_Target_Voltage_Default);
    Register Device_Target_Current          = Register(RegisterType::Default, Device_Target_Current_Default);
    RegisterArray Git_Hash                  = RegisterArray(RegisterType::ReadOnly);

    Register Bus_Voltage                    = Register(RegisterType::ReadOnly);
    Register Shunt_Voltage                  = Register(RegisterType::ReadOnly);             
    Register Current                        = Register(RegisterType::ReadOnly);
    Register Power                          = Register(RegisterType::ReadOnly);
    Register Shunt_Calibration              = Register(RegisterType::Default, Shunt_Calibration_Default);
    Register Bus_ADC_Config                 = Register(RegisterType::Default, Bus_ADC_Config_Default);
    Register Shunt_ADC_Config               = Register(RegisterType::Default, Shunt_ADC_Config_Default);

    Register Display_Brightness             = Register(RegisterType::Default, Display_Brightness_Default);
    Register Display_Brightness_Limit       = Register(RegisterType::Default, Display_Brightness_Limit_Default);
    Register Display_Background_Color       = Register(RegisterType::Default, Display_Background_Color_Default);
    Register Display_Text_Color             = Register(RegisterType::Default, Display_Text_Color_Default);

    Register PFuse_Status                   = Register(RegisterType::ReadOnly);
    Register PFuse_Warning_Current          = Register(RegisterType::Default, PFuse_Warning_Current_Default);
    Register PFuse_Trip_Current             = Register(RegisterType::Default, PFuse_Trip_Current_Default);

    Register USB_PD_Status                  = Register(RegisterType::ReadOnly);
    Register USB_PD_IsPD                    = Register(RegisterType::ReadOnly);
    Register USB_PD_HasPPS                  = Register(RegisterType::ReadOnly);
    Register USB_PD_Voltage                 = Register(RegisterType::ReadOnly);
    Register USB_PD_Current                 = Register(RegisterType::ReadOnly);
    Register USB_PD_Voltage_Step            = Register(RegisterType::ReadOnly);
    Register USB_PD_Current_Step            = Register(RegisterType::ReadOnly);
    Register USB_PD_Voltage_Min             = Register(RegisterType::ReadOnly);
    Register USB_PD_Voltage_Max             = Register(RegisterType::ReadOnly);
    Register USB_PD_Current_Min             = Register(RegisterType::ReadOnly);
    Register USB_PD_Current_Max             = Register(RegisterType::ReadOnly);
    Register USB_PD_Supply_Type             = Register(RegisterType::ReadOnly);
    Register USB_PD_Dual_Role               = Register(RegisterType::ReadOnly);
    Register USB_PD_COM_Capable             = Register(RegisterType::ReadOnly);

    void reset()
    {
        Device_Target_Voltage.reset();
        Device_Target_Current.reset();
        Shunt_Calibration.reset();
        Bus_ADC_Config.reset();
        Shunt_ADC_Config.reset();
        Display_Brightness.reset();
        Display_Brightness_Limit.reset();
        Display_Background_Color.reset();
        Display_Text_Color.reset();
        PFuse_Warning_Current.reset();
        PFuse_Trip_Current.reset();
    }

    RegisterArray* getRegisterArray(Register_Address address)
    {
        switch(address)
        {
            case Register_Address::Device_Self_Test_Result:
                return &Device_Self_Test_Result;
            case Register_Address::Git_Hash:
                return &Git_Hash;
            default:
                return nullptr;
        }
    }

    Register* getRegister(Register_Address address)
    {
        switch(address)
        {
            case Register_Address::Device_Ping:
                return &Device_Ping;
            case Register_Address::Device_Reset:
                return &Device_Reset;
            case Register_Address::Device_Reboot_Bootloader:
                return &Device_Reboot_Bootloader;
            case Register_Address::Device_Self_Test:
                return &Device_Self_Test;
            case Register_Address::Device_Target_Voltage:
                return &Device_Target_Voltage;
            case Register_Address::Device_Target_Current:
                return &Device_Target_Current;
            case Register_Address::Bus_Voltage:
                return &Bus_Voltage;
            case Register_Address::Shunt_Voltage:
                return &Shunt_Voltage;
            case Register_Address::Current:
                return &Current;
            case Register_Address::Power:
                return &Power;
            case Register_Address::Shunt_Calibration:
                return &Shunt_Calibration;
            case Register_Address::Bus_ADC_Config:
                return &Bus_ADC_Config;
            case Register_Address::Shunt_ADC_Config:
                return &Shunt_ADC_Config;
            case Register_Address::Display_Brightness:
                return &Display_Brightness;
            case Register_Address::Display_Brightness_Limit:
                return &Display_Brightness_Limit;
            case Register_Address::Display_Background_Color:
                return &Display_Background_Color;
            case Register_Address::Display_Text_Color:
                return &Display_Text_Color;
            case Register_Address::PFuse_Status:
                return &PFuse_Status;
            case Register_Address::PFuse_Warning_Current:
                return &PFuse_Warning_Current;
            case Register_Address::PFuse_Trip_Current:
                return &PFuse_Trip_Current;
            case Register_Address::USB_PD_Status:
                return &USB_PD_Status;
            case Register_Address::USB_PD_IsPD:
                return &USB_PD_IsPD;
            case Register_Address::USB_PD_HasPPS:
                return &USB_PD_HasPPS;
            case Register_Address::USB_PD_Voltage:
                return &USB_PD_Voltage;
            case Register_Address::USB_PD_Current:
                return &USB_PD_Current;
            case Register_Address::USB_PD_Voltage_Step:
                return &USB_PD_Voltage_Step;
            case Register_Address::USB_PD_Current_Step:
                return &USB_PD_Current_Step;
            case Register_Address::USB_PD_Voltage_Min:
                return &USB_PD_Voltage_Min;
            case Register_Address::USB_PD_Voltage_Max:
                return &USB_PD_Voltage_Max;
            case Register_Address::USB_PD_Current_Min:
                return &USB_PD_Current_Min;
            case Register_Address::USB_PD_Current_Max:
                return &USB_PD_Current_Max;
            case Register_Address::USB_PD_Supply_Type:
                return &USB_PD_Supply_Type;
            case Register_Address::USB_PD_Dual_Role:
                return &USB_PD_Dual_Role;
            case Register_Address::USB_PD_COM_Capable:
                return &USB_PD_COM_Capable;
            default:
                return nullptr;
        }
    }
};

