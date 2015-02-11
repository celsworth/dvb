#include <stdio.h>
#include <libdvbv5/dvb-fe.h>

#include "ruby.h"

#include "frontend.h"

extern VALUE m_dvb;
VALUE c_dvb_frontend;

#define GET_WRAPPER(self) \
	frontend_wrapper *wrapper = NULL; \
	Data_Get_Struct(self, frontend_wrapper, wrapper)


VALUE rb_frontend_open(VALUE rb_self)
{
	struct dvb_v5_fe_parms *parms;

	int adapter = NUM2INT(rb_iv_get(rb_self, "@adapter"));
	int frontend = NUM2INT(rb_iv_get(rb_self, "@frontend"));
	unsigned verbose = TYPE(rb_iv_get(rb_self, "@verbose")) == T_TRUE;
	unsigned use_legacy_call = TYPE(rb_iv_get(rb_self, "@use_legacy_call")) == T_TRUE;

	GET_WRAPPER(rb_self);

	parms = dvb_fe_open(adapter, frontend, 10, use_legacy_call);
	if (!parms)
	{
		wrapper->is_open = 0;
		rb_raise(rb_eSystemCallError, "dvb_fe_open() failed");
	}

	wrapper->parms = parms;
	wrapper->is_open = 1;

	return Qtrue;
}

VALUE rb_frontend_close(VALUE rb_self)
{
	GET_WRAPPER(rb_self);

	if (wrapper->is_open)
	{
		dvb_fe_close(wrapper->parms);
		wrapper->parms = NULL;
		wrapper->is_open = 0;
	}

	return Qnil;
}

VALUE rb_frontend_get_stats(VALUE rb_self)
{
	int r;
	GET_WRAPPER(rb_self);

	r = dvb_fe_get_stats(wrapper->parms);
	return r == 0 ? Qtrue : Qfalse;
}

VALUE rb_frontend_status(VALUE rb_self)
{
	int r;
	uint32_t status;
	VALUE rb_return;
	GET_WRAPPER(rb_self);

	r = dvb_fe_get_stats(wrapper->parms);
	if (r != 0)
	{
		return Qnil;
	}

	r = dvb_fe_retrieve_stats(wrapper->parms, DTV_STATUS, &status);
	if (r != 0)
	{
		return Qnil;
	}

	rb_return = rb_hash_new();

	rb_hash_aset(rb_return, ID2SYM(rb_intern("has_signal")),
			status & FE_HAS_SIGNAL ? Qtrue : Qfalse);
	rb_hash_aset(rb_return, ID2SYM(rb_intern("has_carrier")),
			status & FE_HAS_CARRIER ? Qtrue : Qfalse);
	rb_hash_aset(rb_return, ID2SYM(rb_intern("has_viterbi")),
			status & FE_HAS_VITERBI ? Qtrue : Qfalse);
	rb_hash_aset(rb_return, ID2SYM(rb_intern("has_sync")),
			status & FE_HAS_SYNC ? Qtrue : Qfalse);
	rb_hash_aset(rb_return, ID2SYM(rb_intern("has_lock")),
			status & FE_HAS_LOCK ? Qtrue : Qfalse);

	return rb_return;
}

VALUE rb_frontend_signal_strength(VALUE rb_self)
{

}

VALUE rb_frontend_tune(VALUE rb_self, VALUE opts)
{
	VALUE rb_hashkey, rb_hashvalue;
	GET_WRAPPER(rb_self);

	Check_Type(opts, T_HASH);

	rb_hashkey = ID2SYM(rb_intern("frequency"));
	rb_hashvalue = rb_hash_aref(opts, rb_hashkey);
	if (FIXNUM_P(rb_hashvalue))
	{
		uint32_t value = FIX2INT(rb_hashvalue);
		struct dvb_frontend_info *fe_info = &wrapper->parms->info;

		// try to detect MHz being passed and bump it up to kHz
		if (value < fe_info->frequency_min && value * 1000 >= fe_info->frequency_min)
			value *= 1000;

		if (value < fe_info->frequency_min || value > fe_info->frequency_max)
		{
			rb_raise(rb_eArgError, "frequency outside allowable range of %u-%u",
					fe_info->frequency_min,
					fe_info->frequency_max);
		}

		/* set value.. */
		if (dvb_fe_store_parm(wrapper->parms, DTV_FREQUENCY, value) == EINVAL)
		{
			rb_raise(rb_eSystemCallError, "EINVAL from dvb_fe_store_parm()");
		}
	}

	dvb_fe_store_parm(wrapper->parms, DTV_CODE_RATE_HP, FEC_AUTO);
	dvb_fe_store_parm(wrapper->parms, DTV_CODE_RATE_LP, FEC_AUTO);

	if (dvb_fe_set_parms(wrapper->parms) == EINVAL)
	{
		rb_raise(rb_eSystemCallError, "EINVAL from dvb_fe_set_parms()");
	}

	return Qtrue;
}


void rb_frontend_wrapper_free(void *ptr)
{
	frontend_wrapper *wrapper = ptr;

	printf("rb_frontend_wrapper_free called on %p\n", ptr);

	if (wrapper->is_open)
	{
		dvb_fe_close(wrapper->parms);
		wrapper->parms = NULL;
		wrapper->is_open = 0;
	}

}

static VALUE allocate(VALUE klass)
{
	VALUE obj;
	frontend_wrapper *wrapper;

	obj = Data_Make_Struct(klass, frontend_wrapper, 0, rb_frontend_wrapper_free, wrapper);

	wrapper->is_open = 0;

	return obj;
}

void init_frontend()
{
	c_dvb_frontend = rb_define_class_under(m_dvb, "Frontend", rb_cObject);
	rb_define_alloc_func(c_dvb_frontend, allocate);

	rb_define_method(c_dvb_frontend, "open", rb_frontend_open, 0);
	rb_define_method(c_dvb_frontend, "close", rb_frontend_close, 0);
	rb_define_method(c_dvb_frontend, "status", rb_frontend_status, 0);
	rb_define_method(c_dvb_frontend, "tune", rb_frontend_tune, 1);

}
