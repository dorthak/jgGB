#include "common.h"
#include "cpu.h"


void cpu::fAM_IMP()
{
    //Nada
}
void cpu::fAM_R()
{
    this->fetched_data = this->cpu_read_reg(this->reg_1);
}
void cpu::fAM_R_D8()
{
    this->fetched_data = this->b->bus_read(this->regs.PC);
    this->e->emu_cycles(1);
    this->regs.PC++;

}
void cpu::fAM_D16()
{
    uint16_t lo = this->b->bus_read(this->regs.PC);
    this->e->emu_cycles(1);

    uint16_t hi = this->b->bus_read(this->regs.PC + 1);
    this->e->emu_cycles(1);

    this->fetched_data = lo | (hi << 8);

    this->regs.PC += 2;
}
