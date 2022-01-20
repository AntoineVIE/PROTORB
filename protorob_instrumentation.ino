//timer interrupts
//by Amanda Ghassaei
//June 2012
//https://www.instructables.com/id/Arduino-Timer-Interrupts/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
*/

//timer setup for timer0, timer1, and timer2.
//For arduino uno or any board with ATMEL 328/168.. diecimila, duemilanove, lilypad, nano, mini...

//this code will enable all three arduino timer interrupts.
//timer0 will interrupt at 2kHz
//timer1 will interrupt at 1Hz
//timer2 will interrupt at 8kHz

//storage variables
boolean toggle0 = 0;
boolean toggle1 = 0;
boolean toggle2 = 0;

float answer_unfiltered[4];
float answer_numerical_filtered[4];
float answer_analogical_filtered[4];
float answer_analogical_numerical_filtered[4];

int analogPin_unfiltered = A0;
int analogPin_filterd = A1;

void setup(){
  
  //set pins as outputs
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(13, OUTPUT);

cli();//stop interrupts
//TIMER POUR SIGNAL ARDUINO
//set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 5;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
  
//TIMER POUR ECHANTILLONAGE
//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);


sei();//allow interrupts

}//end setup

ISR(TIMER0_COMPA_vect){//timer0 interrupt 2kHz toggles pin 8
//generates pulse wave of frequency 2kHz/2 = 1kHz (takes two cycles for full wave- toggle high then toggle low)
 if (toggle0){
    digitalWrite(8,HIGH);
    digitalWrite(9,LOW);
    toggle0 = 0;
  }
  else{
    digitalWrite(8,LOW);
    digitalWrite(9,HIGH);
    toggle0 = 1;
  }
}
//CODE TESTE -> OK

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  if (toggle1){
    digitalWrite(13,HIGH);
    toggle1 = 0;
  }
  else{
    digitalWrite(13,LOW);
    toggle1 = 1;
  }

  //Lecture du signal non filtré
  float uk1 = analogRead(A0);
  uk1 = uk1*(5./1023);
  //Lecture du signal filtré analogiquement
  float uk2 = analogRead(A1);
  uk2 = uk2*(5./1023);
  //Mise à jour des données d'entrée
  for (int i=0; i<3; i++){
    answer_unfiltered[i]=answer_unfiltered[i+1];
    answer_analogical_filtered[i]=answer_analogical_filtered[i+1];
  }  
  answer_unfiltered[3]=uk1;
  answer_analogical_filtered[3]=uk2;
    
  //Application du filtre numérique aux signaux filtré et non filtré
  float yk1 = numerical_filter(answer_numerical_filtered,answer_unfiltered);
  float yk2 = numerical_filter(answer_analogical_numerical_filtered,answer_analogical_filtered);
  for (int i=0; i<3; i++){
    answer_numerical_filtered[i]=answer_numerical_filtered[i+1];
    answer_analogical_numerical_filtered[i]=answer_analogical_numerical_filtered[i+1];
  }
  answer_numerical_filtered[3]=yk1;
  answer_analogical_numerical_filtered[3]=yk2;
}

// y = [y[k-3] y[k-2] y[k-1] y[k]]
// u = [u[k-3] u[k-2] u[k-1] u[k]]
float numerical_filter(float y[],float u[]){
  float a0 = -0.9802, a1 = 2.96, a2 = -2.98;
  float b0 = 1.238e-7, b1 = 3.713e-7, b2 = 3.713e-7, b3 = 1.238e-7;
  float yk = -a0*y[0]-a1*y[1]-a2*y[2]+b0*u[0]+b1*u[1]+b2*u[2]+b3*u[3];
  return yk;
}

void loop(){
  //do other things here
}
