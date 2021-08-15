			.text
			.global _osSaveContext, _osRestoreContext
; void _osSaveContext( uint16_t ** pStack);
_osSaveContext:
			popx.a -52(SP)   ; Push scheduler PC to the end of the stack
			pushm.a #12,R15  ; Save context
			movx.a SP,0(R12) ; Save stack ponter
			subx.a #4,SP	 ; Move stack pointer to point to the PC of the scheduler

			reta

; void _osRestoreContext( uint16_t * pStack);
_osRestoreContext:
			movx.a R12,SP   ; Restore stack pointer
			popm.a #12,R15  ; Restore context
			reti			; Return from interrupt
