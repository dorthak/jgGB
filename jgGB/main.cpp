#include "common.h"
#include "emu.h"


int main(int argc, char** argv)
{
	int result;
	emu e = emu();
	result = e.emu_cart_load(argc, argv);
	if (result != 0)
	{
		return result;
	}
	return e.emu_run();
}