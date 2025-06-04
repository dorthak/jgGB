#pragma once
#include "common.h"
#include "bus.h"
#include "instdata.h"
#include "emu.h"
#include "stack.h"

#define ILINE(a, b, c, d, e, g) type = instdata::##a ;  inst = &cpu::f##a;     mode = instdata::##b;  a_mode = &cpu::f##b; reg_1 = instdata::##c; reg_2 = instdata::##d;  cond = instdata::##e; param = g; break;

//forward declare class to avoid loop

class SST;

class cpu
{
public:
	cpu(bus *b, emu *e, stack *s);
	~cpu();
	bool cpu_step();

	uint8_t cpu_get_ie_register();
	void cpu_set_ie_register(uint8_t n);

	uint8_t cpu_get_int_flags();
	void cpu_set_int_flags(uint8_t value);
	void cpu_request_interrupt(instdata::interrupt_type t);

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

	//connections to other classes
	bus* b;
	emu* e;
	stack* s;

	//CPU State
	cpu_registers regs;
	bool halted = false;
	bool stepping = false;
	bool int_master_enabled = false;
	bool enabling_ime;


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

	std::string disassemble_string;

	void (cpu::* a_mode)(void) = NULL;
	void (cpu::*inst)(void) = NULL;


	//Interrupt data

	uint8_t ie_register;
	uint8_t int_flags;

	void cpu_handle_interrupts();
	bool int_check(uint16_t address, instdata::interrupt_type it);
	void int_handle(uint16_t address);

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
	void fIN_ADC();
	void fIN_SUB();
	void fIN_SBC();
	void fIN_AND();
	void fIN_XOR();
	void fIN_OR();
	void fIN_CP();
	void fIN_CB();
	void fIN_RRCA();
	void fIN_RLCA();
	void fIN_RRA();
	void fIN_RLA();
	void fIN_STOP();
	void fIN_HALT();
	void fIN_DAA();
	void fIN_CPL();
	void fIN_SCF();
	void fIN_CCF();
	void fIN_EI();
	void fIN_RETI();

	uint16_t cpu_read_reg(instdata::reg_type rt);
	void cpu_set_reg(instdata::reg_type rt, uint16_t val);
	uint8_t cpu_read_reg8(instdata::reg_type rt);
	void cpu_set_reg8(instdata::reg_type rt, uint8_t val);

	void fetch_instruction();
	bool fetch_data();
	bool execute();
	void cpu_set_flags(char z, char n, char h, char c);
	bool check_cond();

	void goto_addr(uint16_t addr, bool pushpc);

	#define CPU_FLAG_Z BIT(this->regs.Fr, 7)
	#define CPU_FLAG_N BIT(this->regs.Fr, 6)
	#define CPU_FLAG_H BIT(this->regs.Fr, 5)
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

	const std::string rt_string[15] = {
		"<NONE>",
		"A",
		"F",
		"B",
		"C",
		"D",
		"E",
		"H",
		"L",
		"AF",
		"BC",
		"DE",
		"HL",
		"SP",
		"PC"
	};

    std::string inst_name(instdata::in_type t);
	//void inst_to_str(char* str);

	static bool is_16_bit(instdata::reg_type rt);

	instdata::reg_type decode_reg(uint8_t reg) const;

	instdata::reg_type const rt_lookup[8] = {
		instdata::RT_B,
		instdata::RT_C,
		instdata::RT_D,
		instdata::RT_E,
		instdata::RT_H,
		instdata::RT_L,
		instdata::RT_HL,
		instdata::RT_A
	};

	friend class SST;

};


