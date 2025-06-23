#pragma once
#include "common.h"
#include "cart.h"




class innercart
{
public:
    innercart(cart* c, cart::rom_header* header, uint32_t rom_size, uint8_t* rom_data, char* filename);
    ~innercart();

    virtual uint8_t cart_read(uint16_t address);
    virtual void cart_write(uint16_t address, uint8_t value);

    virtual bool cart_need_save();
    virtual bool cart_battery();

    virtual void cart_battery_load();
    virtual void cart_battery_save();

    virtual void cart_tick();

protected:
    cart::rom_header* header;

    char *filename;
    uint32_t rom_size;
    uint8_t* rom_data;

   
    cart* c;


};

