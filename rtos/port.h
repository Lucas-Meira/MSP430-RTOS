#ifndef PORT_H
#define PORT_H
extern void _osSaveContext(uint16_t **pStack);
extern void _osRestoreContext(uint16_t *pStack);
#endif
