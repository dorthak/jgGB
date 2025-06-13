#pragma once
#include "common.h"
#include "innercart.h"
#include "cart.h"

class iCartMBC1 :
    public innercart
{
public:
    iCartMBC1(cart* c, cart::rom_header* header, uint32_t rom_size, uint8_t* rom_data);
    ~iCartMBC1();

    uint8_t cart_read(uint16_t address) override;
    void cart_write(uint16_t address, uint8_t value) override;

    bool cart_need_save() override;
    bool cart_battery() override;

    void cart_battery_load() override;
    void cart_battery_save() override;

private:
    void cart_setup_banking();

 
    bool cart_mbc1();
   


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

