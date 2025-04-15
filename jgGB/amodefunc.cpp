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
void cpu::fAM_R_R()
{
    this->fetched_data = this->cpu_read_reg(this->reg_2);
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
void cpu::fAM_R_D16()
{
    cpu::fAM_D16();  //same data fetch
}
void cpu::fAM_MR_R()
{
    this->fetched_data = this->cpu_read_reg(this->reg_2);
    this->mem_dest = this->cpu_read_reg(this->reg_1);
    this->dest_is_mem = true;
    if (this->reg_1 == instdata::RT_C)
    {
        this->mem_dest |= 0xFF00;
    }
}
void cpu::fAM_R_MR()
{
    uint16_t addr = this->cpu_read_reg(this->reg_2);

    if (this->reg_2 == instdata::RT_C)
    {
        addr |= 0xFF00;
    }
    this->fetched_data = this->b->bus_read(addr);
    this->e->emu_cycles(1);


}
void cpu::fAM_R_HLI()
{ 
    this->fetched_data = this->b->bus_read(this->cpu_read_reg(this->reg_2));
    this->e->emu_cycles(1);
    cpu_set_reg(instdata::RT_HL, this->cpu_read_reg(instdata::RT_HL) + 1);
}
void cpu::fAM_R_HLD()
{ 
    this->fetched_data = this->b->bus_read(this->cpu_read_reg(this->reg_2));
    this->e->emu_cycles(1);
    cpu_set_reg(instdata::RT_HL, this->cpu_read_reg(instdata::RT_HL) - 1);
}
void cpu::fAM_HLI_R()
{
    this->fetched_data = this->cpu_read_reg(this->reg_2);
    this->mem_dest = this->cpu_read_reg(this->reg_1);
    this->dest_is_mem = true;
    cpu_set_reg(instdata::RT_HL, this->cpu_read_reg(instdata::RT_HL) + 1);
}
void cpu::fAM_HLD_R()
{
    this->fetched_data = this->cpu_read_reg(this->reg_2);
    this->mem_dest = this->cpu_read_reg(this->reg_1);
    this->dest_is_mem = true;
    cpu_set_reg(instdata::RT_HL, this->cpu_read_reg(instdata::RT_HL) - 1);
}
void cpu::fAM_R_A8()
{
    this->fetched_data = this->b->bus_read(this->regs.PC);
    this->e->emu_cycles(1);
    this->regs.PC++;
}
void cpu::fAM_A8_R()
{
    this->mem_dest = this->regs.PC | 0xFF00;
    this->dest_is_mem = true;
    this->e->emu_cycles(1);
    this->regs.PC++;
}
void cpu::fAM_HL_SPR()
{
    this->fetched_data = this->b->bus_read(this->regs.PC);
    this->e->emu_cycles(1);
    this->regs.PC++;
}
void cpu::fAM_D8()
{
    this->fetched_data = this->b->bus_read(this->regs.PC);
    this->e->emu_cycles(1);
    this->regs.PC++;

}
void cpu::fAM_D16_R()
{
    uint16_t lo = this->b->bus_read(this->regs.PC);
    this->e->emu_cycles(1);
    uint16_t hi = this->b->bus_read(this->regs.PC+1);
    this->e->emu_cycles(1);

    this->mem_dest = lo | (hi << 8);
    this->dest_is_mem = true;
    this->regs.PC += 2;

    this->fetched_data = this->cpu_read_reg(this->reg_2);

}
void cpu::fAM_A16_R()
{
    cpu::fAM_D16_R(); //same data fetch
}
void cpu::fAM_MR_D8()
{
    this->fetched_data = this->b->bus_read(this->regs.PC);
    this->e->emu_cycles(1);
    this->regs.PC++;
    this->mem_dest = this->cpu_read_reg(this->reg_1);
    this->dest_is_mem = true;

}
void cpu::fAM_MR()
{
    this->mem_dest = this->cpu_read_reg(this->reg_1);
    this->dest_is_mem = true;
    this->fetched_data = this->b->bus_read(this->cpu_read_reg(this->reg_1));
    this->e->emu_cycles(1);
}

void cpu::fAM_R_A16()
{
    uint16_t lo = this->b->bus_read(this->regs.PC);
    this->e->emu_cycles(1);
    uint16_t hi = this->b->bus_read(this->regs.PC + 1);
    this->e->emu_cycles(1);
    
    uint16_t addr = lo | (hi << 8);
           
    this->regs.PC += 2;

    this->fetched_data = this->b->bus_read(addr);
    this->e->emu_cycles(1);

}