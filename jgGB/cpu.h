#pragma once
#include "common.h"
#include "bus.h"
#include "instdata.h"
#include "emu.h"

//forward declare class to avoid loop

class cpu
{
public:
	cpu(bus *b, emu *e);
	~cpu();
	bool cpu_step();

private:

	bus* b;
	emu* e;

	//current instruction parameters
	instdata::in_type type;
	instdata::addr_mode mode;
	instdata::reg_type reg_1;
	instdata::reg_type reg_2;
	instdata::cond_type cond;
	uint8_t param;

	void (cpu::* a_mode)(void) = NULL;
	void (cpu::*inst)(void) = NULL;




	//void fAM_IMP(cpu* c, instruction* inst);
	//void fAM_R(cpu* c, instruction* inst);
	//void fAM_R_D8(cpu* c, instruction* inst);
	//void fAM_D16(cpu* c, instruction* inst);

	// Address mode functions
	void fAM_IMP();
	void fAM_R();
	void fAM_R_D8();
	void fAM_D16();

	//Operational functions
	void fIN_NONE();
	void fIN_NOP();
	void fIN_DEC();
	void fIN_LD();
	void fIN_JP();
	void fIN_DI();
	void fIN_XOR();



	//registers


	typedef struct
	{
		union
		{
			struct
			{ uint8_t A; uint8_t Fr; };
			uint16_t AF;
		};
		union
		{
			struct
			{
				uint8_t B; uint8_t C;
			};
			uint16_t BC;
		};
		union
		{
			struct
			{
				uint8_t D; uint8_t E;
			};
			uint16_t DE;
		};
		union
		{
			struct
			{
				uint8_t H; uint8_t L;
			};
			uint16_t HL;
		};
		uint16_t SP;
		uint16_t PC;
	} cpu_registers;

	cpu_registers regs;
	uint16_t fetched_data = 0;
	uint16_t mem_dest = 0;
	bool dest_is_mem = false;
	uint8_t cur_opcode = 0;
	//instruction* cur_inst = 0;



	bool halted = false;
	bool stepping = false;
	bool int_master_enabled = false;

	uint16_t cpu_read_reg(instdata::reg_type rt);
	void fetch_instruction();
	void fetch_data();
	void execute();
	void set_flags(char z, char n, char h, char c);
	bool check_cond();


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



};

