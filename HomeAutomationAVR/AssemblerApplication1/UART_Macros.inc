;-------------------------------------------
; UART Serial Read/Write Macros
;-------------------------------------------
;	[Usage Example Code]
;
;	; macro to initilize the serial communication
;	Serial_begin	
;
;	; macro to write the character 'A' to UART
;	Serial_writeChar 'A'
;
;	; macro to writes the newline to UART
;	Serial_writeNewLine
;
;	; macro to writes the string pointed to by HELLO_STRING to the UART
;	LDI             ZL, LOW (2 * hello_string)
;	LDI             ZH, HIGH (2 * hello_string)
;	Serial_writeStr
;
;	; writes the first HELLO_LEN characters of the string pointed to by HELLO_STRING to the UART
;	LDI             ZL, LOW (2 * hello_string)
;	LDI             ZH, HIGH (2 * hello_string)
;	LDI             R20, string_len
;	Serial_writeBuffer
;
;	; read a single character from the UART and store the result in the given argument
;	Serial_readChar r20
;
;	loop:
;	RJMP loop
;
;	; it is recommanded to define the strings at the end of the code segment
;	hello_string:	.db	"Hello World!",10,0
;	len: .equ	string_len   = 2 * (len - hello_string)
;


; Macro to initializes the UART to 9600 BAUD at a frequency of 16 MHz (for Arduino UNO ATmega328p)
; Inputs: none
; Outputs: none
; Working: Initilize the UART protocol
; Registers modified: r16.r17
.macro Serial_begin
	.equ            F_CPU           = 16000000
	.equ            BAUD_RATE       = 9600
	.equ            BAUD_PRESCALER  = (F_CPU/(BAUD_RATE * 16)) - 1
	; save the states of R16 and R17
	PUSH            R16
	PUSH            R17
	; initialize UART to 9600 baud
	LDI             R16, LOW (BAUD_PRESCALER)
	LDI             R17, HIGH (BAUD_PRESCALER)
	STS             UBRR0L, R16
	STS             UBRR0H, R17
	; enable transmitter and reciever modes
	LDI             R16, (1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0)
	STS             UCSR0B, R16
	; restore the states of R16 and R17 and return
	POP             R17
	POP             R16
.endmacro


; Macro to writes a single character (single byte) to the UART
; Inputs: an ASCII character as argument
; Outputs: none
; Working: sends that ASCII value of the character to the UART
; Registers modified: r16
.macro Serial_writeChar
	; save the state of R16
	PUSH            R16
	Serial_writeChar_LOOP:
	; wait for the write buffer to become empty (bit UDRE0 of UCSR0A register should be set)
	LDS             R16, UCSR0A
	SBRS            R16, UDRE0
	RJMP            Serial_writeChar_LOOP
	; copy the argument to the UDR0 register to be sent out
	LDI				R16, @0
	STS             UDR0, R16
	; restore the state of R16 and return
	POP             R16
.endmacro


; Macro to check if a character is available in the UART buffer
; Inputs: none
; Outputs: Z flag (cleared if no character is available, set if a character is available)
; Registers modified: r16
.macro Serial_isCharAvailable
    PUSH R16
    ; Load UCSR0A into r16
    LDS R16, UCSR0A
    ; Check RXC0 (Receive Complete) flag
    SBRS R16, RXC0
    ; If RXC0 is set, character is available (Z flag is cleared)
    RJMP Serial_isCharAvailable_End
    ; If RXC0 is cleared, no character is available (Z flag is set)
    Serial_isCharAvailable_End:
    POP R16
.endmacro


; Macro to writes a register to the UART
; Inputs: register as argument
; Outputs: none
; Working: sends the data of the register to the UART
; Registers modified: r16
.macro Serial_writeReg
	; save the state of R16
	PUSH            R16
	Serial_writeReg_LOOP:
	; wait for the write buffer to become empty (bit UDRE0 of UCSR0A register should be set)
	LDS             R16, UCSR0A
	SBRS            R16, UDRE0
	RJMP            Serial_writeReg_LOOP
	; copy the argument to the UDR0 register to be sent out
	mov				R16, @0
	STS             UDR0, R16
	; restore the state of R16 and return
	POP             R16
.endmacro


; Macro to writes a new line character to the UART
; Inputs: none
; Outputs: none
; Working: sends newline character (0x0A) to the UART
; Registers modified: r16
.macro Serial_writeNewLine
	; save the state of R16
	PUSH            R16
	Serial_writeNewLine_LOOP:
	; wait for the write buffer to become empty (bit UDRE0 of UCSR0A register should be set)
	LDS             R16, UCSR0A
	SBRS            R16, UDRE0
	RJMP            Serial_writeNewLine_LOOP
	; move value 0x0a (\n) to the UDR0 register to be sent out
	LDI				R16, 0x0a
	STS             UDR0, R16
	; restore the state of R16 and return
	POP             R16
.endmacro


; Macro to writes a NULL terminated string to the UART 
; Inputs: ZH:ZL - SRAM buffer address from where to the string is stored
; Outputs: none
; Working: sends the string via UART
; Registers modified: r16,r17,Z
.macro Serial_writeStr
	; save the states of R16 and R17
	PUSH            R16
	PUSH            R17
	Serial_writeStr_LOOP:
	; load the current byte/character pointed to be Z and increment the Z pointer
	LPM             R16, Z+
	; if the current character is 0/NULL, return from the routine since the string has been used
	CPI             R16, 0
	BREQ            Serial_writeStr_END
	Serial_writeStr_CHAR:
	; wait for the write buffer to become empty (bit UDRE0 of UCSR0A register should be set)
	LDS             R17, UCSR0A
	SBRS            R17, UDRE0
	RJMP            Serial_writeStr_CHAR
	; copy the current character to the UDR0 register to send it out and jump back to the start
	STS             UDR0, R16
	RJMP            Serial_writeStr_LOOP
	Serial_writeStr_END:
	; restore the states of R16 and R17 and return
	POP             R17
	POP             R16
.endmacro


; Macro to writes a buffer of a given length (<256) to the UART
; Inputs: ZH:ZL - SRAM buffer address from where to the sending data is placed
;		  r20 should have the length of the buffer
; Outputs: none
; Working: receives characters via UART and stores in a register
; Registers modified: r16,r17,Z
.macro Serial_writeBuffer
	; save the states of R16 and R17
	PUSH            R16
	PUSH            R17
	Serial_writeBuffer_LOOP:
	; load the current byte/character pointed to be Z and increment the Z pointer
	LPM             R16, Z+
	; check if the remaining size of the string is non-zero and return if it is
	CPI             R20, 0
	BREQ            Serial_writeBuffer_END
	Serial_writeBuffer_CHAR:
	; wait for the write buffer to become empty (bit UDRE0 of UCSR0A register should be set)
	LDS             R17, UCSR0A
	SBRS            R17, UDRE0
	RJMP            Serial_writeBuffer_CHAR
	; copy the current character to the UDR0 register to send it out and jump back to the start
	STS             UDR0, R16
	DEC             R20
	RJMP            Serial_writeBuffer_LOOP
	Serial_writeBuffer_END:
	; restore the states of R16 and R17 and return
	POP             R17
	POP             R16
.endmacro


; Macro to read a single character from the UART
; Inputs: register to hold the received character
; Outputs: none
; Working: receives characters via UART and stores in a register
; Registers modified: r16
.macro Serial_readChar
	; save the state of R16
	PUSH            R16
	Serial_readChar_WAIT:
	; wait till a character is in the buffer
	LDS             R16, UCSR0A
	SBRS            R16, RXC0
	RJMP            Serial_readChar_WAIT
	; read the character
	LDS             @0, UDR0
	; restore the state of R16 and return
	POP             R16
.endmacro


; Macro to receive a string from the UART
; Inputs: ZH:ZL - SRAM buffer address for received string
; Outputs: none
; Working: receives characters via UART and stores in data memory until carriage return received
; Registers modified: r16, r17, XL, XH
.macro Serial_readStr
	; save the state of R16 and R17
	PUSH            R16
	PUSH			R17
	gets:	lds	r17,UCSR0A			; load UCSR0A into r17
	sbrs	r17,UDRE0				; wait for empty transmit buffer
	rjmp	gets					; repeat loop

	lds		r16,UDR0					; get received character

	cpi		r16,0x0D					; check if received char is CR
	breq	gets_end				; branch if CR received

	st	Z+,r16						; store character to buffer
	rjmp gets					; get another character

gets_end:
	; restore the state of R16 and R17 and return
	POP				R17
	POP             R16
.endmacro
