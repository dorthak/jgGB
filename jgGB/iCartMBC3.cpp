#include "iCartMBC3.h"
#include <chrono>
#include <SDL3/SDL.h>

iCartMBC3::iCartMBC3(cart* c, cart::rom_header* header, uint32_t rom_size, uint8_t* rom_data, char* filename) : innercart(c, header, rom_size, rom_data, filename)
{
    std::cout << "iCartMBC3 created" << std::endl;

    cart_setup_banking();

    battery = false;
    if ((header->type == 0x0F) || (header->type == 0x10) || (header->type == 0x13))
    {
        battery = true;
        cart_battery_load();
        cart_battery_init();

    }
    need_save = false;

    rtc = false;
    if ((header->type == 0x0F) || (header->type == 0x10))
    {
        rtc = true;
        rtc_mode = false;
    }
    
    rtc_real_clock.RTC_S = 0;
    rtc_real_clock.RTC_M = 0;
    rtc_real_clock.RTC_H = 0;
    rtc_real_clock.RTC_DL = 0;
    rtc_real_clock.RTC_DH = (1 << 6);

    rtc_latch_clock.RTC_S = 0;
    rtc_latch_clock.RTC_M = 0;
    rtc_latch_clock.RTC_H = 0;
    rtc_latch_clock.RTC_DL = 0;
    rtc_latch_clock.RTC_DH = (1 << 6);

    rtc_last_value = rtc_get_current_ms();
    rtc_stopped = true;

    std::cout << "Start time: " << rtc_last_value << std::endl;

    ram_enabled = false;

    
    ram_bank_mask = (uint8_t) 0xb11;
    if ((header->ram_size == 0x05) || (header->rom_size == 0x07))  //MBC30
    {
        ram_bank_mask = (uint8_t)0xb111;
    }

    rom_bank_max = rom_size / 0x3FFF;
}

iCartMBC3::~iCartMBC3()
{
    fclose(fp);

    for (int i = 0; i < 16; i++)
    {
        if (ram_banks[i] != 0)
        {
            delete ram_banks[i];
        }
    }
}

void iCartMBC3::cart_setup_banking()
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
    ram_bank_value = 0;
    rom_bank_x = rom_data + 0x4000;
    rom_bank_value = 1;
}

bool iCartMBC3::cart_need_save()
{
    return need_save;
}
bool iCartMBC3::cart_mbc3()
{
    return (header->type >= 0x0F) || (header->type <= 0x13);
}

bool iCartMBC3::cart_battery()
{
    return battery;
}

void iCartMBC3::cart_battery_load()
{
    char fn[1048];
    sprintf(fn, "%s.battery", filename);

    errno_t err;
    FILE* fp;

    err = fopen_s(&fp, fn, "rb");

    if (err)
    {
        std::cout << "Failed to open: " << fn << " for read" << std::endl;
        return;
    }

    for (int i = 0; i < 16; i++)
    {
        if (ram_banks[i] != 0)
        {
            fread(ram_banks[i], 0x2000, 1, fp);
        }
    }

    
    fclose(fp);

}

void iCartMBC3::cart_battery_init()
{
    char fn[1048];
    sprintf(fn, "%s.battery", filename);

    errno_t err;

    err = fopen_s(&fp, fn, "wb");

    if (err)
    {
        std::cout << "Failed to open: " <<  fn << " for writing" << std::endl;
        return;
    }
    std::cout << "Successfully opened: " << fn << " for writing" << std::endl;

}

void iCartMBC3::cart_battery_save()
{
    rewind(fp);
    for (int i = 0; i < 16; i++)
    {
        if (ram_banks[i] != 0)
        {
            fwrite(ram_banks[i], 0x2000, 1, fp);
        }
    }

}

uint8_t iCartMBC3::cart_read(uint16_t address)
{
    //ROM Bank X0
    if (address < 0x4000)  /// 0000-3FFF
    {
        return rom_data[address];
    }

    //RAM Bank, if any
    if ((address & 0xE000) == 0xA000) // A000-BFFF
        if (!rtc_mode)
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
        } else {
            if (!ram_enabled)
            {
                return 0xFF;
            }
            if (!rtc)
            {
                return 0xFF;
            }
            switch (rtc_curr_reg) {
                case 0x08: return rtc_latch_clock.RTC_S; break;
                case 0x09: return rtc_latch_clock.RTC_M; break;
                case 0x0A: return rtc_latch_clock.RTC_H; break;
                case 0x0B: return rtc_latch_clock.RTC_DL; break;
                case 0x0C: return rtc_latch_clock.RTC_DH; break;
            }
        }

    //ROM Banks 01-7F - 4000-7FFF
    return rom_bank_x[address - 0x4000];

}

void iCartMBC3::cart_write(uint16_t address, uint8_t value)
{
    //RAM Enable register
    if (address < 0x2000) //0000-1FFF
    {
        ram_enabled = ((value & 0xF) == 0xA);
        return;
    }
    //ROM Bank Number register
    if ((address & 0xE000) == 0x2000) // 2000-3FFF
    {
        value &= 0x7F; //max banks possible with MBC3

        if (value == 0)
        {
            value = 1;
        }
        value &= (rom_bank_max - 1);  //check that it's no  higher than actual number of banks on chip.

        rom_bank_value = value;
        rom_bank_x = rom_data + (0x4000 * rom_bank_value);
        std::cout << "Switching to ROM bank " << (int)rom_bank_value << std::endl;
        return;
    }

    //RAM Bank Number register
    if ((address & 0xE000) == 0x4000)// 4000-5FFF
    {
        if (value < 0x08)  //accessing RAM Banks
        {
            rtc_mode = false;
            ram_bank_value = value & ram_bank_mask;
            if (cart_need_save())
            {
                cart_battery_save();
            }
            ram_bank = ram_banks[ram_bank_value];
            std::cout << "Switching to RAM bank " << (int)ram_bank_value << std::endl;
            return;
        } 
        else if (value <= 0x0C)
        {  // accessing RTC registers
            rtc_mode = true;
            rtc_curr_reg = value;
        }
        else
        {
            std::cerr << "Invalid RAM/RTC register value: " << (int)value << std::endl;
        }
    }

    //Latch clock data register
    if ((address & 0xE000) == 0x6000) // 6000-7FFF
    {
        if (rtc)
        {
            if (cart_need_save())
            {
                cart_battery_save();
            }
            
            rtc_latch_clock.RTC_S = rtc_real_clock.RTC_S;
            rtc_latch_clock.RTC_M = rtc_real_clock.RTC_M;
            rtc_latch_clock.RTC_H = rtc_real_clock.RTC_H;
            rtc_latch_clock.RTC_DL = rtc_real_clock.RTC_DL;
            rtc_latch_clock.RTC_DH = rtc_real_clock.RTC_DH;
        }
        return;
    }

    if ((address & 0xE000) == 0xA000) // A000-BFFF
    {
        if (!ram_enabled)
        {
            return;
        }

        if (!rtc_mode)
        {
            if (!ram_bank)
            {
                return;
            }

            ram_bank[address - 0xA000] = value;

            if (battery)
            {
                need_save = true;
            }
        } else {
            switch (rtc_curr_reg) {
                case 0x08:
                { 
                    value &= 0x3F;
                    rtc_real_clock.RTC_S = value; 
                    rtc_latch_clock.RTC_S = value; 

                } break;

                case 0x09: 
                {
                    value &= 0x3F;
                    rtc_real_clock.RTC_M = value;
                    rtc_latch_clock.RTC_M = value;

                } break;
                case 0x0A: 
                {
                    value &= 0x1F;
                    rtc_real_clock.RTC_H = value;
                    rtc_latch_clock.RTC_H = value;
                } break;
                case 0x0B: rtc_real_clock.RTC_DL = value; rtc_latch_clock.RTC_DL = value; break;
                case 0x0C:
                {
                    value &= 0b11000001;
                    //Halt flag being set
                    if ((value & (1 << 6)) && !rtc_stopped)
                    {
                        rtc_stop_clock();
                    } 
                    //Halt flag being cleared
                    else if (!(value & (1 << 6)) && rtc_stopped) 
                    {
                        rtc_restart_clock();
                    }
                    rtc_real_clock.RTC_DH = value;
                    rtc_latch_clock.RTC_DH = value;
                } break;
            }

        }
    }
}

uint64_t iCartMBC3::rtc_get_current_ms()
{
    //const std::chrono::time_point<std::chrono::system_clock> current = std::chrono::system_clock::now();
    //uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(current.time_since_epoch()).count();
    //uint64_t ms = SDL_GetTicks();
    uint64_t ms = this->c->b->bus_get_sys_ticks();
    //uint64_t ms = this->c->b->bus_get_emu_ticks();
    //uint64_t ms = this->c->b->bus_get_ticks_ns();
    return ms;
}

void iCartMBC3::rtc_stop_clock() 
{
    rtc_stopped = true;
    rtc_last_value = rtc_get_current_ms();
}

void iCartMBC3::rtc_restart_clock()
{
    rtc_last_value = rtc_get_current_ms();
    rtc_stopped = false;
}

void iCartMBC3::cart_tick()
{
    if (rtc_stopped)
    {
        return;
    }

    uint64_t currentTime = rtc_get_current_ms();
    uint64_t oldTime = rtc_last_value;
  
    //if ((currentTime - oldTime) >= (128.0 * 1000.0 * 25.087))
    //if ((currentTime - oldTime) >= (3211136))
    //if ((currentTime - oldTime) >= (4194304))
    if ((currentTime - oldTime) >= (1000))
        {
        //std::cout << "Cart Tick: " << currentTime << " oldTime: " << rtc_last_value;
        //std::cout << " s: " << (int)rtc_real_clock.RTC_S << " m: " << (int)rtc_real_clock.RTC_M;
        //std::cout << " h: " << (int)rtc_real_clock.RTC_H << " dl: " << (int)rtc_real_clock.RTC_DL;
        //std::cout << " dh: " << (int)rtc_real_clock.RTC_DH << std::endl;

        uint8_t s = rtc_real_clock.RTC_S + 1;

        if (s < 60)
        {
            rtc_real_clock.RTC_S = s;
            rtc_last_value = currentTime;
            return;
        }

        rtc_real_clock.RTC_S = s % 60;

        uint8_t m = rtc_real_clock.RTC_M + 1;
        //std::cout << "Cart Tick: " << currentTime << " Old s" << (int)s << "New s : " << (int)rtc_real_clock.RTC_S << " New m: " << (int)m << std::endl;

        if (m < 60)
        {
            rtc_real_clock.RTC_M = m;
            rtc_last_value = currentTime;
            return;
        }

        rtc_real_clock.RTC_M = m % 60;;
        //std::cout << "Cart Tick: " << currentTime << " New Minute: " << (int)m << std::endl;

        uint8_t h = rtc_real_clock.RTC_H + 1;
        if (h < 24)
        {
            rtc_real_clock.RTC_H = h;
            rtc_last_value = currentTime;
            return;
        }
        rtc_real_clock.RTC_H = h % 24;

        uint8_t dlo = rtc_real_clock.RTC_DL;
        uint8_t dhi = rtc_real_clock.RTC_DH & 0b1;
        uint16_t d = ((dhi << 8) | dlo) + 1;

        setDay(d);
        rtc_last_value = currentTime;
    }

}

void iCartMBC3::setDay(uint16_t day)
{
    if (day < 256)
    {
        rtc_real_clock.RTC_DL = day & 0xFF; 
        rtc_real_clock.RTC_DH &= ~(0x1);  // clear the MSb of day
        return;
    }

    if (day < 512)
    {
        rtc_real_clock.RTC_DL = day & 0xFF;
        rtc_real_clock.RTC_DH |= 0x1;  // set the MSb of day
        return;
    }

    if (day == 512)
    {
        setDay(0);
        rtc_real_clock.RTC_DH |= (1 << 7);  // set the MSb of day
        return;
    }
    std::cerr << "Invalid day number" << std::endl;
}