#include "dbg.h"
#include "bus.h"

dbg::dbg()
{
	dbg_msg[0] = NULL;
	msg_size = 0;
}
dbg::dbg(bus* b)
{
	this->b = b;
	dbg_msg[0] = NULL;
	msg_size = 0;
}
void dbg::set_bus(bus* b)
{
	this->b = b;
}

void dbg::dbg_update()
{
	if (b->bus_read(0xFF02) == 0x81)
	{
		char c = b->bus_read(0xFF01);

		dbg_msg[msg_size++] = c;

		b->bus_write(0xFF02, 0);
	}
}
void dbg::dbg_print()
{
	if (dbg_msg[0])
	{
		std::cout << "DBG: " << dbg_msg << std::endl;
	}

}
