#include <stdio.h>

#include "dvb.h"

#include "ruby.h"
//#include "ruby/io.h"

VALUE m_dvb;

void Init_ext()
{
	/* parent module definition */
	m_dvb = rb_define_module("DVB");

	init_frontend();
	init_demux();
}
