.include "m328pdef.inc"
.include "delay.inc"
.include "UART_Macros.inc"

.def A = r17
.def AH = r18
.def PWM_CONFIG = r19
.def TEMP = r22
.def UART_CHAR = r21


.org 0x0000 ; Reset vector
	RJMP RESET_handler ; Relative jump to the reset handler

.org 0x0024 ; USART RX interrupt vector
	RJMP UART_RX_ISR ; Jump to USART ISR

RESET_handler:
    LDI r16, high(RAMEND) ;stack pointer initialization
    OUT SPH, r16
    LDI r16, low(RAMEND)
    OUT SPL, r16
    SEI


; Macro to read a single byte from the UART
; Inputs: register to hold the received byte
; Outputs: r16
; Working: receives byte via UART and stores in a r16 register
.macro Serial_read

	;Serial_read_WAIT:
	; wait till a byte is in the buffer
	LDS             R25, UCSR0A
	SBRS            R25, RXC0
	RJMP            Serial_read_Skip
	;RJMP            Serial_read_WAIT

	Serial_read_Start:
	; read the byte
	LDS             r25, UDR0

	rjmp Serial_read_END
	Serial_read_Skip:
	LDI				r25,0
	Serial_read_END:
.endmacro

main:
	day_string: .db "Day Time ",0x0D,0x0A,0
	night_string: .db "Night Time ",0x0D,0x0A,0

;-------------------------------- Configurations ---------------------------------
; I/O Pins Configuration
	SBI DDRB,5 ; Set PB5 pin for Output to LED1
	CBI PORTB,5 ; LED1 OFF
	SBI DDRB,3 ; Set PB5 pin for Output to LED2
	CBI PORTB,3 ; LED2 OFF
	SBI DDRB,2 ; Set PB5 pin for Output to LED3
	CBI PORTB,2 ; LED3 OFF
	SBI DDRB,4 ; Set PB4 pin for Output to Fan1
	CBI PORTB,4 ; Fan1 OFF
	SBI DDRD,PD6 ; Set PD6 pin for Output to Fan2 PWM
	SBI PORTD,PD6
;ADC Cofiguration
	LDI A, 0b11000111 ; [ADEN ADSC ADATE ADIF ADIE ADIE ADPS2 ADPS1 ADPS0]
	STS ADCSRA, A
	LDI A, 0b01100000 ; [REFS1 REFS0 ADLAR – MUX3 MUX2 MUX1 MUX0]
	STS ADMUX, A ; Select ADC0 (PC0) pin
	SBI PORTC, PC0 ; Enable Pull-up Resistor
; PWM Configuration for Fan2 (PD6)
	ldi TEMP,0xFF
	out OCR0A,TEMP ; to compare match port A
	ldi TEMP, (1 << COM0A1) | (0 << COM0A0) | (1 << WGM01) | (1 << WGM00)
	out TCCR0A,TEMP
	ldi TEMP,1<<CS00 ; Prescaler = 1
	out TCCR0B,TEMP
; UART Configuration
	SBI DDRD,1 ; Set PD1 (TX) as Output
	CBI PORTD,1 ; TX Low (initial state)
	CBI DDRD,0 ; Set PD0 (RX) as Input
	SBI PORTD,0 ; Enable Pull-up Resistor on RX
	Serial_begin ; Initialize UART Protocol

;-------------------------LED Logic----------------------
loop:
    LDS A, ADCSRA ; Start Analog to Digital Conversion
    ORI A, (1 << ADSC)
    STS ADCSRA, A
wait:
    LDS A, ADCSRA
    SBRC A, ADSC
    RJMP wait
    LDS A, ADCL ; Must Read ADCL before ADCH
    LDS AH, ADCH
	delay 100 ; delay 100ms
	Serial_writeReg AH ; sending the received value to UART
	Serial_writeChar ':' ; just for formating (e.g. 180: Day Time or 220: Night Time)
	Serial_writeChar ' '
	cpi AH,100 ; compare LDR reading with our desired threshold
	BRSH Night_time ; jump if same or higher (AH >= 200)
	RJMP Day_time
	RJMP loop
;--------------------FANS Interrupt Logic------------------------

Day_time:
	LDI ZL, LOW (2 * day_string)
	LDI ZH, HIGH (2 * day_string)
	Serial_writeStr
	delay 500
	RJMP loop
Night_time:
	LDI ZL, LOW (2 * night_string)
	LDI ZH, HIGH (2 * night_string)
	Serial_writeStr
	delay 500
	RJMP loop

UART_RX_ISR:
	LDI r25, 0
	Serial_read

	CPI r25, 1	;Fan 1
    BREQ turn_fan1_on
    CPI r25, 2
    BREQ turn_fan1_off

	CPI r25, 3 ;LED 1
    BREQ turn_light1_on
    CPI r25, 4
    BREQ turn_light1_off

	CPI r25, 5 ;LED 2
    BREQ turn_light2_on
    CPI r25, 6
    BREQ turn_light2_off

	CPI r25, 7	 ;LED 3
    BREQ turn_light3_on
    CPI r25, 8
    BREQ turn_light3_off
	RJMP fan2_speed
	RJMP UART_RX_ISR_End

turn_fan1_on:
	Serial_writeChar '1'
    SBI PORTB,4 ; Fan1 ON
    RJMP UART_RX_ISR_End

turn_fan1_off:
	Serial_writeChar '2'
    CBI PORTB,4 ; Fan1 OFF
	RJMP UART_RX_ISR_End
	
fan2_speed_high:
	Serial_writeChar '3'
	LDI TEMP,0xFF
	OUT OCR0A,TEMP
    RJMP UART_RX_ISR_End

fan2_speed_low:
	Serial_writeChar '4'
	LDI TEMP,150
	OUT OCR0A,TEMP
	RJMP UART_RX_ISR_End

turn_light2_on:
	SBI PORTB,3
	RJMP UART_RX_ISR_End

turn_light1_on:
	SBI PORTB,5
	RJMP UART_RX_ISR_End

turn_light3_on:
	SBI PORTB,2
	RJMP UART_RX_ISR_End

turn_light1_off:
	CBI PORTB,5
	RJMP UART_RX_ISR_End

turn_light2_off:
	CBI PORTB,3
	RJMP UART_RX_ISR_End

turn_light3_off:
	CBI PORTB,2
	RJMP UART_RX_ISR_End
fan2_speed:
	Serial_writeChar '3'
	OUT OCR0A,r25
    RJMP UART_RX_ISR_End

UART_RX_ISR_End:
    RETI ; Return from interrupt
