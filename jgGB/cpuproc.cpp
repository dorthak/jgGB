#include "common.h"
#include "cpu.h"

//Operational functions
void cpu::fIN_NONE()
{
    std::cout << "INVALID INSTRUCTION!" << std::endl;
    exit(-7);
}
void cpu::fIN_NOP()
{
    //no action
}
void cpu::fIN_LD()
{
    //TODO
}
void cpu::fIN_DEC()
{

}
void cpu::fIN_JP()
{
    if (this->check_cond())
    {
        this->regs.PC = this->fetched_data;
        this->e->emu_cycles(1);
    }
}
void cpu::fIN_DI()
{
    this->int_master_enabled = false;
}
void cpu::fIN_XOR()
{
    this->regs.A ^= this->fetched_data & 0xFF;
    this->set_flags((this - regs.A == 0), 0, 0, 0);
}

