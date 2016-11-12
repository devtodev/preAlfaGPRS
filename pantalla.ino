void initGLCD()
{
    // Display
  GLCD.Init();
  GLCD.ClearScreen();
  GLCD.SelectFont(System5x7);
}

char *avisoConDelay[][3] = {
   // Aviso 
   {"", ""},
};

void setAvisoConDelay(String aviso, unsigned char duracion)
{
  //avisoConDelay
  /* 
      TODO:
      getVencimientoDelUltimo mensaje de la lista, 
      setMomentoDelNuevoMensaje = getVencimientoDelUltimo
      setVencimientoDelNuevoMensaje = setMomentoDelNuevoMensaje + duracion;
      momento= GetTimeNow + dentroDeSegundosDesdeAhora;
      vencimiento = momento + duracion;      
  */
}

void  imprimirAvisoEnPantalla()
{
  imprimir(0, 7, avisoEnPantalla);
}


void imprimir(int x, int y, String texto)
{
  GLCD.CursorTo(x, y);
  GLCD.print(texto);
}

void cabeceraMenu(char keyPressed, String titulo) {
  imprimir(0, 0, (LOGS <= DEBUG)? titulo + " " + menuIndice  + " " + menuIndice  + " " +  menuIndiceAnterior : titulo);
  imprimirAvisoEnPantalla();
  imprimir(16, 7, horas + ":" + minutos);
}

void setAviso(String aviso)
{
  GLCD.ClearScreen();
  avisoEnPantalla = aviso;
}

void imprimirLista(char *lista[][3])
{
    int cant = atoi(lista[0][0]); 
    char seleccion;

    for (int i = 1; i < cant; i++)
    {
      //seleccion = (listaSeleccion == i)?'*':'-';
      GLCD.CursorTo(0, i+1);
      GLCD.println(lista[i][idioma]);
    }
}

    /*  Cursor para elegir
    if ((keyPressed == 'A') && (listaSeleccion > 1)) listaSeleccion--;
    if ((keyPressed == 'B') && (listaSeleccion < cant - 1)) listaSeleccion++;
    
    if (keyPressed == 'C') 
    {
      return listaSeleccion;
    }
    for (int i = 1; i < cant; i++)
    {
      seleccion = (listaSeleccion == i)?'*':'-';
      GLCD.CursorTo(0, i+1);
      GLCD.println(lista[i][idioma]);
    }    
    if (keyPressed == 'D') 
    {
      return -1;
    }
    */

