#pragma once
#include "common.h"
#include "instdata.h"

class cpu;

class instruction
{
public:

    instruction(uint8_t opcode);
    ~instruction();

    instdata::in_type type;
    instdata::addr_mode mode;
    instdata::reg_type reg_1;
    instdata::reg_type reg_2;
    instdata::cond_type cond;
    uint8_t param;

    void (cpu::*a_mode)(cpu*, instruction*) = NULL;
    

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

