#pragma once
#include "common.h"
#include "cart.h"




class innercart
{
public:
    innercart(cart* c, cart::rom_header* header, uint32_t rom_size, uint8_t* rom_data);
    ~innercart();

    uint8_t cart_read(uint16_t address);
    void cart_write(uint16_t address, uint8_t value);

private:
    cart::rom_header* header;

    char filename[1024];
    uint32_t rom_size;
    uint8_t* rom_data;

   
    cart* c;
    void cart_setup_banking();

    bool cart_need_save();
    bool cart_mbc1();
    bool cart_battery();

    //mbc1 data
    bool ram_enabled;
    bool ram_banking;

    uint8_t* rom_bank_x;
    uint8_t banking_mode;

    uint8_t rom_bank_value;
    uint8_t ram_bank_value;

    uint8_t* ram_bank;
    uint8_t* ram_banks[16];

    //for battery
    bool battery;
    bool need_save;


};

