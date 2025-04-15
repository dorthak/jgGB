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

        fetch_data();

        if (this->inst == NULL)
        {
            printf("Unknown Instruction! %02X\n", this->cur_opcode);
            exit(-7);
        }
        execute();
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

void cpu::fetch_data()
{
    this->mem_dest = 0;
    this->dest_is_mem = false;

    if (this->cur_opcode == 0)
    {
        return;
    }
    if (this->a_mode == NULL)
    {
        std::cout << "Unknown Addreessing Mode! " << this->cur_opcode << std::endl;
        return;
    }
    (this->*a_mode)();
}
void cpu::execute()
{
    if (this->inst == NULL)
    {
        NO_IMPL
    }
    (this->*inst)();

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