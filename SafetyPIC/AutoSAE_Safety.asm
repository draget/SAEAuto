
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
;AutoSAE_Safety.c,34 :: 		if(hb_interruptcount > 10) {
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _hb_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt70
	MOVF       _hb_interruptcount+0, 0
	SUBLW      10
L__interrupt70:
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
	GOTO       L__interrupt71
	MOVF       _arm_interruptcount+0, 0
	SUBLW      200
L__interrupt71:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt8
	MOVLW      2
	MOVWF      _arm_state+0
L_interrupt8:
;AutoSAE_Safety.c,45 :: 		}
	GOTO       L_interrupt9
L_interrupt7:
;AutoSAE_Safety.c,46 :: 		else if(arm_state == 3) {
	MOVF       _arm_state+0, 0
	XORLW      3
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt10
;AutoSAE_Safety.c,47 :: 		if(arm_interruptcount > 200) { arm_state = 4; }
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt72
	MOVF       _arm_interruptcount+0, 0
	SUBLW      200
L__interrupt72:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt11
	MOVLW      4
	MOVWF      _arm_state+0
L_interrupt11:
;AutoSAE_Safety.c,48 :: 		}
	GOTO       L_interrupt12
L_interrupt10:
;AutoSAE_Safety.c,49 :: 		else if(arm_state == 5) {
	MOVF       _arm_state+0, 0
	XORLW      5
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt13
;AutoSAE_Safety.c,50 :: 		if(arm_interruptcount > 500) { arm_state = 6; }
	MOVLW      128
	XORLW      1
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt73
	MOVF       _arm_interruptcount+0, 0
	SUBLW      244
L__interrupt73:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt14
	MOVLW      6
	MOVWF      _arm_state+0
L_interrupt14:
;AutoSAE_Safety.c,51 :: 		}
	GOTO       L_interrupt15
L_interrupt13:
;AutoSAE_Safety.c,52 :: 		else if(arm_state == 7) {
	MOVF       _arm_state+0, 0
	XORLW      7
	BTFSS      STATUS+0, 2
	GOTO       L_interrupt16
;AutoSAE_Safety.c,53 :: 		if(arm_interruptcount > 100) { arm_state = 8; }
	MOVLW      128
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_interruptcount+1, 0
	SUBWF      R0+0, 0
	BTFSS      STATUS+0, 2
	GOTO       L__interrupt74
	MOVF       _arm_interruptcount+0, 0
	SUBLW      100
L__interrupt74:
	BTFSC      STATUS+0, 0
	GOTO       L_interrupt17
	MOVLW      8
	MOVWF      _arm_state+0
L_interrupt17:
;AutoSAE_Safety.c,54 :: 		}
	GOTO       L_interrupt18
L_interrupt16:
;AutoSAE_Safety.c,55 :: 		else if(arm_state == 9 || arm_state == 0) { arm_interruptcount = 0; }
	MOVF       _arm_state+0, 0
	XORLW      9
	BTFSC      STATUS+0, 2
	GOTO       L__interrupt63
	MOVF       _arm_state+0, 0
	XORLW      0
	BTFSC      STATUS+0, 2
	GOTO       L__interrupt63
	GOTO       L_interrupt21
L__interrupt63:
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
	GOTO       L_interrupt22
L_interrupt21:
;AutoSAE_Safety.c,56 :: 		else { arm_interruptcount++; }
	INCF       _arm_interruptcount+0, 1
	BTFSC      STATUS+0, 2
	INCF       _arm_interruptcount+1, 1
L_interrupt22:
L_interrupt18:
L_interrupt15:
L_interrupt12:
L_interrupt9:
;AutoSAE_Safety.c,58 :: 		TMR0 = 100;
	MOVLW      100
	MOVWF      TMR0+0
;AutoSAE_Safety.c,59 :: 		INTCON.TMR0IF = 0;
	BCF        INTCON+0, 2
;AutoSAE_Safety.c,60 :: 		}
L_end_interrupt:
L__interrupt69:
	MOVF       ___savePCLATH+0, 0
	MOVWF      PCLATH+0
	SWAPF      ___saveSTATUS+0, 0
	MOVWF      STATUS+0
	SWAPF      R15+0, 1
	SWAPF      R15+0, 0
	RETFIE
; end of _interrupt

_main:

;AutoSAE_Safety.c,64 :: 		void main() {
;AutoSAE_Safety.c,66 :: 		OSCCON = 0b1111110;    // Set up internal oscillator.
	MOVLW      126
	MOVWF      OSCCON+0
;AutoSAE_Safety.c,68 :: 		UART1_Init(9600);       // Init hardware UART
	MOVLW      51
	MOVWF      SPBRG+0
	BSF        TXSTA+0, 2
	CALL       _UART1_Init+0
;AutoSAE_Safety.c,69 :: 		Delay_ms(500);
	MOVLW      6
	MOVWF      R11+0
	MOVLW      19
	MOVWF      R12+0
	MOVLW      173
	MOVWF      R13+0
L_main23:
	DECFSZ     R13+0, 1
	GOTO       L_main23
	DECFSZ     R12+0, 1
	GOTO       L_main23
	DECFSZ     R11+0, 1
	GOTO       L_main23
	NOP
	NOP
;AutoSAE_Safety.c,71 :: 		UART1_Write_Text("Hi\n");
	MOVLW      ?lstr2_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,73 :: 		ANSEL  = 0;            // Set PortA pins to digital.
	CLRF       ANSEL+0
;AutoSAE_Safety.c,74 :: 		CMCON = 0x07;
	MOVLW      7
	MOVWF      CMCON+0
;AutoSAE_Safety.c,77 :: 		TRISA = 0b11100101;
	MOVLW      229
	MOVWF      TRISA+0
;AutoSAE_Safety.c,78 :: 		TRISB = 0b10111100;
	MOVLW      188
	MOVWF      TRISB+0
;AutoSAE_Safety.c,81 :: 		PORTA = 0;
	CLRF       PORTA+0
;AutoSAE_Safety.c,82 :: 		PORTB = 0;
	CLRF       PORTB+0
;AutoSAE_Safety.c,85 :: 		INTCON.TMR0IE = 1;
	BSF        INTCON+0, 5
;AutoSAE_Safety.c,86 :: 		INTCON.TMR0IF = 0;
	BCF        INTCON+0, 2
;AutoSAE_Safety.c,87 :: 		OPTION_REG.T0CS = 0;  // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock
	BCF        OPTION_REG+0, 5
;AutoSAE_Safety.c,88 :: 		OPTION_REG.T0SE = 0;  // bit 4 TMR0 Source Edge Select bit 0 = low/high
	BCF        OPTION_REG+0, 4
;AutoSAE_Safety.c,89 :: 		OPTION_REG.PSA = 0;   // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
	BCF        OPTION_REG+0, 3
;AutoSAE_Safety.c,90 :: 		OPTION_REG.PS2 = 1;   // bits 2-0  PS2:PS0: Prescaler Rate Select bits
	BSF        OPTION_REG+0, 2
;AutoSAE_Safety.c,91 :: 		OPTION_REG.PS1 = 1;
	BSF        OPTION_REG+0, 1
;AutoSAE_Safety.c,92 :: 		OPTION_REG.PS0 = 1;
	BSF        OPTION_REG+0, 0
;AutoSAE_Safety.c,93 :: 		TMR0 = 100;             // preset for timer register
	MOVLW      100
	MOVWF      TMR0+0
;AutoSAE_Safety.c,95 :: 		INTCON.GIE = 1;
	BSF        INTCON+0, 7
;AutoSAE_Safety.c,98 :: 		UART1_Write_Text("Init ok\n");
	MOVLW      ?lstr3_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,101 :: 		while(1) {
L_main24:
;AutoSAE_Safety.c,103 :: 		if(tripreq == 1) { trip(); }
	MOVF       _tripreq+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main26
	CALL       _trip+0
L_main26:
;AutoSAE_Safety.c,106 :: 		if (UART1_Data_Ready()) {
	CALL       _UART1_Data_Ready+0
	MOVF       R0+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main27
;AutoSAE_Safety.c,107 :: 		uart_rd = UART1_Read();
	CALL       _UART1_Read+0
	MOVF       R0+0, 0
	MOVWF      _uart_rd+0
;AutoSAE_Safety.c,109 :: 		if(uart_rd == 'E') { trip(); }
	MOVF       R0+0, 0
	XORLW      69
	BTFSS      STATUS+0, 2
	GOTO       L_main28
	CALL       _trip+0
L_main28:
;AutoSAE_Safety.c,111 :: 		oldhbstate = hbstate;
	MOVF       _hbstate+0, 0
	MOVWF      _oldhbstate+0
;AutoSAE_Safety.c,113 :: 		if(uart_rd == '+') { hbstate = 1; }
	MOVF       _uart_rd+0, 0
	XORLW      43
	BTFSS      STATUS+0, 2
	GOTO       L_main29
	MOVLW      1
	MOVWF      _hbstate+0
L_main29:
;AutoSAE_Safety.c,114 :: 		if(uart_rd == '-') { hbstate = 0; }
	MOVF       _uart_rd+0, 0
	XORLW      45
	BTFSS      STATUS+0, 2
	GOTO       L_main30
	CLRF       _hbstate+0
L_main30:
;AutoSAE_Safety.c,116 :: 		if(uart_rd == 'B') { brakeil = 1; }
	MOVF       _uart_rd+0, 0
	XORLW      66
	BTFSS      STATUS+0, 2
	GOTO       L_main31
	MOVLW      1
	MOVWF      _brakeil+0
L_main31:
;AutoSAE_Safety.c,117 :: 		if(uart_rd == 'H') { brakeil = 0; }
	MOVF       _uart_rd+0, 0
	XORLW      72
	BTFSS      STATUS+0, 2
	GOTO       L_main32
	CLRF       _brakeil+0
L_main32:
;AutoSAE_Safety.c,119 :: 		if(oldhbstate != hbstate) { hb_interruptcount = 0; }
	MOVF       _oldhbstate+0, 0
	XORWF      _hbstate+0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main33
	CLRF       _hb_interruptcount+0
	CLRF       _hb_interruptcount+1
L_main33:
;AutoSAE_Safety.c,121 :: 		UART1_Write_Text("ACK ");
	MOVLW      ?lstr4_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,122 :: 		UART1_Write(uart_rd);
	MOVF       _uart_rd+0, 0
	MOVWF      FARG_UART1_Write_data_+0
	CALL       _UART1_Write+0
;AutoSAE_Safety.c,123 :: 		UART1_Write_Text("\n");
	MOVLW      ?lstr5_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,124 :: 		}
L_main27:
;AutoSAE_Safety.c,126 :: 		if(tripstate == 0) { PORTA.B3 = hbstate; }
	MOVF       _tripstate+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main34
	BTFSC      _hbstate+0, 0
	GOTO       L__main76
	BCF        PORTA+0, 3
	GOTO       L__main77
L__main76:
	BSF        PORTA+0, 3
L__main77:
L_main34:
;AutoSAE_Safety.c,127 :: 		PORTB.B1 = brakeil;
	BTFSC      _brakeil+0, 0
	GOTO       L__main78
	BCF        PORTB+0, 1
	GOTO       L__main79
L__main78:
	BSF        PORTB+0, 1
L__main79:
;AutoSAE_Safety.c,129 :: 		if(arm_state > 0 && PORTA.B2 == 0 && tripstate == 0) {        // Dash e-stop
	MOVLW      128
	XORLW      0
	MOVWF      R0+0
	MOVLW      128
	XORWF      _arm_state+0, 0
	SUBWF      R0+0, 0
	BTFSC      STATUS+0, 0
	GOTO       L_main37
	BTFSC      PORTA+0, 2
	GOTO       L_main37
	MOVF       _tripstate+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main37
L__main66:
;AutoSAE_Safety.c,130 :: 		UART1_Write_Text("DES\n");
	MOVLW      ?lstr6_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,131 :: 		trip();
	CALL       _trip+0
;AutoSAE_Safety.c,132 :: 		}
L_main37:
;AutoSAE_Safety.c,134 :: 		if(arm_state == 0) {
	MOVF       _arm_state+0, 0
	XORLW      0
	BTFSS      STATUS+0, 2
	GOTO       L_main38
;AutoSAE_Safety.c,136 :: 		if(PORTB.B3 == 0) {
	BTFSC      PORTB+0, 3
	GOTO       L_main39
;AutoSAE_Safety.c,137 :: 		UART1_Write_Text("AR\n");
	MOVLW      ?lstr7_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,138 :: 		if(brakeil == 1) { PORTA.B1 = 1; }     // Activate brake interlock
	MOVF       _brakeil+0, 0
	XORLW      1
	BTFSS      STATUS+0, 2
	GOTO       L_main40
	BSF        PORTA+0, 1
L_main40:
;AutoSAE_Safety.c,139 :: 		PORTB.B0 = 0;                          // Throttle LED
	BCF        PORTB+0, 0
;AutoSAE_Safety.c,140 :: 		PORTA.B4 = 0;                          // E-stop LED
	BCF        PORTA+0, 4
;AutoSAE_Safety.c,141 :: 		PORTB.B6 = 0;                          // Trip relay
	BCF        PORTB+0, 6
;AutoSAE_Safety.c,142 :: 		tripstate = 0;
	CLRF       _tripstate+0
;AutoSAE_Safety.c,143 :: 		hb_trip = 0;
	CLRF       _hb_trip+0
;AutoSAE_Safety.c,144 :: 		arm_state = 1;
	MOVLW      1
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,145 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,146 :: 		hb_interruptcount = 0;
	CLRF       _hb_interruptcount+0
	CLRF       _hb_interruptcount+1
;AutoSAE_Safety.c,147 :: 		UART1_Write_Text("A 1\n");
	MOVLW      ?lstr8_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,148 :: 		}
L_main39:
;AutoSAE_Safety.c,149 :: 		}
	GOTO       L_main41
L_main38:
;AutoSAE_Safety.c,150 :: 		else if(arm_state == 1 || arm_state == 3) {
	MOVF       _arm_state+0, 0
	XORLW      1
	BTFSC      STATUS+0, 2
	GOTO       L__main65
	MOVF       _arm_state+0, 0
	XORLW      3
	BTFSC      STATUS+0, 2
	GOTO       L__main65
	GOTO       L_main44
L__main65:
;AutoSAE_Safety.c,151 :: 		if(PORTB.B3 == 1) { arm_state = -1; UART1_Write_Text("AF 1\n"); }   // Arm button
	BTFSS      PORTB+0, 3
	GOTO       L_main45
	MOVLW      255
	MOVWF      _arm_state+0
	MOVLW      ?lstr9_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main45:
;AutoSAE_Safety.c,152 :: 		if(PORTB.B4 == 0) { arm_state = -1; PORTB.B0 = 1; UART1_Write_Text("AF 2\n"); }  // Throttle
	BTFSC      PORTB+0, 4
	GOTO       L_main46
	MOVLW      255
	MOVWF      _arm_state+0
	BSF        PORTB+0, 0
	MOVLW      ?lstr10_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main46:
;AutoSAE_Safety.c,153 :: 		}
	GOTO       L_main47
L_main44:
;AutoSAE_Safety.c,154 :: 		else if(arm_state == 2) {
	MOVF       _arm_state+0, 0
	XORLW      2
	BTFSS      STATUS+0, 2
	GOTO       L_main48
;AutoSAE_Safety.c,155 :: 		UART1_Write_Text("A 2\n");
	MOVLW      ?lstr11_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,156 :: 		PORTB.B6 = 1;
	BSF        PORTB+0, 6
;AutoSAE_Safety.c,157 :: 		arm_state = 3;
	MOVLW      3
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,158 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,159 :: 		UART1_Write_Text("A 3\n");
	MOVLW      ?lstr12_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,160 :: 		}
	GOTO       L_main49
L_main48:
;AutoSAE_Safety.c,161 :: 		else if(arm_state == 4) {
	MOVF       _arm_state+0, 0
	XORLW      4
	BTFSS      STATUS+0, 2
	GOTO       L_main50
;AutoSAE_Safety.c,162 :: 		UART1_Write_Text("A 4\n");
	MOVLW      ?lstr13_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,163 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,164 :: 		arm_state = 5;
	MOVLW      5
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,165 :: 		UART1_Write_Text("A 5\n");
	MOVLW      ?lstr14_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,166 :: 		}
	GOTO       L_main51
L_main50:
;AutoSAE_Safety.c,167 :: 		else if(arm_state == 5 || arm_state == 7) {
	MOVF       _arm_state+0, 0
	XORLW      5
	BTFSC      STATUS+0, 2
	GOTO       L__main64
	MOVF       _arm_state+0, 0
	XORLW      7
	BTFSC      STATUS+0, 2
	GOTO       L__main64
	GOTO       L_main54
L__main64:
;AutoSAE_Safety.c,168 :: 		if(PORTB.B4 == 0) { arm_state = -1; PORTB.B0 = 1; UART1_Write_Text("AF 3\n"); }      // Throttle
	BTFSC      PORTB+0, 4
	GOTO       L_main55
	MOVLW      255
	MOVWF      _arm_state+0
	BSF        PORTB+0, 0
	MOVLW      ?lstr15_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
L_main55:
;AutoSAE_Safety.c,169 :: 		}
	GOTO       L_main56
L_main54:
;AutoSAE_Safety.c,170 :: 		else if(arm_state == 6) {
	MOVF       _arm_state+0, 0
	XORLW      6
	BTFSS      STATUS+0, 2
	GOTO       L_main57
;AutoSAE_Safety.c,171 :: 		UART1_Write_Text("A 6\n");
	MOVLW      ?lstr16_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,172 :: 		PORTA.B1 = 0;
	BCF        PORTA+0, 1
;AutoSAE_Safety.c,173 :: 		arm_interruptcount = 0;
	CLRF       _arm_interruptcount+0
	CLRF       _arm_interruptcount+1
;AutoSAE_Safety.c,174 :: 		arm_state = 7;
	MOVLW      7
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,175 :: 		UART1_Write_Text("A 7\n");
	MOVLW      ?lstr17_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,176 :: 		}
	GOTO       L_main58
L_main57:
;AutoSAE_Safety.c,177 :: 		else if(arm_state == 8) {
	MOVF       _arm_state+0, 0
	XORLW      8
	BTFSS      STATUS+0, 2
	GOTO       L_main59
;AutoSAE_Safety.c,178 :: 		UART1_Write_Text("A 9\n");
	MOVLW      ?lstr18_AutoSAE_Safety+0
	MOVWF      FARG_UART1_Write_Text_uart_text+0
	CALL       _UART1_Write_Text+0
;AutoSAE_Safety.c,179 :: 		arm_state = 9;
	MOVLW      9
	MOVWF      _arm_state+0
;AutoSAE_Safety.c,180 :: 		}
	GOTO       L_main60
L_main59:
;AutoSAE_Safety.c,181 :: 		else if(arm_state == -1) {
	MOVF       _arm_state+0, 0
	XORLW      255
	BTFSS      STATUS+0, 2
	GOTO       L_main61
;AutoSAE_Safety.c,182 :: 		if(PORTB.B3 == 1) { PORTB.B6 = 0; arm_state = 0; }
	BTFSS      PORTB+0, 3
	GOTO       L_main62
	BCF        PORTB+0, 6
	CLRF       _arm_state+0
L_main62:
;AutoSAE_Safety.c,183 :: 		}
L_main61:
L_main60:
L_main58:
L_main56:
L_main51:
L_main49:
L_main47:
L_main41:
;AutoSAE_Safety.c,185 :: 		}
	GOTO       L_main24
;AutoSAE_Safety.c,187 :: 		}
L_end_main:
	GOTO       $+0
; end of _main
