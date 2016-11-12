// Lista de nodos
const unsigned char MAXNODOS = 16;

char nodos[MAXNODOS][4]= {}; // "id", "tipo", "version firmware"
unsigned char cantNodos;

#define MAXCANTESTADOS 60
int estadoNodo[MAXCANTESTADOS][10]; // ID1 ID2 VALOR UNIDAD DIA MES ANIO HORA MINUTOS SEGUNDOS
unsigned char cursorEstados = 0;

void setEstadoActual(unsigned char trama[])
{
  estadoNodo[cursorEstados][0] = trama[DIRECCION];
  estadoNodo[cursorEstados][1] = trama[DIRECCION + 1];
  estadoNodo[cursorEstados][2] = trama[DATOS];
  estadoNodo[cursorEstados][3] = trama[DATOS + 1];
  estadoNodo[cursorEstados][4] = RTC.day;
  estadoNodo[cursorEstados][5] = RTC.month;
  estadoNodo[cursorEstados][6] = RTC.year;
  estadoNodo[cursorEstados][7] = RTC.hour;
  estadoNodo[cursorEstados][8] = RTC.minute;
  estadoNodo[cursorEstados][9] = RTC.second;
  cursorEstados++;
  if (cursorEstados >= MAXCANTESTADOS)
    cursorEstados = 0;
}

char existeNuevoNodo()
{
  // hace la consulta en BROADCAST de nuevos
  return 0;
}

void agregarNuevoNodo(unsigned char trama[])
{
  // consulta al id
  // y lo registra
  nodos[cantNodos][0] = trama[DIRECCION];
  nodos[cantNodos][1] = trama[DIRECCION + 1];
  nodos[cantNodos][2] = trama[DATOS]; // tipo
  nodos[cantNodos][3] = trama[DATOS + 1]; // firmware
  cantNodos++;
}

void limpiaListaNodos()
{
  cantNodos = 0;
  for (int i = 0; i < cantNodos; i++)
  {
    // solicita desregistro
    nodos[i][0] = -1;
    nodos[i][1] = -1;
    nodos[i][1] = -1;
  }
}

void barreNodos()
{
  limpiaListaNodos();
  int i = 0;
  boolean nuevos = true;
  unsigned char bus;
  while (i < MAXNODOS)
  {
    busConsultarNuevoSlave(); // pregunta en broadcast
    delay(50);
    bus = leerDelBus();
    nuevos = (bus != -1);
  }
}

