//#include "instruction.h"
//#include "cpu.h"
//
//instruction::instruction(uint8_t opcode)
//{
//	this->type = instdata::IN_NONE;
//	this->mode = instdata::AM_IMP;
//	this->reg_1 = instdata::RT_NONE;
//	this->reg_2 = instdata::RT_NONE;
//	this->cond = instdata::CT_NONE;
//	this->param = 0;
//
//	switch (opcode)
//	{
//	case 0x00:
//		this->type = instdata::IN_NOP; this->a_mode = &cpu::fAM_IMP; break;
//	case 0x05:
//		this->type = instdata::IN_DEC;  this->a_mode = &cpu::fAM_R;this->reg_1 = instdata::RT_B; break;
//	case 0x0E:
//		this->type = instdata::IN_LD; this->a_mode = &cpu::fAM_R_D8; this->reg_1 = instdata::RT_C; break;
//	case 0xAF:
//		this->type = instdata::IN_XOR; this->a_mode = &cpu::fAM_R; this->reg_1 = instdata::RT_A; break;
//	case 0xC3:
//		this->type = instdata::IN_JP; this->a_mode = &cpu::fAM_D16; break;
//	case 0xF3:
//		this->type = instdata::IN_DI; break;
//
//	default:
//		break;
//	}
//}
//
//instruction::~instruction() {}
//
//std::string instruction::inst_name(instdata::in_type t)
//{
//	return instruction::inst_lookup[t];
//}
//
//
