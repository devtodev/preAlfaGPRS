// Guardar el buffer sin el OK de fin
// hacer trim de param valor
// reiniciar la comunicacion

#include <Keypad.h>
#include <glcd.h>
#include <fonts/allFonts.h>
#include <Wire.h>
#include <DS1307new.h>

char GPRS_online = 0;
char BUS_online = 0;
unsigned long delayAT = 0;
String avisoEnPantalla = "AGROROBOTS.com";

/*** logs ***/
void setup()
{
  Serial.begin(19200);
  initGLCD();
  iniciarRTC();
  iniciarPinesBus();
  if (GPRS_online == 1) initSIM900(19200);
}

void loop()
{
  char keyPressed = leerTecla();
  
  // Serial1 GPRS
  // Serial2 BUS
  
  if (GPRS_online == 1)
  {
    if (Serial1.available() != 0) inputCharSIM900();  
    if (millis() >= delayAT) comunicacionGPRS();
  }
  
  if (BUS_online == 1)
  {
    if (Serial2.available() > 0) {
      encolarEnTramaRx(Serial2.read());
    }
  }
  
  actualizarHora();
   
  procesoMenu(keyPressed);
} 
