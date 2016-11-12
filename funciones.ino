//   =============================
//   || FUNCIONES  DE  TECLADO  ||
//   =============================
// VAR teclado
  const byte ROWS = 4;
  const byte COLS = 4;
  const char hexaKeys[ROWS][COLS] = {
    {'9','6','3','#'},
    {'8','5','2','0'},
    {'7','4','1','*'},
    {'C','B','A','D'},
  };
  byte colPins[COLS] = {32, 34, 36, 30}; 
  byte rowPins[ROWS] = {24, 26, 28, 22};
  Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
  char ultimaTecla;
  int cursorTiempo = -1;
  char cCursor = '*', nCursor = '-', vCursor;
  String datoDelTeclado;
  int xDato, yDato, cantDato;

String openByte(unsigned char c)
{
    String data = "            ";
    char x;
    unsigned char mask = 0x80;
    for( x = 0 ; x < 8 ; ++x )
    {
        if( c & mask )
            data[x] = 1;
        else
            data[x] = 0;
        mask >>= 1;        
    }
}

unsigned char closeByte(String s)
{
    unsigned char dato = 0;
    unsigned char mask = 0x80;
    for(char x = 0 ; x < 8 ; ++x )
    {
      dato = (s[x] == '1')? dato ^ x : dato;
    }
    return dato;
}

char leerTecla()
{
  return customKeypad.getKey();
}

int ingresarDatoConMaximoYMinimo(int x, int y, int ncar, char keyPressed, int maximo, int minimo)
{
  int dato = ingresarDato(x, y, ncar, keyPressed);
  if ((dato != -1) && ((dato > maximo) || (dato <= minimo)))
    return -1;
  return dato;
}

int ingresarDatoConMaximo(int x, int y, int ncar, char keyPressed, int maximo)
{
  int dato = ingresarDato(x, y, ncar, keyPressed);
  if ((dato != -1) && (dato > maximo))
    return -1;
  return dato;
}

int ingresarDato(int x, int y, int ncar, char keyPressed)
{
  if (cursorTiempo == -1) 
  {
    datoDelTeclado = "";
  }

  if (cursorTiempo != (millis()/1000))
  {
    vCursor = (vCursor == nCursor)?cCursor:nCursor;
    cursorTiempo = millis()/1000;
  }

  // completa faltantes
  String completar = "";
  for (int i = datoDelTeclado.length()+1; i < ncar; i++)
    completar = completar + "-";
  
  GLCD.CursorTo(x, y);
  GLCD.println(datoDelTeclado + String(vCursor) + completar);

  // Acepta
  if (keyPressed == 'C')
  {
      cursorTiempo = -1;
      return atoi(&datoDelTeclado[0]);    
  }
  
  // Borra
  if ((keyPressed == 'D') && (datoDelTeclado.length() > 1))
  {
      datoDelTeclado = datoDelTeclado.substring(0, datoDelTeclado.length()-1);
  }
  
  // toma los azules
  if ((keyPressed == '0') || (keyPressed == '1') || (keyPressed == '2') || (keyPressed == '3') || (keyPressed == '4') || (keyPressed == '5') || (keyPressed == '6') || (keyPressed == '7') || (keyPressed == '8') || (keyPressed == '9'))
  {
    datoDelTeclado = datoDelTeclado + String(keyPressed);
    if (datoDelTeclado.length() == ncar)
    {
      cursorTiempo = -1;
      return atoi(&datoDelTeclado[0]);
    }
  }

  return -1;
}


//   ==========================
//   || FUNCIONES  DE  LOGS  ||
//   ==========================

const unsigned char PROD  = 3;
const unsigned char DESA  = 2;
const unsigned char DEBUG = 1;
const unsigned char LOGS = PROD;
char modoConsola = 0;
String consola[8] = {"", "", "", "", "", "", "", "" };

void imprimir(String txt, unsigned char nivel)
{
  // TODO: Modo consola!
  if (LOGS <= nivel)
    Serial.println(txt);
}

void imprimir(char caracter, unsigned char nivel)
{
  if (LOGS <= nivel)
    Serial.print(caracter);
}


//   ==========================
//   || FUNCIONES  DE  RELOJ ||
//   ==========================
String horas, minutos, segundos;

uint16_t startAddr = 0x0000;            // Start address to store in the NV-RAM
uint16_t lastAddr;                      // new address for storing in NV-RAM
uint16_t TimeIsSet = 0xaa55;            // Helper that time must not set again
int SQWpin = 12;

void actualizarHora()
{
  RTC.getTime();
  horas = (RTC.hour < 10)? "0" + String(RTC.hour):String(RTC.hour);                  
  minutos = (RTC.minute < 10)? "0" + String(RTC.minute):String(RTC.minute);          
  segundos = (RTC.second < 10)? "0" + String(RTC.second):String(RTC.second);          
}

void iniciarRTC()
{
  pinMode(SQWpin, INPUT);                    // Test of the SQW pin, D2 = INPUT
  digitalWrite(SQWpin, HIGH);                // Test of the SQW pin, D2 = Pullup on

/*
   PLEASE NOTICE: WE HAVE MADE AN ADDRESS SHIFT FOR THE NV-RAM!!!
                  NV-RAM ADDRESS 0x08 HAS TO ADDRESSED WITH ADDRESS 0x00=0
                  TO AVOID OVERWRITING THE CLOCK REGISTERS IN CASE OF
                  ERRORS IN YOUR CODE. SO THE LAST ADDRESS IS 0x38=56!
*/
  RTC.setRAM(0, (uint8_t *)&startAddr, sizeof(uint16_t));// Store startAddr in NV-RAM address 0x08 

/*
   Uncomment the next 2 lines if you want to SET the clock
   Comment them out if the clock is set.
   DON'T ASK ME WHY: YOU MUST UPLOAD THE CODE TWICE TO LET HIM WORK
   AFTER SETTING THE CLOCK ONCE.
*/
  // TimeIsSet = 0xffff;
  // RTC.setRAM(54, (uint8_t *)&TimeIsSet, sizeof(uint16_t));  

/*
  Control the clock.
  Clock will only be set if NV-RAM Address does not contain 0xaa.
  DS1307 should have a battery backup.
*/
  RTC.getRAM(54, (uint8_t *)&TimeIsSet, sizeof(uint16_t));
  if (TimeIsSet != 0xaa55)
  {
    RTC.stopClock();
        
    RTC.fillByYMD(2014,1,16); // 2013,4,11 --- 2013,3,28
    RTC.fillByHMS(14,10,0); // fecha original 17/05/2012
    
    RTC.setTime();
    TimeIsSet = 0xaa55;
    RTC.setRAM(54, (uint8_t *)&TimeIsSet, sizeof(uint16_t));
    RTC.startClock();
  }
  else
  {
    RTC.getTime();
  }

/*
   Control Register for SQW pin which can be used as an interrupt.
*/
  RTC.ctrl = 0x11;                      // 0x00=disable SQW pin, 0x10=1Hz,
                                        // 0x11=4096Hz, 0x12=8192Hz, 0x13=32768Hz
  RTC.setCTRL();

  uint8_t MESZ;

  MESZ = RTC.isMEZSummerTime();
}



