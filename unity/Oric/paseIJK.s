;
; Copyright (c) 2018 Anthony Beaucamp.
;
; This software is provided 'as-is', without any express or implied warranty.
; In no event will the authors be held liable for any damages arising from
; the use of this software.
;
; Permission is granted to anyone to use this software for any purpose,
; including commercial applications, and to alter it and redistribute it
; freely, subject to the following restrictions:
;
;   1. The origin of this software must not be misrepresented; you must not
;   claim that you wrote the original software. If you use this software in a
;   product, an acknowledgment in the product documentation would be
;   appreciated but is not required.
;
;   2. Altered source versions must be plainly marked as such, and must not
;   be misrepresented as being the original software.
;
;   3. This notice may not be removed or altered from any distribution.
;
;   4. The names of this software and/or it's copyright holders may not be
;   used to endorse or promote products derived from this software without
;   specific prior written permission.
;

	.export _InitPaseIJK
	.export _GetPaseIJK
	
via_ddra =	$0303
via_ddrb =	$0302
via_porta =	$0301		
via_portb =	$0300		
	
	.segment	"CODE"	
		
;------------------------------------------
; unsigned char __near__ _InitPaseIJK ()
; -----------------------------------------

.proc _InitPaseIJK: near
	ldx #%10110111		;Ensure Printer Strobe is set to Output
	stx via_ddrb
	ldx #%00000000		;Set Strobe Low
	stx via_portb
	ldx #%11000000		;Set Top two bits of PortA to Output and rest as Input
	stx via_ddra
	sta via_porta
	
	lda via_porta		;Read Joystick state (#$3f = NONE/ALTAI/PASE, #$1f = IJK)
	sta adapter
	
	ldx #%11111111		;Restore VIA PortA state
	stx via_ddra	
	rts
.endproc
	
; ------------------------------------------------
; unsigned char __near__ _GetPaseIJK (unsigned char)
; ------------------------------------------------

.proc _GetPaseIJK: near
	ldx #%00000000		;Set Strobe Low
	stx via_portb
	ldx #%11000000		;Set Top two bits of PortA to Output and rest as Input
	stx via_ddra

	cmp #1
	beq joy2	
joy1:
	lda #%01111111		;Select Left Joystick
	jmp invert
joy2:	
	lda #%10111111		;Select Right Joystick
	
invert:
	ldy adapter
	cpy #$1f
	beq read
	eor #%11000000		;Altai/Pase: Invert bits

read:
	sta via_porta
	lda via_porta		;Read Joystick state
	ldy adapter			;Branch to decoder (Altai/Pase or IJK)
	cpy #$1f
	bne pase
	
ijk:	
	and #%00011111		;Mask out unused bits
	eor #%00011111		;Invert Bits
	tax
	lda bitmasksIJK,X
	jmp restore

pase:
	and #%00111111		;Mask out unused bits
	eor #%00111111		;Invert Bits
	tax
	and #%00100000		;Check fire bit
	cmp #%00100000		
	bne fire
	txa
	eor #%00100100		;Move fire bit
	tax
fire:
	lda bitmasksPASE,X
	
restore:
	ldx #%11111111		;Restore VIA PortA state
	stx via_ddra
	rts
.endproc

	.segment	"DATA"
	
adapter: 
  .res 1,$00	;#$3f = NONE/ALTAI/PASE, #$1f = IJK

bitmasksIJK:
  .byte 255,247,251,243,239,231,235,227,253,245,249,241,237,229,233,225
  .byte 254,246,250,242,238,230,234,226,252,244,248,240,236,228,232,224
  
bitmasksPASE:
  .byte 255,251,247,243,239,235,231,227,253,249,245,241,237,233,229,225
  .byte 254,250,246,242,238,234,230,226,252,248,244,240,236,232,228,224  
