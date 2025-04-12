#include "cart.h"

cart::cart()
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
    LIC_CODE[0x96] = "Yonezawa/s’pal";
    LIC_CODE[0x97] = "Kaneko";
    LIC_CODE[0x99] = "Pack in soft";
    LIC_CODE[0xA4] = "Konami (Yu-Gi-Oh!)";

    
    

    
}

std::string cart::cart_lic_name()
{
    if (this->header->new_lic_code <= 0xA4)
    {
        return this->LIC_CODE[this->header->lic_code];
    }

    return "UNKNOWN";
}


std::string cart::cart_type_name()
{
    if (this->header->type <= 0x22)
    {
        return this->ROM_TYPES[this->header->type];
    }
    
    return "UNKNOWN";
}

bool cart::cart_load(char* cartfilename)
{
    snprintf(this->filename, sizeof(this->filename), "%s", cartfilename);

    errno_t err;
    FILE* fp;

    err = fopen_s(&fp, cartfilename, "r");
    
    if (err)
    {
        std::cout << "Failed to open: " << cartfilename << std::endl;
        return false;
    }

    fseek(fp, 0, SEEK_END);
    this->rom_size = ftell(fp);

    rewind(fp);

    this->rom_data = (uint8_t *)malloc(this->rom_size);
    fread(this->rom_data, this->rom_size, 1, fp);
    fclose(fp);

    this->header = (cart::rom_header*) (this->rom_data + 0x100);
    this->header->title[15] = 0;

    printf("Cartridge Loaded:\n");
    printf("\t Title    : %s\n", this->header->title);
    printf("\t Type     : %2.2X (%s)\n", this->header->type, cart_type_name().c_str());
    printf("\t ROM Size : %d KB\n", 32 << this->header->rom_size);
    printf("\t RAM Size : %2.2X\n", this->header->ram_size);
    printf("\t LIC Code : %2.2X (%s)\n", this->header->lic_code, cart_lic_name().c_str());
    printf("\t ROM Vers : %2.2X\n", this->header->version);


    return true;

}
