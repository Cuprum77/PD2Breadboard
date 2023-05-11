#include "Registers.hpp"

/**
 * @brief Resets all registers to their default values
*/
void Registers::reset()
{
    registers.reset();
}

/**
 * @brief Sets the value of a register
 * @param address Address of the register
 * @param value Value to set the register to
*/
void Registers::set(Register_Address address, unsigned int value)
{
    Register* reg = this->registers.getRegister(address);
    if (reg != NULL)
    {
        if(reg->isReadOnly())
            return;

        reg->value = value;
    }
}

/**
 * @brief Sets the value of a register array
 * @param address Address of the register array
 * @param position Position of the register in the array
 * @param value Value to set the register to
*/
void Registers::set(Register_Address address, unsigned int position, unsigned int value)
{
    RegisterArray* reg = this->registers.getRegisterArray(address);
    if (reg != NULL)
    {
        if(reg->isReadOnly())
            return;

        reg->array[position] = value;
    }
}

/**
 * @brief Gets the value of a register
 * @param address Address of the register
 * @return Value of the register, or __UINT32_MAX__ if the register is write only
*/
unsigned int Registers::get(Register_Address address)
{
    Register* reg = this->registers.getRegister(address);
    if (reg == NULL)
        return __UINT32_MAX__;

    if(reg->isWriteOnly())
        return __UINT32_MAX__;

    return reg->value;
}

/**
 * @brief Gets the value of a register array
 * @param address Address of the register array
 * @param position Position of the register in the array
 * @return Value of the register, or __UINT32_MAX__ if the register is write only
*/
unsigned int Registers::get(Register_Address address, unsigned int position)
{
    RegisterArray* reg = this->registers.getRegisterArray(address);
    if (reg == NULL)
        return __UINT32_MAX__;

    if(reg->isWriteOnly())
        return __UINT32_MAX__;

    return reg->array[position];
}

/**
 * @brief Checks if a register is an array
 * @param address Address of the register
 * @return True if the register is an array, false otherwise
*/
bool Registers::isArray(Register_Address address)
{
    RegisterArray* reg = this->registers.getRegisterArray(address);
    if (reg == NULL)
        return false;

    return true;
}