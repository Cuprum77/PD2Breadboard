#pragma once

#include <stdio.h>
#include "Register_Map.hpp"

class Registers
{
public:
    void reset();
    void setProtected(Register_Address address, unsigned int value);
    void setProtected(Register_Address address, unsigned int position, unsigned int value);
    void set(Register_Address address, unsigned int value);
    void set(Register_Address address, unsigned int position, unsigned int value);
    unsigned int get(Register_Address address);
    unsigned int get(Register_Address address, unsigned int position);
    bool isArray(Register_Address address);
private:
    RegisterMap registers;

};
