#pragma once
#include "common.h"
#include <string>
#include <vector>
#include <map>
#include <emu.h>
#include <cpu.h>

#include "simdjson.h"
using namespace simdjson;

class SST {
public:
	SST(std::string filename);
	~SST();

	void SetSSTState(emu* e, int testnum);
	void AdvanceCPU(emu* e);
	bool CheckSSTState(emu* e, int testnum);

	typedef struct
	{
		uint16_t PC;
		uint16_t SP;
		uint8_t A;
		uint8_t B;
		uint8_t C;
		uint8_t D;
		uint8_t E;
		uint8_t F;
		uint8_t H;
		uint8_t L;
		uint8_t IME;
		uint8_t EI;
		std::map<uint16_t, uint8_t> RAM;

	} CPUState;

	typedef struct
	{
		std::string name;
		CPUState initial;
		CPUState final;
	} Test;

	std::vector<Test> tests;

private:
	ondemand::parser parser;
	ondemand::document JTests;
	padded_string json;

};
