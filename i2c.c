#include<msp430.h>
#include "i2c.h"

#include "msp430g2553.h"
#include<stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>




const char* menu[]={"CHARGE[TO[100[[[[","CHARGE[TO[50","DISCHARGE[TO[50"};


const uint8_t font5x8_A_Z[] = {
     // '0' to '9'
    0x7C, 0xA2, 0x92, 0x8A, 0x7C, // '0'
    0x00, 0x84, 0xFE, 0x80, 0x00, // '1'
    0xC4, 0xA2, 0x92, 0x92, 0x8C, // '2'
    0x42, 0x82, 0x8A, 0x96, 0x62, // '3'
    0x30, 0x28, 0x24, 0xFE, 0x20, // '4'
    0x4E, 0x8A, 0x8A, 0x8A, 0x72, // '5'
    0x78, 0x94, 0x92, 0x92, 0x60, // '6'
    0x02, 0xE2, 0x12, 0x0A, 0x06, // '7'
    0x6C, 0x92, 0x92, 0x92, 0x6C, // '8'
    0x0C, 0x92, 0x92, 0x52, 0x3C, // '9'
    // ':' to '@' in 5x8
    0x00, 0x36, 0x36, 0x00, 0x00, // ':' (58)
    0x00, 0x56, 0x36, 0x00, 0x00, // ';' (59)
    0x08, 0x14, 0x22, 0x41, 0x00, // '<' (60)
    0x14, 0x14, 0x14, 0x14, 0x14, // '=' (61)
    0x00, 0x41, 0x22, 0x14, 0x08, // '>' (62)
    0x02, 0x01, 0x59, 0x09, 0x06, // '?' (63)
    0x3E, 0x41, 0x5D, 0x55, 0x1E, // '@' (64)
    // A-Z (5 bytes per character)
    0x7C, 0x12, 0x11, 0x12, 0x7C, // A
    0x7F, 0x49, 0x49, 0x49, 0x36, // B
    0x3E, 0x41, 0x41, 0x41, 0x22, // C
    0x7F, 0x41, 0x41, 0x22, 0x1C, // D
    0x7F, 0x49, 0x49, 0x49, 0x41, // E
    0x7F, 0x09, 0x09, 0x09, 0x01, // F
    0x3E, 0x41, 0x49, 0x49, 0x7A, // G
    0x7F, 0x08, 0x08, 0x08, 0x7F, // H
    0x00, 0x41, 0x7F, 0x41, 0x00, // I
    0x20, 0x40, 0x41, 0x3F, 0x01, // J
    0x7F, 0x08, 0x14, 0x22, 0x41, // K
    0x7F, 0x40, 0x40, 0x40, 0x40, // L
    0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
    0x7F, 0x04, 0x08, 0x10, 0x7F, // N
    0x3E, 0x41, 0x41, 0x41, 0x3E, // O
    0x7F, 0x09, 0x09, 0x09, 0x06, // P
    0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
    0x7F, 0x09, 0x19, 0x29, 0x46, // R
    0x46, 0x49, 0x49, 0x49, 0x31, // S
    0x01, 0x01, 0x7F, 0x01, 0x01, // T
    0x3F, 0x40, 0x40, 0x40, 0x3F, // U
    0x1F, 0x20, 0x40, 0x20, 0x1F, // V
    0x7F, 0x20, 0x18, 0x20, 0x7F, // W
    0x63, 0x14, 0x08, 0x14, 0x63, // X
    0x07, 0x08, 0x70, 0x08, 0x07, // Y
    0x61, 0x51, 0x49, 0x45, 0x43,  // Z
    0X00, 0X00, 0X00, 0X00, 0X00,  // [   actually space
    0x00, 0x60, 0x60, 0x00, 0x00   // '\' actually dot
};




void I2C_Init() {
    P1SEL  |= BIT6 + BIT7;  // P1.6 -> SCL, P1.7 -> SDA
    P1SEL2 |= BIT6 + BIT7;

    UCB0CTL1 |= UCSWRST;    // Reset USCI module
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC; // I2C Master, Synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST; // Use SMCLK, keep in reset state
    UCB0BR0 = 10;   // Set I2C speed to ~100kHz (SMCLK = 1MHz)
    UCB0BR1 = 0;

    UCB0CTL1 &= ~UCSWRST;   // Enable USCI module
    
}
void OLED_Init() {
    delay_ms(100); // Wait after power on

    OLED_Command(0xAE); // Display OFF
    OLED_Command(0xD5); // Set Display Clock
    OLED_Command(0x80);
    OLED_Command(0xA8); // Set Multiplex Ratio
    OLED_Command(0x3F);
    OLED_Command(0xD3); // Set Display Offset
    OLED_Command(0x00);
    OLED_Command(0x40); // Set Display Start Line
    OLED_Command(0x8D); // Charge Pump
    OLED_Command(0x14);
    OLED_Command(0xA1); // Segment Remap
    OLED_Command(0xC8); // COM Scan Direction
    OLED_Command(0xDA); // COM Pins Configuration
    OLED_Command(0x12);
    OLED_Command(0x81); // Contrast
    OLED_Command(0x7F);
    OLED_Command(0xD9); // Precharge Period
    OLED_Command(0xF1);
    OLED_Command(0xDB); // VCOMH Level
    OLED_Command(0x40);
    OLED_Command(0xA4); // Resume to RAM
    OLED_Command(0xA6); // Normal Display
    OLED_Command(0xAF); // Display ON
    
}
void OLED_Command(unsigned char command) {
    start_I2C();
    I2C_WriteByte(0x00);
    I2C_WriteByte(command);
    stop_I2C();
}


void OLED_display_setup(unsigned char col_st, unsigned char col_end, unsigned char pg_st, unsigned char pg_end){

    //selecting horizontal addressing mode addressing mode
    OLED_Command(0x20);
    OLED_Command(0x00);



    // set the  starting and ending col
    OLED_Command(0x21);
    OLED_Command(col_st);
    OLED_Command(col_end);

    //set the starting and ending page
    OLED_Command(0x22);
    OLED_Command(pg_st);
    OLED_Command(pg_end);

}

void OLED_display_clear(unsigned char col_st, unsigned char col_end, unsigned char pg_st, unsigned char pg_end){
    size_t i;
    size_t j;
    OLED_display_setup(col_st, col_end, pg_st, pg_end);
    start_I2C();

    I2C_WriteByte(0x40);
    while(UCB0CTL1 & UCTXSTT);// wait till the start is cleared
    for(i=0;i<8;i++){
        for(j=0;j<128;j++){
            I2C_WriteByte(0x00);
        }
        
    }
    stop_I2C();


}

void OLED_display_data(char c, size_t len){
    size_t i;
    
    //size_t j;
    size_t t=c-'0';
    start_I2C();// generates start signal

    while(!(IFG2 & UCB0TXIFG));// waits till the buffer is empty

    UCB0TXBUF=0x40;//  0x40 indicating its data

    while(UCB0STAT & UCTXSTT);// wait till the start is cleared

    // continously sends remaining data from the array
    for(  i=5*t;i<5*t+5;i++){//col
         if(len==0){
        //for( j=0;j<128;j++){// row
        I2C_WriteByte(font5x8_A_Z[i]);  //data_Ar[i][j]
        //}
         }
         else if(len==1){
            I2C_WriteByte(font5x8_A_Z[i]^0xFF);
         }

    }
        if(len==0)
        I2C_WriteByte(0x00);
        else if(len==1)
        I2C_WriteByte(0xFF);
    stop_I2C();//stop singal 
   
}





void OLED_display_string(char s[], size_t Inv){
    size_t length=strlen(s);
    size_t i;
    for(i=0;i<length;i++){
        OLED_display_data(s[i], Inv);
    }
}

void display_screen( int current){
    int i;
    for(i=0;i<opt_no;i++){
        if(i==current){
            
            OLED_display_setup(10, 120, i+4, i+4);
            OLED_display_string(menu[i],1);
        }
        else{
            
            OLED_display_setup(10, 120, i+4, i+4);
            OLED_display_string(menu[i],0);
        }
    }
}


// start
void start_I2C(){
    while (UCB0CTL1 & UCTXSTP); // Ensure stop condition is sent
    UCB0I2CSA = SSD1306_I2C_ADDR; // Set OLED I2C address
    UCB0CTL1 |= UCTR + UCTXSTT; // Transmit mode & Start condition
}

void I2C_WriteByte(unsigned char data) {
    

    while (!(IFG2 & UCB0TXIFG)); // Wait for TX buffer ready
    UCB0TXBUF = data; // sending data

}

void stop_I2C(){
    while (!(IFG2 & UCB0TXIFG)); // Ensure last byte is sent
    UCB0CTL1 |= UCTXSTP; // Stop condition
    delay_ms(1);
}





void delay_ms(unsigned int ms) {
    while (ms--) {
        __delay_cycles(1000); // Assuming 1MHz clock (1ms delay)
    }
}

