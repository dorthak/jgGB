#include "common.h"
#include "cpu.h"
#include "instdata.h"


void cpu::fAM_IMP()
{
    //Nada
    std::sprintf(&disassemble_string[0], "%s ", inst_name(type).c_str());
}
void cpu::fAM_R()
{
    fetched_data = cpu_read_reg(reg_1);
    std::sprintf(&disassemble_string[0], "%s %s", inst_name(type).c_str(), rt_string[reg_1].c_str());
}
void cpu::fAM_R_R()
{
    fetched_data = cpu_read_reg(reg_2);
    std::sprintf(&disassemble_string[0], "%s %s, %s", inst_name(type).c_str(), rt_string[reg_1].c_str(), rt_string[reg_2].c_str());
}
void cpu::fAM_R_D8()
{
    fetched_data = b->bus_read(regs.PC);
    e->emu_cycles(1);
    regs.PC++;
    std::sprintf(&disassemble_string[0], "%s %s, $%02X", inst_name(type).c_str(), rt_string[reg_1].c_str(), fetched_data & 0xFF);
}
void cpu::fAM_D16()
{
    uint16_t lo = b->bus_read(regs.PC);
    e->emu_cycles(1);

    uint16_t hi = b->bus_read(regs.PC + 1);
    e->emu_cycles(1);

    fetched_data = lo | (hi << 8);

    regs.PC += 2;
    std::sprintf(&disassemble_string[0], "%s $%04X", inst_name(type).c_str(), fetched_data);
}
void cpu::fAM_R_D16()
{
    cpu::fAM_D16();  //same data fetch
    std::sprintf(&disassemble_string[0], "%s %s,$%04X", inst_name(type).c_str(), rt_string[reg_1].c_str(), fetched_data);
}
void cpu::fAM_MR_R()
{
    fetched_data = cpu_read_reg(reg_2);
    mem_dest = cpu_read_reg(reg_1);
    dest_is_mem = true;
    if (reg_1 == instdata::RT_C)
    {
        mem_dest |= 0xFF00;
    }
    std::sprintf(&disassemble_string[0], "%s (%s), %s", inst_name(type).c_str(), rt_string[reg_1].c_str(), rt_string[reg_2].c_str());
}
void cpu::fAM_R_MR()
{
    uint16_t addr = cpu_read_reg(reg_2);

    if (reg_2 == instdata::RT_C)
    {
        addr |= 0xFF00;
    }
    fetched_data = b->bus_read(addr);
    e->emu_cycles(1);
    std::sprintf(&disassemble_string[0], "%s %s, (%s)", inst_name(type).c_str(), rt_string[reg_1].c_str(), rt_string[reg_2].c_str());

}
void cpu::fAM_R_HLI()
{ 
    fetched_data = b->bus_read(cpu_read_reg(reg_2));
    e->emu_cycles(1);
    cpu_set_reg(instdata::RT_HL, cpu_read_reg(instdata::RT_HL) + 1);
    std::sprintf(&disassemble_string[0], "%s %s, (%s+)", inst_name(type).c_str(), rt_string[reg_1].c_str(), rt_string[reg_2].c_str());
}
void cpu::fAM_R_HLD()
{ 
    fetched_data = b->bus_read(cpu_read_reg(reg_2));
    e->emu_cycles(1);
    cpu_set_reg(instdata::RT_HL, cpu_read_reg(instdata::RT_HL) - 1);
    std::sprintf(&disassemble_string[0], "%s %s, (%s-)", inst_name(type).c_str(), rt_string[reg_1].c_str(), rt_string[reg_2].c_str());
}
void cpu::fAM_HLI_R()
{
    fetched_data = cpu_read_reg(reg_2);
    mem_dest = cpu_read_reg(reg_1);
    dest_is_mem = true;
    cpu_set_reg(instdata::RT_HL, cpu_read_reg(instdata::RT_HL) + 1);
    std::sprintf(&disassemble_string[0], "%s (%s+), %s", inst_name(type).c_str(), rt_string[reg_1].c_str(), rt_string[reg_2].c_str());
}
void cpu::fAM_HLD_R()
{
    fetched_data = cpu_read_reg(reg_2);
    mem_dest = cpu_read_reg(reg_1);
    dest_is_mem = true;
    cpu_set_reg(instdata::RT_HL, cpu_read_reg(instdata::RT_HL) - 1);
    std::sprintf(&disassemble_string[0], "%s (%s-), %s", inst_name(type).c_str(), rt_string[reg_1].c_str(), rt_string[reg_2].c_str());
}
void cpu::fAM_R_A8()
{
    fetched_data = b->bus_read(regs.PC);
    e->emu_cycles(1);
    regs.PC++;
    std::sprintf(&disassemble_string[0], "%s %s, $%02X", inst_name(type).c_str(), rt_string[reg_1].c_str(), fetched_data & 0xFF);
}
void cpu::fAM_A8_R()
{
    mem_dest = regs.PC | 0xFF00;
    dest_is_mem = true;
    e->emu_cycles(1);
    regs.PC++;
    std::sprintf(&disassemble_string[0], "%s $%02X, %s", inst_name(type).c_str(), b->bus_read(regs.PC - 1), rt_string[reg_2].c_str());
}
void cpu::fAM_HL_SPR()
{
    fetched_data = b->bus_read(regs.PC);
    e->emu_cycles(1);
    regs.PC++;
    std::sprintf(&disassemble_string[0], "%s (%s), SP+%d", inst_name(type).c_str(), rt_string[reg_1].c_str(), fetched_data & 0xFF);
}
void cpu::fAM_D8()
{
    fetched_data = b->bus_read(regs.PC);
    e->emu_cycles(1);
    regs.PC++;
    std::sprintf(&disassemble_string[0], "%s $%02X", inst_name(type).c_str(), fetched_data & 0xFF);
}


void cpu::fAM_D16_R()
{
    uint16_t lo = b->bus_read(regs.PC);
    e->emu_cycles(1);
    uint16_t hi = b->bus_read(regs.PC+1);
    e->emu_cycles(1);

    mem_dest = lo | (hi << 8);
    dest_is_mem = true;
    regs.PC += 2;

    fetched_data = cpu_read_reg(reg_2);
    std::sprintf(&disassemble_string[0], "%s ($%04X), %s", inst_name(type).c_str(), fetched_data, rt_string[reg_2].c_str());
}
void cpu::fAM_A16_R()
{
    uint16_t lo = b->bus_read(regs.PC);
    e->emu_cycles(1);
    uint16_t hi = b->bus_read(regs.PC + 1);
    e->emu_cycles(1);

    mem_dest = lo | (hi << 8);
    dest_is_mem = true;
    regs.PC += 2;

    fetched_data = cpu_read_reg(reg_2);

    std::sprintf(&disassemble_string[0], "%s ($%04X), %s", inst_name(type).c_str(), mem_dest, rt_string[reg_2].c_str());
}
void cpu::fAM_MR_D8()
{
    fetched_data = b->bus_read(regs.PC);
    e->emu_cycles(1);
    regs.PC++;
    mem_dest = cpu_read_reg(reg_1);
    dest_is_mem = true;
    std::sprintf(&disassemble_string[0], "%s (%s), $%02X", inst_name(type).c_str(), rt_string[reg_1].c_str(), fetched_data & 0xFF);
}
void cpu::fAM_MR()
{
    mem_dest = cpu_read_reg(reg_1);
    dest_is_mem = true;
    fetched_data = b->bus_read(cpu_read_reg(reg_1));
    e->emu_cycles(1);
    std::sprintf(&disassemble_string[0], "%s (%s)", inst_name(type).c_str(), rt_string[reg_1].c_str());
}

void cpu::fAM_R_A16()
{
    uint16_t lo = b->bus_read(regs.PC);
    e->emu_cycles(1);
    uint16_t hi = b->bus_read(regs.PC + 1);
    e->emu_cycles(1);
    
    uint16_t addr = lo | (hi << 8);
           
    regs.PC += 2;

    fetched_data = b->bus_read(addr);
    e->emu_cycles(1);
    std::sprintf(&disassemble_string[0], "%s %s,$%04X", inst_name(type).c_str(), rt_string[reg_1].c_str(), fetched_data);
}