#include "cart.h"
#include "innercart.h"
#include "iCartMBC1.h"
#include "iCartMBC3.h"
#include "bus.h"

cart::cart(bus* b)
{
    this->b = b;
    init_lic_code();
    rom_data = nullptr;
    filename = new char[1024];
}

cart::~cart()
{
    delete rom_data;
    delete filename;
}

bool cart::cart_load(char* cartfilename)
{
   

    snprintf(filename, 1024, "%s", cartfilename);

    errno_t err;
    FILE* fp;

    err = fopen_s(&fp, cartfilename, "r");

    if (err)
    {
        std::cout << "Failed to open: " << cartfilename << std::endl;
        cartloaded = false;
    }
    else
    {

        fseek(fp, 0, SEEK_END);
        rom_size = ftell(fp);

        rewind(fp);

        if (rom_size < 0x7FFF)
        {
            std::cerr << "Invalid size ROM - must be at least 32kb" << std::endl;
            return false;
        }
        else if (rom_size <= 0x7FFF + 1)
        {
            rom_size = 0x7FFF + 1;
        } 
        else if (rom_size <= 0xFFFF + 1)
        {
            rom_size = 0xFFFF + 1; //64KiB
        }
        else if (rom_size <= 0x1FFFF + 1)
        {
            rom_size = 0x1FFFF + 1; //128KiB
        }
        else if (rom_size <= 0x3FFFF + 1)
        {
            rom_size = 0x3FFFF + 1; //256KiB
        }
        else if (rom_size <= 0x7FFFF + 1)
        {
            rom_size = 0x7FFFF + 1; //512KiB
        }
        else if (rom_size <= 0xFFFFF + 1)
        {
            rom_size = 0xFFFFF + 1; //1MiB
        }
        else if (rom_size <= 0x1FFFFF + 1)
        {
            rom_size = 0x1FFFFF + 1; //2MiB
        }
        else if (rom_size <= 0x3FFFFF + 1)
        {
            rom_size = 0x3FFFFF + 1; //4MiB
        }
        else if (rom_size <= 0x7FFFFF + 1)
        {
            rom_size = 0x7FFFFF + 1; //8MiB
        }



        //rom_data = (uint8_t*)malloc(rom_size);
        rom_data = new uint8_t[rom_size];

        memset(rom_data, 0xFF, rom_size - 1);

        fread(rom_data, rom_size, 1, fp);
        fclose(fp);

        header = (cart::rom_header*)(rom_data + 0x100);
        header->title[15] = 0;

        printf("Cartridge Loaded:\n");
        printf("%s\n", filename);
        printf("\t Title    : %s\n", header->title);
        printf("\t Type     : %2.2X (%s)\n", header->type, cart_type_name().c_str());
        printf("\t ROM Size : %d KB\n", 32 << header->rom_size);
        printf("\t RAM Size : %2.2X\n", header->ram_size);
        printf("\t LIC Code : %2.2X (%s)\n", header->lic_code, cart_lic_name().c_str());
        printf("\t ROM Vers : %2.2X\n", header->version);

        cartloaded = true;


        //select appropriate MBC
        switch (header->type)
        {
            //no MBC
            case 0x00:  
                ic = new innercart(this, header, rom_size, rom_data, filename); break;
            // MBC1
            case 0x01:
            case 0x02:
            case 0x03: ic = new iCartMBC1(this, header, rom_size, rom_data, filename); break;
            // MBC3
            case 0x0F:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13: ic = new iCartMBC3(this, header, rom_size, rom_data, filename); break;

            //all others
            default: cartloaded = false;
        }
    }

    return cartloaded;

}

void cart::cartFake(int size)
{
    rom_size = size;
    if (rom_data == nullptr)
    {
        rom_data = new uint8_t[rom_size];
        cartloaded = true;

    }
}

void cart::init_lic_code()
{

	LIC_CODE[0] = "None";
    LIC_CODE[0x00] = "None";
    LIC_CODE[0x01] = "Nintendo R&D1";
    LIC_CODE[0x08] = "Capcom";
    LIC_CODE[0x13] = "Electronic Arts";
    LIC_CODE[0x18] = "Hudson Soft";
    LIC_CODE[0x19] = "b-ai";
    LIC_CODE[0x20] = "kss";
    LIC_CODE[0x22] = "pow";
    LIC_CODE[0x24] = "PCM Complete";
    LIC_CODE[0x25] = "san-x";
    LIC_CODE[0x28] = "Kemco Japan";
    LIC_CODE[0x29] = "seta";
    LIC_CODE[0x30] = "Viacom";
    LIC_CODE[0x31] = "Nintendo";
    LIC_CODE[0x32] = "Bandai";
    LIC_CODE[0x33] = "Ocean/Acclaim";
    LIC_CODE[0x34] = "Konami";
    LIC_CODE[0x35] = "Hector";
    LIC_CODE[0x37] = "Taito";
    LIC_CODE[0x38] = "Hudson";
    LIC_CODE[0x39] = "Banpresto";
    LIC_CODE[0x41] = "Ubi Soft";
    LIC_CODE[0x42] = "Atlus";
    LIC_CODE[0x44] = "Malibu";
    LIC_CODE[0x46] = "angel";
    LIC_CODE[0x47] = "Bullet-Proof";
    LIC_CODE[0x49] = "irem";
    LIC_CODE[0x50] = "Absolute";
    LIC_CODE[0x51] = "Acclaim";
    LIC_CODE[0x52] = "Activision";
    LIC_CODE[0x53] = "American sammy";
    LIC_CODE[0x54] = "Konami";
    LIC_CODE[0x55] = "Hi tech entertainment";
    LIC_CODE[0x56] = "LJN";
    LIC_CODE[0x57] = "Matchbox";
    LIC_CODE[0x58] = "Mattel";
    LIC_CODE[0x59] = "Milton Bradley";
    LIC_CODE[0x60] = "Titus";
    LIC_CODE[0x61] = "Virgin";
    LIC_CODE[0x64] = "LucasArts";
    LIC_CODE[0x67] = "Ocean";
    LIC_CODE[0x69] = "Electronic Arts";
    LIC_CODE[0x70] = "Infogrames";
    LIC_CODE[0x71] = "Interplay";
    LIC_CODE[0x72] = "Broderbund";
    LIC_CODE[0x73] = "sculptured";
    LIC_CODE[0x75] = "sci";
    LIC_CODE[0x78] = "THQ";
    LIC_CODE[0x79] = "Accolade";
    LIC_CODE[0x80] = "misawa";
    LIC_CODE[0x83] = "lozc";
    LIC_CODE[0x86] = "Tokuma Shoten Intermedia";
    LIC_CODE[0x87] = "Tsukuda Original";
    LIC_CODE[0x91] = "Chunsoft";
    LIC_CODE[0x92] = "Video system";
    LIC_CODE[0x93] = "Ocean/Acclaim";
    LIC_CODE[0x95] = "Varie";
    LIC_CODE[0x96] = "Yonezawa/s�pal";
    LIC_CODE[0x97] = "Kaneko";
    LIC_CODE[0x99] = "Pack in soft";
    LIC_CODE[0xA4] = "Konami (Yu-Gi-Oh!)";
   
}

std::string cart::cart_lic_name()
{
    if (header->new_lic_code <= 0xA4)
    {
        return LIC_CODE[header->lic_code];
    }

    return "UNKNOWN";
}


std::string cart::cart_type_name()
{
    if (header->type <= 0x22)
    {
        return ROM_TYPES[header->type];
    }
    
    return "UNKNOWN";
}

bool cart::cart_loaded()
{
    return cartloaded;
}

uint8_t cart::cart_read(uint16_t address)
{
    return ic->cart_read(address);
}
void cart::cart_write(uint16_t address, uint8_t value)
{
    ic->cart_write(address, value);
    //printf("cart_write(%04X)\n", address);
    //NO_IMPL
    //rom_data[address] = value;
}

bool cart::cart_need_save()
{
    return ic->cart_need_save();
}
bool cart::cart_battery()
{
    return ic->cart_battery();
}

void cart::cart_battery_load()
{
    ic->cart_battery_load();
}
void cart::cart_battery_save()
{
    ic->cart_battery_save();
}

void cart::cart_tick() 
{
    ic->cart_tick();
}