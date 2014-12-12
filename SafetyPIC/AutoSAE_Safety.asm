
_trip:

;AutoSAE_Safety.c,24 :: 		void trip() {
;AutoSAE_Safety.c,26 :: 		tripstate = 1;
	MOVLW      1
	MOVWF      _tripstate+0
;AutoSAE_Safety.c,28 :: 		PORTB.B6 = 0;    // Open trip circuit
	BCF        PORTB+0, 6
;AutoSAE_Safety.c,29 :: 		PORTA.B4 = 1;    // E-stop LED on
	BSF        PORTA+0, 4
;AutoSAE_Safety.c,30 :: 		PORTA.B0 = 1;    // Alarm on
	BSF        PORTA+0, 0
;AutoSAE_Safety.c,31 :: 		if(brakeil == 1) { PORTA.B1 = 0; }     // Activate brake interlock
	MOVF       _brakeil+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_trip0
	BCF        PORTA+0, 1
L_trip0:
;AutoSAE_Safety.c,33 :: 		if(hb_trip == 1) { PORTA.B3 = 1; }
	MOVF       _hb_trip+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_trip1
	BSF        PORTA+0, 3
	GOTO       L_trip2
L_trip1:
;AutoSAE_Safety.c,34 :: 		else { PORTA.B3 = 0; }
	BCF        PORTA+0, 3
L_trip2:
;AutoSAE_Safety.c,36 :: 		UART1_Write_Text("TRIP\n");
	MOVLW      ?lstr1_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,38 :: 		arm_state = -1;
	MOVLW      255
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,39 :: 		tripreq = 0;
	CLRF       _tripreq+0
;AutoSAE_Safety.c,41 :: 		}
L_end_trip:
	RETURN
; end of _trip

_untrip:

;AutoSAE_Safety.c,46 :: 		void untrip() {
;AutoSAE_Safety.c,48 :: 		tripstate = 0;
	CLRF       _tripstate+0
;AutoSAE_Safety.c,50 :: 		PORTB.B6 = 1;    // Close trip circuit
	BSF        PORTB+0, 6
;AutoSAE_Safety.c,51 :: 		PORTA.B4 = 0;    // E-stop LED off
	BCF        PORTA+0, 4
;AutoSAE_Safety.c,52 :: 		PORTA.B0 = 0;    // Alarm off
	BCF        PORTA+0, 0
;AutoSAE_Safety.c,53 :: 		PORTA.B1 = 1;     // Deactivate brake interlock
	BSF        PORTA+0, 1
;AutoSAE_Safety.c,54 :: 		PORTA.B3 = 0;    // Deactivate hb trip led
	BCF        PORTA+0, 3
;AutoSAE_Safety.c,56 :: 		UART1_Write_Text("Untrip\n");
	MOVLW      ?lstr2_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,58 :: 		arm_state = 0;
	CLRF       _arm_state+0
;AutoSAE_Safety.c,60 :: 		}
L_end_untrip:
	RETURN
; end of _untrip

_interrupt:
	MOVWF      R15+0
	SWAPF      STATUS+0, 0
	CLRF       STATUS+0
	MOVWF      ___saveSTATUS+0
	MOVF       PCLATH+0, 0
	MOVWF      ___savePCLATH+0
	CLRF       PCLATH+0

;AutoSAE_Safety.c,67 :: 		void interrupt() {
;AutoSAE_Safety.c,69 :: 		if(hb_interruptcount > 50) {
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _hb_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt91
	MOVF       _hb_interruptcount+0, 0
	SUBLW      50
L__interrupt91:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt3
;AutoSAE_Safety.c,70 :: 		if((arm_state > 0) && (brakeil == 1)) {
	MOVLW      128
	XORLW      0
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_state+0, 0
	SUBWF      R0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt6
	MOVF       _brakeil+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt6
L__interrupt81:
;AutoSAE_Safety.c,71 :: 		hb_trip = 1;    // HB LED constant on
	MOVLW      1
	MOVWF      _hb_trip+0
;AutoSAE_Safety.c,72 :: 		tripreq = 1;
	MOVLW      1
	MOVWF      _tripreq+0
;AutoSAE_Safety.c,73 :: 		}
	GOTO       L_interrupt7
L_interrupt6:
;AutoSAE_Safety.c,74 :: 		else { hb_interruptcount = 0; }
	CLRF       _hb_interruptcount+0
	CLRF       _hb_interruptcount+1
L_interrupt7:
;AutoSAE_Safety.c,75 :: 		}
	GOTO       L_interrupt8
L_interrupt3:
;AutoSAE_Safety.c,76 :: 		else { hb_interruptcount++; }
	INCF       _hb_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _hb_interruptcount+1, 1
L_interrupt8:
;AutoSAE_Safety.c,78 :: 		if(arm_state == 1) {
	MOVF       _arm_state+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt9
;AutoSAE_Safety.c,79 :: 		if(arm_interruptcount > 200) { arm_state = 2; }
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt92
	MOVF       _arm_interruptcount+0, 0
	SUBLW      200
L__interrupt92:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt10
	MOVLW      2
	MOVWF      _arm_state+0
	GOTO       L_interrupt11
L_interrupt10:
;AutoSAE_Safety.c,80 :: 		else { arm_interruptcount++; }
	INCF       _arm_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _arm_interruptcount+1, 1
L_interrupt11:
;AutoSAE_Safety.c,81 :: 		}
	GOTO       L_interrupt12
L_interrupt9:
;AutoSAE_Safety.c,82 :: 		else if(arm_state == 3) {
	MOVF       _arm_state+0, 0
	XORLW      3
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt13
;AutoSAE_Safety.c,83 :: 		if(arm_interruptcount > 200) { arm_state = 4; }
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt93
	MOVF       _arm_interruptcount+0, 0
	SUBLW      200
L__interrupt93:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt14
	MOVLW      4
	MOVWF      _arm_state+0
	GOTO       L_interrupt15
L_interrupt14:
;AutoSAE_Safety.c,84 :: 		else { arm_interruptcount++; }
	INCF       _arm_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _arm_interruptcount+1, 1
L_interrupt15:
;AutoSAE_Safety.c,85 :: 		}
	GOTO       L_interrupt16
L_interrupt13:
;AutoSAE_Safety.c,86 :: 		else if(arm_state == 5) {
	MOVF       _arm_state+0, 0
	XORLW      5
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt17
;AutoSAE_Safety.c,87 :: 		if(arm_interruptcount > 500) { arm_state = 6; }
	MOVLW      128
	XORLW      1
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt94
	MOVF       _arm_interruptcount+0, 0
	SUBLW      244
L__interrupt94:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt18
	MOVLW      6
	MOVWF      _arm_state+0
	GOTO       L_interrupt19
L_interrupt18:
;AutoSAE_Safety.c,88 :: 		else { arm_interruptcount++; }
	INCF       _arm_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _arm_interruptcount+1, 1
L_interrupt19:
;AutoSAE_Safety.c,89 :: 		}
	GOTO       L_interrupt20
L_interrupt17:
;AutoSAE_Safety.c,90 :: 		else if(arm_state == 7) {
	MOVF       _arm_state+0, 0
	XORLW      7
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt21
;AutoSAE_Safety.c,91 :: 		if(arm_interruptcount > 100) { arm_state = 8; }
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt95
	MOVF       _arm_interruptcount+0, 0
	SUBLW      100
L__interrupt95:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt22
	MOVLW      8
	MOVWF      _arm_state+0
	GOTO       L_interrupt23
L_interrupt22:
;AutoSAE_Safety.c,92 :: 		else { arm_interruptcount++; }
	INCF       _arm_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _arm_interruptcount+1, 1
L_interrupt23:
;AutoSAE_Safety.c,93 :: 		}
	GOTO       L_interrupt24
L_interrupt21:
;AutoSAE_Safety.c,94 :: 		else if(arm_state == 9 || arm_state == 0) { arm_interruptcount = 0; }
	MOVF       _arm_state+0, 0
	XORLW      9
	BTFSC      STATUS+0, 2
	GOTO       L__interrupt80
	MOVF       _arm_state+0, 0
	XORLW      0
	BTFSC      STATUS+0, 2
	GOTO       L__interrupt80
	GOTO       L_interrupt27
L__interrupt80:
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
L_interrupt27:
L_interrupt24:
L_interrupt20:
L_interrupt16:
L_interrupt12:
;AutoSAE_Safety.c,96 :: 		if(alarm_counter > 0) { alarm_counter--; }
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _alarm_counter+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt96
	MOVF       _alarm_counter+0, 0
	SUBLW      0
L__interrupt96:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt28
	MOVLW      1
	SUBWF      _alarm_counter+0, 1
	BTFSS      STATUS+0, 0
	DECF       _alarm_counter+1, 1
L_interrupt28:
;AutoSAE_Safety.c,98 :: 		TMR0 = 100;
	MOVLW      100
	MOVWF      TMR0+0
;AutoSAE_Safety.c,99 :: 		INTCON.TMR0IF = 0;
	BCF        INTCON+0, 2
;AutoSAE_Safety.c,100 :: 		}
L_end_interrupt:
L__interrupt90:
	MOVF       ___savePCLATH+0, 0
	MOVWF      PCLATH+0
	SWAPF      ___saveSTATUS+0, 0
	MOVWF      STATUS+0
	SWAPF      R15+0, 1
	SWAPF      R15+0, 0
	RETFIE
; end of _interrupt

_main:

;AutoSAE_Safety.c,106 :: 		void main() {
;AutoSAE_Safety.c,108 :: 		OSCCON = 0b1111110;    // Set up internal oscillator.
	MOVLW      126
	MOVWF      OSCCON+0
;AutoSAE_Safety.c,110 :: 		WDTCON.WDTPS0 = 0;
	BCF        WDTCON+0, 1
;AutoSAE_Safety.c,111 :: 		WDTCON.WDTPS1 = 0;
	BCF        WDTCON+0, 2
;AutoSAE_Safety.c,112 :: 		WDTCON.WDTPS2 = 0;
	BCF        WDTCON+0, 3
;AutoSAE_Safety.c,113 :: 		WDTCON.WDTPS3 = 1;
	BSF        WDTCON+0, 4
;AutoSAE_Safety.c,115 :: 		UART1_Init(38400);       // Init hardware UART
	MOVLW      12
	MOVWF      SPBRG+0
	BSF        TXSTA+0, 2
	CALL       _UART1_Init+0
;AutoSAE_Safety.c,116 :: 		Delay_ms(500);
	MOVLW      6
	MOVWF      R11+0
	MOVLW      19
	MOVWF      R12+0
	MOVLW      173
	MOVWF      R13+0
L_main29:
	DECFSZ     R13+0, 1
	GOTO       L_main29
	DECFSZ     R12+0, 1
	GOTO       L_main29
	DECFSZ     R11+0, 1
	GOTO       L_main29
	NOP
	NOP
;AutoSAE_Safety.c,118 :: 		UART1_Write_Text("Hi\n");
	MOVLW      ?lstr3_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,120 :: 		ANSEL  = 0;            // Set PortA pins to digital.
	CLRF       ANSEL+0
;AutoSAE_Safety.c,121 :: 		CMCON = 0x07;
	MOVLW      7
	MOVWF      CMCON+0
;AutoSAE_Safety.c,124 :: 		TRISA = 0b11100100;
	MOVLW      228
	MOVWF      TRISA+0
;AutoSAE_Safety.c,125 :: 		TRISB = 0b10111100;
	MOVLW      188
	MOVWF      TRISB+0
;AutoSAE_Safety.c,128 :: 		PORTA = 0b00000010;          // Brake IL line is high by default.
	MOVLW      2
	MOVWF      PORTA+0
;AutoSAE_Safety.c,129 :: 		PORTB = 0;
	CLRF       PORTB+0
;AutoSAE_Safety.c,132 :: 		INTCON.TMR0IE = 1;
	BSF        INTCON+0, 5
;AutoSAE_Safety.c,133 :: 		INTCON.TMR0IF = 0;
	BCF        INTCON+0, 2
;AutoSAE_Safety.c,134 :: 		OPTION_REG.T0CS = 0;  // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock
	BCF        OPTION_REG+0, 5
;AutoSAE_Safety.c,135 :: 		OPTION_REG.T0SE = 0;  // bit 4 TMR0 Source Edge Select bit 0 = low/high
	BCF        OPTION_REG+0, 4
;AutoSAE_Safety.c,136 :: 		OPTION_REG.PSA = 0;   // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
	BCF        OPTION_REG+0, 3
;AutoSAE_Safety.c,137 :: 		OPTION_REG.PS2 = 1;   // bits 2-0  PS2:PS0: Prescaler Rate Select bits
	BSF        OPTION_REG+0, 2
;AutoSAE_Safety.c,138 :: 		OPTION_REG.PS1 = 1;
	BSF        OPTION_REG+0, 1
;AutoSAE_Safety.c,139 :: 		OPTION_REG.PS0 = 1;
	BSF        OPTION_REG+0, 0
;AutoSAE_Safety.c,140 :: 		OPTION_REG.NOT_RBPU = 0;
	BCF        OPTION_REG+0, 7
;AutoSAE_Safety.c,141 :: 		TMR0 = 100;             // preset for timer register
	MOVLW      100
	MOVWF      TMR0+0
;AutoSAE_Safety.c,143 :: 		INTCON.GIE = 1;
	BSF        INTCON+0, 7
;AutoSAE_Safety.c,146 :: 		UART1_Write_Text("Init ok\n");
	MOVLW      ?lstr4_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,149 :: 		while(1) {
L_main30:
;AutoSAE_Safety.c,151 :: 		if(tripreq == 1) { trip(); }
	MOVF       _tripreq+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main32
	CALL       _trip+0
L_main32:
;AutoSAE_Safety.c,153 :: 		asm { CLRWDT; } // Reset watchdog timer
	CLRWDT
;AutoSAE_Safety.c,155 :: 		if (UART1_Data_Ready()) {                // Receive serial data.
	CALL       _UART1_Data_Ready+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main33
;AutoSAE_Safety.c,156 :: 		uart_rd = UART1_Read();
	CALL       _UART1_Read+0
	MOVF       R0+0, 0
	MOVWF      _uart_rd+0
;AutoSAE_Safety.c,158 :: 		if(uart_rd == 'E' && tripstate == 0) { trip(); }
	MOVF       R0+0, 0
	XORLW      69
	BTFSS      STATUS+0, 2
	GOTO       L_main36
	MOVF       _tripstate+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main36
L__main86:
	CALL       _trip+0
L_main36:
;AutoSAE_Safety.c,159 :: 		if(uart_rd == 'R' && tripstate == 1) { untrip(); }
	MOVF       _uart_rd+0, 0
	XORLW      82
	BTFSS      STATUS+0, 2
	GOTO       L_main39
	MOVF       _tripstate+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main39
L__main85:
	CALL       _untrip+0
L_main39:
;AutoSAE_Safety.c,161 :: 		oldhbstate = hbstate;
	MOVF       _hbstate+0, 0
	MOVWF      _oldhbstate+0
;AutoSAE_Safety.c,163 :: 		if(uart_rd == '+') { hbstate = 1; }
	MOVF       _uart_rd+0, 0
	XORLW      43
	BTFSS      STATUS+0, 2
	GOTO       L_main40
	MOVLW      1
	MOVWF      _hbstate+0
L_main40:
;AutoSAE_Safety.c,164 :: 		if(uart_rd == '-') { hbstate = 0; }
	MOVF       _uart_rd+0, 0
	XORLW      45
	BTFSS      STATUS+0, 2
	GOTO       L_main41
	CLRF       _hbstate+0
L_main41:
;AutoSAE_Safety.c,166 :: 		if(uart_rd == 'B') { brakeil = 1; }
	MOVF       _uart_rd+0, 0
	XORLW      66
	BTFSS      STATUS+0, 2
	GOTO       L_main42
	MOVLW      1
	MOVWF      _brakeil+0
L_main42:
;AutoSAE_Safety.c,167 :: 		if(uart_rd == 'H') { brakeil = 0; }
	MOVF       _uart_rd+0, 0
	XORLW      72
	BTFSS      STATUS+0, 2
	GOTO       L_main43
	CLRF       _brakeil+0
L_main43:
;AutoSAE_Safety.c,169 :: 		if(uart_rd == 'A') { alarm_counter = 25; PORTA.B0 = 1; }
	MOVF       _uart_rd+0, 0
	XORLW      65
	BTFSS      STATUS+0, 2
	GOTO       L_main44
	MOVLW      25
	MOVWF      _alarm_counter+0
	MOVLW      0
	MOVWF      _alarm_counter+1
	BSF        PORTA+0, 0
L_main44:
;AutoSAE_Safety.c,171 :: 		if(oldhbstate != hbstate) { hb_interruptcount = 0; }
	MOVF       _oldhbstate+0, 0
	XORWF      _hbstate+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main45
	CLRF       _hb_interruptcount+0
	CLRF       _hb_interruptcount+1
L_main45:
;AutoSAE_Safety.c,173 :: 		UART1_Write_Text("ACK ");        // Reply with acknowledgement of message.
	MOVLW      ?lstr5_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,174 :: 		UART1_Write(uart_rd);
	MOVF       _uart_rd+0, 0
	MOVWF      FARG_UART1_Write_data_+0
	CALL       _UART1_Write+0
;AutoSAE_Safety.c,175 :: 		UART1_Write_Text("\n");
	MOVLW      ?lstr6_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,176 :: 		}
L_main33:
;AutoSAE_Safety.c,178 :: 		if(tripstate == 0) { // Update heartbeat LED.
	MOVF       _tripstate+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main46
;AutoSAE_Safety.c,179 :: 		PORTA.B3 = hbstate;
	BTFSC      _hbstate+0, 0
	GOTO       L__main98
	BCF        PORTA+0, 3
	GOTO       L__main99
L__main98:
	BSF        PORTA+0, 3
L__main99:
;AutoSAE_Safety.c,180 :: 		if(alarm_counter == 0) { PORTA.B0 = 0; } // Turn off the alarm if it shouldn't be on.
	MOVLW      0
	XORWF      _alarm_counter+1, 0
	BTFSS      STATUS+0, 2
	GOTO       L__main100
	MOVLW      0
	XORWF      _alarm_counter+0, 0
L__main100:
	BTFSS      STATUS+0, 2
	GOTO       L_main47
	BCF        PORTA+0, 0
L_main47:
;AutoSAE_Safety.c,181 :: 		}
L_main46:
;AutoSAE_Safety.c,183 :: 		brakeil = PORTB.B7;
	MOVLW      0
	BTFSC      PORTB+0, 7
	MOVLW      1
	MOVWF      _brakeil+0
;AutoSAE_Safety.c,184 :: 		PORTB.B1 = brakeil;
	BTFSC      _brakeil+0, 0
	GOTO       L__main101
	BCF        PORTB+0, 1
	GOTO       L__main102
L__main101:
	BSF        PORTB+0, 1
L__main102:
;AutoSAE_Safety.c,186 :: 		if(arm_state > 0 && PORTA.B2 == 0 && tripstate == 0) {        // Dash e-stop
	MOVLW      128
	XORLW      0
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_state+0, 0
	SUBWF      R0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main50
	BTFSC      PORTA+0, 2
	GOTO       L_main50
	MOVF       _tripstate+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main50
L__main84:
;AutoSAE_Safety.c,187 :: 		UART1_Write_Text("DES\n");
	MOVLW      ?lstr7_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,188 :: 		trip();
	CALL       _trip+0
;AutoSAE_Safety.c,189 :: 		}
L_main50:
;AutoSAE_Safety.c,191 :: 		if(arm_state == 0) {
	MOVF       _arm_state+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main51
;AutoSAE_Safety.c,193 :: 		if(Button(&PORTB,3,20,0)) {
	MOVLW      PORTB+0
	MOVWF      FARG_Button_port+0
	MOVLW      3
	MOVWF      FARG_Button_pin+0
	MOVLW      20
	MOVWF      FARG_Button_time_ms+0
	CLRF       FARG_Button_active_state+0
	CALL       _Button+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main52
;AutoSAE_Safety.c,194 :: 		UART1_Write_Text("AR\n");
	MOVLW      ?lstr8_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,195 :: 		if(brakeil == 1) { PORTA.B1 = 0; }     // Activate brake interlock
	MOVF       _brakeil+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main53
	BCF        PORTA+0, 1
L_main53:
;AutoSAE_Safety.c,196 :: 		PORTB.B0 = 0;                          // Throttle LED
	BCF        PORTB+0, 0
;AutoSAE_Safety.c,197 :: 		PORTA.B4 = 0;                          // E-stop LED
	BCF        PORTA+0, 4
;AutoSAE_Safety.c,198 :: 		PORTA.B0 = 0;                          // Alarm off
	BCF        PORTA+0, 0
;AutoSAE_Safety.c,199 :: 		PORTB.B6 = 0;                          // Trip relay
	BCF        PORTB+0, 6
;AutoSAE_Safety.c,200 :: 		tripstate = 0;
	CLRF       _tripstate+0
;AutoSAE_Safety.c,201 :: 		hb_trip = 0;
	CLRF       _hb_trip+0
;AutoSAE_Safety.c,202 :: 		arm_state = 1;
	MOVLW      1
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,203 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,204 :: 		hb_interruptcount = 0;
	CLRF       _hb_interruptcount+0
	CLRF       _hb_interruptcount+1
;AutoSAE_Safety.c,205 :: 		UART1_Write_Text("A 1\n");
	MOVLW      ?lstr9_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,206 :: 		}
L_main52:
;AutoSAE_Safety.c,207 :: 		}
	GOTO       L_main54
L_main51:
;AutoSAE_Safety.c,208 :: 		else if(arm_state == 1) {
	MOVF       _arm_state+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main55
;AutoSAE_Safety.c,209 :: 		if(PORTB.B3 == 1) { arm_state = -1; UART1_Write_Text("AF 1\n"); }   // Arm button
	BTFSS      PORTB+0, 3
	GOTO       L_main56
	MOVLW      255
	MOVWF      _arm_state+0
	MOVLW      ?lstr10_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main56:
;AutoSAE_Safety.c,210 :: 		}
	GOTO       L_main57
L_main55:
;AutoSAE_Safety.c,211 :: 		else if(arm_state == 2) {
	MOVF       _arm_state+0, 0
	XORLW      2
	BTFSS      STATUS+0, 2
	GOTO       L_main58
;AutoSAE_Safety.c,212 :: 		UART1_Write_Text("A 2\n");
	MOVLW      ?lstr11_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,213 :: 		PORTB.B6 = 1;
	BSF        PORTB+0, 6
;AutoSAE_Safety.c,214 :: 		Delay_ms(200);      // Wait for relay to pull in and power to stabilise.
	MOVLW      3
	MOVWF      R11+0
	MOVLW      8
	MOVWF      R12+0
	MOVLW      119
	MOVWF      R13+0
L_main59:
	DECFSZ     R13+0, 1
	GOTO       L_main59
	DECFSZ     R12+0, 1
	GOTO       L_main59
	DECFSZ     R11+0, 1
	GOTO       L_main59
;AutoSAE_Safety.c,215 :: 		arm_state = 3;
	MOVLW      3
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,216 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,217 :: 		UART1_Write_Text("A 3\n");
	MOVLW      ?lstr12_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,218 :: 		}
	GOTO       L_main60
L_main58:
;AutoSAE_Safety.c,219 :: 		else if(arm_state == 3) {
	MOVF       _arm_state+0, 0
	XORLW      3
	BTFSS      STATUS+0, 2
	GOTO       L_main61
;AutoSAE_Safety.c,220 :: 		if(PORTB.B3 == 1) { arm_state = -1; UART1_Write_Text("AF 1\n"); }   // Arm button
	BTFSS      PORTB+0, 3
	GOTO       L_main62
	MOVLW      255
	MOVWF      _arm_state+0
	MOVLW      ?lstr13_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main62:
;AutoSAE_Safety.c,221 :: 		if(Button(&PORTB,4,1,0)) { arm_state = -1; PORTB.B0 = 1; UART1_Write_Text("AF 2\n"); }  // Throttle
	MOVLW      PORTB+0
	MOVWF      FARG_Button_port+0
	MOVLW      4
	MOVWF      FARG_Button_pin+0
	MOVLW      1
	MOVWF      FARG_Button_time_ms+0
	CLRF       FARG_Button_active_state+0
	CALL       _Button+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main63
	MOVLW      255
	MOVWF      _arm_state+0
	BSF        PORTB+0, 0
	MOVLW      ?lstr14_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main63:
;AutoSAE_Safety.c,222 :: 		}
	GOTO       L_main64
L_main61:
;AutoSAE_Safety.c,223 :: 		else if(arm_state == 4) {
	MOVF       _arm_state+0, 0
	XORLW      4
	BTFSS      STATUS+0, 2
	GOTO       L_main65
;AutoSAE_Safety.c,224 :: 		UART1_Write_Text("A 4\n");
	MOVLW      ?lstr15_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,225 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,226 :: 		arm_state = 5;
	MOVLW      5
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,227 :: 		UART1_Write_Text("A 5\n");
	MOVLW      ?lstr16_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,228 :: 		}
	GOTO       L_main66
L_main65:
;AutoSAE_Safety.c,229 :: 		else if(arm_state == 5 || arm_state == 7) {
	MOVF       _arm_state+0, 0
	XORLW      5
	BTFSC      STATUS+0, 2
	GOTO       L__main83
	MOVF       _arm_state+0, 0
	XORLW      7
	BTFSC      STATUS+0, 2
	GOTO       L__main83
	GOTO       L_main69
L__main83:
;AutoSAE_Safety.c,230 :: 		if(Button(&PORTB,4,1,0)) { arm_state = -1; PORTB.B0 = 1; UART1_Write_Text("AF 3\n"); }      // Throttle
	MOVLW      PORTB+0
	MOVWF      FARG_Button_port+0
	MOVLW      4
	MOVWF      FARG_Button_pin+0
	MOVLW      1
	MOVWF      FARG_Button_time_ms+0
	CLRF       FARG_Button_active_state+0
	CALL       _Button+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main70
	MOVLW      255
	MOVWF      _arm_state+0
	BSF        PORTB+0, 0
	MOVLW      ?lstr17_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main70:
;AutoSAE_Safety.c,231 :: 		}
	GOTO       L_main71
L_main69:
;AutoSAE_Safety.c,232 :: 		else if(arm_state == 6 && PORTB.B3 == 1) {
	MOVF       _arm_state+0, 0
	XORLW      6
	BTFSS      STATUS+0, 2
	GOTO       L_main74
	BTFSS      PORTB+0, 3
	GOTO       L_main74
L__main82:
;AutoSAE_Safety.c,233 :: 		UART1_Write_Text("A 6\n");
	MOVLW      ?lstr18_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,234 :: 		PORTA.B1 = 1;              // Brake IL off
	BSF        PORTA+0, 1
;AutoSAE_Safety.c,235 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,236 :: 		arm_state = 7;
	MOVLW      7
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,237 :: 		UART1_Write_Text("A 7\n");
	MOVLW      ?lstr19_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,238 :: 		}
	GOTO       L_main75
L_main74:
;AutoSAE_Safety.c,239 :: 		else if(arm_state == 8) {
	MOVF       _arm_state+0, 0
	XORLW      8
	BTFSS      STATUS+0, 2
	GOTO       L_main76
;AutoSAE_Safety.c,240 :: 		UART1_Write_Text("A 9\n");
	MOVLW      ?lstr20_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,241 :: 		arm_state = 9;
	MOVLW      9
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,242 :: 		}
	GOTO       L_main77
L_main76:
;AutoSAE_Safety.c,243 :: 		else if(arm_state == -1) {
	MOVF       _arm_state+0, 0
	XORLW      255
	BTFSS      STATUS+0, 2
	GOTO       L_main78
;AutoSAE_Safety.c,244 :: 		PORTB.B6 = 0;
	BCF        PORTB+0, 6
;AutoSAE_Safety.c,245 :: 		if(PORTB.B3 == 1) { arm_state = 0; }
	BTFSS      PORTB+0, 3
	GOTO       L_main79
	CLRF       _arm_state+0
L_main79:
;AutoSAE_Safety.c,246 :: 		}
L_main78:
L_main77:
L_main75:
L_main71:
L_main66:
L_main64:
L_main60:
L_main57:
L_main54:
;AutoSAE_Safety.c,248 :: 		}
	GOTO       L_main30
;AutoSAE_Safety.c,250 :: 		}
L_end_main:
	GOTO       $+0
; end of _main
