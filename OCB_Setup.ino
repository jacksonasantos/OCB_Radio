///////////////////
// Executa Setup //
///////////////////
void executaSetup()
{
  DateTime _datahora             = relogio.now();
  bool     _dadosAlterados       = false;
  byte     _qtdeCampos           = 5;
  int      _campos[_qtdeCampos][5];
  String   _label[_qtdeCampos];
  
  //          Label                    Campo                               Coluna               Linha               limite minimo      limite maximo
  _label[0] = "DATA";  _campos[0][1] = _datahora.day();    _campos[0][2] = 110; _campos[0][3] = 45; _campos[0][4] = 1; _campos[0][5] = 31;
  _label[1] = "/";     _campos[1][1] = _datahora.month();  _campos[1][2] = 155; _campos[1][3] = 45; _campos[1][4] = 1; _campos[1][5] = 12;
  _label[2] = "/";     _campos[2][1] = _datahora.year();   _campos[2][2] = 200; _campos[2][3] = 45; _campos[2][4] = 0; _campos[2][5] = 2099;
  _label[3] = "HORA";  _campos[3][1] = _datahora.hour();   _campos[3][2] = 110; _campos[3][3] = 65; _campos[3][4] = 0; _campos[3][5] = 23;
  _label[4] = ":";     _campos[4][1] = _datahora.minute(); _campos[4][2] = 155; _campos[4][3] = 65; _campos[4][4] = 0; _campos[4][5] = 59;

  limpaArea();

  monitor.setTextSize(2);
  monitor.setTextColor(WHITE, BLUE);
  imprimeTexto("Setup","E",5);
  monitor.setTextColor(WHITE, BLACK);
  monitor.setCursor(_campos[0][2]-60,_campos[0][3]); monitor.print(_label[0]); 
  monitor.setCursor(_campos[1][2]-15,_campos[1][3]); monitor.print(_label[1]); 
  monitor.setCursor(_campos[2][2]-15,_campos[2][3]); monitor.print(_label[2]); 
  monitor.setCursor(_campos[3][2]-60,_campos[3][3]); monitor.print(_label[3]); 
  monitor.setCursor(_campos[4][2]-15,_campos[4][3]); monitor.print(_label[4]); 
  
  for (byte i=0; i<_qtdeCampos; i++) { // Mostra Campos
    monitor.setCursor(_campos[i][2],_campos[i][3]); monitor.print(_campos[i][1],DEC);
  }

  for (byte i=0; i<_qtdeCampos; i++) {              // Percorre Cada campo  

     int _var     = _campos[i][1];
     int _new_var = -1;
     encoderVlr.setPosition(_var);        // Define Encoder com o Valor Atual do Campo
      
     while (digitalRead(btnOkPin)) {
   
        if (digitalRead(btnModoPin)) return;
        
        monitor.setTextColor(BLACK,WHITE);
        monitor.setCursor(_campos[i][2],_campos[i][3]); 
        monitor.print(_var,DEC);

        encoderVlr.tick();
        _new_var = encoderVlr.getPosition();
#ifdef DEBUG
   Serial.print(_new_var);Serial.print(" - ");
#endif
        if (_new_var < _campos[i][4]) {
          encoderVlr.setPosition(_campos[i][4]);
          _new_var = _campos[i][4];
        } else if (_new_var > _campos[i][5]) {
          encoderVlr.setPosition(_campos[i][5]);
          _new_var = _campos[i][5];
        } 

        if (_var != _new_var) {
#ifdef DEBUG
   Serial.println(_new_var);
#endif
          _var = _new_var;
          _dadosAlterados = true;
        } 
     }
     _campos[i][1] = _var;
     monitor.setTextColor(WHITE,BLACK);
     monitor.setCursor(_campos[i][2],_campos[i][3]); monitor.print(_campos[i][1],DEC);
     delay(500);
  }
  
  if (_dadosAlterados) {                          // Efetua a gravação dos valores caso tenham sido alterados
     relogio.adjust(DateTime(_campos[2][1], _campos[1][1], _campos[0][1], _campos[3][1], _campos[4][1], 0));
  }
  
  mostra_relogio();
}

