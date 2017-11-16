#include <LiquidCrystal_I2C.h>

/*
TIMER BROMOGRAFO ARDUINO

Questo progetto è di libero utilizzo, basta che venga citato il programmatore. 
Programmatore Firmware: Salvatore Mele, IZ0VLL

email: iz0victorlimalima@gmail.com  
Licenza GNU GPLv3
 */



#include <EEPROM.h>
#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define encoder0PinA  2
#define encoder0PinB  3


volatile int encoder0Pos = 4;
volatile boolean PastB = 0;
volatile boolean update = false;

int contatoreFineTempo;
int lastButtonState = 0;
// ------------------------++++++++++++++++++++++   ENCODER
int encoderPin1 = 2;
int encoderPin2 = 3;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;
volatile long encoderValue2 = 0;
volatile long encoderValue3 = 0;

long lastencoderValue = 0;
long lastencoderValue2 = 0;
long lastencoderValue3 = 7;

int lastMSB = 0;
int lastLSB = 0;

// ------------------------++++++++++++++++++++++
int ore = 45;
int minuti = 5;
int secondi = 6;



//---------  timer variabili

int buzzer = 7;           //Piedino del Buzzer piezoelettrico, D7
int rele = 6;             //D6 del relè di accensione dei neon
int secondiTotali = 0;     //Tempo totale
int msg = 0;
int reset = 0;
int start = A6;            //Pulsante di avvio
int empieza = 1024;        // Variabile per la memorizzazione del pulsante di avvio


// assegnazione ingressi per i due pulsanti start e encoder
int pushButton = 4;       //D4 Pulsante dell'Encoder
int pushButtonSTART = 5;  //D5 Pulsante di Start del conteggio.



int pushButtonSTARTstate = 0;


int timeHOLD;
int timeSEC = 0;
int HorMinSec = 0;
int step_funzione = 0;


// Grafica per clessidra e cursore
uint8_t testChar[8] = {0x1F, 0x17, 0x1B, 0x1D, 0x1D, 0x1B, 0x17, 0x1F,};
uint8_t testClessidra[8] = {0x1F, 0x11, 0x0A, 0x04, 0x04, 0x0E, 0x1F, 0x1F,};


/*
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
                            VOID  SETUP                    VOID  SETUP
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
*/


void setup() {

  lcd.begin();
  Serial.begin(9600);
  Serial.begin(9600);
  pinMode(pushButton, INPUT); // dichiara che il "pushButton" è un input:
  pinMode(pushButtonSTART, INPUT);
  pinMode(start, INPUT);    //Pin de pulsador de arranque --> Entrada

  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);

  digitalWrite(encoder0PinB, HIGH);
  digitalWrite(encoder0PinA, HIGH);
  int pushButtonSTARTstate = HIGH;   //  RESISTENZA PULL UP DEL PULSANTE  START
  attachInterrupt(1, doEncoderB, FALLING);



  // CARICAMENTO dei valori temporali (ore, minuti, secondi)
 
  lastencoderValue3 = EEPROM.read(1);
  encoderValue3 = EEPROM.read(1);
  lastencoderValue2 = EEPROM.read(2);
  encoderValue2 = EEPROM.read(2);
  lastencoderValue = EEPROM.read(3);
  encoderValue = EEPROM.read(3);


// portare i resitori interni a pull up

  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);
  pinMode(pushButtonSTART, INPUT_PULLUP);
  pinMode(pushButtonSTARTstate, INPUT_PULLUP);
  pinMode(pushButton, INPUT_PULLUP);

  digitalWrite(pushButton, HIGH);  // forza il pin  a livello alto sfruttando la resistenza di pull-up interna ad arduino
 
  pinMode(buzzer, OUTPUT);  
  pinMode(rele, OUTPUT);   


 // presentazione iniziale
 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Timer Bromografo");
  lcd.setCursor(0, 1);
  lcd.print("Salvatore Mele, IZ0VLL");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vers. Firmware");
  lcd.setCursor(0, 1);
  lcd.print("Beta 0.3");
  delay(2000);
  lcd.clear();



  msg = 0;                
  empieza = 1024;           

// CREAZIONE DEI DUE CARATTERI CUSTOM

  lcd.createChar(0, testChar); // Sends the custom char to lcd
  lcd.createChar(1, testClessidra); // Sends the custom char to lcd

}

/*
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
   xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
                            VOID loop                    VOID  loop
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
*/


void loop() {
  lcd.setCursor(0, 1);
  lcd.print((char)1);  // STAMPA LA CLESSIDRA


  if (update) {
    update = false;
    PastB ? encoder0Pos++ : encoder0Pos--;


  }


  //  ore minuti secondi
  ore = lastencoderValue;
  minuti = lastencoderValue2;
  secondi = lastencoderValue3;




  switch (step_funzione) {
    case 0:    // STATO INIZIALE
      INIZIO();
      break;
    case 1:    // VAI ALLA FUNZIONE: EDIT TEMPO
      EDIT_TEMPO();

      break;
    case 2:    // VAI ALLA FUNZIONE: CONTO ALLA ROVESCIA
      COUNT_DOWN();
      break;
    case 3:    // VAI ALLA FUNZIONE: TEMPO SCADUTO
      TEMPO_SCADUTO();
      break;
  }
}


//---------------------- ---------------------- ---------------------- ----------------------
//---------------------- FUNZIONE: INIZIO ---------------------- ----------------------------
//---------------------- ---------------------- ---------------------- ----------------------

void INIZIO() {
  lcd.setCursor(0, 0);
  lcd.print("   PREMI START  ");
  lcd.setCursor(0, 1);


  lcd.setCursor(4, 1);
  if (ore < 10) lcd.print("0");     // SE LE ORE SONO MENO DI 10, SCRIVI LO ZERO A SINISTRA.
  lcd.print(ore);                   // STAMPA LE ORE
  lcd.print(":");                   // STAMPA I DUE PUNTI

  if (minuti < 10) lcd.print("0");  // SE I MINUTI SONO MENO DI 10, SCRIVI LO ZERO A SINISTRA.
  lcd.print(minuti);                // STAMPA I MINUTI
  lcd.print(":");                   // STAMPA I DUE PUNTI

  if (secondi < 10) lcd.print("0");  // SE I SECONDI SONO MENO DI 10, SCRIVI LO ZERO A SINISTRA.
  lcd.print(secondi);                // STAMPA I SECONDI



  //  VAI A SETTA TEMPO - - - - - - - - - - - - - - - - - - -


  int buttonState = digitalRead(pushButton);  //// PER ACCEDERE AL MENU EDITOR CON IL MANTENIMENTO DEL PULSANTE ENCODER
  if (buttonState == LOW) {
    delay (1000);
    Serial.println(timeSEC);
    timeSEC++;
  }
  else {
    timeSEC = 0; //  AZZERAMENTO VARIABILE SEI SECONDI
  }
  delay(100);

  if (timeSEC >= 2) {   //   VAI A EDITOR

    lcd.clear();
    timeSEC = 0;
    step_funzione = 1;
  }


  //// ALLA PRESSIONE DEL PULSANTE START, VAI ALLA FUNZIONE: COUNT DOWN

  int pushButtonSTARTstate = digitalRead(pushButtonSTART);
  if (pushButtonSTARTstate == LOW) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Partenza tempo");
    lcd.clear();
    timeSEC = 0;
    secondiTotali = secondi + (minuti * 60) + (ore * 60 * 60);  //Convierte el tiempo elegido en segundos!!
    digitalWrite (rele, HIGH);
    step_funzione = 2;
  }
}

//---------------------- ---------------------- ---------------------- ----------------------
//---------------------- FUNZIONE: EDIT TEMPO ---------------------- ------------------------
//---------------------- ---------------------- ---------------------- ----------------------

void EDIT_TEMPO() {

  lcd.setCursor(0, 1);
  lcd.print("h");
  lcd.setCursor(1, 1);
  if (encoderValue < 10) lcd.print("0");
  if (encoderValue < 0) {
    encoderValue = 0;
    encoder0Pos = 0;
  }

  if (timeSEC >= 2) {
    lcd.clear();
    timeSEC = 0;
    HorMinSec = 0;
    step_funzione = 0;

  }

  if (HorMinSec >= 3) HorMinSec = 0;   //// AZZERA LA VARIABILE PER LO STEP  ORA MINUTI  SECONDI.


  //  FAI IN MODO CHE L' ENCODER NON SUPERI I 59

  if (encoderValue > 59) {
    encoderValue = 59;
    encoder0Pos = 59;
  }
  lcd.print(encoderValue, DEC);
  lcd.print(":");
  // -------------------------------------------------------------

  lcd.setCursor(4, 1);
  lcd.print("m");
  if (encoderValue2 < 10) lcd.print("0");
  if (encoderValue2 < 0) {
    encoderValue2 = 0;
    encoder0Pos = 0;
  }
  if (encoderValue2 > 59) {
    encoderValue2 = 59;
    encoder0Pos = 59;
  }

  lcd.print(encoderValue2, DEC);
  lcd.print(":");

  // -------------------------------------------------------------

  lcd.setCursor(8, 1);
  lcd.print("s");
  if (encoderValue3 < 10) lcd.print("0");
  if (encoderValue3 < 0) {
    encoderValue3 = 0;
    encoder0Pos = 0;
  }
  if (encoderValue3 > 59) {
    encoderValue3 = 59;
    encoder0Pos = 59;
  }

  lcd.print(encoderValue3, DEC);
  lcd.print(" ");

  // VISUALIZZA LO STATO SE STIAMO SETTANDO LE ORE, I MINUTI OPPURE I SECONDI

  switch (HorMinSec) {
    case 0:    // ore
      lcd.setCursor(0, 0);
      lcd.print("Set ora");
      lcd.setCursor(0, 1);
      lcd.print((char)0);

      break;
    case 1:    // minuti
      lcd.setCursor(0, 0);
      lcd.print("Set minuti");
      lcd.setCursor(4, 1);
      lcd.print((char)0);
      break;

    case 2:    // secondi
      lcd.setCursor(0, 0);
      lcd.print("Set secondi");
      lcd.setCursor(8, 1);
      lcd.print((char)0);
      break;

    case 3:    // funzione  eccitazione / diseccitazione
      // lcd.setCursor(0, 0);
      // lcd.print("Set funzione");
      // lcd.setCursor(8, 1);
      // lcd.print((char)0);

      break;
  }


  //  PASSA DA ORE A MINUTI A SECONDI QUANDO SI PREME IL PULSANTE DEL' ENCODER

  int buttonState = digitalRead(pushButton);
  if (buttonState != lastButtonState)
    if (buttonState == LOW)
    { HorMinSec++;
      lcd.clear();
    }
  delay(100);
  lastButtonState = buttonState;



  // ESCI DALLA FUNZIONE EDITOR TEMPI PER ANDARE AD INIZIO CON IL TEMPO AGGIORNATO AL' EDITOR

  int pushButtonSTARTstate = digitalRead(pushButtonSTART);
  if (pushButtonSTARTstate == LOW)
  {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Partenza tempo");
    lcd.clear();
    timeSEC = 0;
    lastencoderValue3 = encoderValue3;
    lastencoderValue2 = encoderValue2;
    lastencoderValue = encoderValue;

    EEPROM.write(1, lastencoderValue3);  // salvataggio su slot il valore
    EEPROM.write(2, lastencoderValue2);  // salvataggio su slot il valore
    EEPROM.write(3, lastencoderValue);  // salvataggio su slot il valore
    delay(500);
    step_funzione = 0;

  }
}

//---------------------- ---------------------- ---------------------- ----------------------
//---------------------- CONTO ALLA ROVESCIA ---------------------- ----------------------
//---------------------- ---------------------- ---------------------- ----------------------


void COUNT_DOWN() {

  // PARTENZA CONTEGGIO SE LA VARIABILE secondiTotali è maggiore di 0

  while (secondiTotali > 0)
  {

    //   RESET FORZATO SE MANTENIAMO PREMUTO IL TASTO START PER 3 SECONDI E RITORNA AD INIZIO

    int pushButtonSTARTstate = digitalRead(pushButtonSTART);  //// START   START   START    START   START
    if (pushButtonSTARTstate == LOW) {
      timeSEC++;
    }
    //----------------------------------

    if (timeSEC >= 3) {

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" reset forzato  ");
      lcd.setCursor(0, 1);
      lcd.print("xxxxxxxxxxxxxxxx");
      lcd.setCursor(1, 1);
      lcd.print("xxxxxxxxxxxxxxxx");
      digitalWrite (rele, LOW);  // SPEGNE IL RELè
      delay(2000);
      lcd.clear();
      timeSEC = 0;
      step_funzione = 0;

      break;
    }


    lcd.setCursor(0, 0);
    lcd.print(" Tempo residuo: ");

    delay (1000);         // DELAY PER CONTARE STEP DI UN SECONDO...NON TOCCARE

    secondiTotali--;

    ore = ((secondiTotali / 60) / 60);  // converte i secondi totali in ore
    minuti = (secondiTotali / 60) % 60; // converte i secondi totali in minuti
    secondi = secondiTotali % 60;       // converte i secondi totali in secondi

    lcd.setCursor(0, 0);
    lcd.print(" Tempo residuo: ");

    lcd.setCursor(4, 1);
    if (ore < 10) lcd.print("0");
    lcd.print(ore);
    lcd.print(":");

    if (minuti < 10) lcd.print("0");
    lcd.print(minuti);

    lcd.print(":");
    if (secondi < 10) lcd.print("0");
    lcd.print(secondi);


    // VAI A FUNZIONE FINE TEMPO quando i secondi totali raggiungono il valore =0

    if (secondiTotali == 0)
    {
      step_funzione = 3;  //  VAI AL' ULTIMO STADIO...TEMPO CHE RAGGIUNGE LO ZERO

      digitalWrite (rele, LOW);  // SPEGNE IL RELè
      break;
    }
  }
}


//---------------------- ---------------------- ---------------------- ----------------------
//---------------------- TEMPO ESAURITO ---------------------- ------------------
//---------------------- ---------------------- ---------------------- ----------------------

void TEMPO_SCADUTO() {

  // AZZERA ALLA PRESSIONE DEL TASTO START E VAI AD INIZIO PRIMA DEL TEMPO DI 10 SECONDI
  int pushButtonSTARTstate = digitalRead(pushButtonSTART);
  if (pushButtonSTARTstate == LOW)

  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   ripristino   ");
    delay(1000);
    lcd.clear();
    timeSEC = 0;
    step_funzione = 0;
  }


  lcd.setCursor(0, 0);
  lcd.print("   Fine tempo   ");
  lcd.setCursor(0, 1);
  lcd.print("Start=Ripristina");
  tone(buzzer,1000,200);  // attiva il cicalino alla frequenza di 1kHz
  //digitalWrite(buzzer, HIGH);
  delay(15000);
  //digitalWrite(buzzer, LOW);
  noTone(buzzer);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  contatoreFineTempo++;
  delay(500);
  lcd.clear();


// VAI A INIZIO LIVELLO QUANDO IL TEMPO DI 10 SECONDI è SCADUTO

  if (contatoreFineTempo >= 10) {
    contatoreFineTempo = 0;
    timeSEC = 0;
    step_funzione = 0;
  }
}


//---------------------- ---------------------- ---------------------- ----------------------
//---------------------- INTERRUPT ENCODER LETTURA  ---------------------- ------------------
//---------------------- ---------------------- ---------------------- ----------------------

// PARAMETRI LEGATI ALLE LETTURE ENCODE TRAMITE INTERRUPT

void doEncoderB()
{
  PastB = (boolean)digitalRead(encoder0PinA);
  update = true;

  if (HorMinSec == 2)encoderValue3 = encoder0Pos;
  if (HorMinSec == 1)encoderValue2 = encoder0Pos;
  if (HorMinSec == 0)encoderValue = encoder0Pos;

}


