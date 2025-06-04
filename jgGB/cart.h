#pragma once
#include "common.h"

//forward declare
class SST;


class cart
{

public:
    cart();
    ~cart();
//    cart(char* cartfilename);
    bool cart_loaded();
    bool cart_load(char* cartfilename);

    uint8_t cart_read(uint16_t address);
    void cart_write(uint16_t address, uint8_t value);


private:
    bool cartloaded = false;

    std::string cart_lic_name();
    std::string cart_type_name();

    void init_lic_code();


    typedef struct {
        uint8_t entry[4];
        uint8_t logo[0x30];             
                
        char title[16];             
        uint16_t new_lic_code;              
        uint8_t sgb_flag;               
        uint8_t type;               
        uint8_t rom_size;               
        uint8_t ram_size;               
        uint8_t dest_code;              
        uint8_t lic_code;               
        uint8_t version;                
        uint8_t checksum;               
        uint16_t global_checksum;               
    } rom_header;               
                    
    rom_header *header;              
                
    char filename[1024];                
    uint32_t rom_size;              
    uint8_t* rom_data;
                    
    const std::string ROM_TYPES[0x23] = {             
    "ROM ONLY",             
    "MBC1",             
    "MBC1+RAM",             
    "MBC1+RAM+BATTERY",             
    "0x04 ???",             
    "MBC2",             
    "MBC2+BATTERY",             
    "0x07 ???",             
    "ROM+RAM 1",                
    "ROM+RAM+BATTERY 1",                
    "0x0A ???",             
    "MMM01",                
    "MMM01+RAM",                
    "MMM01+RAM+BATTERY",                
    "0x0E ???",             
    "MBC3+TIMER+BATTERY",               
    "MBC3+TIMER+RAM+BATTERY 2",             
    "MBC3",             
    "MBC3+RAM 2",               
    "MBC3+RAM+BATTERY 2",               
    "0x14 ???",             
    "0x15 ???",             
    "0x16 ???",             
    "0x17 ???",             
    "0x18 ???",             
    "MBC5",             
    "MBC5+RAM",             
    "MBC5+RAM+BATTERY",             
    "MBC5+RUMBLE",              
    "MBC5+RUMBLE+RAM",              
    "MBC5+RUMBLE+RAM+BATTERY",              
    "0x1F ???",             
    "MBC6",             
    "0x21 ???",             
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",               
    };              
                
    std::string LIC_CODE[0xA5];

    //used for testing without a real cart
    void cartFake(int size);
    friend class SST;


};

