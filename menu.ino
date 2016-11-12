// Varibles del programa ------------------------------------------------------
  int menuIndice, menuIndiceAnterior;
  int idioma; // 1 castellano 2 ingles 3 portugues 4 frances 5 aleman 6 aleman
//  int listaSeleccion = 1;

char *menuPrincipalLista[][3] = {
   {"5", "", ""},
   {"A)Observar Sensores", "WatchSensors", ""},
   {"B)Set enchufes", "Set switches", ""},
   {"C)Configuracion", "Configuracion", ""},
   {"D)Acerca del producto", "Acerca de...", ""},
};

char *menuNodos[][3] = {
   {"5", "", ""},
   {"A) Set Led", "", ""},
   {"B) Consultar Tipo", "", ""},
   {"C) Consultar Nuevo", "", ""},
   {"D) Barre Nodos", "", ""},
};

char *listaConfiguracion[][3] = {
   {"5", "", ""},
   {"A) APN", "APN", ""},
   {"B) server", "server", ""},
   {"C) time outs", "time outs", ""},
   {"D) modo avion", "Airplane mode", ""},
};

void procesoMenu(char keyPressed)
{
  if (keyPressed) 
  { // Vuelve al inicio
    if (keyPressed == '#') {
      menuIndice = 0; // ver 
      GLCD.ClearScreen();
    }
  }
 switch (menuIndice) {
   case 0:
     menuPrincipal(keyPressed);
     break;
   case 1:
     menuSensores(keyPressed);
     break;
   case 2:
     menuEnchufes(keyPressed);
     break;
   case 3:
     menuConfiguracion(keyPressed);
     break;
   case 4:
     menuAcercaDelProducto(keyPressed);
     break;
   case 5:
//     menuSetFotoperiodo(keyPressed);
     break;
   case 6:
//     menuSetVentilacion(keyPressed);
     break;
   case 7:
 //    menuSetHidroponia(keyPressed);
     break;
   case 8:
 //    menuAcercaDe(keyPressed);
     break;
   case 9:
 //    menuVerEstados(keyPressed);
     break;
 }  
 
   // cuando cambia de pantalla, refresca el lcd, y hace listaSeleccion en 1;
  if (menuIndice != menuIndiceAnterior)
  { 
    menuIndiceAnterior = menuIndice;
    GLCD.ClearScreen();
  }
}


//   =========================
//   || MENU  DE  PRINCIPAL ||
//   =========================

//int getOpcion

void menuPrincipal(char keyPressed) {
  if(idioma == 0) cabeceraMenu(keyPressed, "Menu Principal");
  if(idioma == 1) cabeceraMenu(keyPressed, "Main Menu");
  
  imprimirLista(menuPrincipalLista);

  if (keyPressed == 'A') menuIndice = 1;  // sensores
  if (keyPressed == 'B') menuIndice = 2;  // enchufes
  if (keyPressed == 'C') menuIndice = 3;  // configuracion  
  if (keyPressed == 'D') menuIndice = 4;  // acerca de
}

void menuSensores(char keyPressed)
{
  cabeceraMenu(keyPressed, "Menu\\Sensores");
//    int respuesta = imprimirLista(menuPrincipalLista);

  if (keyPressed == '*') menuIndice = 0;  // sensores

}

void menuEnchufes(char keyPressed)
{
  cabeceraMenu(keyPressed, "Menu\\Enchufes ");
  //  int respuesta = imprimirLista(menuPrincipalLista);

  if (keyPressed == '*') menuIndice = 0;  // sensores

}

boolean swLed = false;

void menuConfiguracion(char keyPressed)
{
  cabeceraMenu(keyPressed, "Menu\\Configuracion");
  imprimirLista(menuNodos);

  if (keyPressed == '*') menuIndice = 0; 
  if (keyPressed == 'A') { busSetLed(ID1BROADCAST + ID1BROADCAST + "", swLed); swLed = !swLed; }
  if (keyPressed == 'B') busConsultarTipo(ID1BROADCAST + ID1BROADCAST + ""); 
  if (keyPressed == 'C') busConsultarNuevoSlave(); 
  if (keyPressed == 'D') barreNodos(); 

}

void menuAcercaDelProducto(char keyPressed)
{
  cabeceraMenu(keyPressed, "Menu\\Acerca de");
  imprimirLista(listaConfiguracion);

  if (keyPressed == '*') menuIndice = 0;  // sensores
}

     
