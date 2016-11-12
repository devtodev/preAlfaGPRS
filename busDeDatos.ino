#define PIN_BUS_DE    9 // DATA ENABLED

/*

 *    Interfaz de comunicacion
 *    cmiguens agrorobots
 *    11/05/13 freescale
 *    16/09/13 arduino
 */

// Identificador unico de Dispositivo

#define ID1                   0
#define ID2                   0

#define ID1BROADCAST          0
#define ID2BROADCAST          1

// bytes de la trama
#define DIRECCION             0 //  XX00000000
#define FUNCION               2 //  00X0000000
#define DATOS                 3 //  000XXXXXX0
#define CRC                   9 //  000000000X
#define LARGOTRAMA            10

// estados

#define ESPERA_TRAMA                          0

#define DELAY_ENTRE_TRAMA                     2
#define TIME_TO_TIMEOUT                      50

// funciones

#define RESEND                0  // solicitud de reenvio
#define GETNUEVO              1
#define GETTIPO               2
#define SETLED                3
#define SETSWITCHES           4
#define GETSWITCHES           5
#define GETSENSOR             6

// excepciones
#define RESPUESTA_OK          400

#define OTRO_DESTINATARIO     200
#define FUNCION_DESCONOCIDA   201
#define CRC_ERROR             203
#define TIMEOUT               204 // atrapar por time out

unsigned char cantReintentosBus = 0;


//   ==============================
//   || FUNCIONES  DE INTERPRETAR ||
//   ==============================

unsigned char leerDelBus()
{
  habilitarLectura();
  if (Serial.available() > 0) {
    //digitalWrite(PIN_BUS_LED, HIGH);
    return Serial.read();
    //digitalWrite(PIN_BUS_LED, LOW);
  }
  return -1;
}

unsigned char tramaTx[LARGOTRAMA];
unsigned char tramaRx[LARGOTRAMA];
unsigned char iTrama = 0;

char encolarEnTramaRx(unsigned char caracter)
{
  tramaRx[iTrama] = caracter;
  iTrama++;
  if (iTrama == LARGOTRAMA)
  {
    iTrama = 0;
    return evaluarTrama();
  }
  return 0;
}

char evaluarTrama()
{
    unsigned char CRC_Rx = tramaRx[CRC];
    unsigned char CRC_Rx_calc = tramaCRC(tramaRx);
    
    // Se evalua CRC Error por parte del master y del esclavo
    // Evaluar time out 
    
    if (CRC_Rx != CRC_Rx_calc)
    {
      busSolicitarReenvio("" + tramaRx[0] + tramaRx[1]);
      return CRC_ERROR;
    }
    
    if ((tramaRx[DIRECCION] != tramaTx[DIRECCION]) ||
        (tramaRx[DIRECCION+1] != tramaTx[DIRECCION+1]))
    {
      return OTRO_DESTINATARIO;
    }
    
    if (tramaRx[FUNCION] == RESEND)
    {
      // contar reintentos !!!
      enviarTrama(tramaTx);
      return CRC_ERROR;
    } 

    if (tramaRx[FUNCION] != tramaTx[FUNCION])
    {
      return FUNCION_DESCONOCIDA;
    }
    
    if (tramaRx[FUNCION] == GETTIPO)
    {
      agregarNuevoNodo(tramaRx);
      return RESPUESTA_OK;
    }
     
    if ((tramaRx[FUNCION] == GETTIPO) || (tramaRx[FUNCION] == SETSWITCHES) || (tramaRx[FUNCION] == GETSWITCHES))
    {
      // agregar nuevo estado
      agregarNuevoNodo(tramaRx);
      limpiarTramas();
      return RESPUESTA_OK;
    }
    
    if (tramaRx[FUNCION] == GETSENSOR)
    {
      setEstadoActual(tramaRx);
      limpiarTramas();
      return RESPUESTA_OK;
    }
     
    return FUNCION_DESCONOCIDA;
}

void limpiarTramas()
{
  for (int i = 0; i < LARGOTRAMA; i++)
  {
    tramaRx[i] = 0;
    tramaTx[i] = 0;
  }
}

//   =======================
//   || FUNCIONES  DEL BUS ||
//   =======================

void prepararTrama(String IP, unsigned char funcion, unsigned char data[6])
{
  unsigned char trama[LARGOTRAMA];
  trama[0] = IP[0];
  trama[1] = IP[1];
  trama[2] = funcion;
  trama[3] = (data != NULL)?data[0]:0;
  trama[4] = (data != NULL)?data[1]:0;
  trama[5] = (data != NULL)?data[2]:0;
  trama[6] = (data != NULL)?data[3]:0;
  trama[7] = (data != NULL)?data[4]:0;
  trama[8] = (data != NULL)?data[5]:0;
  trama[9] = tramaCRC(trama);
  enviarTrama(trama);

}

// Hace una llamada a broadcast para escuchar si hay algun nodo no registrado
void busConsultarNuevoSlave()
{
  // broadcast 00 funcion 2 datos 3 crc 9
  prepararTrama("" + ID1BROADCAST + ID2BROADCAST, GETNUEVO, NULL);
}

// Llamada unicast para consultar el tipo de un nodo
void busConsultarTipo(String IP)
{
  // broadcast 00 funcion 2 datos 3 crc 9
  prepararTrama(IP, GETTIPO, NULL);
}

//   ==============================
//   || FUNCIONES  DEL ZAPATILLA ||
//   ==============================

unsigned char getSwitch(String IP)
{
  prepararTrama(IP, GETSWITCHES, NULL);
}

unsigned char setSwitch(String IP, unsigned char switches)
{
  unsigned char datos[6] = {switches, 0, 0, 0, 0, 0};
  prepararTrama(IP, SETSWITCHES, datos);
}


//   ==================================
//   || FUNCIONES  DE  COMUNICACION  ||
//   ==================================


// Set led del nodo en forma unicast para debug
void busSetLed(String IP, unsigned char modo)
{
  unsigned char data[6] = {modo,0,0,0,0,0};
  prepararTrama(IP, SETLED, data);
}


// Set led del nodo en forma unicast para debug
void busSolicitarReenvio(String IP)
{
  prepararTrama(IP, RESEND, NULL);
}

void enviarTrama(unsigned char trama[])
{ 
  avisoEnPantalla = "";
  for (int i = 0; i < LARGOTRAMA; i++)
  {
    tramaTx[i] = trama[i];
    delay(DELAY_ENTRE_TRAMA);
    enviarAlBus(trama[i]);
    avisoEnPantalla = avisoEnPantalla + String(trama[i]) + ",";
  }
  avisoEnPantalla = avisoEnPantalla + String("}");
}

void iniciarPinesBus()
{
  pinMode(PIN_BUS_DE, OUTPUT); // pin digital 9
  pinMode(16, OUTPUT);
  pinMode(17, INPUT);
  digitalWrite(17, HIGH);
  habilitarLectura();
  Serial2.begin(9600, SERIAL_8N1);
}


void habilitarEscritura()
{
  digitalWrite(PIN_BUS_DE, 1);
}

void habilitarLectura()
{
  digitalWrite(PIN_BUS_DE, 0);
}

void enviarAlBus(unsigned char caracter)
{
    //digitalWrite(PIN_BUS_LED, HIGH);
    habilitarEscritura();
    delay(1);
    Serial.write(caracter);
    delay(2);
    habilitarLectura();
    //digitalWrite(PIN_BUS_LED, LOW);
}

unsigned char tramaCRC(unsigned char trama[10])
{
	unsigned char crc = 0xFF;
	unsigned char tempCRC;
	
	unsigned char tempLSB, flag;
	int i, j;
	
	for (i = 0; i < CRC; i++) {
		tempLSB = 0xFF & crc;

		crc = tempLSB ^ trama[i];

		for (j = 0; j < 8; j++) {
			tempCRC = crc;
			flag = tempCRC & 0x01;
			crc = crc >> 1;

			if (flag)
				crc = crc ^ 0xA1;
		} 
	} 
	return crc;
}

