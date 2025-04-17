#include "common.h"
#include "cpu.h"


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
    if (!(halted))
    {
        uint16_t pc = regs.PC;

        fetch_instruction();

        char flags[16];
        sprintf_s(flags, "%c%c%c%c",
            regs.Fr & (1 << 7) ? 'Z' : '-',
            regs.Fr & (1 << 6) ? 'N' : '-',
            regs.Fr & (1 << 5) ? 'H' : '-',
            regs.Fr & (1 << 4) ? 'C' : '-'
        );

        printf("%08lX - %04X: %-7s (%02X %02X %02X) A: %02X F: %s BC: %02X%02X DE: %02X%02X HL: %02X%02X\n",
            (unsigned long)e->get_ticks(),
            pc, inst_name(type).c_str(), cur_opcode,
            b->bus_read(pc + 1), b->bus_read(pc + 2), regs.A, flags, regs.B, regs.C,
            regs.D, regs.E, regs.H, regs.L);

        if (!(fetch_data()))
        {
            return false;
        }

        if (inst == NULL)
        {
            printf("Unknown Instruction! %02X\n", cur_opcode);
            return false;
        }
        if (!(execute()))
        {
            return false;
        }
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
    case 0x00: ILINE(IN_NOP,    AM_IMP,     RT_NONE,    RT_NONE, CT_NONE, 0)
    case 0x01: ILINE(IN_LD,     AM_R_D16,   RT_BC,      RT_NONE, CT_NONE, 0)
    case 0x02: ILINE(IN_LD,     AM_MR_R,    RT_BC,      RT_A,    CT_NONE, 0)

    case 0x03: ILINE(IN_INC,    AM_R,       RT_BC,      RT_NONE, CT_NONE, 0)
    case 0x04: ILINE(IN_INC,    AM_R,       RT_B,       RT_NONE, CT_NONE, 0)

    case 0x05: ILINE(IN_DEC,    AM_R,       RT_B,       RT_NONE, CT_NONE, 0)
    case 0x06: ILINE(IN_LD,     AM_R_D8,    RT_B,       RT_NONE, CT_NONE, 0)

    case 0x0A: ILINE(IN_LD,     AM_R_MR,    RT_A,       RT_BC,   CT_NONE, 0)
    case 0x0B: ILINE(IN_DEC,    AM_R,       RT_BC,      RT_NONE, CT_NONE, 0)

    case 0x0C: ILINE(IN_INC,    AM_R,       RT_C,       RT_NONE, CT_NONE, 0)

    case 0x0D: ILINE(IN_DEC,    AM_R,       RT_C,       RT_NONE, CT_NONE, 0)


    case 0x0E: ILINE(IN_LD,     AM_R_D8,    RT_C,       RT_NONE, CT_NONE, 0)

    // 1x
    case 0x11: ILINE(IN_LD,     AM_R_D16,   RT_DE,      RT_NONE, CT_NONE, 0)
    case 0x12: ILINE(IN_LD,     AM_MR_R,    RT_DE,      RT_A,    CT_NONE, 0)
    case 0x13: ILINE(IN_INC,    AM_R,       RT_DE,      RT_NONE, CT_NONE, 0)
    case 0x14: ILINE(IN_INC,    AM_R,       RT_D,       RT_NONE, CT_NONE, 0)

    case 0x15: ILINE(IN_DEC,    AM_R,       RT_D,       RT_NONE, CT_NONE, 0)
    case 0x16: ILINE(IN_LD,     AM_R_D8,    RT_D,       RT_NONE, CT_NONE, 0)
    case 0x1A: ILINE(IN_LD,     AM_R_MR,    RT_A,       RT_DE,   CT_NONE, 0)
    case 0x1B: ILINE(IN_DEC,    AM_R,       RT_DE,      RT_NONE, CT_NONE, 0)
    case 0x1C: ILINE(IN_INC,    AM_R,       RT_E,       RT_NONE, CT_NONE, 0)

    case 0x1D: ILINE(IN_DEC,    AM_R,       RT_E,       RT_NONE, CT_NONE, 0)

    // 2x
    case 0x21: ILINE(IN_LD,     AM_R_D16,   RT_HL,      RT_NONE, CT_NONE, 0)
    case 0x22: ILINE(IN_LD,     AM_HLI_R,   RT_HL,      RT_A,    CT_NONE, 0)

    case 0x23: ILINE(IN_INC,    AM_R,       RT_HL,      RT_NONE, CT_NONE, 0)
    case 0x24: ILINE(IN_INC,    AM_R,       RT_H,       RT_NONE, CT_NONE, 0)

    case 0x25: ILINE(IN_DEC,    AM_R,       RT_H,       RT_NONE, CT_NONE, 0)
    case 0x26: ILINE(IN_LD,     AM_R_D8,    RT_H,       RT_NONE, CT_NONE, 0)
    case 0x2A: ILINE(IN_LD,     AM_R_HLI,   RT_A,       RT_HL,   CT_NONE, 0)
    case 0x2B: ILINE(IN_DEC,    AM_R,       RT_HL,      RT_NONE, CT_NONE, 0)
    case 0x2C: ILINE(IN_INC,    AM_R,       RT_L,       RT_NONE, CT_NONE, 0)
    case 0x2D: ILINE(IN_DEC,    AM_R,       RT_L,       RT_NONE, CT_NONE, 0)

    // 3x
    case 0x31: ILINE(IN_LD,     AM_R_D16,   RT_SP,      RT_NONE, CT_NONE, 0)
    case 0x32: ILINE(IN_LD,     AM_HLD_R,   RT_HL,      RT_A,    CT_NONE, 0)
    case 0x33: ILINE(IN_INC,    AM_R,       RT_SP,      RT_NONE, CT_NONE, 0)
    case 0x34: ILINE(IN_INC,    AM_MR,      RT_HL,      RT_NONE, CT_NONE, 0)

    case 0x35: ILINE(IN_DEC,    AM_R,       RT_BC,      RT_NONE, CT_NONE, 0)
    case 0x36: ILINE(IN_LD,     AM_MR_D8,   RT_HL,      RT_NONE, CT_NONE, 0)
    case 0x3A: ILINE(IN_LD,     AM_R_HLD,   RT_A,       RT_HL,   CT_NONE, 0)
    case 0x3B: ILINE(IN_DEC,    AM_MR,      RT_HL,      RT_NONE, CT_NONE, 0)
    case 0x3C: ILINE(IN_INC,    AM_R,       RT_A,       RT_NONE, CT_NONE, 0)
    case 0x3D: ILINE(IN_DEC,    AM_R,       RT_A,       RT_NONE, CT_NONE, 0)

    // Ax
    case 0xAF: ILINE(IN_XOR,    AM_R,       RT_A,       RT_NONE, CT_NONE, 0)

    // Cx
    case 0xC3: ILINE(IN_JP,     AM_D16,     RT_NONE,    RT_NONE, CT_NONE, 0)

    // Fx
    case 0xF3: ILINE(IN_DI,     AM_IMP,     RT_NONE,    RT_NONE, CT_NONE, 0)
    default:   ILINE(IN_NONE,   AM_IMP,     RT_NONE,    RT_NONE, CT_NONE, 0)
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
        case instdata::RT_AF: regs.SP = val; break;
        case instdata::RT_PC: regs.SP = val; break;
        case instdata::RT_NONE: break;
        default:
            std::cout << "Invalid register for DEC instruction." << std::endl; exit(-9); break;
    }
}

std::string cpu::inst_name(instdata::in_type t)
{
    return cpu::inst_lookup[t];
}


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