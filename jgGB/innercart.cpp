#include "innercart.h"
#include "cart.h"


innercart::innercart(cart* c, cart::rom_header* header, uint32_t rom_size, uint8_t* rom_data)
{
    this->c = c;
    this->header = header;
    this->rom_size = rom_size;
    this->rom_data = rom_data;
}

innercart::~innercart()
{

}

uint8_t innercart::cart_read(uint16_t address)
{
    return rom_data[address];
}

void innercart::cart_write(uint16_t address, uint8_t value)
{
    //printf("cart_write(%04X)\n", address);
    //NO_IMPL
    //rom_data[address] = value;
}


void innercart::cart_setup_banking() {
    for (int i = 0; i < 16; i++)
    {
        ram_banks[i] = 0;
        bool set = false;

        switch (header->ram_size)
        {
            case(0): set = false; break;
            case(1): set = false; break;
            case(2):
            {
                if (i == 0)
                {
                    set = true;
                }
                break;
            }
            case(3):
            {
                if (i < 4)
                {
                    set = true;
                }
                break;
            }
            case(4):
            {
                if (i < 16)
                {
                    set = true;
                }
                break;
            }
            case(5):
            {
                if (i < 8)
                {
                    set = true;
                }
                break;
            }
        }

        if (set)
        {
            ram_banks[i] = new uint8_t[0x2000];
            memset(ram_banks[i], 0, 0x2000);
        }
    }
    ram_bank = ram_banks[0];
    rom_bank_x = rom_data + 0x4000;
}

bool innercart::cart_need_save()
{
    return need_save;
}
bool innercart::cart_mbc1()
{
    return (header->type >= 1) || (header->type <= 3);
}

bool innercart::cart_battery()
{
    return battery;
}

