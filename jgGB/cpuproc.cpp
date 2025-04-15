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
    if (this->mode == instdata::AM_R)
    {
        switch (this->reg_1)
        {
            case instdata::RT_A: this->regs.A = this->fetched_data - 1; break;
            case instdata::RT_B: this->regs.B = this->fetched_data - 1; break;
            case instdata::RT_C: this->regs.C = this->fetched_data - 1; break;
            case instdata::RT_D: this->regs.D = this->fetched_data - 1; break;
            case instdata::RT_E: this->regs.E = this->fetched_data - 1; break;
            case instdata::RT_H: this->regs.H = this->fetched_data - 1; break;
            case instdata::RT_L: this->regs.L = this->fetched_data - 1; break;
            case instdata::RT_BC: this->regs.BC = this->fetched_data - 1; break;
            case instdata::RT_DE: this->regs.DE = this->fetched_data - 1; break;
            case instdata::RT_HL: this->regs.HL = this->fetched_data - 1; break;
            case instdata::RT_SP: this->regs.SP = this->fetched_data - 1; break;
            default: 
                std::cout << "Invalid register for DEC instruction." << std::endl; exit(-9);

        }
    }
    else 
    {
        NO_IMPL
    }
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

