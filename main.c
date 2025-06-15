// #include <cstdint>


#include <msp430.h>
#include "i2c.h"  // Include the I2C header file
#include "input.h"
#include "ADC.h"
#include "msp430g2553.h"
#include "sys/cdefs.h"
#include <signal.h>
#include <stdint.h>
#include <stddef.h>

// #define single_press 1000
#define double_press 2400



typedef enum{
    STATE_MAIN_MENU,
    STATE_CHARGING_100,
    STATE_CHARGING_50,
    STATE_DISCHARGING_50
}state;

state CURRENT_STATE=STATE_MAIN_MENU, PREVIOUS_STATE=STATE_DISCHARGING_50;



// input fn variables
volatile int timer_count = 0;

volatile int current_option=-1;
volatile int task_A = 0, task_B = 0; 
volatile long wait_time=50;
uint8_t r=0;



char exit[]={"EXIT"};

void Exit(){
   
               if(task_A){
                OLED_display_setup(20, 127, 6, 6);
                OLED_display_string(exit,1);
                r++;
                task_B=0;
                task_A=0;
                
                }
               else if(task_B&&r){
                    task_B=0;
                    r=0;
                    CURRENT_STATE=STATE_MAIN_MENU;
                    
                }

}


   

void main(void) {
    
    char IN_Charge[]={"CHARGING[TO[[100"};
    char IN_Storage[]={"CHARGING[TO[50"};
    char IN_3[]={"DISCHARGING[TO[50"};
    
    char D2[]={"2D"};
    
   // Configure WDT in interval timer mode
    WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL + WDTIS0 ;

   // Set clock to 8MHz
    BCSCTL1 = CALBC1_8MHZ;
    DCOCTL = CALDCO_8MHZ;

    BCSCTL3 |= LFXT1S_2;  // ACLK = VLO (~12kHz)

    IFG1 &= ~WDTIFG;
    IE1 |= WDTIE;

    // output pins config


    // Set directions: P1.1, P1.3, P1.4 as outputs (discharge, main, charge)
    P1DIR |= BIT1 | BIT3 | BIT4;// main switch , dischargin , charging

    P1OUT = 0x00;  // All P1 outputs low
    P1OUT |= BIT3;       // turn OFF discharge (P1.3 high) 
    P1OUT |=BIT1;    // MAIN SWITCH
    // P1OUT |=BIT4; // CHARGING ON

    
    ADCinit();// initializing ADC
    digital_init();// conf pin 1.2 as input
    LEDinit();

    // Timer: ACLK (~32.768kHz), up mode, stop for now// 
    // TA0CCTL0 = CCIE;
    TA0CCR0 = 16384;                // 0.5 sec
    TA0CTL = TASSEL_1 | MC_0 | ID0;       // ACLK, stop mode

    // Timer: 1A
    TA1CCTL0=CCIE;
    TA1CCR0=12000-1;
    TA1CTL=TASSEL1 |MC_0 |ID0;

    
    

    __enable_interrupt();           // Enable global interrupts

    char c[]="VOLTAGE:[";
    
    size_t Inv=0;
    I2C_Init();
    OLED_Init();
    OLED_display_clear(0, 127, 0, 7);
    OLED_display_setup(10, 127, 1, 1);
    OLED_display_string(c, Inv);
    

    
    
    while (1) {
      
        display_voltage();
        __low_power_mode_3();    
        switch(CURRENT_STATE){
            case STATE_MAIN_MENU:
                if(PREVIOUS_STATE!=CURRENT_STATE){
                    display_screen(current_option);
                    PREVIOUS_STATE=CURRENT_STATE;
                    }
                if(task_A){
                    current_option=(current_option+1)%opt_no;
                    display_screen(current_option);
                    task_A=0;
                    }
                else if(task_B){
                    CURRENT_STATE=(state)(current_option+1);
                    task_B=0;
                    }
        
                break;

            case STATE_CHARGING_100:
                if(PREVIOUS_STATE!=CURRENT_STATE){
                    P3OUT&=~BIT3;    // BLUE LED  ON
                    P1OUT|=BIT4;// CHARGING ON
                    
                    OLED_display_clear(0, 127, 3, 7);
                    OLED_display_setup(10, 127, 4, 4);
                    OLED_display_string(IN_Charge,0);
                    OLED_display_setup(20, 127, 6, 6);
                    OLED_display_string(exit,0);
                    PREVIOUS_STATE=CURRENT_STATE;
                }

                Exit();// checks for button press to exit
                if(CURRENT_STATE==STATE_MAIN_MENU){
                    P3OUT|=BIT3;// BLUE LED OFF
                    P1OUT&=~BIT4; // CHARGING OFF
                }

                if(ADC10MEM>835){
                        //stop charging
                        P1OUT&=~BIT4; // CHARGING OFF
                        P3OUT|=BIT3;  // BLUE LED OFF
                        //start timer 
                        TA1CTL|=MC_1;
                        OLED_display_setup(110, 125, 1, 1);
                        OLED_display_string(D2, 0);
                        CURRENT_STATE=STATE_MAIN_MENU;

                    }
                break;


            case STATE_CHARGING_50:
                if(PREVIOUS_STATE!=CURRENT_STATE){
                    P3OUT&=~BIT3;    // BLUE LED  ON
                    P1OUT|=BIT4;// CHARGING ON
                    
                    OLED_display_clear(0, 127, 3, 7);
                    OLED_display_setup(10, 127, 4, 4);
                    OLED_display_string(IN_Storage,0);
                    OLED_display_setup(20, 127, 6, 6);
                    OLED_display_string(exit,0);
                    PREVIOUS_STATE=CURRENT_STATE;
                }

                Exit();// checks for button press to exit
                if(CURRENT_STATE==STATE_MAIN_MENU){
                    P3OUT|=BIT3;// BLUE LED OFF
                    P1OUT&=~BIT4; // CHARGING OFF
                }

                if(ADC10MEM>835){
                    //stop charging
                    P1OUT&=~BIT4; // CHARGING OFF
                    P3OUT|=BIT3;  // BLUE LED OFF
                    CURRENT_STATE=STATE_MAIN_MENU;
                    // stop the microcontroller
                }

                break;


            case STATE_DISCHARGING_50:       // OPTION 3
                if(PREVIOUS_STATE!=CURRENT_STATE){
                    P3OUT&=~BIT0;    // RED LED  ON
                    P1OUT&=~BIT3;    // DISCHARING ON

                    OLED_display_clear(0, 127, 3, 7);
                    OLED_display_setup(10, 127, 4, 4);
                    OLED_display_string(IN_3,0);
                    OLED_display_setup(20, 127, 6, 6);
                    OLED_display_string(exit,0);
                    PREVIOUS_STATE=CURRENT_STATE;
                }

                Exit();// checks for button press to exit
                if(CURRENT_STATE==STATE_MAIN_MENU){
                    P3OUT|=BIT0; // RED LED OFF
                    P1OUT|=BIT3;// DISCHARING OFF
                }

                if(ADC10MEM>835){
                    // stop discharging
                    P1OUT|=BIT3;// DISCHARING OFF
                    P3OUT|=BIT0;
                    CURRENT_STATE=STATE_MAIN_MENU;
                }
                break;             
        

        }
    }





        
        
        
        
    


    
        
        
}


#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void) {
           
         __low_power_mode_off_on_exit();  // Wake main loop
}

// // Port 1 ISR (Button)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
if(P1IES&BIT2){
    // P1IES|=BIT2;    // high to low transistion
    P1IES&=~BIT2;// low to high transition
    
    TA0R=0;      // reset timer
    TA0CTL|=MC_1;// up mode
    P1IFG&=~BIT2;// clearing flag
    
}
else if(!(P1IES&BIT2)){
      timer_count=TA0R;
      TA0CTL&= ~MC_3;   // stop timer
      
    //   P1IES&=~BIT2;// low to high transition
    P1IES|=BIT2;    // high to low transistion

      if(timer_count>=double_press){
        task_B=1;
      }
      else //if(timer_count<=single_press){
        task_A=1;
      //}
      P1IFG&=~BIT2;// clearing flag
      __low_power_mode_off_on_exit();  // Wake main loop
}
    
    
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void timer1(void){
      
   if(wait_time==0){
    wait_time=0;
    CURRENT_STATE=STATE_DISCHARGING_50;
    TA1CTL&=~MC_3;
    OLED_display_clear(110, 127, 1, 1);

   }
   else{

    wait_time--;

   }

}






