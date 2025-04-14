#include "common.h"
#include "cpu.h"
#include "instruction.h"


cpu::cpu(bus* b, emu* e)
{
    this->regs.PC = 0x100;
    this->regs.A = 0x01;
    this->b = b;
    this->e = e;
}

cpu::~cpu()
{

}

bool cpu::cpu_step() {
    printf("Cpu not yet implemented.\n");
    return false;
}


void cpu::fetch_instruction()
{
    //memory clear
    if (this->cur_inst != 0)
    {
        delete this->cur_inst;
    }
    this->cur_opcode = this->b->bus_read(this->regs.PC);
    
    this->cur_inst =  new instruction(this->cur_opcode);
}


void cpu::fetch_data()
{
    this->mem_dest = 0;
    this->dest_is_mem = false;

    if (this->cur_inst == 0)
    {
        return;
    }
    if (this->cur_inst->a_mode == NULL)
    {
        std::cout << "Unknown Addreessing Mode! " << this->cur_opcode << std::endl;
        return;
    }
    this->cur_inst->a_mode(this, this->cur_inst);
}
void cpu::execute()
{

}


uint16_t cpu::cpu_read_reg(instdata::reg_type rt)
{
    switch (rt)
    {
    case instdata::RT_A: return this->regs.A;
    case instdata::RT_F: return this->regs.Fr;
    case instdata::RT_B: return this->regs.B;
    case instdata::RT_C: return this->regs.C;
    case instdata::RT_D: return this->regs.D;
    case instdata::RT_E: return this->regs.E;
    case instdata::RT_H: return this->regs.H;
    case instdata::RT_L: return this->regs.L;
         
    case instdata::RT_AF: return this->regs.AF;
    case instdata::RT_BC: return this->regs.BC;
    case instdata::RT_DE: return this->regs.DE;
    case instdata::RT_HL: return this->regs.HL;
         
    case instdata::RT_PC: return this->regs.PC;
    case instdata::RT_SP: return this->regs.SP;

    default: return 0;
    }
}

void cpu::fAM_IMP(cpu* c, instruction* inst)
{
    //Nada
}
void cpu::fAM_R(cpu* c, instruction* inst)
{
    c->fetched_data = c->cpu_read_reg(inst->reg_1);
}
void cpu::fAM_R_D8(cpu* c, instruction* inst)
{
    c->fetched_data = c->b->bus_read(c->regs.PC);
    c->e->emu_cycles(1);
    c->regs.PC++;

}
void cpu::fAM_D16(cpu* c, instruction* inst)
{
    uint16_t lo = c->b->bus_read(c->regs.PC);
    c->e->emu_cycles(1);

    uint16_t hi = c->b->bus_read(c->regs.PC + 1);
    c->e->emu_cycles(1);

    c->fetched_data = lo | (hi << 8);

    c->regs.PC += 2;
}