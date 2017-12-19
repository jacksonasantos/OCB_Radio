///////////////////
// Executa Setup //
///////////////////
void executaSetup()
{
  DateTime _datahora = relogio.now();
  char _daysOfTheWeek[7][12] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"};
  int _campos[5][5];
  
  // Campo        Valor               Coluna               Linha               limite minimo      limite maximo
  _campos[0][1] = _datahora.day();    _campos[0][2] = 110; _campos[0][3] = 45; _campos[0][4] = 1; _campos[0][5] = 31;
  _campos[1][1] = _datahora.month();  _campos[1][2] = 155; _campos[1][3] = 45; _campos[1][4] = 1; _campos[1][5] = 12;
  _campos[2][1] = _datahora.year();   _campos[2][2] = 200; _campos[2][3] = 45; _campos[2][4] = 0; _campos[2][5] = 2099;
  _campos[3][1] = _datahora.hour();   _campos[3][2] = 110; _campos[3][3] = 65; _campos[3][4] = 0; _campos[3][5] = 23;
  _campos[4][1] = _datahora.minute(); _campos[4][2] = 155; _campos[4][3] = 65; _campos[4][4] = 0; _campos[4][5] = 59;
  
  monitor.setTextColor(WHITE,BLACK);
  monitor.setTextSize(2);
  monitor.setCursor(_campos[0][2]-70,_campos[0][3]); monitor.print("DATA"); monitor.setCursor(_campos[1][2]-15,_campos[0][3]); monitor.print("/"); monitor.setCursor(_campos[2][2]-20,_campos[0][3]); monitor.print("/"); 
  monitor.setCursor(_campos[0][2]-70,_campos[3][3]); monitor.print("HORA"); monitor.setCursor(_campos[3][2]-15,_campos[3][3]); monitor.print(":"); 
  for (byte i=0; i<5; i++) { // Mostra Campos
    monitor.setCursor(_campos[i][2],_campos[i][3]); monitor.print(_campos[i][1],DEC);
  }

  for (byte i=0; i<5; i++) {              // Percorre Cada campo  

  
     int _var = _campos[i][1];
     int new_var = -1;//_var;
     encoderVlr.setPosition(_var);        // Define Encoder com o Valor Atual do Campo
      
     while (digitalRead(btnMutePin)) {
   
        if (digitalRead(btnModoPin)) return;
        
        monitor.setTextColor(BLACK,WHITE);
        monitor.setCursor(_campos[i][2],_campos[i][3]); 
        monitor.print(_var,DEC);

        encoderVlr.tick();
        new_var = encoderVlr.getPosition();
#ifdef DEBUG
   Serial.print(new_var);Serial.print(" - ");
#endif
        if (new_var < _campos[i][4]) {
          encoderVlr.setPosition(_campos[i][4]);
          new_var = _campos[i][4];
        } else if (new_var > _campos[i][5]) {
          encoderVlr.setPosition(_campos[i][5]);
          new_var = _campos[i][5];
        } 

        if (_var != new_var) {
#ifdef DEBUG
   Serial.println(new_var);
#endif
          _var = new_var;
        } 
     }
     _campos[i][1] = _var;
     monitor.setTextColor(WHITE,BLACK);
     monitor.setCursor(_campos[i][2],_campos[i][3]); monitor.print(_campos[i][1],DEC);
     delay(500);
  }
  relogio.adjust(DateTime(_campos[2][1], _campos[1][1], _campos[0][1], _campos[3][1], _campos[4][1], 0));
  mostra_relogio();
  btnModoState = 2;                       				// Volta para o Radio
}

