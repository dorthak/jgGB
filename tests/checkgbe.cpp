#include "emu.h"
#include "cpu.h"
#include "bus.h"
#include "cart.h"
#include "stack.h"

#include <iostream>

#include "SST.h"



#include <catch2/catch_test_macros.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>


TEST_CASE("Test 9E", "[cpu]") {
	SST sst = SST("E:/prog/gbTests/9e.json");
	emu e = emu();
	auto i = GENERATE(range(0, 1000));
	std::cout << "Test row: " << sst.tests[i].name << " i = " << i << std::endl;
	sst.SetSSTState(&e, i);
	sst.AdvanceCPU(&e);
	sst.CheckSSTState(&e, i);

}

TEST_CASE("Test DE", "[cpu]") {
	SST sst = SST("E:/prog/gbTests/de.json");
	emu e = emu();
	auto i = GENERATE(range(0, 1000));
	std::cout << "Test row: " << sst.tests[i].name << " i = " << i << std::endl;
	sst.SetSSTState(&e, i);
	sst.AdvanceCPU(&e);
	sst.CheckSSTState(&e, i);

}

//TEST_CASE("Test Nothing", "[cpu]") {
//	emu e = emu();
//	bus b = bus();
//	stack s = stack(&b);
//	
//	cpu cp = cpu(&b, &e, &s);
//	//cart crt = cart();
//
//	bool result = cp.cpu_step();
//	REQUIRE(result == true);
//}
//
//TEST_CASE("Emulator loads the cart or errors out", "[emu]")
//{
//	emu e = emu();
//
//	char* argv[3];
//	argv[1] = (char *)"";
//
//	
//	REQUIRE(e.emu_cart_load(0, argv) == -1);
//	REQUIRE(e.emu_cart_load(1, argv) == -1);
//	REQUIRE(e.emu_cart_load(2, argv) == -2);
//
//	argv[1] = (char*)"e:/prog/jgGB/roms/dmg-acid2.gb";
//	REQUIRE(e.emu_cart_load(2, argv) == 0);
//	
//}
//
//TEST_CASE("Emulator runs successfully", "[emu]")
//{
//	int result;
//	emu e = emu();
//	char* argv[3];
//	argv[1] = (char*)"e:/prog/jgGB/roms/dmg-acid2.gb";
//	result = e.emu_cart_load(2, argv);
//
//	REQUIRE(result == 0);
//	
//	result = e.emu_run();
//
//	//REQUIRE(result == 0);
//}
//
//TEST_CASE("Cart Loads", "[cart]")
//{
//	cart crt = cart();
//
//	REQUIRE(crt.cart_load((char*)"e:/prog/jgGB/roms/dmg-acid2.gb"));
//
//}
//
//
//TEST_CASE("Bus Reads", "[bus]")
//{
//	bus b = bus();
//
//	REQUIRE(b.bus_read(0x0100) == 0);
//
//	cart crt = cart();
//	b.set_cart(&crt);
//	crt.cart_load((char *)"e:/prog/jgGB/roms/dmg-acid2.gb");
//
//		
//	REQUIRE(b.bus_read(0x0100) == 0x00);
//	REQUIRE(b.bus_read(0x0101) == 0xC3);
//
//	REQUIRE(b.bus_read(0x8100) == 0);
//
//
//
//
//}

int main(int argc, char** argv)
{

	//SST sst = SST();

	int result = Catch::Session().run(argc, argv);
	return result;
}
