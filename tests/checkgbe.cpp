#include "emu.h"
#include "cpu.h"
#include "bus.h"
#include "cart.h"

#include <catch2/catch_test_macros.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>


TEST_CASE("Test Nothing", "[cpu]") {
	emu e = emu();
	bus b = bus();
	cpu cp = cpu(&b, &e);
	//cart crt = cart();

	bool result = cp.cpu_step();
	REQUIRE(result == false);
}

int main(int argc, char** argv)
{
	int result = Catch::Session().run(argc, argv);
	return result;
}
