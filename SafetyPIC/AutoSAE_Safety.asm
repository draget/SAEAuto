
_trip:

;AutoSAE_Safety.c,14 :: 		void trip() {
;AutoSAE_Safety.c,16 :: 		tripstate = 1;
	MOVLW      1
	MOVWF      _tripstate+0
;AutoSAE_Safety.c,18 :: 		PORTB.B6 = 0;    // Open trip circuit
	BCF        PORTB+0, 6
;AutoSAE_Safety.c,19 :: 		PORTA.B4 = 1;    // E-stop LED on
	BSF        PORTA+0, 4
;AutoSAE_Safety.c,20 :: 		PORTA.B0 = 1;    // Alarm on
	BSF        PORTA+0, 0
;AutoSAE_Safety.c,21 :: 		if(brakeil == 1) { PORTA.B1 = 0; }     // Activate brake interlock
	MOVF       _brakeil+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_trip0
	BCF        PORTA+0, 1
L_trip0:
;AutoSAE_Safety.c,23 :: 		if(hb_trip == 1) { PORTA.B3 = 1; }
	MOVF       _hb_trip+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_trip1
	BSF        PORTA+0, 3
	GOTO       L_trip2
L_trip1:
;AutoSAE_Safety.c,24 :: 		else { PORTA.B3 = 0; }
	BCF        PORTA+0, 3
L_trip2:
;AutoSAE_Safety.c,26 :: 		UART1_Write_Text("TRIP\n");
	MOVLW      ?lstr1_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,28 :: 		arm_state = -1;
	MOVLW      255
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,29 :: 		tripreq = 0;
	CLRF       _tripreq+0
;AutoSAE_Safety.c,31 :: 		}
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

;AutoSAE_Safety.c,34 :: 		void interrupt() {
;AutoSAE_Safety.c,36 :: 		if(hb_interruptcount > 30) {
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _hb_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt83
	MOVF       _hb_interruptcount+0, 0
	SUBLW      30
L__interrupt83:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt3
;AutoSAE_Safety.c,37 :: 		if(arm_state > 0) {
	MOVLW      128
	XORLW      0
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_state+0, 0
	SUBWF      R0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt4
;AutoSAE_Safety.c,38 :: 		hb_trip = 1;    // HB LED constant on
	MOVLW      1
	MOVWF      _hb_trip+0
;AutoSAE_Safety.c,39 :: 		tripreq = 1;
	MOVLW      1
	MOVWF      _tripreq+0
;AutoSAE_Safety.c,40 :: 		}
	GOTO       L_interrupt5
L_interrupt4:
;AutoSAE_Safety.c,41 :: 		else { hb_interruptcount = 0; }
	CLRF       _hb_interruptcount+0
	CLRF       _hb_interruptcount+1
L_interrupt5:
;AutoSAE_Safety.c,42 :: 		}
	GOTO       L_interrupt6
L_interrupt3:
;AutoSAE_Safety.c,43 :: 		else { hb_interruptcount++; }
	INCF       _hb_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _hb_interruptcount+1, 1
L_interrupt6:
;AutoSAE_Safety.c,45 :: 		if(arm_state == 1) {
	MOVF       _arm_state+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt7
;AutoSAE_Safety.c,46 :: 		if(arm_interruptcount > 200) { arm_state = 2; }
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt84
	MOVF       _arm_interruptcount+0, 0
	SUBLW      200
L__interrupt84:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt8
	MOVLW      2
	MOVWF      _arm_state+0
	GOTO       L_interrupt9
L_interrupt8:
;AutoSAE_Safety.c,47 :: 		else { arm_interruptcount++; }
	INCF       _arm_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _arm_interruptcount+1, 1
L_interrupt9:
;AutoSAE_Safety.c,48 :: 		}
	GOTO       L_interrupt10
L_interrupt7:
;AutoSAE_Safety.c,49 :: 		else if(arm_state == 3) {
	MOVF       _arm_state+0, 0
	XORLW      3
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt11
;AutoSAE_Safety.c,50 :: 		if(arm_interruptcount > 200) { arm_state = 4; }
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt85
	MOVF       _arm_interruptcount+0, 0
	SUBLW      200
L__interrupt85:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt12
	MOVLW      4
	MOVWF      _arm_state+0
	GOTO       L_interrupt13
L_interrupt12:
;AutoSAE_Safety.c,51 :: 		else { arm_interruptcount++; }
	INCF       _arm_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _arm_interruptcount+1, 1
L_interrupt13:
;AutoSAE_Safety.c,52 :: 		}
	GOTO       L_interrupt14
L_interrupt11:
;AutoSAE_Safety.c,53 :: 		else if(arm_state == 5) {
	MOVF       _arm_state+0, 0
	XORLW      5
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt15
;AutoSAE_Safety.c,54 :: 		if(arm_interruptcount > 500) { arm_state = 6; }
	MOVLW      128
	XORLW      1
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt86
	MOVF       _arm_interruptcount+0, 0
	SUBLW      244
L__interrupt86:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt16
	MOVLW      6
	MOVWF      _arm_state+0
	GOTO       L_interrupt17
L_interrupt16:
;AutoSAE_Safety.c,55 :: 		else { arm_interruptcount++; }
	INCF       _arm_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _arm_interruptcount+1, 1
L_interrupt17:
;AutoSAE_Safety.c,56 :: 		}
	GOTO       L_interrupt18
L_interrupt15:
;AutoSAE_Safety.c,57 :: 		else if(arm_state == 7) {
	MOVF       _arm_state+0, 0
	XORLW      7
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt19
;AutoSAE_Safety.c,58 :: 		if(arm_interruptcount > 100) { arm_state = 8; }
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt87
	MOVF       _arm_interruptcount+0, 0
	SUBLW      100
L__interrupt87:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt20
	MOVLW      8
	MOVWF      _arm_state+0
	GOTO       L_interrupt21
L_interrupt20:
;AutoSAE_Safety.c,59 :: 		else { arm_interruptcount++; }
	INCF       _arm_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _arm_interruptcount+1, 1
L_interrupt21:
;AutoSAE_Safety.c,60 :: 		}
	GOTO       L_interrupt22
L_interrupt19:
;AutoSAE_Safety.c,61 :: 		else if(arm_state == 9 || arm_state == 0) { arm_interruptcount = 0; }
	MOVF       _arm_state+0, 0
	XORLW      9
	BTFSC      STATUS+0, 2
	GOTO       L__interrupt75
	MOVF       _arm_state+0, 0
	XORLW      0
	BTFSC      STATUS+0, 2
	GOTO       L__interrupt75
	GOTO       L_interrupt25
L__interrupt75:
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
L_interrupt25:
L_interrupt22:
L_interrupt18:
L_interrupt14:
L_interrupt10:
;AutoSAE_Safety.c,63 :: 		if(alarm_counter > 0) { alarm_counter--; }
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _alarm_counter+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt88
	MOVF       _alarm_counter+0, 0
	SUBLW      0
L__interrupt88:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt26
	MOVLW      1
	SUBWF      _alarm_counter+0, 1
	BTFSS      STATUS+0, 0
	DECF       _alarm_counter+1, 1
L_interrupt26:
;AutoSAE_Safety.c,65 :: 		TMR0 = 100;
	MOVLW      100
	MOVWF      TMR0+0
;AutoSAE_Safety.c,66 :: 		INTCON.TMR0IF = 0;
	BCF        INTCON+0, 2
;AutoSAE_Safety.c,67 :: 		}
L_end_interrupt:
L__interrupt82:
	MOVF       ___savePCLATH+0, 0
	MOVWF      PCLATH+0
	SWAPF      ___saveSTATUS+0, 0
	MOVWF      STATUS+0
	SWAPF      R15+0, 1
	SWAPF      R15+0, 0
	RETFIE
; end of _interrupt

_main:

;AutoSAE_Safety.c,71 :: 		void main() {
;AutoSAE_Safety.c,73 :: 		OSCCON = 0b1111110;    // Set up internal oscillator.
	MOVLW      126
	MOVWF      OSCCON+0
;AutoSAE_Safety.c,75 :: 		WDTCON.WDTPS0 = 0;
	BCF        WDTCON+0, 1
;AutoSAE_Safety.c,76 :: 		WDTCON.WDTPS1 = 0;
	BCF        WDTCON+0, 2
;AutoSAE_Safety.c,77 :: 		WDTCON.WDTPS2 = 0;
	BCF        WDTCON+0, 3
;AutoSAE_Safety.c,78 :: 		WDTCON.WDTPS3 = 1;
	BSF        WDTCON+0, 4
;AutoSAE_Safety.c,80 :: 		UART1_Init(38400);       // Init hardware UART
	MOVLW      12
	MOVWF      SPBRG+0
	BSF        TXSTA+0, 2
	CALL       _UART1_Init+0
;AutoSAE_Safety.c,81 :: 		Delay_ms(500);
	MOVLW      6
	MOVWF      R11+0
	MOVLW      19
	MOVWF      R12+0
	MOVLW      173
	MOVWF      R13+0
L_main27:
	DECFSZ     R13+0, 1
	GOTO       L_main27
	DECFSZ     R12+0, 1
	GOTO       L_main27
	DECFSZ     R11+0, 1
	GOTO       L_main27
	NOP
	NOP
;AutoSAE_Safety.c,83 :: 		UART1_Write_Text("Hi\n");
	MOVLW      ?lstr2_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,85 :: 		ANSEL  = 0;            // Set PortA pins to digital.
	CLRF       ANSEL+0
;AutoSAE_Safety.c,86 :: 		CMCON = 0x07;
	MOVLW      7
	MOVWF      CMCON+0
;AutoSAE_Safety.c,89 :: 		TRISA = 0b11100100;
	MOVLW      228
	MOVWF      TRISA+0
;AutoSAE_Safety.c,90 :: 		TRISB = 0b10111100;
	MOVLW      188
	MOVWF      TRISB+0
;AutoSAE_Safety.c,93 :: 		PORTA = 0b00000010;          // Brake IL line is high by default.
	MOVLW      2
	MOVWF      PORTA+0
;AutoSAE_Safety.c,94 :: 		PORTB = 0;
	CLRF       PORTB+0
;AutoSAE_Safety.c,97 :: 		INTCON.TMR0IE = 1;
	BSF        INTCON+0, 5
;AutoSAE_Safety.c,98 :: 		INTCON.TMR0IF = 0;
	BCF        INTCON+0, 2
;AutoSAE_Safety.c,99 :: 		OPTION_REG.T0CS = 0;  // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock
	BCF        OPTION_REG+0, 5
;AutoSAE_Safety.c,100 :: 		OPTION_REG.T0SE = 0;  // bit 4 TMR0 Source Edge Select bit 0 = low/high
	BCF        OPTION_REG+0, 4
;AutoSAE_Safety.c,101 :: 		OPTION_REG.PSA = 0;   // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
	BCF        OPTION_REG+0, 3
;AutoSAE_Safety.c,102 :: 		OPTION_REG.PS2 = 1;   // bits 2-0  PS2:PS0: Prescaler Rate Select bits
	BSF        OPTION_REG+0, 2
;AutoSAE_Safety.c,103 :: 		OPTION_REG.PS1 = 1;
	BSF        OPTION_REG+0, 1
;AutoSAE_Safety.c,104 :: 		OPTION_REG.PS0 = 1;
	BSF        OPTION_REG+0, 0
;AutoSAE_Safety.c,105 :: 		TMR0 = 100;             // preset for timer register
	MOVLW      100
	MOVWF      TMR0+0
;AutoSAE_Safety.c,107 :: 		INTCON.GIE = 1;
	BSF        INTCON+0, 7
;AutoSAE_Safety.c,110 :: 		UART1_Write_Text("Init ok\n");
	MOVLW      ?lstr3_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,113 :: 		while(1) {
L_main28:
;AutoSAE_Safety.c,115 :: 		if(tripreq == 1) { trip(); }
	MOVF       _tripreq+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main30
	CALL       _trip+0
L_main30:
;AutoSAE_Safety.c,117 :: 		asm { CLRWDT; }
	CLRWDT
;AutoSAE_Safety.c,118 :: 		if (UART1_Data_Ready()) {
	CALL       _UART1_Data_Ready+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main31
;AutoSAE_Safety.c,119 :: 		uart_rd = UART1_Read();
	CALL       _UART1_Read+0
	MOVF       R0+0, 0
	MOVWF      _uart_rd+0
;AutoSAE_Safety.c,121 :: 		if(uart_rd == 'E' && tripstate == 0) { trip(); }
	MOVF       R0+0, 0
	XORLW      69
	BTFSS      STATUS+0, 2
	GOTO       L_main34
	MOVF       _tripstate+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main34
L__main79:
	CALL       _trip+0
L_main34:
;AutoSAE_Safety.c,123 :: 		oldhbstate = hbstate;
	MOVF       _hbstate+0, 0
	MOVWF      _oldhbstate+0
;AutoSAE_Safety.c,125 :: 		if(uart_rd == '+') { hbstate = 1; }
	MOVF       _uart_rd+0, 0
	XORLW      43
	BTFSS      STATUS+0, 2
	GOTO       L_main35
	MOVLW      1
	MOVWF      _hbstate+0
L_main35:
;AutoSAE_Safety.c,126 :: 		if(uart_rd == '-') { hbstate = 0; }
	MOVF       _uart_rd+0, 0
	XORLW      45
	BTFSS      STATUS+0, 2
	GOTO       L_main36
	CLRF       _hbstate+0
L_main36:
;AutoSAE_Safety.c,128 :: 		if(uart_rd == 'B') { brakeil = 1; }
	MOVF       _uart_rd+0, 0
	XORLW      66
	BTFSS      STATUS+0, 2
	GOTO       L_main37
	MOVLW      1
	MOVWF      _brakeil+0
L_main37:
;AutoSAE_Safety.c,129 :: 		if(uart_rd == 'H') { brakeil = 0; }
	MOVF       _uart_rd+0, 0
	XORLW      72
	BTFSS      STATUS+0, 2
	GOTO       L_main38
	CLRF       _brakeil+0
L_main38:
;AutoSAE_Safety.c,131 :: 		if(uart_rd == 'A') { alarm_counter = 25; PORTA.B0 = 1; }
	MOVF       _uart_rd+0, 0
	XORLW      65
	BTFSS      STATUS+0, 2
	GOTO       L_main39
	MOVLW      25
	MOVWF      _alarm_counter+0
	MOVLW      0
	MOVWF      _alarm_counter+1
	BSF        PORTA+0, 0
L_main39:
;AutoSAE_Safety.c,133 :: 		if(oldhbstate != hbstate) { hb_interruptcount = 0; }
	MOVF       _oldhbstate+0, 0
	XORWF      _hbstate+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main40
	CLRF       _hb_interruptcount+0
	CLRF       _hb_interruptcount+1
L_main40:
;AutoSAE_Safety.c,135 :: 		UART1_Write_Text("ACK ");
	MOVLW      ?lstr4_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,136 :: 		UART1_Write(uart_rd);
	MOVF       _uart_rd+0, 0
	MOVWF      FARG_UART1_Write_data_+0
	CALL       _UART1_Write+0
;AutoSAE_Safety.c,137 :: 		UART1_Write_Text("\n");
	MOVLW      ?lstr5_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,138 :: 		}
L_main31:
;AutoSAE_Safety.c,140 :: 		if(tripstate == 0) {
	MOVF       _tripstate+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main41
;AutoSAE_Safety.c,141 :: 		PORTA.B3 = hbstate;
	BTFSC      _hbstate+0, 0
	GOTO       L__main90
	BCF        PORTA+0, 3
	GOTO       L__main91
L__main90:
	BSF        PORTA+0, 3
L__main91:
;AutoSAE_Safety.c,142 :: 		if(alarm_counter == 0) { PORTA.B0 = 0; }
	MOVLW      0
	XORWF      _alarm_counter+1, 0
	BTFSS      STATUS+0, 2
	GOTO       L__main92
	MOVLW      0
	XORWF      _alarm_counter+0, 0
L__main92:
	BTFSS      STATUS+0, 2
	GOTO       L_main42
	BCF        PORTA+0, 0
L_main42:
;AutoSAE_Safety.c,143 :: 		}
L_main41:
;AutoSAE_Safety.c,145 :: 		PORTB.B1 = brakeil;
	BTFSC      _brakeil+0, 0
	GOTO       L__main93
	BCF        PORTB+0, 1
	GOTO       L__main94
L__main93:
	BSF        PORTB+0, 1
L__main94:
;AutoSAE_Safety.c,147 :: 		if(arm_state > 0 && PORTA.B2 == 0 && tripstate == 0) {        // Dash e-stop
	MOVLW      128
	XORLW      0
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_state+0, 0
	SUBWF      R0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main45
	BTFSC      PORTA+0, 2
	GOTO       L_main45
	MOVF       _tripstate+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main45
L__main78:
;AutoSAE_Safety.c,148 :: 		UART1_Write_Text("DES\n");
	MOVLW      ?lstr6_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,149 :: 		trip();
	CALL       _trip+0
;AutoSAE_Safety.c,150 :: 		}
L_main45:
;AutoSAE_Safety.c,152 :: 		if(arm_state == 0) {
	MOVF       _arm_state+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main46
;AutoSAE_Safety.c,154 :: 		if(Button(&PORTB,3,20,0)) {
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
	GOTO       L_main47
;AutoSAE_Safety.c,155 :: 		UART1_Write_Text("AR\n");
	MOVLW      ?lstr7_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,156 :: 		if(brakeil == 1) { PORTA.B1 = 0; }     // Activate brake interlock
	MOVF       _brakeil+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main48
	BCF        PORTA+0, 1
L_main48:
;AutoSAE_Safety.c,157 :: 		PORTB.B0 = 0;                          // Throttle LED
	BCF        PORTB+0, 0
;AutoSAE_Safety.c,158 :: 		PORTA.B4 = 0;                          // E-stop LED
	BCF        PORTA+0, 4
;AutoSAE_Safety.c,159 :: 		PORTA.B0 = 0;                          // Alarm off
	BCF        PORTA+0, 0
;AutoSAE_Safety.c,160 :: 		PORTB.B6 = 0;                          // Trip relay
	BCF        PORTB+0, 6
;AutoSAE_Safety.c,161 :: 		tripstate = 0;
	CLRF       _tripstate+0
;AutoSAE_Safety.c,162 :: 		hb_trip = 0;
	CLRF       _hb_trip+0
;AutoSAE_Safety.c,163 :: 		arm_state = 1;
	MOVLW      1
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,164 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,165 :: 		hb_interruptcount = 0;
	CLRF       _hb_interruptcount+0
	CLRF       _hb_interruptcount+1
;AutoSAE_Safety.c,166 :: 		UART1_Write_Text("A 1\n");
	MOVLW      ?lstr8_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,167 :: 		}
L_main47:
;AutoSAE_Safety.c,168 :: 		}
	GOTO       L_main49
L_main46:
;AutoSAE_Safety.c,169 :: 		else if(arm_state == 1) {
	MOVF       _arm_state+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main50
;AutoSAE_Safety.c,170 :: 		if(PORTB.B3 == 1) { arm_state = -1; UART1_Write_Text("AF 1\n"); }   // Arm button
	BTFSS      PORTB+0, 3
	GOTO       L_main51
	MOVLW      255
	MOVWF      _arm_state+0
	MOVLW      ?lstr9_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main51:
;AutoSAE_Safety.c,171 :: 		}
	GOTO       L_main52
L_main50:
;AutoSAE_Safety.c,172 :: 		else if(arm_state == 2) {
	MOVF       _arm_state+0, 0
	XORLW      2
	BTFSS      STATUS+0, 2
	GOTO       L_main53
;AutoSAE_Safety.c,173 :: 		UART1_Write_Text("A 2\n");
	MOVLW      ?lstr10_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,174 :: 		PORTB.B6 = 1;
	BSF        PORTB+0, 6
;AutoSAE_Safety.c,175 :: 		Delay_ms(200);      // Wait for relay to pull in and power to stabilise.
	MOVLW      3
	MOVWF      R11+0
	MOVLW      8
	MOVWF      R12+0
	MOVLW      119
	MOVWF      R13+0
L_main54:
	DECFSZ     R13+0, 1
	GOTO       L_main54
	DECFSZ     R12+0, 1
	GOTO       L_main54
	DECFSZ     R11+0, 1
	GOTO       L_main54
;AutoSAE_Safety.c,176 :: 		arm_state = 3;
	MOVLW      3
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,177 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,178 :: 		UART1_Write_Text("A 3\n");
	MOVLW      ?lstr11_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,179 :: 		}
	GOTO       L_main55
L_main53:
;AutoSAE_Safety.c,180 :: 		else if(arm_state == 3) {
	MOVF       _arm_state+0, 0
	XORLW      3
	BTFSS      STATUS+0, 2
	GOTO       L_main56
;AutoSAE_Safety.c,181 :: 		if(PORTB.B3 == 1) { arm_state = -1; UART1_Write_Text("AF 1\n"); }   // Arm button
	BTFSS      PORTB+0, 3
	GOTO       L_main57
	MOVLW      255
	MOVWF      _arm_state+0
	MOVLW      ?lstr12_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main57:
;AutoSAE_Safety.c,182 :: 		if(Button(&PORTB,4,1,0)) { arm_state = -1; PORTB.B0 = 1; UART1_Write_Text("AF 2\n"); }  // Throttle
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
	GOTO       L_main58
	MOVLW      255
	MOVWF      _arm_state+0
	BSF        PORTB+0, 0
	MOVLW      ?lstr13_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main58:
;AutoSAE_Safety.c,183 :: 		}
	GOTO       L_main59
L_main56:
;AutoSAE_Safety.c,184 :: 		else if(arm_state == 4) {
	MOVF       _arm_state+0, 0
	XORLW      4
	BTFSS      STATUS+0, 2
	GOTO       L_main60
;AutoSAE_Safety.c,185 :: 		UART1_Write_Text("A 4\n");
	MOVLW      ?lstr14_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,186 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,187 :: 		arm_state = 5;
	MOVLW      5
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,188 :: 		UART1_Write_Text("A 5\n");
	MOVLW      ?lstr15_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,189 :: 		}
	GOTO       L_main61
L_main60:
;AutoSAE_Safety.c,190 :: 		else if(arm_state == 5 || arm_state == 7) {
	MOVF       _arm_state+0, 0
	XORLW      5
	BTFSC      STATUS+0, 2
	GOTO       L__main77
	MOVF       _arm_state+0, 0
	XORLW      7
	BTFSC      STATUS+0, 2
	GOTO       L__main77
	GOTO       L_main64
L__main77:
;AutoSAE_Safety.c,191 :: 		if(Button(&PORTB,4,1,0)) { arm_state = -1; PORTB.B0 = 1; UART1_Write_Text("AF 3\n"); }      // Throttle
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
	GOTO       L_main65
	MOVLW      255
	MOVWF      _arm_state+0
	BSF        PORTB+0, 0
	MOVLW      ?lstr16_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main65:
;AutoSAE_Safety.c,192 :: 		}
	GOTO       L_main66
L_main64:
;AutoSAE_Safety.c,193 :: 		else if(arm_state == 6 && PORTB.B3 == 1) {
	MOVF       _arm_state+0, 0
	XORLW      6
	BTFSS      STATUS+0, 2
	GOTO       L_main69
	BTFSS      PORTB+0, 3
	GOTO       L_main69
L__main76:
;AutoSAE_Safety.c,194 :: 		UART1_Write_Text("A 6\n");
	MOVLW      ?lstr17_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,195 :: 		PORTA.B1 = 1;              // Brake IL off
	BSF        PORTA+0, 1
;AutoSAE_Safety.c,196 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,197 :: 		arm_state = 7;
	MOVLW      7
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,198 :: 		UART1_Write_Text("A 7\n");
	MOVLW      ?lstr18_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,199 :: 		}
	GOTO       L_main70
L_main69:
;AutoSAE_Safety.c,200 :: 		else if(arm_state == 8) {
	MOVF       _arm_state+0, 0
	XORLW      8
	BTFSS      STATUS+0, 2
	GOTO       L_main71
;AutoSAE_Safety.c,201 :: 		UART1_Write_Text("A 9\n");
	MOVLW      ?lstr19_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,202 :: 		arm_state = 9;
	MOVLW      9
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,203 :: 		}
	GOTO       L_main72
L_main71:
;AutoSAE_Safety.c,204 :: 		else if(arm_state == -1) {
	MOVF       _arm_state+0, 0
	XORLW      255
	BTFSS      STATUS+0, 2
	GOTO       L_main73
;AutoSAE_Safety.c,205 :: 		PORTB.B6 = 0;
	BCF        PORTB+0, 6
;AutoSAE_Safety.c,206 :: 		if(PORTB.B3 == 1) { arm_state = 0; }
	BTFSS      PORTB+0, 3
	GOTO       L_main74
	CLRF       _arm_state+0
L_main74:
;AutoSAE_Safety.c,207 :: 		}
L_main73:
L_main72:
L_main70:
L_main66:
L_main61:
L_main59:
L_main55:
L_main52:
L_main49:
;AutoSAE_Safety.c,209 :: 		}
	GOTO       L_main28
;AutoSAE_Safety.c,211 :: 		}
L_end_main:
	GOTO       $+0
; end of _main
