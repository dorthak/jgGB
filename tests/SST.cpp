#include "SST.h"
#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <catch.hpp>


SST::SST(std::string filename)
{
	
	this->json = padded_string::load(filename);
	this->JTests = parser.iterate(json);  //simdjson::ondemand::document

	auto root_array = JTests.get_array();

	for (ondemand::object node : root_array)
	{
		Test* test = new(Test);

		test->name = std::string_view(node["name"]);
				
		ondemand::object init = node["initial"];
		try
		{
			test->initial.PC = (uint16_t)init["pc"].get_uint64();
			test->initial.SP = (uint16_t)init["sp"].get_uint64();
			test->initial.A = (uint8_t)init["a"].get_uint64();
			test->initial.B = (uint8_t)init["b"].get_uint64();
			test->initial.C = (uint8_t)init["c"].get_uint64();
			test->initial.D = (uint8_t)init["d"].get_uint64();
			test->initial.E = (uint8_t)init["e"].get_uint64();
			test->initial.F = (uint8_t)init["f"].get_uint64();
			test->initial.H = (uint8_t)init["h"].get_uint64();
			test->initial.L = (uint8_t)init["l"].get_uint64();
			test->initial.IME = (uint8_t)init["ime"].get_uint64();
			test->initial.EI = (uint8_t)init["ie"].get_uint64();
			
			//ondemand::object initRAM = init["ram"];
			auto mem_array = init["ram"];
			for (auto mem : mem_array)
			{
				uint64_t a[2] = {0, 0};
				int i = 0;
				
				// could not figure out a better way to extract json in the format [[addr, val],[addr,val]]
				// Key/value pairs don't work, so doing this weird double iteration
				for (auto item : mem)
				{
					a[i] = item.get_uint64();
					i++;
				}
				uint16_t addr = (uint16_t) a[0];
				uint8_t data = (uint8_t) a[1];

				test->initial.RAM.insert({ addr, data });
			}
		}
		catch (simdjson_error& e)
		{
			std::cout << e.error() << std::endl;
		}

		ondemand::object final = node["final"];
		try
		{
			test->final.PC = (uint16_t)final["pc"].get_uint64();
			test->final.SP = (uint16_t)final["sp"].get_uint64();
			test->final.A = (uint8_t)final["a"].get_uint64();
			test->final.B = (uint8_t)final["b"].get_uint64();
			test->final.C = (uint8_t)final["c"].get_uint64();
			test->final.D = (uint8_t)final["d"].get_uint64();
			test->final.E = (uint8_t)final["e"].get_uint64();
			test->final.F = (uint8_t)final["f"].get_uint64();
			test->final.H = (uint8_t)final["h"].get_uint64();
			test->final.L = (uint8_t)final["l"].get_uint64();
			test->final.IME = (uint8_t)final["ime"].get_uint64();
			

			//ondemand::object initRAM = final["ram"];
			auto mem_array = final["ram"];
			for (auto mem : mem_array)
			{
				uint64_t a[2] = { 0, 0 };
				int i = 0;

				// could not figure out a better way to extract json in the format [[addr, val],[addr,val]]
				// Key/value pairs don't work, so doing this weird double iteration
				for (auto item : mem)
				{
					a[i] = item.get_uint64();
					i++;
				}
				uint16_t addr = (uint16_t)a[0];
				uint8_t data = (uint8_t)a[1];

				test->final.RAM.insert({ addr, data });
			}
		}
		catch (simdjson_error& e)
		{
			std::cout << e.error() << std::endl;
		}

		this->tests.push_back(*test);


	}


}

SST::~SST()
{
	for (int i = 0; i < tests.size(); i++)
	{
		//delete tests[i];
	}
}

void SST::SetSSTState(emu* e, int testnum)
{
	e->crt->cartFake(0xBFFF);
	e->b->set_cart(e->crt);

	Test* test = &(this->tests[testnum]);

	e->c->cpu_set_reg(instdata::RT_PC, test->initial.PC);
	e->c->cpu_set_reg(instdata::RT_SP, test->initial.SP);
	e->c->cpu_set_reg(instdata::RT_A, test->initial.A);
	e->c->cpu_set_reg(instdata::RT_B, test->initial.B);
	e->c->cpu_set_reg(instdata::RT_C, test->initial.C);
	e->c->cpu_set_reg(instdata::RT_D, test->initial.D);
	e->c->cpu_set_reg(instdata::RT_E, test->initial.E);
	e->c->cpu_set_reg(instdata::RT_F, test->initial.F);
	e->c->cpu_set_reg(instdata::RT_H, test->initial.H);
	e->c->cpu_set_reg(instdata::RT_L, test->initial.L);
	
	e->c->int_master_enabled = test->initial.IME ? true : false;

	for (auto it = test->initial.RAM.begin(); it != test->initial.RAM.end(); ++it)
	{
		uint16_t addr = it->first;
		uint8_t data = it->second;

		e->b->bus_write(addr, data);

		//if (addr < 0xC000)
		//{
		//	e->crt->rom_data[addr] = data;
		//} else {
		//	e->b->bus_write(addr, data);
		//}
		
	}
	
}

void SST::AdvanceCPU(emu* e)
{
	e->c->cpu_step();

}
bool SST::CheckSSTState(emu* e, int testnum)
{
	Test* test = &(this->tests[testnum]);

	CAPTURE(e->c->regs.PC);
	CHECK(e->c->regs.PC == test->final.PC);
	CHECK(e->c->regs.SP == test->final.SP);
	CHECK(e->c->regs.A == test->final.A);
	CHECK(e->c->regs.B == test->final.B);
	CHECK(e->c->regs.C == test->final.C);
	CHECK(e->c->regs.D == test->final.D);
	CHECK(e->c->regs.E == test->final.E);
	CHECK(e->c->regs.Fr == test->final.F);
	CHECK(e->c->regs.H == test->final.H);
	CHECK(e->c->regs.L == test->final.L);


	for (auto it = test->final.RAM.begin(); it != test->final.RAM.end(); ++it)
	{
		uint16_t addr = it->first;
		uint8_t data = it->second;
		if (addr >= 0xFEA0 && addr <= 0xFF7F)
		{
			std::string str = "Skipping on addres ";
			str += std::to_string(addr);
			SKIP(str);
		}
		CAPTURE(addr);
		CHECK(e->b->bus_read(addr) == data);

		//if (addr < 0xC000)
		//{
		//	e->crt->rom_data[addr] = data;
		//}
		//else {
		//	e->b->bus_write(addr, data);
		//}

	}

	return true;
}