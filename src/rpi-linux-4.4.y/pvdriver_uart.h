/*
 * Para-virtualized character driver for UART
 * Author: Amit Vasudevan (amitvasudevan@acm.org)
 *
 */


#ifndef __PVDRIVER_UART_H__
#define __PVDRIVER_UART_H__

//////
//externals
//////
void __hvc(u32 uhcall_function, void *uhcall_buffer, u32 uhcall_buffer_len);
void uxmhfpvduart_init(void);
bool uxmhfpvduart_send(u8 *buffer, u32 buf_len);
bool uxmhfpvduart_recv(u8 *buffer, u32 max_len, u32 *len_read, bool *uartreadbufexhausted);
bool uxmhfpvduart_can_send(void);
bool uxmhfpvduart_can_recv(void);
bool uxmhfpvduart_flush(void);



#endif //__PVDRIVER_UART_H__
