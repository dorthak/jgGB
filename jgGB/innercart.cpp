#include "innercart.h"
#include "cart.h"


innercart::innercart(cart* c, cart::rom_header* header, uint32_t rom_size, uint8_t* rom_data)
{
    std::cout << "Innercart created" << std::endl;
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


bool innercart::cart_need_save()
{
    return false;
}
bool innercart::cart_battery()
{
    return false;
}

void innercart::cart_battery_load()
{
    return;
}
void innercart::cart_battery_save()
{
    return;
}