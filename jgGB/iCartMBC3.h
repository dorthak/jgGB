
#pragma once
#include "common.h"
#include "innercart.h"
#include "cart.h"
#include "bus.h"

class iCartMBC3 :
    public innercart
{
public:
    iCartMBC3(cart* c, cart::rom_header* header, uint32_t rom_size, uint8_t* rom_data, char* filename);
    ~iCartMBC3();

    uint8_t cart_read(uint16_t address) override;
    void cart_write(uint16_t address, uint8_t value) override;

    bool cart_need_save() override;
    bool cart_battery() override;

    void cart_battery_load() override;
    void cart_battery_save() override;

    typedef struct {
        uint8_t RTC_S;
        uint8_t RTC_M;
        uint8_t RTC_H;
        uint8_t RTC_DL;
        uint8_t RTC_DH;

    } rtclock;

    void cart_tick() override;

private:
    void cart_setup_banking();
    uint64_t rtc_get_current_ms();
    void rtc_stop_clock();
    void rtc_restart_clock();
    
    

    bool cart_mbc3();

    //mbc3 data
    bool ram_enabled;

    uint8_t* rom_bank_x;

    uint8_t rom_bank_value;
    uint8_t ram_bank_value;

    uint8_t* ram_bank;
    uint8_t* ram_banks[16];

    uint8_t ram_bank_mask;
    uint8_t rom_bank_max;

    //for battery
    bool battery;
    bool need_save;
    FILE* fp;

    void cart_battery_init();

    //for RTC
    bool rtc;
    bool rtc_mode;
    uint8_t rtc_curr_reg;
    uint64_t rtc_last_value;
    bool rtc_stopped;

    void setDay(uint16_t day);

    rtclock rtc_real_clock;
    rtclock rtc_latch_clock;


};


