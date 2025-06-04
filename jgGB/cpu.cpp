#include "common.h"
#include "cpu.h"
#include "emu.h"


cpu::cpu(bus* b, emu* e, stack* s)
{
    this->regs.PC = 0x100;
    this->regs.AF = 0x01B0;
    this->regs.BC = 0x0013;
    this->regs.DE = 0x00D8;
    this->regs.AF = 0x01B0;
    this->regs.SP = 0xFFFE;
    this->regs.HL = 0x014D;
    this->b = b;
    this->e = e;
    this->s = s;

    disassemble_string = "                "; //reserve 16 spaces
}

cpu::~cpu()
{

}

bool cpu::cpu_step() {
    if (!(halted))
    {
        uint16_t pc = regs.PC;
        
        disassemble_string = "                ";
        
        fetch_instruction();
        e->emu_cycles(1);

        if (!(fetch_data()))
        {
            return false;
        }

        char flags[16];
        sprintf_s(flags, "%c%c%c%c",
            regs.Fr & (1 << 7) ? 'Z' : '-',
            regs.Fr & (1 << 6) ? 'N' : '-',
            regs.Fr & (1 << 5) ? 'H' : '-',
            regs.Fr & (1 << 4) ? 'C' : '-'
        );


        //printf("%08lX - %04X: %-12s (%02X %02X %02X) A: %02X F: %s BC: %02X%02X DE: %02X%02X HL: %02X%02X SP: %04X\n",
        //    (unsigned long)e->get_ticks(),
        //    pc, disassemble_string.c_str(), cur_opcode,
        //    b->bus_read(pc + 1), b->bus_read(pc + 2), regs.A, flags, regs.B, regs.C,
        //    regs.D, regs.E, regs.H, regs.L, regs.SP);

        if (inst == NULL)
        {
            printf("Unknown Instruction! %02X\n", (uint8_t)cur_opcode);
            return false;
        }

        b->bus_dbg_update();
        b->bus_dbg_print();


        if (!(execute()))
        {
            return false;
        }

    } else {
        //is halted
        e->emu_cycles(1);

        if (int_flags)
        {
            halted = false;
        }
    }

    if (int_master_enabled)
    {
        cpu_handle_interrupts();
        enabling_ime = false;
    }

    if (enabling_ime)
    {
        int_master_enabled = true;
    }

    return true;
}


void cpu::fetch_instruction()
{
    
    type = instdata::IN_NONE;
	mode = instdata::AM_IMP;
	reg_1 = instdata::RT_NONE;
	reg_2 = instdata::RT_NONE;
	cond = instdata::CT_NONE;
	param = 0;
    a_mode = NULL;
    inst = NULL;
   


    cur_opcode = b->bus_read(regs.PC++);

    
    //         ILINE(Instruction, Addr Mode, Reg 1, Reg 2, Condition, Paramter)

    switch (cur_opcode)
    {
        //this->type = instdata::IN_NOP;  this->inst = &cpu::fIN_NOP;     this->mode = instdata::AM_IMP;  this->a_mode = &cpu::fAM_IMP;   break;
        //this->type = instdata::IN_DEC;  this->inst = &cpu::fIN_DEC;     this->mode = instdata::AM_R;    this->a_mode = &cpu::fAM_R;     this->reg_1 = instdata::RT_B; break;
        //this->type = instdata::IN_LD;   this->inst = &cpu::fIN_LD;      this->mode = instdata::AM_R_D8; this->a_mode = &cpu::fAM_R_D8;  this->reg_1 = instdata::RT_C; break;
        //this->type = instdata::IN_XOR;  this->inst = &cpu::fIN_XOR;     this->mode = instdata::AM_R;    this->a_mode = &cpu::fAM_R;     this->reg_1 = instdata::RT_A; break;
        //this->type = instdata::IN_JP;   this->inst = &cpu::fIN_JP;      this->mode = instdata::AM_D16;  this->a_mode = &cpu::fAM_D16;   break;
        //this->type = instdata::IN_DI;   this->inst = &cpu::fIN_DI;      this->mode = instdata::AM_IMP;  this->a_mode = &cpu::fAM_IMP;   break;
        //this->type = instdata::IN_LD;   this->inst = &cpu::fIN_LD;      this->mode = instdata::AM_D16;  this->a_mode = &cpu::fAM_D16;  this->reg_1 = instdata::RT_SP; break;
        //this->type = instdata::IN_NONE; this->inst = &cpu::fIN_NONE;    break;
    case 0x00: ILINE(IN_NOP,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)
    case 0x01: ILINE(IN_LD,     AM_R_D16,   RT_BC,      RT_NONE, CT_NONE,       0)
    case 0x02: ILINE(IN_LD,     AM_MR_R,    RT_BC,      RT_A,    CT_NONE,       0)
    case 0x03: ILINE(IN_INC,    AM_R,       RT_BC,      RT_NONE, CT_NONE,       0)
    case 0x04: ILINE(IN_INC,    AM_R,       RT_B,       RT_NONE, CT_NONE,       0)
    case 0x05: ILINE(IN_DEC,    AM_R,       RT_B,       RT_NONE, CT_NONE,       0)
    case 0x06: ILINE(IN_LD,     AM_R_D8,    RT_B,       RT_NONE, CT_NONE,       0)
    case 0x07: ILINE(IN_RLCA,   AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)
    case 0x08: ILINE(IN_LD,     AM_A16_R,   RT_NONE,    RT_SP,   CT_NONE,       0)
    case 0x09: ILINE(IN_ADD,    AM_R_R,     RT_HL,      RT_BC,   CT_NONE,       0)
    case 0x0A: ILINE(IN_LD,     AM_R_MR,    RT_A,       RT_BC,   CT_NONE,       0)
    case 0x0B: ILINE(IN_DEC,    AM_R,       RT_BC,      RT_NONE, CT_NONE,       0)
    case 0x0C: ILINE(IN_INC,    AM_R,       RT_C,       RT_NONE, CT_NONE,       0)
    case 0x0D: ILINE(IN_DEC,    AM_R,       RT_C,       RT_NONE, CT_NONE,       0)
    case 0x0E: ILINE(IN_LD,     AM_R_D8,    RT_C,       RT_NONE, CT_NONE,       0)
    case 0x0F: ILINE(IN_RRCA,   AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)
    
    // 1x
    case 0x10: ILINE(IN_STOP,   AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)
    case 0x11: ILINE(IN_LD,     AM_R_D16,   RT_DE,      RT_NONE, CT_NONE,       0)
    case 0x12: ILINE(IN_LD,     AM_MR_R,    RT_DE,      RT_A,    CT_NONE,       0)
    case 0x13: ILINE(IN_INC,    AM_R,       RT_DE,      RT_NONE, CT_NONE,       0)
    case 0x14: ILINE(IN_INC,    AM_R,       RT_D,       RT_NONE, CT_NONE,       0)
    case 0x15: ILINE(IN_DEC,    AM_R,       RT_D,       RT_NONE, CT_NONE,       0)
    case 0x16: ILINE(IN_LD,     AM_R_D8,    RT_D,       RT_NONE, CT_NONE,       0)
    case 0x17: ILINE(IN_RLA,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)
    case 0x18: ILINE(IN_JR,     AM_D8,      RT_NONE,    RT_NONE, CT_NONE,       0)
    case 0x19: ILINE(IN_ADD,    AM_R_R,     RT_HL,      RT_DE,   CT_NONE,       0)
    case 0x1A: ILINE(IN_LD,     AM_R_MR,    RT_A,       RT_DE,   CT_NONE,       0)
    case 0x1B: ILINE(IN_DEC,    AM_R,       RT_DE,      RT_NONE, CT_NONE,       0)
    case 0x1C: ILINE(IN_INC,    AM_R,       RT_E,       RT_NONE, CT_NONE,       0)
    case 0x1D: ILINE(IN_DEC,    AM_R,       RT_E,       RT_NONE, CT_NONE,       0)
    case 0x1E: ILINE(IN_LD,     AM_R_D8,    RT_E,       RT_NONE, CT_NONE,       0)
    case 0x1F: ILINE(IN_RRA,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)

    // 2x
    case 0x20: ILINE(IN_JR,     AM_D8,      RT_NONE,    RT_NONE, CT_NZ,         0)
    case 0x21: ILINE(IN_LD,     AM_R_D16,   RT_HL,      RT_NONE, CT_NONE,       0)
    case 0x22: ILINE(IN_LD,     AM_HLI_R,   RT_HL,      RT_A,    CT_NONE,       0)
    case 0x23: ILINE(IN_INC,    AM_R,       RT_HL,      RT_NONE, CT_NONE,       0)
    case 0x24: ILINE(IN_INC,    AM_R,       RT_H,       RT_NONE, CT_NONE,       0)
    case 0x25: ILINE(IN_DEC,    AM_R,       RT_H,       RT_NONE, CT_NONE,       0)
    case 0x26: ILINE(IN_LD,     AM_R_D8,    RT_H,       RT_NONE, CT_NONE,       0)
    case 0x27: ILINE(IN_DAA,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)
    case 0x28: ILINE(IN_JR,     AM_D8,      RT_NONE,    RT_NONE, CT_Z,          0)
    case 0x29: ILINE(IN_ADD,    AM_R_R,     RT_HL,      RT_HL,   CT_NONE,       0)
    case 0x2A: ILINE(IN_LD,     AM_R_HLI,   RT_A,       RT_HL,   CT_NONE,       0)
    case 0x2B: ILINE(IN_DEC,    AM_R,       RT_HL,      RT_NONE, CT_NONE,       0)
    case 0x2C: ILINE(IN_INC,    AM_R,       RT_L,       RT_NONE, CT_NONE,       0)
    case 0x2D: ILINE(IN_DEC,    AM_R,       RT_L,       RT_NONE, CT_NONE,       0)
    case 0x2E: ILINE(IN_LD,     AM_R_D8,    RT_L,       RT_NONE, CT_NONE,       0)
    case 0x2F: ILINE(IN_CPL,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)

    // 3x
    case 0x30: ILINE(IN_JR,     AM_D8,      RT_NONE,    RT_NONE, CT_NC,         0)
    case 0x31: ILINE(IN_LD,     AM_R_D16,   RT_SP,      RT_NONE, CT_NONE,       0)
    case 0x32: ILINE(IN_LD,     AM_HLD_R,   RT_HL,      RT_A,    CT_NONE,       0)
    case 0x33: ILINE(IN_INC,    AM_R,       RT_SP,      RT_NONE, CT_NONE,       0)
    case 0x34: ILINE(IN_INC,    AM_MR,      RT_HL,      RT_NONE, CT_NONE,       0)
    case 0x35: ILINE(IN_DEC,    AM_MR,      RT_HL,      RT_NONE, CT_NONE,       0)
    case 0x36: ILINE(IN_LD,     AM_MR_D8,   RT_HL,      RT_NONE, CT_NONE,       0)
    case 0x37: ILINE(IN_SCF,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)
    case 0x38: ILINE(IN_JR,     AM_D8,      RT_NONE,    RT_NONE, CT_C,          0)
    case 0x39: ILINE(IN_ADD,    AM_R_R,     RT_HL,      RT_SP,   CT_NONE,       0)
    case 0x3A: ILINE(IN_LD,     AM_R_HLD,   RT_A,       RT_HL,   CT_NONE,       0)
    case 0x3B: ILINE(IN_DEC,    AM_R,       RT_SP,      RT_NONE, CT_NONE,       0)
    case 0x3C: ILINE(IN_INC,    AM_R,       RT_A,       RT_NONE, CT_NONE,       0)
    case 0x3D: ILINE(IN_DEC,    AM_R,       RT_A,       RT_NONE, CT_NONE,       0)
    case 0x3E: ILINE(IN_LD,     AM_R_D8,    RT_A,       RT_NONE, CT_NONE,       0)
    case 0x3F: ILINE(IN_CCF,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)

    // 4x
    case 0x40: ILINE(IN_LD,     AM_R_R,     RT_B,       RT_B,    CT_NONE,       0)
    case 0x41: ILINE(IN_LD,     AM_R_R,     RT_B,       RT_C,    CT_NONE,       0)
    case 0x42: ILINE(IN_LD,     AM_R_R,     RT_B,       RT_D,    CT_NONE,       0)
    case 0x43: ILINE(IN_LD,     AM_R_R,     RT_B,       RT_E,    CT_NONE,       0)
    case 0x44: ILINE(IN_LD,     AM_R_R,     RT_B,       RT_H,    CT_NONE,       0)
    case 0x45: ILINE(IN_LD,     AM_R_R,     RT_B,       RT_L,    CT_NONE,       0)
    case 0x46: ILINE(IN_LD,     AM_R_MR,    RT_B,       RT_HL,   CT_NONE,       0)
    case 0x47: ILINE(IN_LD,     AM_R_R,     RT_B,       RT_A,    CT_NONE,       0)
    case 0x48: ILINE(IN_LD,     AM_R_R,     RT_C,       RT_B,    CT_NONE,       0)
    case 0x49: ILINE(IN_LD,     AM_R_R,     RT_C,       RT_C,    CT_NONE,       0)
    case 0x4A: ILINE(IN_LD,     AM_R_R,     RT_C,       RT_D,    CT_NONE,       0)
    case 0x4B: ILINE(IN_LD,     AM_R_R,     RT_C,       RT_E,    CT_NONE,       0)
    case 0x4C: ILINE(IN_LD,     AM_R_R,     RT_C,       RT_H,    CT_NONE,       0)
    case 0x4D: ILINE(IN_LD,     AM_R_R,     RT_C,       RT_L,    CT_NONE,       0)
    case 0x4E: ILINE(IN_LD,     AM_R_MR,    RT_C,       RT_HL,   CT_NONE,       0)
    case 0x4F: ILINE(IN_LD,     AM_R_R,     RT_C,       RT_A,    CT_NONE,       0)

    // 5x
    case 0x50: ILINE(IN_LD,     AM_R_R,     RT_D,       RT_B,    CT_NONE,       0)
    case 0x51: ILINE(IN_LD,     AM_R_R,     RT_D,       RT_C,    CT_NONE,       0)
    case 0x52: ILINE(IN_LD,     AM_R_R,     RT_D,       RT_D,    CT_NONE,       0)
    case 0x53: ILINE(IN_LD,     AM_R_R,     RT_D,       RT_E,    CT_NONE,       0)
    case 0x54: ILINE(IN_LD,     AM_R_R,     RT_D,       RT_H,    CT_NONE,       0)
    case 0x55: ILINE(IN_LD,     AM_R_R,     RT_D,       RT_L,    CT_NONE,       0)
    case 0x56: ILINE(IN_LD,     AM_R_MR,    RT_D,       RT_HL,   CT_NONE,       0)
    case 0x57: ILINE(IN_LD,     AM_R_R,     RT_D,       RT_A,    CT_NONE,       0)
    case 0x58: ILINE(IN_LD,     AM_R_R,     RT_E,       RT_B,    CT_NONE,       0)
    case 0x59: ILINE(IN_LD,     AM_R_R,     RT_E,       RT_C,    CT_NONE,       0)
    case 0x5A: ILINE(IN_LD,     AM_R_R,     RT_E,       RT_D,    CT_NONE,       0)
    case 0x5B: ILINE(IN_LD,     AM_R_R,     RT_E,       RT_E,    CT_NONE,       0)
    case 0x5C: ILINE(IN_LD,     AM_R_R,     RT_E,       RT_H,    CT_NONE,       0)
    case 0x5D: ILINE(IN_LD,     AM_R_R,     RT_E,       RT_L,    CT_NONE,       0)
    case 0x5E: ILINE(IN_LD,     AM_R_MR,    RT_E,       RT_HL,   CT_NONE,       0)
    case 0x5F: ILINE(IN_LD,     AM_R_R,     RT_E,       RT_A,    CT_NONE,       0)


    // 6x
    case 0x60: ILINE(IN_LD,     AM_R_R,     RT_H,       RT_B,    CT_NONE,       0)
    case 0x61: ILINE(IN_LD,     AM_R_R,     RT_H,       RT_C,    CT_NONE,       0)
    case 0x62: ILINE(IN_LD,     AM_R_R,     RT_H,       RT_D,    CT_NONE,       0)
    case 0x63: ILINE(IN_LD,     AM_R_R,     RT_H,       RT_E,    CT_NONE,       0)
    case 0x64: ILINE(IN_LD,     AM_R_R,     RT_H,       RT_H,    CT_NONE,       0)
    case 0x65: ILINE(IN_LD,     AM_R_R,     RT_H,       RT_L,    CT_NONE,       0)
    case 0x66: ILINE(IN_LD,     AM_R_MR,    RT_H,       RT_HL,   CT_NONE,       0)
    case 0x67: ILINE(IN_LD,     AM_R_R,     RT_H,       RT_A,    CT_NONE,       0)
    case 0x68: ILINE(IN_LD,     AM_R_R,     RT_L,       RT_B,    CT_NONE,       0)
    case 0x69: ILINE(IN_LD,     AM_R_R,     RT_L,       RT_C,    CT_NONE,       0)
    case 0x6A: ILINE(IN_LD,     AM_R_R,     RT_L,       RT_D,    CT_NONE,       0)
    case 0x6B: ILINE(IN_LD,     AM_R_R,     RT_L,       RT_E,    CT_NONE,       0)
    case 0x6C: ILINE(IN_LD,     AM_R_R,     RT_L,       RT_H,    CT_NONE,       0)
    case 0x6D: ILINE(IN_LD,     AM_R_R,     RT_L,       RT_L,    CT_NONE,       0)
    case 0x6E: ILINE(IN_LD,     AM_R_MR,    RT_L,       RT_HL,   CT_NONE,       0)
    case 0x6F: ILINE(IN_LD,     AM_R_R,     RT_L,       RT_A,    CT_NONE,       0)

    // 7x
    case 0x70: ILINE(IN_LD,     AM_MR_R,    RT_HL,      RT_B,    CT_NONE,       0)
    case 0x71: ILINE(IN_LD,     AM_MR_R,    RT_HL,      RT_C,    CT_NONE,       0)
    case 0x72: ILINE(IN_LD,     AM_MR_R,    RT_HL,      RT_D,    CT_NONE,       0)
    case 0x73: ILINE(IN_LD,     AM_MR_R,    RT_HL,      RT_E,    CT_NONE,       0)
    case 0x74: ILINE(IN_LD,     AM_MR_R,    RT_HL,      RT_H,    CT_NONE,       0)
    case 0x75: ILINE(IN_LD,     AM_MR_R,    RT_HL,      RT_L,    CT_NONE,       0)
    case 0x76: ILINE(IN_HALT,   AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)
    case 0x77: ILINE(IN_LD,     AM_MR_R,    RT_HL,      RT_A,    CT_NONE,       0)
    case 0x78: ILINE(IN_LD,     AM_R_R,     RT_A,       RT_B,    CT_NONE,       0)
    case 0x79: ILINE(IN_LD,     AM_R_R,     RT_A,       RT_C,    CT_NONE,       0)
    case 0x7A: ILINE(IN_LD,     AM_R_R,     RT_A,       RT_D,    CT_NONE,       0)
    case 0x7B: ILINE(IN_LD,     AM_R_R,     RT_A,       RT_E,    CT_NONE,       0)
    case 0x7C: ILINE(IN_LD,     AM_R_R,     RT_A,       RT_H,    CT_NONE,       0)
    case 0x7D: ILINE(IN_LD,     AM_R_R,     RT_A,       RT_L,    CT_NONE,       0)
    case 0x7E: ILINE(IN_LD,     AM_R_MR,    RT_A,       RT_HL,   CT_NONE,       0)
    case 0x7F: ILINE(IN_LD,     AM_R_R,     RT_A,       RT_A,    CT_NONE,       0)

        // 8x
    case 0x80: ILINE(IN_ADD,    AM_R_R,     RT_A,       RT_B,    CT_NONE,       0)
    case 0x81: ILINE(IN_ADD,    AM_R_R,     RT_A,       RT_C,    CT_NONE,       0)
    case 0x82: ILINE(IN_ADD,    AM_R_R,     RT_A,       RT_D,    CT_NONE,       0)
    case 0x83: ILINE(IN_ADD,    AM_R_R,     RT_A,       RT_E,    CT_NONE,       0)
    case 0x84: ILINE(IN_ADD,    AM_R_R,     RT_A,       RT_H,    CT_NONE,       0)
    case 0x85: ILINE(IN_ADD,    AM_R_R,     RT_A,       RT_L,    CT_NONE,       0)
    case 0x86: ILINE(IN_ADD,    AM_R_MR,    RT_A,       RT_HL,   CT_NONE,       0)
    case 0x87: ILINE(IN_ADD,    AM_R_R,     RT_A,       RT_A,    CT_NONE,       0)
    case 0x88: ILINE(IN_ADC,    AM_R_R,     RT_A,       RT_B,    CT_NONE,       0)
    case 0x89: ILINE(IN_ADC,    AM_R_R,     RT_A,       RT_C,    CT_NONE,       0)
    case 0x8A: ILINE(IN_ADC,    AM_R_R,     RT_A,       RT_D,    CT_NONE,       0)
    case 0x8B: ILINE(IN_ADC,    AM_R_R,     RT_A,       RT_E,    CT_NONE,       0)
    case 0x8C: ILINE(IN_ADC,    AM_R_R,     RT_A,       RT_H,    CT_NONE,       0)
    case 0x8D: ILINE(IN_ADC,    AM_R_R,     RT_A,       RT_L,    CT_NONE,       0)
    case 0x8E: ILINE(IN_ADC,    AM_R_MR,    RT_A,       RT_HL,   CT_NONE,       0)
    case 0x8F: ILINE(IN_ADC,    AM_R_R,     RT_A,       RT_A,    CT_NONE,       0)

        // 9x
    case 0x90: ILINE(IN_SUB,    AM_R_R,     RT_A,   RT_B,        CT_NONE,       0)
    case 0x91: ILINE(IN_SUB,    AM_R_R,     RT_A,   RT_C,        CT_NONE,       0)
    case 0x92: ILINE(IN_SUB,    AM_R_R,     RT_A,   RT_D,        CT_NONE,       0)
    case 0x93: ILINE(IN_SUB,    AM_R_R,     RT_A,   RT_E,        CT_NONE,       0)
    case 0x94: ILINE(IN_SUB,    AM_R_R,     RT_A,   RT_H,        CT_NONE,       0)
    case 0x95: ILINE(IN_SUB,    AM_R_R,     RT_A,   RT_L,        CT_NONE,       0)
    case 0x96: ILINE(IN_SUB,    AM_R_MR,    RT_A,   RT_HL,       CT_NONE,       0)
    case 0x97: ILINE(IN_SUB,    AM_R_R,     RT_A,   RT_A,        CT_NONE,       0)
    case 0x98: ILINE(IN_SBC,    AM_R_R,     RT_A,   RT_B,        CT_NONE,       0)
    case 0x99: ILINE(IN_SBC,    AM_R_R,     RT_A,   RT_C,        CT_NONE,       0)
    case 0x9A: ILINE(IN_SBC,    AM_R_R,     RT_A,   RT_D,        CT_NONE,       0)
    case 0x9B: ILINE(IN_SBC,    AM_R_R,     RT_A,   RT_E,        CT_NONE,       0)
    case 0x9C: ILINE(IN_SBC,    AM_R_R,     RT_A,   RT_H,        CT_NONE,       0)
    case 0x9D: ILINE(IN_SBC,    AM_R_R,     RT_A,   RT_L,        CT_NONE,       0)
    case 0x9E: ILINE(IN_SBC,    AM_R_MR,    RT_A,   RT_HL,       CT_NONE,       0)
    case 0x9F: ILINE(IN_SBC,    AM_R_R,     RT_A,   RT_A,        CT_NONE,       0)


        // Ax
    case 0xA0: ILINE(IN_AND,    AM_R_R,     RT_A,   RT_B,        CT_NONE,       0)
    case 0xA1: ILINE(IN_AND,    AM_R_R,     RT_A,   RT_C,        CT_NONE,       0)
    case 0xA2: ILINE(IN_AND,    AM_R_R,     RT_A,   RT_D,        CT_NONE,       0)
    case 0xA3: ILINE(IN_AND,    AM_R_R,     RT_A,   RT_E,        CT_NONE,       0)
    case 0xA4: ILINE(IN_AND,    AM_R_R,     RT_A,   RT_H,        CT_NONE,       0)
    case 0xA5: ILINE(IN_AND,    AM_R_R,     RT_A,   RT_L,        CT_NONE,       0)
    case 0xA6: ILINE(IN_AND,    AM_R_MR,    RT_A,   RT_HL,       CT_NONE,       0)
    case 0xA7: ILINE(IN_AND,    AM_R_R,     RT_A,   RT_A,        CT_NONE,       0)
    case 0xA8: ILINE(IN_XOR,    AM_R_R,     RT_A,   RT_B,        CT_NONE,       0)
    case 0xA9: ILINE(IN_XOR,    AM_R_R,     RT_A,   RT_C,        CT_NONE,       0)
    case 0xAA: ILINE(IN_XOR,    AM_R_R,     RT_A,   RT_D,        CT_NONE,       0)
    case 0xAB: ILINE(IN_XOR,    AM_R_R,     RT_A,   RT_E,        CT_NONE,       0)
    case 0xAC: ILINE(IN_XOR,    AM_R_R,     RT_A,   RT_H,        CT_NONE,       0)
    case 0xAD: ILINE(IN_XOR,    AM_R_R,     RT_A,   RT_L,        CT_NONE,       0)
    case 0xAE: ILINE(IN_XOR,    AM_R_MR,    RT_A,   RT_HL,       CT_NONE,       0)
    case 0xAF: ILINE(IN_XOR,    AM_R_R,     RT_A,   RT_A,        CT_NONE,       0)

        // Ax
    case 0xB0: ILINE(IN_OR,     AM_R_R,     RT_A,       RT_B,    CT_NONE,       0)
    case 0xB1: ILINE(IN_OR,     AM_R_R,     RT_A,       RT_C,    CT_NONE,       0)
    case 0xB2: ILINE(IN_OR,     AM_R_R,     RT_A,       RT_D,    CT_NONE,       0)
    case 0xB3: ILINE(IN_OR,     AM_R_R,     RT_A,       RT_E,    CT_NONE,       0)
    case 0xB4: ILINE(IN_OR,     AM_R_R,     RT_A,       RT_H,    CT_NONE,       0)
    case 0xB5: ILINE(IN_OR,     AM_R_R,     RT_A,       RT_L,    CT_NONE,       0)
    case 0xB6: ILINE(IN_OR,     AM_R_MR,    RT_A,       RT_HL,   CT_NONE,       0)
    case 0xB7: ILINE(IN_OR,     AM_R_R,     RT_A,       RT_A,    CT_NONE,       0)
    case 0xB8: ILINE(IN_CP,     AM_R_R,     RT_A,       RT_B,    CT_NONE,       0)
    case 0xB9: ILINE(IN_CP,     AM_R_R,     RT_A,       RT_C,    CT_NONE,       0)
    case 0xBA: ILINE(IN_CP,     AM_R_R,     RT_A,       RT_D,    CT_NONE,       0)
    case 0xBB: ILINE(IN_CP,     AM_R_R,     RT_A,       RT_E,    CT_NONE,       0)
    case 0xBC: ILINE(IN_CP,     AM_R_R,     RT_A,       RT_H,    CT_NONE,       0)
    case 0xBD: ILINE(IN_CP,     AM_R_R,     RT_A,       RT_L,    CT_NONE,       0)
    case 0xBE: ILINE(IN_CP,     AM_R_MR,    RT_A,       RT_HL,   CT_NONE,       0)
    case 0xBF: ILINE(IN_CP,     AM_R_R,     RT_A,       RT_A,    CT_NONE,       0)


        // Cx
    case 0xC0: ILINE(IN_RET,    AM_IMP,     RT_NONE,    RT_NONE, CT_NZ,         0)
    case 0xC1: ILINE(IN_POP,    AM_R,       RT_BC,      RT_NONE, CT_NONE,       0)
    case 0xC2: ILINE(IN_JP,     AM_D16,     RT_NONE,    RT_NONE, CT_NZ,         0)
    case 0xC3: ILINE(IN_JP,     AM_D16,     RT_NONE,    RT_NONE, CT_NONE,       0)
    case 0xC4: ILINE(IN_CALL,   AM_D16,     RT_NONE,    RT_NONE, CT_NZ,         0)
    case 0xC5: ILINE(IN_PUSH,   AM_R,       RT_BC,      RT_NONE, CT_NONE,       0)
    case 0xC6: ILINE(IN_ADD,    AM_R_D8,    RT_A,       RT_NONE, CT_NONE,       0)
    case 0xC7: ILINE(IN_RST,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,    0x00)
    case 0xC8: ILINE(IN_RET,    AM_IMP,     RT_NONE,    RT_NONE, CT_Z,          0)
    case 0xC9: ILINE(IN_RET,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)
    case 0xCA: ILINE(IN_JP,     AM_D16,     RT_NONE,    RT_NONE, CT_Z,          0)
    case 0xCB: ILINE(IN_CB,     AM_D8,      RT_NONE,    RT_NONE, CT_NONE,       0)
    case 0xCC: ILINE(IN_CALL,   AM_D16,     RT_NONE,    RT_NONE, CT_Z,          0)
    case 0xCD: ILINE(IN_CALL,   AM_D16,     RT_NONE,    RT_NONE, CT_NONE,       0)
    case 0xCE: ILINE(IN_ADC,    AM_R_D8,    RT_A,       RT_NONE, CT_NONE,       0)
    case 0xCF: ILINE(IN_RST,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,    0x08)

        // Dx
    case 0xD0: ILINE(IN_RET,    AM_IMP,     RT_NONE,    RT_NONE, CT_NC,         0)
    case 0xD1: ILINE(IN_POP,    AM_R,       RT_DE,      RT_NONE, CT_NONE,       0)
    case 0xD2: ILINE(IN_JP,     AM_D16,     RT_NONE,    RT_NONE, CT_NC,         0)
    case 0xD3: break;
    case 0xD4: ILINE(IN_CALL,   AM_D16,     RT_NONE,    RT_NONE, CT_NC,         0)
    case 0xD5: ILINE(IN_PUSH,   AM_R,       RT_DE,      RT_NONE, CT_NONE,       0)
    case 0xD6: ILINE(IN_SUB,    AM_R_D8,    RT_A,       RT_NONE, CT_NONE,       0)  
    case 0xD7: ILINE(IN_RST,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,    0x10)
    case 0xD8: ILINE(IN_RET,    AM_IMP,     RT_NONE,    RT_NONE, CT_C,          0)
    case 0xD9: ILINE(IN_RETI,   AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)   
    case 0xDA: ILINE(IN_JP,     AM_D16,     RT_NONE,    RT_NONE, CT_C,          0)
    case 0xDB: break;
    case 0xDC: ILINE(IN_CALL,   AM_D16,     RT_NONE,    RT_NONE, CT_C,          0)
    case 0xDD: break;
    case 0xDE: ILINE(IN_SBC,    AM_R_D8,    RT_A,       RT_NONE, CT_NONE,       0)  
    case 0xDF: ILINE(IN_RST,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,    0x18)

    // Ex
    case 0xE0: ILINE(IN_LDH,    AM_A8_R,    RT_NONE,    RT_A,    CT_NONE,       0)
    case 0xE1: ILINE(IN_POP,    AM_R,       RT_HL,      RT_NONE, CT_NONE,       0)
    case 0xE2: ILINE(IN_LDH,    AM_MR_R,    RT_C,       RT_A,    CT_NONE,       0)
    case 0xE3: break;
    case 0xE4: break;
    case 0xE5: ILINE(IN_PUSH,   AM_R,       RT_HL,      RT_NONE, CT_NONE,       0)
    case 0xE6: ILINE(IN_AND,    AM_R_D8,    RT_A,       RT_NONE, CT_NONE,       0) 
    case 0xE7: ILINE(IN_RST,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,    0x20)
    case 0xE8: ILINE(IN_ADD,    AM_R_D8,    RT_SP,      RT_NONE, CT_NONE,       0)
    case 0xE9: ILINE(IN_JP,     AM_R,       RT_HL,      RT_NONE, CT_NONE,       0)
    case 0xEA: ILINE(IN_LD,     AM_A16_R,   RT_NONE,    RT_A,    CT_NONE,       0)
    case 0xEB: break;
    case 0xEC: break;
    case 0xED: break;
    case 0xEE: ILINE(IN_XOR,    AM_R_D8,    RT_A,       RT_NONE, CT_NONE,       0) 
    case 0xEF: ILINE(IN_RST,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,    0x28)
        
    // Fx
    case 0xF0: ILINE(IN_LDH,    AM_R_A8,    RT_A,       RT_NONE, CT_NONE,       0)
    case 0xF1: ILINE(IN_POP,    AM_R,       RT_AF,      RT_NONE, CT_NONE,       0)
    case 0xF2: ILINE(IN_LD,     AM_R_MR,    RT_A,       RT_C,    CT_NONE,       0)
    case 0xF3: ILINE(IN_DI,     AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)
    case 0xF4: break;
    case 0xF5: ILINE(IN_PUSH,   AM_R,       RT_AF,      RT_NONE, CT_NONE,       0)
    case 0xF6: ILINE(IN_OR,     AM_R_D8,    RT_A,       RT_NONE, CT_NONE,       0)
    case 0xF7: ILINE(IN_RST,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,    0x30)
    case 0xF8: ILINE(IN_LD,     AM_HL_SPR,  RT_HL,      RT_SP,   CT_NONE,       0)
    case 0xF9: ILINE(IN_LD,     AM_R_R,     RT_SP,      RT_HL,   CT_NONE,       0)
    case 0xFA: ILINE(IN_LD,     AM_R_A16,   RT_A,       RT_NONE, CT_NONE,       0)
    case 0xFB: ILINE(IN_EI,     AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)    
    case 0xFC: break;
    case 0xFD: break;
    case 0xFE: ILINE(IN_CP,     AM_R_D8,    RT_A,       RT_NONE, CT_NONE,       0)
    case 0xFF: ILINE(IN_RST,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,    0x38)    

    default:   ILINE(IN_NONE,   AM_IMP,     RT_NONE,    RT_NONE, CT_NONE,       0)
    }
}

bool cpu::fetch_data()
{
    mem_dest = 0;
    dest_is_mem = false;

/*    if (this->cur_opcode == 0)
    {
        return false;
    }
*/
    if (a_mode == NULL)
    {
        std::cout << "Unknown Addreessing Mode! " << cur_opcode << std::endl;
        return false;
    }

    (this->*a_mode)();
    return true;
}
bool cpu::execute()
{
    if (inst == NULL)
    {
        return false;
    }
    (this->*inst)();
    return true;

}

uint16_t cpu::cpu_read_reg(instdata::reg_type rt)
{
    switch (rt)
    {
        case instdata::RT_A: return regs.A;
        case instdata::RT_F: return regs.Fr;
        case instdata::RT_B: return regs.B;
        case instdata::RT_C: return regs.C;
        case instdata::RT_D: return regs.D;
        case instdata::RT_E: return regs.E;
        case instdata::RT_H: return regs.H;
        case instdata::RT_L: return regs.L;

        case instdata::RT_AF: return regs.AF;
        case instdata::RT_BC: return regs.BC;
        case instdata::RT_DE: return regs.DE;
        case instdata::RT_HL: return regs.HL;

        case instdata::RT_PC: return regs.PC;
        case instdata::RT_SP: return regs.SP;

        default: return 0;
    }
}

void cpu::cpu_set_reg(instdata::reg_type rt, uint16_t val)
{
    switch (rt)
    {
        case instdata::RT_A: regs.A = val & 0xFF; break;
        case instdata::RT_F: regs.Fr = val & 0xFF; break;
        case instdata::RT_B: regs.B = val & 0xFF; break;
        case instdata::RT_C: regs.C = val & 0xFF; break;
        case instdata::RT_D: regs.D = val & 0xFF; break;
        case instdata::RT_E: regs.E = val & 0xFF; break;
        case instdata::RT_H: regs.H = val & 0xFF; break;
        case instdata::RT_L: regs.L = val & 0xFF; break;
        case instdata::RT_BC: regs.BC = val; break;
        case instdata::RT_DE: regs.DE = val; break;
        case instdata::RT_HL: regs.HL = val; break;
        case instdata::RT_SP: regs.SP = val; break;
        case instdata::RT_AF: regs.AF = val; break;
        case instdata::RT_PC: regs.PC = val; break;
        case instdata::RT_NONE: break;
        default:
            std::cout << "Invalid register for DEC instruction." << std::endl; exit(-9); break;
    }
}

uint8_t cpu::cpu_read_reg8(instdata::reg_type rt)
{
    switch (rt)
    {
        case instdata::RT_A: return regs.A;
        case instdata::RT_F: return regs.Fr;
        case instdata::RT_B: return regs.B;
        case instdata::RT_C: return regs.C;
        case instdata::RT_D: return regs.D;
        case instdata::RT_E: return regs.E;
        case instdata::RT_H: return regs.H;
        case instdata::RT_L: return regs.L;

        case instdata::RT_HL: return b->bus_read(cpu_read_reg(instdata::RT_HL));


        default: return 0;
    }
}

void cpu::cpu_set_reg8(instdata::reg_type rt, uint8_t val)
{
    switch (rt)
    {
        case instdata::RT_A: regs.A = val & 0xFF; break;
        case instdata::RT_F: regs.Fr = val & 0xFF; break;
        case instdata::RT_B: regs.B = val & 0xFF; break;
        case instdata::RT_C: regs.C = val & 0xFF; break;
        case instdata::RT_D: regs.D = val & 0xFF; break;
        case instdata::RT_E: regs.E = val & 0xFF; break;
        case instdata::RT_H: regs.H = val & 0xFF; break;
        case instdata::RT_L: regs.L = val & 0xFF; break;
        case instdata::RT_HL: b->bus_write(cpu_read_reg(instdata::RT_HL), (uint8_t)val); break;
        case instdata::RT_NONE: break;
        default:
            std::cout << "Invalid register for DEC instruction." << std::endl; exit(-9); break;
    }
}


std::string cpu::inst_name(instdata::in_type t)
{
    return cpu::inst_lookup[t];
}

//void cpu::inst_to_str(char* str)
//{
//    sprintf(str, "%s ", inst_name(type));
//
//    switch (mode)
//    {
//        case instdata::AM_IMP:
//            return;
//        case instdata::AM_R_D16:
//        case instdata::AM_R_A16:
//            sprintf(str, "%s %s, $%04X", inst_name(type), rt_lookup[reg_1], fetched_data);
//            return;
//
//        case instdata::AM_R:
//            sprintf(str, "%s %s", inst_name(type), rt_lookup[reg_1]);
//            return;
//
//    }
//}

void cpu::cpu_set_flags(char z, char n, char h, char c)
{
    if (z != -1)
    {
        BIT_SET(regs.Fr, 7, z);
    }
    if (n != -1)
    {
        BIT_SET(regs.Fr, 6, n);
    }
    if (h != -1)
    {
        BIT_SET(regs.Fr, 5, h);
    }
    if (c != -1)
    {
        BIT_SET(regs.Fr, 4, c);
    }
}

bool cpu::check_cond()
{
    bool z = CPU_FLAG_Z;
    bool c = CPU_FLAG_C;

    switch (cond)
    {
        case instdata::CT_NONE: return true;
        case instdata::CT_C: return c;
        case instdata::CT_NC: return !c;
        case instdata::CT_Z: return z;
        case instdata::CT_NZ: return !z;
        default:
            std::cout << "Invalid Condition!" << std::endl;
            exit(-8);
            break;
    }
}

bool cpu::is_16_bit(instdata::reg_type rt)
{
    return rt >= instdata::RT_AF;
}

uint8_t cpu::cpu_get_ie_register()
{
    return ie_register;
}
void cpu::cpu_set_ie_register(uint8_t n)
{
    ie_register = n;
}


instdata::reg_type cpu::decode_reg(uint8_t reg) const
{
    if (reg > 0b111)
    {
        return instdata::RT_NONE;
    }

    return rt_lookup[reg];
}


bool cpu::int_check(uint16_t address, instdata::interrupt_type it)
{
    if ((int_flags & it) && (ie_register & it))
    {
        int_handle(address);
        int_flags &= ~it;
        halted = false;
        int_master_enabled = false;

        return true;
    }

    return false;
}

void cpu::int_handle(uint16_t address)
{
    s->stack_push16(regs.PC);
    regs.PC = address;
}

void cpu::cpu_request_interrupt(instdata::interrupt_type t)
{
    //TODO
    int_flags |= t;
}

void cpu::cpu_handle_interrupts()
{
    if (int_check(0x40, instdata::IT_VBLANK)) {
        return;
    }
    if (int_check(0x48, instdata::IT_LCD_STAT)) {
        return;
    }
    if (int_check(0x50, instdata::IT_TIMER)) {
        return;
    }
    if (int_check(0x58, instdata::IT_SERIAL)) {
        return;
    }
    if (int_check(0x60, instdata::IT_JOYPAD)) {
        return;
    }

}


uint8_t cpu::cpu_get_int_flags()
{
    return int_flags;
}
void cpu::cpu_set_int_flags(uint8_t value)
{
    int_flags = value;
}