unsigned long DelayEntreComandosAT = 100;
unsigned long DelayREGISTRARIP = 100;

unsigned int comandoATtimeout = 15000;

const String URLREMOTE = "www.agrorobots.com/remoto.php";
const unsigned char LARGOTXT = 100; // buffer de entrada del txt
const int LARGOPAGINA = 18;       // tamanio de pagina 
unsigned int numPagina = 0;
unsigned int longitudRespuestaWEB = 0; // la longitud de lo que trae en la respuesta

// Buffer de entrada
String bufferRespuestaWEB = "";
char txtRecibido[LARGOTXT+1];
unsigned int  errorWeb;

// Estados SIM900
String IPGPRS = "0.0.0.0";
boolean callReady;

      unsigned char estadoGPRS = 0;
const unsigned char POWERON = 0;
const unsigned char CGDCONT = 1;
const unsigned char CONTYPE = 2;
const unsigned char APN = 3;
const unsigned char REGISTRARIP = 4;
const unsigned char GETIP = 5;
const unsigned char HTTPINIT = 6;
const unsigned char HTTPPARA = 7;
const unsigned char SETURL = 8;
const unsigned char HTTPACTION = 9;
const unsigned char HTTPREAD = 10;
const unsigned char HTTPTERM = 11;

void SIM900_PowerOn()
{
  delay(100);
  digitalWrite(6, LOW);
  imprimir("iniciando GPRS...", DESA);
  delay(100);
  digitalWrite(6, HIGH);
  delay(100);
  digitalWrite(6, HIGH);
  setDelayAT(comandoATtimeout);
}

void ejecutarFuncionRemota(String param, String valor)
{
  imprimir(param + " <param-valor> " + valor, DESA );
}

int analizarRespuestaWeb()
{
  String textoRecibido = String(txtRecibido);  
  
  unsigned char longitudRecibido = 0;
    
  char inicio = 0; // inicia en el primer espacio despues del :
  char fin = 0;    // separa parametros por ;
  String param = "", valor = "";

  imprimir("Detectar parametros!", DESA);  
  while (fin != -1)
  {
    inicio = (fin != 0)? fin + 1 /* sig param */ :textoRecibido.indexOf(" ", textoRecibido.indexOf(":") + 1);
    fin = textoRecibido.indexOf(";", inicio + 1);
    imprimir("analizado >" + textoRecibido + "<", DEBUG);
    imprimir(String(inicio) + " <-inicio fin-> " + String(fin), DEBUG);
    if (fin == -1)
    { 
      // analizar donde poner el buffer
      bufferRespuestaWEB = textoRecibido.substring(inicio + 1, textoRecibido.length());
      imprimir(bufferRespuestaWEB + "<< al buffer", DEBUG);
      numPagina = numPagina + LARGOPAGINA;
      return 1;
    } else {
      // separa en dato y valor y ejecuta
      String asignacion = textoRecibido.substring(inicio, fin);
      imprimir(asignacion + "<< analiza", DEBUG);
      if ((bufferRespuestaWEB != NULL) && (bufferRespuestaWEB != ""))
      {
        asignacion = bufferRespuestaWEB + asignacion;
        bufferRespuestaWEB = "";
        imprimir(bufferRespuestaWEB + " < buffer", DEBUG);
        imprimir(asignacion + "<< analiza con buffer ", DEBUG);
      }
      char igual = asignacion.indexOf("=");
      // en caso de igual = -1 exception !
      param = asignacion.substring(0, igual);
      valor = asignacion.substring(igual, asignacion.length());
      
      ejecutarFuncionRemota(param, valor);
    }
  }
  
  return 0;
}

unsigned char analizaRespuesta(String respuesta)
{
  if (respuesta.indexOf("+HTTPREAD:") > -1)
  { 
    if (respuesta.indexOf("OK") > -1)
    {    
      analizarRespuestaWeb();
      
      imprimir(String(numPagina) + " <num-long> " + String(longitudRespuestaWEB), DEBUG);
      
      if (longitudRespuestaWEB <= numPagina)
        siguienteComandoAT();
      else
        sendComandoAT(String("AT+HTTPREAD="+String(numPagina)+","+String(LARGOPAGINA)));
        
      return 1;
    }

    if (respuesta.indexOf("ERROR") > -1)
    {
      repetirComandoAT(); 
      return 2;
    }
    return 3;
  }

  if ((respuesta.indexOf("AT+CGDCONT") > -1) ||      
    (respuesta.indexOf("AT+HTTPPARA=") > -1) ||
    (respuesta.indexOf("AT+SAPBR=") > -1) || 
    (respuesta.indexOf("AT+HTTPTERM") > -1) )
  {
    if (respuesta.indexOf("OK") > -1)
    {
      if (respuesta.indexOf("AT+HTTPTERM") > -1) 
      {
        bufferRespuestaWEB = "";
        setEstadoGPRS(HTTPINIT);
      }
      
      if (respuesta.indexOf("+SAPBR:") > -1) 
      {
        unsigned char inicio = respuesta .indexOf("\"");
        unsigned char fin = respuesta.lastIndexOf("\"");
        IPGPRS = respuesta.substring(inicio + 1, fin);
        imprimir(String("IP Obtenida: " + IPGPRS), DESA);
        if (String(IPGPRS).indexOf("0.0.0.0") > -1)
        {
          delay(DelayREGISTRARIP);
          DelayREGISTRARIP = DelayREGISTRARIP * 2;
          setEstadoGPRS(REGISTRARIP);
        }
      }      
      siguienteComandoAT();
      return 1;
    }

    if (respuesta.indexOf("ERROR") > -1)
    {
      repetirComandoAT();  // Atenti repeti el comando que tiene en el estado, pero no el que repondio el chip TODO: Fix
      return 2;
    }
    
    return 3;
  }

  if (respuesta.indexOf("+HTTPACTION") > -1) 
  {
    if (respuesta.indexOf("OK") > -1)
    {
     bufferRespuestaWEB = "";
     unsigned char primero = respuesta.indexOf(",");
     unsigned char segundo = (segundo != 255)? respuesta.indexOf(",", primero + 1) : 255;
     unsigned char findelinea  = (segundo != 255)? respuesta.indexOf(" ", segundo + 1) : 255;

     imprimir(String(">> [" + String(primero) + "] - [" + String(segundo) + "] - [" + String(findelinea) + "]"), DESA);

     if ((primero != 255) && (segundo != 255) && (findelinea != 255))
     {
       errorWeb = respuesta.substring(primero+1, segundo).toInt();
       longitudRespuestaWEB = respuesta.substring(segundo+1, findelinea).toInt();
       
       imprimir(String(">> [resp: " + String(errorWeb) + "] - [long: " + String(longitudRespuestaWEB) + "]"), DESA);
       
       if (errorWeb != 200)
       {
          setEstadoGPRS(HTTPTERM); 
       } else {
          numPagina = 0;
          siguienteComandoAT();
       }
     }
    }

    if (respuesta.indexOf("ERROR") > -1)
    {
      repetirComandoAT();  // Atenti repeti el comando que tiene en el estado, pero no el que repondio el chip TODO: Fix
      return 2;
    }
    
    return 3;
  }

  if (respuesta.indexOf("AT+HTTPINIT") > -1)
  {
    if (respuesta.indexOf("OK") > -1)
    {
      siguienteComandoAT();  // Atenti repeti el comando que tiene en el estado, pero no el que repondio el chip TODO: Fix
      return 1;
    }
    if (respuesta.indexOf("ERROR") > -1)
    {
      sendComandoAT("AT+HTTPTERM");  // Atenti repeti el comando que tiene en el estado, pero no el que repondio el chip TODO: Fix
      return 2;
    }
    
    return 3;
  }
  
  if (respuesta.indexOf("CALL READY") > -1)
  {
    blanquerTxtRecibido();
    callReady = true;
    setEstadoGPRS(CGDCONT); // 1
    setDelayAT(100);
    return 1;
  }

  if (respuesta.indexOf("NORMAL POWER DOWN") > -1)
  {
    blanquerTxtRecibido();
    callReady = false;
    if (estadoGPRS != POWERON) {
      setEstadoGPRS(POWERON); // 0
      setDelayAT(100);
    }
    return 2;
  }

}

void setDelayAT(unsigned long retardo)
{
  delayAT =  millis() + retardo;
}

void blanquerTxtRecibido()
{
  imprimir("blanquea", DEBUG);
  for (int i = 0; i <= LARGOTXT; i++)
  {
    txtRecibido[i] = ' ';
  }
}

void setEstadoGPRS(unsigned char estado)
{
  imprimir(String("De " + String(estadoGPRS) + " a " + String(estado)), DESA);
  estadoGPRS = estado;
}

// va al loop
void inputCharSIM900()
{
  // lee e imprime
  // while(Serial1.available()==0);
  char caracter = toupper(Serial1.read());
  imprimir(caracter, DESA);

  // encola caracter en el texto
  for (int i = 0; i < LARGOTXT; i++)
  {
    txtRecibido[i] = txtRecibido[i+1];
  }
  txtRecibido[LARGOTXT] = ((caracter > 32) && (caracter < 126))?caracter:' ';
  imprimir(String(">> " +String(txtRecibido) + "<<"), DEBUG);
  analizaRespuesta(String(txtRecibido));  
}

void sendComandoAT(String comando)
{
  setDelayAT(comandoATtimeout);
  imprimir(String("send: " + comando), DESA);
  blanquerTxtRecibido();
  Serial1.println(comando);
}

void initSIM900(int baudRate)
{
  Serial1.begin(baudRate);
  estadoGPRS = 0;
  setDelayAT(0);
  blanquerTxtRecibido();
}

void repetirComandoAT()
{
  blanquerTxtRecibido();
  setDelayAT(DelayEntreComandosAT);
}

void siguienteComandoAT()
{
  blanquerTxtRecibido();
  setEstadoGPRS(estadoGPRS + 1);
  if (estadoGPRS == REGISTRARIP)
  {
     imprimir(String("delay " + String(DelayREGISTRARIP)), DEBUG);
    setDelayAT(DelayREGISTRARIP);
  }
  else
  {
    setDelayAT(DelayEntreComandosAT);
    imprimir(String("delay " + String(DelayEntreComandosAT)), DEBUG);
  }
}

void comunicacionGPRS()
{
  imprimir(String("revisa estado GPRS " + String(estadoGPRS)), DESA);
  switch (estadoGPRS) {
  case POWERON:
    SIM900_PowerOn();
    break;
  case CGDCONT:
    sendComandoAT("AT+CGDCONT=1,\"IP\",\"gprs.claro.com.ar\"");
    break;
  case CONTYPE: 
    sendComandoAT("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
    break;
  case APN: 
    sendComandoAT("AT+SAPBR=3,1,\"APN\",\"gprs.claro.com.ar\"");
    break;
  case REGISTRARIP:
    sendComandoAT("AT+SAPBR=1,1");
    break;
  case GETIP:
    sendComandoAT("AT+SAPBR=2,1");
    break;
  case HTTPINIT:
    sendComandoAT("AT+HTTPINIT");
    break;
  case HTTPPARA:
    sendComandoAT("AT+HTTPPARA=\"CID\",1");
    break;
  case SETURL:
    sendComandoAT("AT+HTTPPARA=\"URL\",\""+URLREMOTE+"\"");
    break;
  case HTTPACTION:
    sendComandoAT("AT+HTTPACTION=0");
    break;
  case HTTPREAD:
    // puede pasar por time out
    if (String(txtRecibido).indexOf("+HTTPACTION") > -1) 
    {
      imprimir("Por time out!! ", DEBUG);
      analizarRespuestaWeb();
    }
    else
    {
      sendComandoAT(String("AT+HTTPREAD="+String(numPagina)+","+String(LARGOPAGINA)));
      setDelayAT(comandoATtimeout*4);
    }
    break;
  case HTTPTERM:
    sendComandoAT("AT+HTTPTERM");
    break;
  }
}




