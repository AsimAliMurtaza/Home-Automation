
 ; Macros to Read/Write 16-bit registers
; Usage example:
; LDSw r17, r16, TCNT1H ; Reads the counter value (high, low, adr)
; STSw TCNT1H, r17, r16 ; Writes the counter value (adr, high, low)
.macro STSw
cli
STS @0, @1
STS @0-1, @2
sei
.endmacro
.macro LDSw
cli
LDS @1, @2-1
LDS @0, @2
sei
.endmacro