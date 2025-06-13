#include "iCartMBC1.h"

iCartMBC1::iCartMBC1(cart* c, cart::rom_header* header, uint32_t rom_size, uint8_t* rom_data) : innercart(c, header, rom_size, rom_data)
{
    std::cout << "iCartMBC1 created" << std::endl;
    battery = false;
    if (header->type == 3)
    {
        battery = true;
        cart_battery_load();
    }
    need_save = false;

    cart_setup_banking();
    ram_enabled = false;
    ram_banking = false;
    banking_mode = false;
}

iCartMBC1::~iCartMBC1()
{
    for (int i = 0; i < 16; i++)
    {
        if (ram_banks[i] != 0)
        {
            delete ram_banks[i];
        }
    }
}

void iCartMBC1::cart_setup_banking() 
{
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

bool iCartMBC1::cart_need_save()
{
    return need_save;
}
bool iCartMBC1::cart_mbc1()
{
    return (header->type >= 1) || (header->type <= 3);
}

bool iCartMBC1::cart_battery()
{
    return battery;
}

void iCartMBC1::cart_battery_load()
{

}

void iCartMBC1::cart_battery_save()
{

}

uint8_t iCartMBC1::cart_read(uint16_t address) 
{
    //ROM Bank X0
    if (address < 0x4000)
    {
        return rom_data[address];
    }

    //RAM Bank, if any
    if ((address & 0xE000) == 0xA000)
    {
        if (!ram_enabled)
        {
            return 0xFF;
        }
        if (!ram_bank)
        {
            return 0xFF;
        }
        return ram_bank[address - 0xA000];
    }

    //ROM Banks 01-7F
    return rom_bank_x[address - 0x4000];

}

void iCartMBC1::cart_write(uint16_t address, uint8_t value) 
{
    //RAM Enable register
    if (address < 0x2000)
    {
        ram_enabled = ((value & 0xF) == 0xA);
        return;
    }
    //ROM Bank Number register
    if ((address & 0xE000) == 0x2000)
    {
        if (value == 0)
        {
            value = 1;
        }

        value &= 0b11111;

        rom_bank_value = value;
        rom_bank_x = rom_data + (0x4000 * rom_bank_value);
        return;
    }

    //RAM Bank Number register
    if ((address & 0xE000) == 0x4000)
    {
        ram_bank_value = value & 0b11;
        if (ram_banking)
        {
            if (cart_need_save())
            {
                cart_battery_save();
            }
            ram_bank = ram_banks[ram_bank_value];
        }
        return;
    }

    //Banking mode select register
    if ((address & 0xE000) == 0x6000)
    {
        banking_mode = value & 1;
        ram_banking = banking_mode;
        
        if (ram_banking)
        {
            if (cart_need_save())
            {
                cart_battery_save();
            }
            ram_bank = ram_banks[ram_bank_value];
        }
        return;
    }

    if ((address & 0xE000) == 0xA000)
    {
        if (!ram_enabled)
        {
            return;
        }

        if (!ram_bank)
        {
            return;
        }

        ram_bank[address - 0xA000] = value;

        if (battery)
        {
            need_save = true;
        }
    }
}