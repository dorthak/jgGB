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
    if (!(this->halted))
    {
        uint16_t pc = this->regs.PC;

        fetch_instruction();


        printf("%04X: %-7s (%02X %02X %02X) A: %02X B: %02X C: %02X\n", 
            pc, this->inst_name(this->type).c_str(), this->cur_opcode,
            b->bus_read(pc + 1), b->bus_read(pc + 2), this->regs.A, this->regs.B, this->regs.C);

        if (!(fetch_data()))
        {
            return false;
        }

        if (this->inst == NULL)
        {
            printf("Unknown Instruction! %02X\n", this->cur_opcode);
            return false;
        }
        execute();
        if (!(execute()))
        {
            return false;
        }
    }

    return true;
}


void cpu::fetch_instruction()
{
    
    this->type = instdata::IN_NONE;
	this->mode = instdata::AM_IMP;
	this->reg_1 = instdata::RT_NONE;
	this->reg_2 = instdata::RT_NONE;
	this->cond = instdata::CT_NONE;
	this->param = 0;
    this->a_mode = NULL;
    this->inst = NULL;
   


    this->cur_opcode = this->b->bus_read(this->regs.PC++);

    
    switch (this->cur_opcode)
    {
    case 0x00:
        this->type = instdata::IN_NOP;  this->inst = &cpu::fIN_NOP;     this->mode = instdata::AM_IMP;  this->a_mode = &cpu::fAM_IMP;   break;
    case 0x05:
        this->type = instdata::IN_DEC;  this->inst = &cpu::fIN_DEC;     this->mode = instdata::AM_R;    this->a_mode = &cpu::fAM_R;     this->reg_1 = instdata::RT_B; break;
    case 0x0E:
        this->type = instdata::IN_LD;   this->inst = &cpu::fIN_LD;      this->mode = instdata::AM_R_D8; this->a_mode = &cpu::fAM_R_D8;  this->reg_1 = instdata::RT_C; break;
    case 0xAF:
        this->type = instdata::IN_XOR;  this->inst = &cpu::fIN_XOR;     this->mode = instdata::AM_R;    this->a_mode = &cpu::fAM_R;     this->reg_1 = instdata::RT_A; break;
    case 0xC3:
        this->type = instdata::IN_JP;   this->inst = &cpu::fIN_JP;      this->mode = instdata::AM_D16;  this->a_mode = &cpu::fAM_D16;   break;
    case 0xF3:
        this->type = instdata::IN_DI;   this->inst = &cpu::fIN_DI;      this->mode = instdata::AM_IMP;  this->a_mode = &cpu::fAM_IMP;   break;
    case 0x31:
        this->type = instdata::IN_LD;   this->inst = &cpu::fIN_LD;      this->mode = instdata::AM_D16;  this->a_mode = &cpu::fAM_D16;  this->reg_1 = instdata::RT_SP; break;
    default:
        this->type = instdata::IN_NONE; this->inst = &cpu::fIN_NONE;    break;
    }
}

bool cpu::fetch_data()
{
    this->mem_dest = 0;
    this->dest_is_mem = false;

    if (this->cur_opcode == 0)
    {
        return false;
    }
    if (this->a_mode == NULL)
    {
        std::cout << "Unknown Addreessing Mode! " << this->cur_opcode << std::endl;
        return false;
    }
    (this->*a_mode)();
    return true;
}
bool cpu::execute()
{
    if (this->inst == NULL)
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

void cpu::cpu_set_reg(instdata::reg_type rt, uint16_t val)
{
    switch (rt)
    {
        case instdata::RT_A: this->regs.A = val & 0xFF; break;
        case instdata::RT_F: this->regs.Fr = val & 0xFF; break;
        case instdata::RT_B: this->regs.B = val & 0xFF; break;
        case instdata::RT_C: this->regs.C = val & 0xFF; break;
        case instdata::RT_D: this->regs.D = val & 0xFF; break;
        case instdata::RT_E: this->regs.E = val & 0xFF; break;
        case instdata::RT_H: this->regs.H = val & 0xFF; break;
        case instdata::RT_L: this->regs.L = val & 0xFF; break;
        case instdata::RT_BC: this->regs.BC = val; break;
        case instdata::RT_DE: this->regs.DE = val; break;
        case instdata::RT_HL: this->regs.HL = val; break;
        case instdata::RT_SP: this->regs.SP = val; break;
        case instdata::RT_AF: this->regs.SP = val; break;
        case instdata::RT_PC: this->regs.SP = val; break;
        case instdata::RT_NONE: break;
        default:
            std::cout << "Invalid register for DEC instruction." << std::endl; exit(-9); break;
    }
}

std::string cpu::inst_name(instdata::in_type t)
{
    return cpu::inst_lookup[t];
}


void cpu::set_flags(char z, char n, char h, char c)
{
    if (z != -1)
    {
        BIT_SET(this->regs.Fr, 7, z);
    }
    if (n != -1)
    {
        BIT_SET(this->regs.Fr, 6, n);
    }
    if (h != -1)
    {
        BIT_SET(this->regs.Fr, 5, h);
    }
    if (c != -1)
    {
        BIT_SET(this->regs.Fr, 4, c);
    }
}

bool cpu::check_cond()
{
    bool z = CPU_FLAG_Z;
    bool c = CPU_FLAG_C;

    switch (this->cond)
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