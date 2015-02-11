#ifndef __FRONTEND_H
#define __FRONTEND_H

typedef struct {
	unsigned is_open;
	struct dvb_v5_fe_parms *parms;
} frontend_wrapper;

void init_frontend();


#endif
