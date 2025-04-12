#include "common.h"
#include "emu.h"


int main(int argc, char** argv)
{
	emu e = emu();
	return e.emu_run(argc, argv);
}