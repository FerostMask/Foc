#ifndef _SCOPE_H
#define _SCOPE_H
#include "stdlib.h"
#include "stdbool.h"
#include "zf_uart.h"
/*------------------------------------------------------*/
/*                       类型定义                       */
/*======================================================*/

/*------------------------------------------------------*/
/*                       外部声明                       */
/*======================================================*/
void scopeInit(void);
void scopePushValue(void *p, size_t length, char *name, bool isInt);
void test();
#endif
