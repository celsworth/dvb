#include <stdio.h>
#include <libdvbv5/dvb-demux.h>

#include "ruby.h"

#include "demux.h"

extern VALUE m_dvb;
VALUE c_dvb_demux;

#define GET_WRAPPER(self) \
	demux_wrapper *wrapper = NULL; \
	Data_Get_Struct(self, demux_wrapper, wrapper)


VALUE rb_demux_open(VALUE rb_self)
{
	int fd, adapter, demux;
	GET_WRAPPER(rb_self);

	adapter = NUM2INT(rb_iv_get(rb_self, "@adapter"));
	demux = NUM2INT(rb_iv_get(rb_self, "@demux"));

	if ((fd = dvb_dmx_open(adapter, demux)) == -1)
	{
		rb_raise(rb_eSystemCallError, "dvb_dmx_open() failed");
	}

	wrapper->fd = fd;

	return Qtrue;
}
VALUE rb_demux_close(VALUE rb_self)
{
	GET_WRAPPER(rb_self);

	if (wrapper->fd)
	{
		dvb_dmx_close(wrapper->fd);
		wrapper->fd = 0;
	}

	return Qnil;
}

VALUE rb_demux_stop(VALUE rb_self)
{
	GET_WRAPPER(rb_self);

	if (wrapper->fd)
		dvb_dmx_stop(wrapper->fd);

	return Qnil;
}

VALUE rb_demux_set_pid_filter(VALUE rb_self, VALUE rb_pid)
{
	int pid, buffersize;
	dmx_pes_type_t type = DMX_PES_OTHER;
	dmx_output_t output = DMX_OUT_TS_TAP;

	GET_WRAPPER(rb_self);

	/* zero buffersize seems to be ignored in dvb_set_pesfilter */
	buffersize = 0;

	Check_Type(rb_pid, T_FIXNUM);
	pid = FIX2INT(rb_pid);

	dvb_set_pesfilter(wrapper->fd, pid, type, output, buffersize);

	return Qtrue;
}

VALUE rb_demux_set_section_filter(VALUE rb_self, VALUE rb_pid)
{
	int pid, flags;
	GET_WRAPPER(rb_self);

	Check_Type(rb_pid, T_FIXNUM);
	pid = FIX2INT(rb_pid);

	flags = DMX_CHECK_CRC | DMX_IMMEDIATE_START;

	dvb_set_section_filter(wrapper->fd, pid, 0, NULL, NULL, NULL, flags);

	return Qtrue;
}

VALUE rb_demux_read(VALUE rb_self)
{
	int r;
	void *buf;
	size_t buflen = 32768;

	GET_WRAPPER(rb_self);

	buf = xmalloc(buflen);

	r = read(wrapper->fd, buf, buflen);

	return rb_str_new(buf, r);
}

// get_pmt_pid


void rb_demux_wrapper_free(void *ptr)
{
	demux_wrapper *wrapper = ptr;

	printf("rb_demux_wrapper_free called on %p\n", ptr);

	if (wrapper->fd)
	{
		dvb_dmx_close(wrapper->fd);
		wrapper->fd = 0;
	}
}

static VALUE allocate(VALUE klass)
{
	VALUE obj;
	demux_wrapper *wrapper;

	obj = Data_Make_Struct(klass, demux_wrapper, 0, rb_demux_wrapper_free, wrapper);

	wrapper->fd = 0;

	return obj;
}
void init_demux()
{
	c_dvb_demux = rb_define_class_under(m_dvb, "Demux", rb_cObject);
	rb_define_alloc_func(c_dvb_demux, allocate);

	rb_define_method(c_dvb_demux, "open", rb_demux_open, 0);
	rb_define_method(c_dvb_demux, "close", rb_demux_close, 0);

	rb_define_method(c_dvb_demux, "stop", rb_demux_stop, 0);

	rb_define_method(c_dvb_demux, "read", rb_demux_read, 0);

	rb_define_method(c_dvb_demux, "set_pid_filter", rb_demux_set_pid_filter, 1);
	rb_define_method(c_dvb_demux, "set_section_filter", rb_demux_set_section_filter,1);

}
