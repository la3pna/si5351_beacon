/*

  Si5351 morse beacon.
  Thomas S. Knutsen <la3pna@gmail.com>

  Based on the simple Morse Beacon by: Mark VandeWettering K6HX http://brainwagon.org
  
  To calibrate the TCXO run the calibrate routine with the NT7s SI5351 library. 
  Input the frequency offset from 10MHz in "calvalue"


  The Beacon message can be any length. Only large letters, numbers and . , ? /
  
  Published on a "do as you want" licence. We would love to hear what you do with it tho!
  

*/


char message[ ] = "LA3PNA LA3PNA LA3PNA JO59BR JO59BR // SI5351 BEACON // "; // Here you put in your beacon message.

int calvalue = 14;// here you set the cal value for the TCXO

unsigned long int frequency =501000010 ; // here you set the beacon nominal frequency

long tonetime = 32000; // Here you set the continuous tone time.

struct t_mtab {
  char c, pat;
} ;

struct t_mtab morsetab[] = {
  {'.', 106},
  {',', 115},
  {'?', 76},
  {'/', 41},
  {'A', 6},
  {'B', 17},
  {'C', 21},
  {'D', 9},
  {'E', 2},
  {'F', 20},
  {'G', 11},
  {'H', 16},
  {'I', 4},
  {'J', 30},
  {'K', 13},
  {'L', 18},
  {'M', 7},
  {'N', 5},
  {'O', 15},
  {'P', 22},
  {'Q', 27},
  {'R', 10},
  {'S', 8},
  {'T', 3},
  {'U', 12},
  {'V', 24},
  {'W', 14},
  {'X', 25},
  {'Y', 29},
  {'Z', 19},
  {'1', 62},
  {'2', 60},
  {'3', 56},
  {'4', 48},
  {'5', 32},
  {'6', 33},
  {'7', 35},
  {'8', 39},
  {'9', 47},
  {'0', 63}
} ;

#define N_MORSE  (sizeof(morsetab)/sizeof(morsetab[0]))

#define SPEED  (12)
#define DOTLEN  (1200/SPEED)
#define DASHLEN  (3*(1200/SPEED))

#include <si5351.h>
#include "Wire.h"

Si5351 si5351;

int LEDpin = 13 ;

void dash()
{
  digitalWrite(LEDpin, HIGH) ;
  si5351.output_enable(SI5351_CLK0, 1);
  delay(DASHLEN);
  digitalWrite(LEDpin, LOW) ;
  si5351.output_enable(SI5351_CLK0, 0);
  delay(DOTLEN) ;
}

void dit()
{
  digitalWrite(LEDpin, HIGH) ;
  si5351.output_enable(SI5351_CLK0, 1);
  delay(DOTLEN);
  digitalWrite(LEDpin, LOW) ;
  si5351.output_enable(SI5351_CLK0, 0);
  delay(DOTLEN);
}

void
send(char c)
{
  int i ;
  if (c == ' ') {
    Serial.print(c) ;
    delay(7 * DOTLEN) ;
    return ;
  }
  for (i = 0; i < N_MORSE; i++) {
    if (morsetab[i].c == c) {
      unsigned char p = morsetab[i].pat ;
      Serial.print(morsetab[i].c) ;

      while (p != 1) {
        if (p & 1)
          dash() ;
        else
          dit() ;
        p = p / 2 ;
      }
      delay(2 * DOTLEN) ;
      return ;
    }
  }
  /* if we drop off the end, then we send a space */
  Serial.print("?") ;
}

void sendmsg(char *str)
{
  while (*str)
    send(*str++) ;
  Serial.println("");
}

void setup() {
  pinMode(LEDpin, OUTPUT) ;
  Serial.begin(9600) ;
  Serial.println("Si5351 beacon initiated") ;
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, calvalue);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
  si5351.output_enable(SI5351_CLK0, 0);
  si5351.set_pll(12ULL*frequency, SI5351_PLLA);
  si5351.set_freq(frequency*100ULL, SI5351_CLK0);
  delay(2000);
}

void loop() {
  sendmsg(message) ;
  delay(1000) ;
  digitalWrite(LEDpin, HIGH) ;
  si5351.output_enable(SI5351_CLK0, 1);
  delay(tonetime);
  digitalWrite(LEDpin, LOW) ;
  si5351.output_enable(SI5351_CLK0, 1);
  delay(1000);
}

