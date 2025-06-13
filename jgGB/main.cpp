#include "common.h"
#include "emu.h"


int main(int argc, char** argv)
{
	int result;
	emu e = emu();

	//TODO Temp:
	char* argvt[3];


	int target = 15;  //set here
	switch (target)
	{
		case 0: argvt[1] = (char*)"E:\\prog\\gbasm\\gb_emu_test\\emu-test1.gb"; break;
		case 1: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\01-special.gb"; break;
		case 2: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\02-interrupts.gb"; break;
		case 3: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\03-op sp,hl.gb"; break;
		case 4: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\04-op r,imm.gb"; break;
		case 5: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\05-op rp.gb"; break;
		case 6: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\06-ld r,r.gb"; break;
		case 7: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\07-jr,jp,call,ret,rst.gb"; break;
		case 8: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\08-misc instrs.gb"; break;
		case 9: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\09-op r,r.gb"; break;
		case 10: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\10-bit ops.gb"; break;
		case 11: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\11-op a,(hl).gb"; break;
		case 12: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\tetris.gb"; break;
		case 13: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\DrMario.gb"; break;
		case 14: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\dmg-acid2.gb"; break;
		case 15: argvt[1] = (char*)"E:\\prog\\jgGB\\roms\\zelda.gb"; break;

			
	}


	
	result = e.emu_cart_load(2, argvt);
	// END TODO, uncomment next line to load filename from command line
//	result = e.emu_cart_load(argc, argv);
	if (result != 0)
	{
		return result;
	}
	return e.emu_run();
}