#include "stack.h"
#include "cpu.h"

stack::stack(bus* b)
{
	this->b = b;
}

void stack::set_cpu(cpu* c)
{
	this->c = c;
}

void stack::stack_push(uint8_t data)
{
	c->cpu_get_regs()->SP--;
	b->bus_write(c->cpu_get_regs()->SP, data);
}
void stack::stack_push16(uint16_t data)
{
	stack_push((data >> 8) & 0xFF);
	stack_push(data & 0xFF);
}

uint8_t stack::stack_pop()
{
	return b->bus_read(c->cpu_get_regs()->SP++);
}
uint16_t stack::stack_pop16()
{
	uint16_t lo = stack_pop();
	uint16_t hi = stack_pop();

	return (hi << 8) | lo;
}

