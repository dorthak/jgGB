#include "common.h"
#include "cpu.h"
#include "emu.h"

//Operational functions
void cpu::fIN_NONE()
{
    std::cout << "INVALID INSTRUCTION!" << std::endl;
    //exit(-7);
    //NO_IMPL
}
void cpu::fIN_NOP()
{
    //no action
}
void cpu::fIN_LD()
{
    if (dest_is_mem)
    {
        //LD (BC), A for example

        if (is_16_bit(reg_2))
        {
            e->emu_cycles(1);
            b->bus_write16(mem_dest, fetched_data);
        } else {
            b->bus_write(mem_dest, (uint8_t) fetched_data);
        }

        e->emu_cycles(1);
        return;
    }

    if (mode == instdata::AM_HL_SPR)
    {
        // this is just for LD HL, SP + e8, Opcode F8
        uint8_t hflag = ((cpu_read_reg(reg_2) & 0xF) + (fetched_data & 0xF)) >= 0x10;
        uint8_t cflag = ((cpu_read_reg(reg_2) & 0xFF) + (fetched_data & 0xFF)) >= 0x100;

        cpu_set_flags(0, 0, hflag, cflag);
        cpu_set_reg(reg_1, cpu_read_reg(reg_2) + (char)fetched_data);

        return;
    }

    cpu_set_reg(reg_1, fetched_data);

}
void cpu::fIN_LDH()
{
    if (reg_1 == instdata::RT_A)
    {
        cpu_set_reg(reg_1, b->bus_read(0xFF00 | fetched_data));
    } else {
        b->bus_write(mem_dest, regs.A);
    }
    e->emu_cycles(1);
}
void cpu::fIN_DEC()
{
    uint16_t val = cpu_read_reg(reg_1) - 1;

    if (cpu::is_16_bit(reg_1))
    {
        e->emu_cycles(1);
    }

    if (reg_1 == instdata::RT_HL && mode == instdata::AM_MR)
    {
        val = b->bus_read(cpu_read_reg(instdata::RT_HL)) - 1;
        b->bus_write(cpu_read_reg(instdata::RT_HL), (uint8_t)val);
    } else {
        cpu_set_reg(reg_1, val);
        val = cpu_read_reg(reg_1);
    }

    if ((cur_opcode & 0x0B) == 0x0B)
    {
        return;
    }

    cpu_set_flags(val == 0, 1, (val & 0x0F) == 0x0F, -1);
}

void cpu::fIN_DI()
{
    this->int_master_enabled = false;
}

void cpu::fIN_INC()
{
    uint16_t val = cpu_read_reg(reg_1) + 1;

    if (cpu::is_16_bit(reg_1))
    {
        e->emu_cycles(1);
    }

    if (reg_1 == instdata::RT_HL && mode == instdata::AM_MR)
    {
        val = b->bus_read(cpu_read_reg(instdata::RT_HL)) + 1;
        val &= 0xFF;
        b->bus_write(cpu_read_reg(instdata::RT_HL), (uint8_t)val);
    }
    else {
        cpu_set_reg(reg_1, val);
        val = cpu_read_reg(reg_1);
    }

    if ((cur_opcode & 0x03) == 0x03)
    {
        return;
    }

    cpu_set_flags(val == 0, 0, (val & 0x0F) == 0, -1);

}

void cpu::fIN_POP()
{
    uint16_t lo = s->stack_pop();
    e->emu_cycles(1);
    uint16_t hi = s->stack_pop();
    e->emu_cycles(1);

    uint16_t n = (hi << 8) | lo;

    cpu_set_reg(reg_1, n);

    if (reg_1 == instdata::RT_AF)
    {
        cpu_set_reg(reg_1, n & 0xFFF0);
    }
}
void cpu::fIN_PUSH()
{
    uint16_t hi = (cpu_read_reg(reg_1) >> 8) & 0xFF;
    e->emu_cycles(1);
    s->stack_push((uint8_t) hi);

    uint16_t lo = cpu_read_reg(reg_1) & 0xFF;
    e->emu_cycles(1);
    s->stack_push((uint8_t) lo);

    e->emu_cycles(1);

}


void cpu::goto_addr(uint16_t addr, bool pushpc)
{
    if (check_cond())
    {
        if (pushpc)
        {
            e->emu_cycles(2);
            s->stack_push16(regs.PC);
        }

        regs.PC = addr;
        e->emu_cycles(1);
    }
}

void cpu::fIN_JP()
{
    goto_addr(fetched_data, false);
}

void cpu::fIN_JR()
{
    char rel = (char)(fetched_data & 0xFF);
    uint16_t addr = regs.PC + rel;
    goto_addr(addr, false);
}

void cpu::fIN_CALL()
{
   goto_addr(fetched_data, true);
}

void cpu::fIN_RET()
{
    if (cond != instdata::CT_NONE)
    {
        e->emu_cycles(1);
    }

    if (check_cond())
    {
        uint16_t lo = s->stack_pop();
        e->emu_cycles(1);
        uint16_t hi = s->stack_pop();
        e->emu_cycles(1);
        
        uint16_t n = (hi << 8) | lo;
        regs.PC = n;
        e->emu_cycles(1);
    }
    
}

void cpu::fIN_RST()
{
    goto_addr(param, true);
}

void cpu::fIN_RETI()
{
    int_master_enabled = true;
    fIN_RET();
}

void cpu::fIN_ADD()
{
    uint32_t val = cpu_read_reg(reg_1) + fetched_data;

    bool is_16bit = is_16_bit(reg_1);

    if (is_16bit)
    {
        e->emu_cycles(1);
    }

    if (reg_1 == instdata::RT_SP)
    {
        val = cpu_read_reg(reg_1) + (char)fetched_data;
    }

    int z = (val & 0xFF) == 0;
    int h = (cpu_read_reg(reg_1) & 0xF) + (fetched_data & 0xF) >= 0x10;
    int c = (int)(cpu_read_reg(reg_1) & 0xFF) + (int)(fetched_data & 0xFF) >= 0x100;

    if (is_16bit)
    {
        z = -1;
        h = (cpu_read_reg(reg_1) & 0xFFF) + (fetched_data & 0xFFF) >= 0x1000;
        uint32_t n = ((uint32_t)cpu_read_reg(reg_1)) + ((uint32_t)fetched_data);
        c = n >= 0x10000;
    }

    if (reg_1 == instdata::RT_SP)
    {
        z = 0;
        h = (cpu_read_reg(reg_1) & 0xF) + (fetched_data & 0xF) >= 0x10;
        c = (int)(cpu_read_reg(reg_1) & 0xFF) + (int)(fetched_data & 0xFF) >= 0x100;
    }
    cpu_set_reg(reg_1, val & 0xFFFF);
    cpu_set_flags(z, 0, h, c);
}

void cpu::fIN_ADC()
{
    uint16_t u = fetched_data;
    uint16_t a = regs.A;
    uint16_t c = CPU_FLAG_C;

    uint16_t val = (a + u + c);

    cpu_set_reg(reg_1, val);

    cpu_set_flags(regs.A == 0, 0,
        (a & 0xF) + (u & 0xF) + c > 0xF,
        a + u + c > 0xFF);
}

void cpu::fIN_SUB()
{
    uint16_t val = cpu_read_reg(reg_1) - fetched_data;

    int z = val == 0;
    int h = ((int)cpu_read_reg(reg_1) & 0xF) - ((int)fetched_data & 0xF) < 0;
    int c = ((int)cpu_read_reg(reg_1)) - ((int)fetched_data) < 0;

    cpu_set_reg(reg_1, val);
    
    cpu_set_flags(z, 1, h, c);
}


void cpu::fIN_SBC()
{
    uint16_t val = fetched_data + CPU_FLAG_C;
    uint16_t regVal = cpu_read_reg(reg_1);

    int z = regVal - val == 0;

    int h = ((int)regVal & 0xF)
        - ((int)fetched_data & 0xF) - ((int)CPU_FLAG_C) < 0;
    int c = ((int)regVal)
        - ((int)fetched_data) - ((int)CPU_FLAG_C) < 0;

    cpu_set_reg(reg_1, regVal - val);

    cpu_set_flags(z, 1, h, c);
}

void cpu::fIN_XOR()
{
    regs.A ^= fetched_data & 0xFF;
    cpu_set_flags((regs.A == 0), 0, 0, 0);
}

void cpu::fIN_AND()
{
    regs.A &= fetched_data;
    cpu_set_flags((regs.A == 0), 0, 1, 0);
}

void cpu::fIN_OR()
{
    regs.A |= fetched_data & 0xFF;
    cpu_set_flags((regs.A == 0), 0, 0, 0);
}

void cpu::fIN_CP()
{
    int n = (int)regs.A - (int)fetched_data;

    cpu_set_flags((n == 0), 1, 
        ((int)regs.A & 0x0F) - ((int)fetched_data & 0x0F) < 0, n < 0);
}

void cpu::fIN_CB()
{
    uint8_t op = (uint8_t) fetched_data;
    instdata::reg_type reg = decode_reg(op & 0b111);

    uint8_t bit = (op >> 3) & 0b111;
    uint8_t bit_op = (op >> 6) & 0b11;
    uint8_t reg_val = (uint8_t) cpu_read_reg8(reg);

    e->emu_cycles(1);

    if (reg == instdata::RT_HL)
    {
        e->emu_cycles(2);
    }

    switch (bit_op)
    {
    case 1:
        //BIT
        cpu_set_flags(!(reg_val & (1 << bit)), 0, 1, -1);
        return;
    case 2:
        //RES
        reg_val &= ~(1 << bit);
        cpu_set_reg8(reg, reg_val);
        return;
    case 3:
        //SET
        reg_val |= (1 << bit);
        cpu_set_reg8(reg, reg_val);
        return;
    }

    bool flagC = CPU_FLAG_C;

    switch (bit)
    {
        case 0: 
        {
            //RLC
            bool setC = false;
            uint8_t result = (reg_val << 1) & 0xFF;

            if ((reg_val & (1 << 7)) != 0)
            {
                result |= 1;
                setC = true;
            }
            cpu_set_reg8(reg, result);
            cpu_set_flags(result == 0, 0, 0, setC);
            return;
        }
        case 1:
        {
            //RRC
            uint8_t old = reg_val;
            reg_val >>= 1;
            reg_val |= (old << 7);

            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(!reg_val, 0, 0, old & 1);
            return;
        }
        case 2:
        {
            //RL
            uint8_t old = reg_val;
            reg_val <<= 1;
            reg_val |= (uint8_t)flagC;

            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(!reg_val, 0, 0, !!(old & 0x80));
            return;
        }
        case 3:
        {
            //RR
            uint8_t old = reg_val;
            reg_val >>= 1;
            reg_val |= (flagC << 7);

            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(!reg_val, 0, 0, old & 1);
            return;
        }
        case 4:
        {
            //SLA
            uint8_t old = reg_val;
            reg_val <<= 1;


            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(!reg_val, 0, 0, !!(old & 0x80));
            return;
        }
        case 5:
        {
            //SRA
            uint8_t u = (int8_t)reg_val >> 1;

            cpu_set_reg8(reg, u);
            cpu_set_flags(!u, 0, 0, reg_val & 1);
            return;
        }
        case 6:
        {
            //SWAP
            reg_val = ((reg_val & 0xF0) >> 4) | ((reg_val & 0x0F) << 4);

            cpu_set_reg8(reg, reg_val);
            cpu_set_flags(reg_val == 0, 0, 0, 0);
            return;
        }
        case 7:
        {
            //SRL
            uint8_t u = reg_val >> 1;

            cpu_set_reg8(reg, u);
            cpu_set_flags(!u, 0, 0, reg_val & 1);
            return;
        }
    }

    std::cerr << "ERROR: INVALID CB: " << op << std::endl;
    NO_IMPL
}

void cpu::fIN_RRCA()
{
    uint8_t b = regs.A & 1;
    regs.A >>= 1;
    regs.A |= (b << 7);

    cpu_set_flags(0, 0, 0, b);

}
void cpu::fIN_RLCA()
{
    uint8_t u = regs.A;
    bool c = (u >> 7) & 1;
    u = (u << 1) | (uint8_t) c;
    regs.A = u;
    
    cpu_set_flags(0, 0, 0, c);
}
void cpu::fIN_RRA()
{
    uint8_t carry = CPU_FLAG_C;
    uint8_t new_c = regs.A & 1;

    regs.A >>= 1;
    regs.A |= (carry << 7);

    cpu_set_flags(0, 0, 0, new_c);
}
void cpu::fIN_RLA()
{
    uint8_t u = regs.A;
    uint8_t cf = CPU_FLAG_C;
    uint8_t c = (u >> 7) & 1;

    
    regs.A = (u << 1) | cf;

    cpu_set_flags(0, 0, 0, c);
}

void cpu::fIN_STOP()
{
    std::cerr << "STOPPING!" << std::endl;
    NO_IMPL
}

void cpu::fIN_HALT()
{
    halted = true;
}

void cpu::fIN_DAA()
{
    uint8_t u = 0;
    int cf = 0;

    if (CPU_FLAG_H || (!CPU_FLAG_N && (regs.A & 0xF) > 0x09))
    {
        u = 0x06;
    }
    if (CPU_FLAG_C || (!CPU_FLAG_N && regs.A > 0x99))
    {
        u |= 0x60;
        cf = 1;
    }

    regs.A += CPU_FLAG_N ? -u : u;

    cpu_set_flags(regs.A == 0, -1, 0, cf);
}
void cpu::fIN_CPL()
{
    regs.A = ~regs.A;
    cpu_set_flags(-1, 1, 1, -1);
}
void cpu::fIN_SCF()
{
    cpu_set_flags(-1, 0, 0, 1);
}
void cpu::fIN_CCF()
{
    cpu_set_flags(-1, 0, 0, CPU_FLAG_C ^ 1);
}

void cpu::fIN_EI()
{
    enabling_ime = true;
}