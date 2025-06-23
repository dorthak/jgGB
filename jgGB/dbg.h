#pragma once
#include "common.h"

class bus;

class dbg
{
public:
	dbg();
	dbg(bus* b);
	void set_bus(bus* b);

	void dbg_update();
	void dbg_print();
	

private:
	bus* b;

	char dbg_msg[1024];
	int msg_size;

};

