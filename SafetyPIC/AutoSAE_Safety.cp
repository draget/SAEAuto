#line 1 "C:/Users/Martin French/Desktop/SafetyPIC/AutoSAE_Safety.c"
#line 9 "C:/Users/Martin French/Desktop/SafetyPIC/AutoSAE_Safety.c"
char uart_rd;
short brakeil = 1;
short hbstate = 0;
short oldhbstate = 0;
short tripreq = 0;
short tripstate = 0;
short arm_state = 0;
short hb_trip = 0;
int hb_interruptcount = 0;
int arm_interruptcount = 0;
int alarm_counter = 0;
#line 24 "C:/Users/Martin French/Desktop/SafetyPIC/AutoSAE_Safety.c"
void trip() {

 tripstate = 1;

 PORTB.B6 = 0;
 PORTA.B4 = 1;
 PORTA.B0 = 1;
 if(brakeil == 1) { PORTA.B1 = 0; }

 if(hb_trip == 1) { PORTA.B3 = 1; }
 else { PORTA.B3 = 0; }

 UART1_Write_Text("TRIP\n");

 arm_state = -1;
 tripreq = 0;

}
#line 46 "C:/Users/Martin French/Desktop/SafetyPIC/AutoSAE_Safety.c"
void untrip() {

 tripstate = 0;

 PORTB.B6 = 1;
 PORTA.B4 = 0;
 PORTA.B0 = 0;
 PORTA.B1 = 1;
 PORTA.B3 = 0;

 UART1_Write_Text("Untrip\n");

 arm_state = 0;

}
#line 67 "C:/Users/Martin French/Desktop/SafetyPIC/AutoSAE_Safety.c"
void interrupt() {

 if(hb_interruptcount > 50) {
 if((arm_state > 0) && (brakeil == 1)) {
 hb_trip = 1;
 tripreq = 1;
 }
 else { hb_interruptcount = 0; }
 }
 else { hb_interruptcount++; }

 if(arm_state == 1) {
 if(arm_interruptcount > 200) { arm_state = 2; }
 else { arm_interruptcount++; }
 }
 else if(arm_state == 3) {
 if(arm_interruptcount > 200) { arm_state = 4; }
 else { arm_interruptcount++; }
 }
 else if(arm_state == 5) {
 if(arm_interruptcount > 500) { arm_state = 6; }
 else { arm_interruptcount++; }
 }
 else if(arm_state == 7) {
 if(arm_interruptcount > 100) { arm_state = 8; }
 else { arm_interruptcount++; }
 }
 else if(arm_state == 9 || arm_state == 0) { arm_interruptcount = 0; }

 if(alarm_counter > 0) { alarm_counter--; }

 TMR0 = 100;
 INTCON.TMR0IF = 0;
}
#line 106 "C:/Users/Martin French/Desktop/SafetyPIC/AutoSAE_Safety.c"
void main() {

 OSCCON = 0b1111110;

 WDTCON.WDTPS0 = 0;
 WDTCON.WDTPS1 = 0;
 WDTCON.WDTPS2 = 0;
 WDTCON.WDTPS3 = 1;

 UART1_Init(38400);
 Delay_ms(500);

 UART1_Write_Text("Hi\n");

 ANSEL = 0;
 CMCON = 0x07;


 TRISA = 0b11100100;
 TRISB = 0b10111100;


 PORTA = 0b00000010;
 PORTB = 0;


 INTCON.TMR0IE = 1;
 INTCON.TMR0IF = 0;
 OPTION_REG.T0CS = 0;
 OPTION_REG.T0SE = 0;
 OPTION_REG.PSA = 0;
 OPTION_REG.PS2 = 1;
 OPTION_REG.PS1 = 1;
 OPTION_REG.PS0 = 1;
 OPTION_REG.NOT_RBPU = 0;
 TMR0 = 100;

 INTCON.GIE = 1;


 UART1_Write_Text("Init ok\n");


 while(1) {

 if(tripreq == 1) { trip(); }

 asm { CLRWDT; }

 if (UART1_Data_Ready()) {
 uart_rd = UART1_Read();

 if(uart_rd == 'E' && tripstate == 0) { trip(); }
 if(uart_rd == 'R' && tripstate == 1) { untrip(); }

 oldhbstate = hbstate;

 if(uart_rd == '+') { hbstate = 1; }
 if(uart_rd == '-') { hbstate = 0; }

 if(uart_rd == 'B') { brakeil = 1; }
 if(uart_rd == 'H') { brakeil = 0; }

 if(uart_rd == 'A') { alarm_counter = 25; PORTA.B0 = 1; }

 if(oldhbstate != hbstate) { hb_interruptcount = 0; }

 UART1_Write_Text("ACK ");
 UART1_Write(uart_rd);
 UART1_Write_Text("\n");
 }

 if(tripstate == 0) {
 PORTA.B3 = hbstate;
 if(alarm_counter == 0) { PORTA.B0 = 0; }
 }

 brakeil = PORTB.B7;
 PORTB.B1 = brakeil;

 if(arm_state > 0 && PORTA.B2 == 0 && tripstate == 0) {
 UART1_Write_Text("DES\n");
 trip();
 }

 if(arm_state == 0) {

 if(Button(&PORTB,3,20,0)) {
 UART1_Write_Text("AR\n");
 if(brakeil == 1) { PORTA.B1 = 0; }
 PORTB.B0 = 0;
 PORTA.B4 = 0;
 PORTA.B0 = 0;
 PORTB.B6 = 0;
 tripstate = 0;
 hb_trip = 0;
 arm_state = 1;
 arm_interruptcount = 0;
 hb_interruptcount = 0;
 UART1_Write_Text("A 1\n");
 }
 }
 else if(arm_state == 1) {
 if(PORTB.B3 == 1) { arm_state = -1; UART1_Write_Text("AF 1\n"); }
 }
 else if(arm_state == 2) {
 UART1_Write_Text("A 2\n");
 PORTB.B6 = 1;
 Delay_ms(200);
 arm_state = 3;
 arm_interruptcount = 0;
 UART1_Write_Text("A 3\n");
 }
 else if(arm_state == 3) {
 if(PORTB.B3 == 1) { arm_state = -1; UART1_Write_Text("AF 1\n"); }
 if(Button(&PORTB,4,1,0)) { arm_state = -1; PORTB.B0 = 1; UART1_Write_Text("AF 2\n"); }
 }
 else if(arm_state == 4) {
 UART1_Write_Text("A 4\n");
 arm_interruptcount = 0;
 arm_state = 5;
 UART1_Write_Text("A 5\n");
 }
 else if(arm_state == 5 || arm_state == 7) {
 if(Button(&PORTB,4,1,0)) { arm_state = -1; PORTB.B0 = 1; UART1_Write_Text("AF 3\n"); }
 }
 else if(arm_state == 6 && PORTB.B3 == 1) {
 UART1_Write_Text("A 6\n");
 PORTA.B1 = 1;
 arm_interruptcount = 0;
 arm_state = 7;
 UART1_Write_Text("A 7\n");
 }
 else if(arm_state == 8) {
 UART1_Write_Text("A 9\n");
 arm_state = 9;
 }
 else if(arm_state == -1) {
 PORTB.B6 = 0;
 if(PORTB.B3 == 1) { arm_state = 0; }
 }

}

}
