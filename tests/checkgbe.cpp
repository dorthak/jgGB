#include "emu.h"
#include "cpu.h"

#include <catch2/catch_test_macros.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>


TEST_CASE("Test Nothing", "[cpu]") {
	cpu cp = cpu();
	bool b = cp.cpu_step();
	REQUIRE(b == false);
}

int main(int argc, char** argv)
{
	int result = Catch::Session().run(argc, argv);
	return result;
}
