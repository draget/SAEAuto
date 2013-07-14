
_trip:

;AutoSAE_Safety.c,13 :: 		void trip() {
;AutoSAE_Safety.c,15 :: 		tripstate = 1;
	MOVLW      1
	MOVWF      _tripstate+0
;AutoSAE_Safety.c,17 :: 		PORTB.B6 = 0;    // Open trip circuit
	BCF        PORTB+0, 6
;AutoSAE_Safety.c,18 :: 		PORTA.B4 = 1;    // E-stop LED on
	BSF        PORTA+0, 4
;AutoSAE_Safety.c,19 :: 		if(brakeil == 1) { PORTA.B1 = 1; }     // Activate brake interlock
	MOVF       _brakeil+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_trip0
	BSF        PORTA+0, 1
L_trip0:
;AutoSAE_Safety.c,21 :: 		if(hb_trip == 1) { PORTA.B3 = 1; }
	MOVF       _hb_trip+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_trip1
	BSF        PORTA+0, 3
	GOTO       L_trip2
L_trip1:
;AutoSAE_Safety.c,22 :: 		else { PORTA.B3 = 0; }
	BCF        PORTA+0, 3
L_trip2:
;AutoSAE_Safety.c,24 :: 		UART1_Write_Text("TRIP\n");
	MOVLW      ?lstr1_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,26 :: 		arm_state = -1;
	MOVLW      255
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,27 :: 		tripreq = 0;
	CLRF       _tripreq+0
;AutoSAE_Safety.c,29 :: 		}
L_end_trip:
	RETURN
; end of _trip

_interrupt:
	MOVWF      R15+0
	SWAPF      STATUS+0, 0
	CLRF       STATUS+0
	MOVWF      ___saveSTATUS+0
	MOVF       PCLATH+0, 0
	MOVWF      ___savePCLATH+0
	CLRF       PCLATH+0

;AutoSAE_Safety.c,32 :: 		void interrupt() {
;AutoSAE_Safety.c,34 :: 		if(hb_interruptcount > 12) {
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _hb_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt80
	MOVF       _hb_interruptcount+0, 0
	SUBLW      12
L__interrupt80:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt3
;AutoSAE_Safety.c,35 :: 		if(arm_state > 0) {
	MOVLW      128
	XORLW      0
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_state+0, 0
	SUBWF      R0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt4
;AutoSAE_Safety.c,36 :: 		hb_trip = 1;    // HB LED constant on
	MOVLW      1
	MOVWF      _hb_trip+0
;AutoSAE_Safety.c,37 :: 		tripreq = 1;
	MOVLW      1
	MOVWF      _tripreq+0
;AutoSAE_Safety.c,38 :: 		}
	GOTO       L_interrupt5
L_interrupt4:
;AutoSAE_Safety.c,39 :: 		else { hb_interruptcount = 0; }
	CLRF       _hb_interruptcount+0
	CLRF       _hb_interruptcount+1
L_interrupt5:
;AutoSAE_Safety.c,40 :: 		}
	GOTO       L_interrupt6
L_interrupt3:
;AutoSAE_Safety.c,41 :: 		else { hb_interruptcount++; }
	INCF       _hb_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _hb_interruptcount+1, 1
L_interrupt6:
;AutoSAE_Safety.c,43 :: 		if(arm_state == 1) {
	MOVF       _arm_state+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt7
;AutoSAE_Safety.c,44 :: 		if(arm_interruptcount > 200) { arm_state = 2; }
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt81
	MOVF       _arm_interruptcount+0, 0
	SUBLW      200
L__interrupt81:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt8
	MOVLW      2
	MOVWF      _arm_state+0
	GOTO       L_interrupt9
L_interrupt8:
;AutoSAE_Safety.c,45 :: 		else { arm_interruptcount++; }
	INCF       _arm_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _arm_interruptcount+1, 1
L_interrupt9:
;AutoSAE_Safety.c,46 :: 		}
	GOTO       L_interrupt10
L_interrupt7:
;AutoSAE_Safety.c,47 :: 		else if(arm_state == 3) {
	MOVF       _arm_state+0, 0
	XORLW      3
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt11
;AutoSAE_Safety.c,48 :: 		if(arm_interruptcount > 200) { arm_state = 4; }
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt82
	MOVF       _arm_interruptcount+0, 0
	SUBLW      200
L__interrupt82:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt12
	MOVLW      4
	MOVWF      _arm_state+0
	GOTO       L_interrupt13
L_interrupt12:
;AutoSAE_Safety.c,49 :: 		else { arm_interruptcount++; }
	INCF       _arm_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _arm_interruptcount+1, 1
L_interrupt13:
;AutoSAE_Safety.c,50 :: 		}
	GOTO       L_interrupt14
L_interrupt11:
;AutoSAE_Safety.c,51 :: 		else if(arm_state == 5) {
	MOVF       _arm_state+0, 0
	XORLW      5
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt15
;AutoSAE_Safety.c,52 :: 		if(arm_interruptcount > 500) { arm_state = 6; }
	MOVLW      128
	XORLW      1
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt83
	MOVF       _arm_interruptcount+0, 0
	SUBLW      244
L__interrupt83:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt16
	MOVLW      6
	MOVWF      _arm_state+0
	GOTO       L_interrupt17
L_interrupt16:
;AutoSAE_Safety.c,53 :: 		else { arm_interruptcount++; }
	INCF       _arm_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _arm_interruptcount+1, 1
L_interrupt17:
;AutoSAE_Safety.c,54 :: 		}
	GOTO       L_interrupt18
L_interrupt15:
;AutoSAE_Safety.c,55 :: 		else if(arm_state == 7) {
	MOVF       _arm_state+0, 0
	XORLW      7
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt19
;AutoSAE_Safety.c,56 :: 		if(arm_interruptcount > 100) { arm_state = 8; }
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt84
	MOVF       _arm_interruptcount+0, 0
	SUBLW      100
L__interrupt84:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt20
	MOVLW      8
	MOVWF      _arm_state+0
	GOTO       L_interrupt21
L_interrupt20:
;AutoSAE_Safety.c,57 :: 		else { arm_interruptcount++; }
	INCF       _arm_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _arm_interruptcount+1, 1
L_interrupt21:
;AutoSAE_Safety.c,58 :: 		}
	GOTO       L_interrupt22
L_interrupt19:
;AutoSAE_Safety.c,59 :: 		else if(arm_state == 9 || arm_state == 0) { arm_interruptcount = 0; }
	MOVF       _arm_state+0, 0
	XORLW      9
	BTFSC      STATUS+0, 2
	GOTO       L__interrupt72
	MOVF       _arm_state+0, 0
	XORLW      0
	BTFSC      STATUS+0, 2
	GOTO       L__interrupt72
	GOTO       L_interrupt25
L__interrupt72:
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
L_interrupt25:
L_interrupt22:
L_interrupt18:
L_interrupt14:
L_interrupt10:
;AutoSAE_Safety.c,62 :: 		TMR0 = 100;
	MOVLW      100
	MOVWF      TMR0+0
;AutoSAE_Safety.c,63 :: 		INTCON.TMR0IF = 0;
	BCF        INTCON+0, 2
;AutoSAE_Safety.c,64 :: 		}
L_end_interrupt:
L__interrupt79:
	MOVF       ___savePCLATH+0, 0
	MOVWF      PCLATH+0
	SWAPF      ___saveSTATUS+0, 0
	MOVWF      STATUS+0
	SWAPF      R15+0, 1
	SWAPF      R15+0, 0
	RETFIE
; end of _interrupt

_main:

;AutoSAE_Safety.c,68 :: 		void main() {
;AutoSAE_Safety.c,70 :: 		OSCCON = 0b1111110;    // Set up internal oscillator.
	MOVLW      126
	MOVWF      OSCCON+0
;AutoSAE_Safety.c,72 :: 		UART1_Init(9600);       // Init hardware UART
	MOVLW      51
	MOVWF      SPBRG+0
	BSF        TXSTA+0, 2
	CALL       _UART1_Init+0
;AutoSAE_Safety.c,73 :: 		Delay_ms(500);
	MOVLW      6
	MOVWF      R11+0
	MOVLW      19
	MOVWF      R12+0
	MOVLW      173
	MOVWF      R13+0
L_main26:
	DECFSZ     R13+0, 1
	GOTO       L_main26
	DECFSZ     R12+0, 1
	GOTO       L_main26
	DECFSZ     R11+0, 1
	GOTO       L_main26
	NOP
	NOP
;AutoSAE_Safety.c,75 :: 		UART1_Write_Text("Hi\n");
	MOVLW      ?lstr2_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,77 :: 		ANSEL  = 0;            // Set PortA pins to digital.
	CLRF       ANSEL+0
;AutoSAE_Safety.c,78 :: 		CMCON = 0x07;
	MOVLW      7
	MOVWF      CMCON+0
;AutoSAE_Safety.c,81 :: 		TRISA = 0b11100101;
	MOVLW      229
	MOVWF      TRISA+0
;AutoSAE_Safety.c,82 :: 		TRISB = 0b10111100;
	MOVLW      188
	MOVWF      TRISB+0
;AutoSAE_Safety.c,85 :: 		PORTA = 0;
	CLRF       PORTA+0
;AutoSAE_Safety.c,86 :: 		PORTB = 0;
	CLRF       PORTB+0
;AutoSAE_Safety.c,89 :: 		INTCON.TMR0IE = 1;
	BSF        INTCON+0, 5
;AutoSAE_Safety.c,90 :: 		INTCON.TMR0IF = 0;
	BCF        INTCON+0, 2
;AutoSAE_Safety.c,91 :: 		OPTION_REG.T0CS = 0;  // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock
	BCF        OPTION_REG+0, 5
;AutoSAE_Safety.c,92 :: 		OPTION_REG.T0SE = 0;  // bit 4 TMR0 Source Edge Select bit 0 = low/high
	BCF        OPTION_REG+0, 4
;AutoSAE_Safety.c,93 :: 		OPTION_REG.PSA = 0;   // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
	BCF        OPTION_REG+0, 3
;AutoSAE_Safety.c,94 :: 		OPTION_REG.PS2 = 1;   // bits 2-0  PS2:PS0: Prescaler Rate Select bits
	BSF        OPTION_REG+0, 2
;AutoSAE_Safety.c,95 :: 		OPTION_REG.PS1 = 1;
	BSF        OPTION_REG+0, 1
;AutoSAE_Safety.c,96 :: 		OPTION_REG.PS0 = 1;
	BSF        OPTION_REG+0, 0
;AutoSAE_Safety.c,97 :: 		TMR0 = 100;             // preset for timer register
	MOVLW      100
	MOVWF      TMR0+0
;AutoSAE_Safety.c,99 :: 		INTCON.GIE = 1;
	BSF        INTCON+0, 7
;AutoSAE_Safety.c,102 :: 		UART1_Write_Text("Init ok\n");
	MOVLW      ?lstr3_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,105 :: 		while(1) {
L_main27:
;AutoSAE_Safety.c,107 :: 		if(tripreq == 1) { trip(); }
	MOVF       _tripreq+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main29
	CALL       _trip+0
L_main29:
;AutoSAE_Safety.c,110 :: 		if (UART1_Data_Ready()) {
	CALL       _UART1_Data_Ready+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main30
;AutoSAE_Safety.c,111 :: 		uart_rd = UART1_Read();
	CALL       _UART1_Read+0
	MOVF       R0+0, 0
	MOVWF      _uart_rd+0
;AutoSAE_Safety.c,113 :: 		if(uart_rd == 'E' && tripstate == 0) { trip(); }
	MOVF       R0+0, 0
	XORLW      69
	BTFSS      STATUS+0, 2
	GOTO       L_main33
	MOVF       _tripstate+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main33
L__main76:
	CALL       _trip+0
L_main33:
;AutoSAE_Safety.c,115 :: 		oldhbstate = hbstate;
	MOVF       _hbstate+0, 0
	MOVWF      _oldhbstate+0
;AutoSAE_Safety.c,117 :: 		if(uart_rd == '+') { hbstate = 1; }
	MOVF       _uart_rd+0, 0
	XORLW      43
	BTFSS      STATUS+0, 2
	GOTO       L_main34
	MOVLW      1
	MOVWF      _hbstate+0
L_main34:
;AutoSAE_Safety.c,118 :: 		if(uart_rd == '-') { hbstate = 0; }
	MOVF       _uart_rd+0, 0
	XORLW      45
	BTFSS      STATUS+0, 2
	GOTO       L_main35
	CLRF       _hbstate+0
L_main35:
;AutoSAE_Safety.c,120 :: 		if(uart_rd == 'B') { brakeil = 1; }
	MOVF       _uart_rd+0, 0
	XORLW      66
	BTFSS      STATUS+0, 2
	GOTO       L_main36
	MOVLW      1
	MOVWF      _brakeil+0
L_main36:
;AutoSAE_Safety.c,121 :: 		if(uart_rd == 'H') { brakeil = 0; }
	MOVF       _uart_rd+0, 0
	XORLW      72
	BTFSS      STATUS+0, 2
	GOTO       L_main37
	CLRF       _brakeil+0
L_main37:
;AutoSAE_Safety.c,123 :: 		if(oldhbstate != hbstate) { hb_interruptcount = 0; }
	MOVF       _oldhbstate+0, 0
	XORWF      _hbstate+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main38
	CLRF       _hb_interruptcount+0
	CLRF       _hb_interruptcount+1
L_main38:
;AutoSAE_Safety.c,125 :: 		UART1_Write_Text("ACK ");
	MOVLW      ?lstr4_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,126 :: 		UART1_Write(uart_rd);
	MOVF       _uart_rd+0, 0
	MOVWF      FARG_UART1_Write_data_+0
	CALL       _UART1_Write+0
;AutoSAE_Safety.c,127 :: 		UART1_Write_Text("\n");
	MOVLW      ?lstr5_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,128 :: 		}
L_main30:
;AutoSAE_Safety.c,130 :: 		if(tripstate == 0) { PORTA.B3 = hbstate; }
	MOVF       _tripstate+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main39
	BTFSC      _hbstate+0, 0
	GOTO       L__main86
	BCF        PORTA+0, 3
	GOTO       L__main87
L__main86:
	BSF        PORTA+0, 3
L__main87:
L_main39:
;AutoSAE_Safety.c,131 :: 		PORTB.B1 = brakeil;
	BTFSC      _brakeil+0, 0
	GOTO       L__main88
	BCF        PORTB+0, 1
	GOTO       L__main89
L__main88:
	BSF        PORTB+0, 1
L__main89:
;AutoSAE_Safety.c,133 :: 		if(arm_state > 0 && PORTA.B2 == 0 && tripstate == 0) {        // Dash e-stop
	MOVLW      128
	XORLW      0
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_state+0, 0
	SUBWF      R0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main42
	BTFSC      PORTA+0, 2
	GOTO       L_main42
	MOVF       _tripstate+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main42
L__main75:
;AutoSAE_Safety.c,134 :: 		UART1_Write_Text("DES\n");
	MOVLW      ?lstr6_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,135 :: 		trip();
	CALL       _trip+0
;AutoSAE_Safety.c,136 :: 		}
L_main42:
;AutoSAE_Safety.c,138 :: 		if(arm_state == 0) {
	MOVF       _arm_state+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main43
;AutoSAE_Safety.c,140 :: 		if(Button(&PORTB,3,20,0)) {
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
	GOTO       L_main44
;AutoSAE_Safety.c,141 :: 		UART1_Write_Text("AR\n");
	MOVLW      ?lstr7_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,142 :: 		if(brakeil == 1) { PORTA.B1 = 1; }     // Activate brake interlock
	MOVF       _brakeil+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main45
	BSF        PORTA+0, 1
L_main45:
;AutoSAE_Safety.c,143 :: 		PORTB.B0 = 0;                          // Throttle LED
	BCF        PORTB+0, 0
;AutoSAE_Safety.c,144 :: 		PORTA.B4 = 0;                          // E-stop LED
	BCF        PORTA+0, 4
;AutoSAE_Safety.c,145 :: 		PORTB.B6 = 0;                          // Trip relay
	BCF        PORTB+0, 6
;AutoSAE_Safety.c,146 :: 		tripstate = 0;
	CLRF       _tripstate+0
;AutoSAE_Safety.c,147 :: 		hb_trip = 0;
	CLRF       _hb_trip+0
;AutoSAE_Safety.c,148 :: 		arm_state = 1;
	MOVLW      1
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,149 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,150 :: 		hb_interruptcount = 0;
	CLRF       _hb_interruptcount+0
	CLRF       _hb_interruptcount+1
;AutoSAE_Safety.c,151 :: 		UART1_Write_Text("A 1\n");
	MOVLW      ?lstr8_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,152 :: 		}
L_main44:
;AutoSAE_Safety.c,153 :: 		}
	GOTO       L_main46
L_main43:
;AutoSAE_Safety.c,154 :: 		else if(arm_state == 1) {
	MOVF       _arm_state+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main47
;AutoSAE_Safety.c,155 :: 		if(PORTB.B3 == 1) { arm_state = -1; UART1_Write_Text("AF 1\n"); }   // Arm button
	BTFSS      PORTB+0, 3
	GOTO       L_main48
	MOVLW      255
	MOVWF      _arm_state+0
	MOVLW      ?lstr9_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main48:
;AutoSAE_Safety.c,156 :: 		}
	GOTO       L_main49
L_main47:
;AutoSAE_Safety.c,157 :: 		else if(arm_state == 2) {
	MOVF       _arm_state+0, 0
	XORLW      2
	BTFSS      STATUS+0, 2
	GOTO       L_main50
;AutoSAE_Safety.c,158 :: 		UART1_Write_Text("A 2\n");
	MOVLW      ?lstr10_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,159 :: 		PORTB.B6 = 1;
	BSF        PORTB+0, 6
;AutoSAE_Safety.c,160 :: 		Delay_ms(40);      // Wait for relay & contactor to pull in.
	MOVLW      104
	MOVWF      R12+0
	MOVLW      228
	MOVWF      R13+0
L_main51:
	DECFSZ     R13+0, 1
	GOTO       L_main51
	DECFSZ     R12+0, 1
	GOTO       L_main51
	NOP
;AutoSAE_Safety.c,161 :: 		arm_state = 3;
	MOVLW      3
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,162 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,163 :: 		UART1_Write_Text("A 3\n");
	MOVLW      ?lstr11_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,164 :: 		}
	GOTO       L_main52
L_main50:
;AutoSAE_Safety.c,165 :: 		else if(arm_state == 3) {
	MOVF       _arm_state+0, 0
	XORLW      3
	BTFSS      STATUS+0, 2
	GOTO       L_main53
;AutoSAE_Safety.c,166 :: 		if(PORTB.B3 == 1) { arm_state = -1; UART1_Write_Text("AF 1\n"); }   // Arm button
	BTFSS      PORTB+0, 3
	GOTO       L_main54
	MOVLW      255
	MOVWF      _arm_state+0
	MOVLW      ?lstr12_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main54:
;AutoSAE_Safety.c,167 :: 		if(PORTB.B4 == 0) { arm_state = -1; PORTB.B0 = 1; UART1_Write_Text("AF 2\n"); }  // Throttle
	BTFSC      PORTB+0, 4
	GOTO       L_main55
	MOVLW      255
	MOVWF      _arm_state+0
	BSF        PORTB+0, 0
	MOVLW      ?lstr13_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main55:
;AutoSAE_Safety.c,168 :: 		}
	GOTO       L_main56
L_main53:
;AutoSAE_Safety.c,169 :: 		else if(arm_state == 4) {
	MOVF       _arm_state+0, 0
	XORLW      4
	BTFSS      STATUS+0, 2
	GOTO       L_main57
;AutoSAE_Safety.c,170 :: 		UART1_Write_Text("A 4\n");
	MOVLW      ?lstr14_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,171 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,172 :: 		arm_state = 5;
	MOVLW      5
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,173 :: 		UART1_Write_Text("A 5\n");
	MOVLW      ?lstr15_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,174 :: 		}
	GOTO       L_main58
L_main57:
;AutoSAE_Safety.c,175 :: 		else if(arm_state == 5 || arm_state == 7) {
	MOVF       _arm_state+0, 0
	XORLW      5
	BTFSC      STATUS+0, 2
	GOTO       L__main74
	MOVF       _arm_state+0, 0
	XORLW      7
	BTFSC      STATUS+0, 2
	GOTO       L__main74
	GOTO       L_main61
L__main74:
;AutoSAE_Safety.c,176 :: 		if(PORTB.B4 == 0) { arm_state = -1; PORTB.B0 = 1; UART1_Write_Text("AF 3\n"); }      // Throttle
	BTFSC      PORTB+0, 4
	GOTO       L_main62
	MOVLW      255
	MOVWF      _arm_state+0
	BSF        PORTB+0, 0
	MOVLW      ?lstr16_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main62:
;AutoSAE_Safety.c,177 :: 		}
	GOTO       L_main63
L_main61:
;AutoSAE_Safety.c,178 :: 		else if(arm_state == 6 && PORTB.B3 == 1) {
	MOVF       _arm_state+0, 0
	XORLW      6
	BTFSS      STATUS+0, 2
	GOTO       L_main66
	BTFSS      PORTB+0, 3
	GOTO       L_main66
L__main73:
;AutoSAE_Safety.c,179 :: 		UART1_Write_Text("A 6\n");
	MOVLW      ?lstr17_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,180 :: 		PORTA.B1 = 0;
	BCF        PORTA+0, 1
;AutoSAE_Safety.c,181 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,182 :: 		arm_state = 7;
	MOVLW      7
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,183 :: 		UART1_Write_Text("A 7\n");
	MOVLW      ?lstr18_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,184 :: 		}
	GOTO       L_main67
L_main66:
;AutoSAE_Safety.c,185 :: 		else if(arm_state == 8) {
	MOVF       _arm_state+0, 0
	XORLW      8
	BTFSS      STATUS+0, 2
	GOTO       L_main68
;AutoSAE_Safety.c,186 :: 		UART1_Write_Text("A 9\n");
	MOVLW      ?lstr19_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,187 :: 		arm_state = 9;
	MOVLW      9
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,188 :: 		}
	GOTO       L_main69
L_main68:
;AutoSAE_Safety.c,189 :: 		else if(arm_state == -1) {
	MOVF       _arm_state+0, 0
	XORLW      255
	BTFSS      STATUS+0, 2
	GOTO       L_main70
;AutoSAE_Safety.c,190 :: 		if(PORTB.B3 == 1) { PORTB.B6 = 0; arm_state = 0; }
	BTFSS      PORTB+0, 3
	GOTO       L_main71
	BCF        PORTB+0, 6
	CLRF       _arm_state+0
L_main71:
;AutoSAE_Safety.c,191 :: 		}
L_main70:
L_main69:
L_main67:
L_main63:
L_main58:
L_main56:
L_main52:
L_main49:
L_main46:
;AutoSAE_Safety.c,193 :: 		}
	GOTO       L_main27
;AutoSAE_Safety.c,195 :: 		}
L_end_main:
	GOTO       $+0
; end of _main
