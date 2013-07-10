#line 1 "E:/Tom_doc/university/5thyear/FYP/Code/Git/SAEAuto/SafetyPIC/AutoSAE_Safety.c"

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

void trip() {

 tripstate = 1;

 PORTB.B6 = 0;
 PORTA.B4 = 1;
 if(brakeil == 1) { PORTA.B1 = 1; }

 if(hb_trip == 1) { PORTA.B3 = 1; }
 else { PORTA.B3 = 0; }

 UART1_Write_Text("TRIP\n");

 arm_state = -1;
 tripreq = 0;

}


void interrupt() {

 if(hb_interruptcount > 12) {
 if(arm_state > 0) {
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


 TMR0 = 100;
 INTCON.TMR0IF = 0;
}



void main() {

 OSCCON = 0b1111110;

 UART1_Init(9600);
 Delay_ms(500);

 UART1_Write_Text("Hi\n");

 ANSEL = 0;
 CMCON = 0x07;


 TRISA = 0b11100101;
 TRISB = 0b10111100;


 PORTA = 0;
 PORTB = 0;


 INTCON.TMR0IE = 1;
 INTCON.TMR0IF = 0;
 OPTION_REG.T0CS = 0;
 OPTION_REG.T0SE = 0;
 OPTION_REG.PSA = 0;
 OPTION_REG.PS2 = 1;
 OPTION_REG.PS1 = 1;
 OPTION_REG.PS0 = 1;
 TMR0 = 100;

 INTCON.GIE = 1;


 UART1_Write_Text("Init ok\n");


 while(1) {

 if(tripreq == 1) { trip(); }


 if (UART1_Data_Ready()) {
 uart_rd = UART1_Read();

 if(uart_rd == 'E' && tripstate == 0) { trip(); }

 oldhbstate = hbstate;

 if(uart_rd == '+') { hbstate = 1; }
 if(uart_rd == '-') { hbstate = 0; }

 if(uart_rd == 'B') { brakeil = 1; }
 if(uart_rd == 'H') { brakeil = 0; }

 if(oldhbstate != hbstate) { hb_interruptcount = 0; }

 UART1_Write_Text("ACK ");
 UART1_Write(uart_rd);
 UART1_Write_Text("\n");
 }

 if(tripstate == 0) { PORTA.B3 = hbstate; }
 PORTB.B1 = brakeil;

 if(arm_state > 0 && PORTA.B2 == 0 && tripstate == 0) {
 UART1_Write_Text("DES\n");
 trip();
 }

 if(arm_state == 0) {

 if(PORTB.B3 == 0) {
 UART1_Write_Text("AR\n");
 if(brakeil == 1) { PORTA.B1 = 1; }
 PORTB.B0 = 0;
 PORTA.B4 = 0;
 PORTB.B6 = 0;
 tripstate = 0;
 hb_trip = 0;
 arm_state = 1;
 arm_interruptcount = 0;
 hb_interruptcount = 0;
 UART1_Write_Text("A 1\n");
 }
 }
 else if(arm_state == 1 || arm_state == 3) {
 if(PORTB.B3 == 1) { arm_state = -1; UART1_Write_Text("AF 1\n"); }
 if(PORTB.B4 == 0) { arm_state = -1; PORTB.B0 = 1; UART1_Write_Text("AF 2\n"); }
 }
 else if(arm_state == 2) {
 UART1_Write_Text("A 2\n");
 PORTB.B6 = 1;
 arm_state = 3;
 arm_interruptcount = 0;
 UART1_Write_Text("A 3\n");
 }
 else if(arm_state == 4) {
 UART1_Write_Text("A 4\n");
 arm_interruptcount = 0;
 arm_state = 5;
 UART1_Write_Text("A 5\n");
 }
 else if(arm_state == 5 || arm_state == 7) {
 if(PORTB.B4 == 0) { arm_state = -1; PORTB.B0 = 1; UART1_Write_Text("AF 3\n"); }
 }
 else if(arm_state == 6 && PORTB.B3 == 1) {
 UART1_Write_Text("A 6\n");
 PORTA.B1 = 0;
 arm_interruptcount = 0;
 arm_state = 7;
 UART1_Write_Text("A 7\n");
 }
 else if(arm_state == 8) {
 UART1_Write_Text("A 9\n");
 arm_state = 9;
 }
 else if(arm_state == -1) {
 if(PORTB.B3 == 1) { PORTB.B6 = 0; arm_state = 0; }
 }

}

}
