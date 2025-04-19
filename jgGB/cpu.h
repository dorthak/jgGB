#pragma once
#include "common.h"
#include "bus.h"
#include "instdata.h"
#include "emu.h"
#include "stack.h"

#define ILINE(a, b, c, d, e, g) type = instdata::##a ;  inst = &cpu::f##a;     mode = instdata::##b;  a_mode = &cpu::f##b; reg_1 = instdata::##c; reg_2 = instdata::##d;  cond = instdata::##e; param = g; break;

//forward declare class to avoid loop

class cpu
{
public:
	cpu(bus *b, emu *e, stack *s);
	~cpu();
	bool cpu_step();

	uint8_t cpu_get_ie_register();
	void cpu_set_ie_register(uint8_t n);

	typedef struct
	{
		union
		{
			struct
			{
				uint8_t Fr; uint8_t A;
			};
			uint16_t AF;
		};
		union
		{
			struct
			{
				uint8_t C; uint8_t B;
			};
			uint16_t BC;
		};
		union
		{
			struct
			{
				uint8_t E; uint8_t D;
			};
			uint16_t DE;
		};
		union
		{
			struct
			{
				uint8_t L; uint8_t H;
			};
			uint16_t HL;
		};
		uint16_t SP;
		uint16_t PC;
	} cpu_registers;

	cpu_registers* cpu_get_regs()
	{
		return &regs;
	}

private:

	bus* b;
	emu* e;
	stack* s;

	//current instruction parameters
	uint16_t fetched_data = 0;
	uint16_t mem_dest = 0;
	bool dest_is_mem = false;
	uint8_t cur_opcode = 0;
	instdata::in_type type;
	instdata::addr_mode mode;
	instdata::reg_type reg_1;
	instdata::reg_type reg_2;
	instdata::cond_type cond;
	uint8_t param;

	void (cpu::* a_mode)(void) = NULL;
	void (cpu::*inst)(void) = NULL;


	// Address mode functions
	void fAM_IMP();
	void fAM_R();
	void fAM_R_D8();
	void fAM_D16();
	
	void fAM_R_D16();
	void fAM_R_R();
	void fAM_MR_R();
	void fAM_R_MR();
	void fAM_R_HLI();
	void fAM_R_HLD();
	void fAM_HLI_R();
	void fAM_HLD_R();
	void fAM_R_A8();
	void fAM_A8_R();
	void fAM_HL_SPR();
	void fAM_D8();
	void fAM_D16_R();
	void fAM_MR_D8();
	void fAM_MR();
	void fAM_A16_R();
	void fAM_R_A16();


	//Operational functions
	void fIN_NONE();
	void fIN_NOP();
	void fIN_LD();
	void fIN_LDH();
	void fIN_JP();
	void fIN_DI();
	void fIN_POP();
	void fIN_PUSH();
	void fIN_JR();
	void fIN_CALL();
	void fIN_RET();
	void fIN_RST();
	void fIN_DEC();
	void fIN_INC();
	void fIN_ADD();

	void fIN_RETI();
	void fIN_XOR();


	//registers




	cpu_registers regs;
	//instruction* cur_inst = 0;

	uint8_t ie_register;

	bool halted = false;
	bool stepping = false;
	bool int_master_enabled = false;

	uint16_t cpu_read_reg(instdata::reg_type rt);
	void cpu_set_reg(instdata::reg_type rt, uint16_t val);

	void fetch_instruction();
	bool fetch_data();
	bool execute();
	void cpu_set_flags(char z, char n, char h, char c);
	bool check_cond();

	void goto_addr(uint16_t addr, bool pushpc);

	#define CPU_FLAG_Z BIT(this->regs.Fr, 7)
	#define CPU_FLAG_C BIT(this->regs.Fr, 4)


	const std::string inst_lookup[49] = 
    {
        "<NONE>",
        "NOP",
        "LD",
        "INC",
        "DEC",
        "RLCA",
        "ADD",
        "RRCA",
        "STOP",
        "RLA",
        "JR",
        "RRA",
        "DAA",
        "CPL",
        "SCF",
        "CCF",
        "HALT",
        "ADC",
        "SUB",
        "SBC",
        "AND",
        "XOR",
        "OR",
        "CP",
        "POP",
        "JP",
        "PUSH",
        "RET",
        "CB",
        "CALL",
        "RETI",
        "LDH",
        "JPHL",
        "DI",
        "EI",
        "RST",
        "IN_ERR",
        "IN_RLC",
        "IN_RRC",
        "IN_RL",
        "IN_RR",
        "IN_SLA",
        "IN_SRA",
        "IN_SWAP",
        "IN_SRL",
        "IN_BIT",
        "IN_RES",
        "IN_SET",
    };

    std::string inst_name(instdata::in_type t);

	static bool is_16_bit(instdata::reg_type rt);


};

