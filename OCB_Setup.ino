/*
 * OCB Setup
 * 
void executaSetup      ()
void executaSetupClock ()
void executaSetupConfig()
void saveSettings      ()
void loadSettings      ()
*/

///////////////////
// Executa Setup //
///////////////////
void executaSetup()
{
  if (btnSetupState==0) {
    btnSetupState++;
    executaSetupClock();
    return;
  }
  else {
    if (btnSetupState==1) {
      btnSetupState++;
      executaSetupConfig();
      return;
    }
    else {
      btnSetupState=0;
      return;     
    }
  }
}
///////////////////
// Executa Setup //
///////////////////
void executaSetupConfig()
{
  bool         _dadosAlterados       = false;
  byte         _qtdeCampos           = 11;
  int          _campos[_qtdeCampos][5];
  String       _label[_qtdeCampos];
  unsigned int _colSetup = 90;
  unsigned int _linSetup = 35;

  loadSettings();

  //           Label                       Campo                                         Coluna                     Linha                      limite minimo      limite maximo
  _label[0]  = "ID";      _campos[0][1]  = settings.id;                 _campos[0][2]  = _colSetup;     _campos[0][3]  = _linSetup+0;   _campos[0][4]  = 0; _campos[0][5] = 0;
  _label[1]  = "Modo";    _campos[1][1]  = settings.source;             _campos[1][2]  = _colSetup;     _campos[1][3]  = _linSetup+20;  _campos[1][4]  = 1; _campos[1][5] = 12;
  _label[2]  = "Preset";  _campos[2][1]  = settings.currentPreset;      _campos[2][2]  = _colSetup;     _campos[2][3]  = _linSetup+40;  _campos[2][4]  = 0; _campos[2][5] = 2099;
  _label[3]  = "PST 123"; _campos[3][1]  = settings.presetFrequency[0]; _campos[3][2]  = _colSetup;     _campos[3][3]  = _linSetup+60;  _campos[3][4]  = 0; _campos[3][5] = 23;
  _label[4]  = "";        _campos[4][1]  = settings.presetFrequency[1]; _campos[4][2]  = _colSetup+80;  _campos[4][3]  = _linSetup+60;  _campos[4][4]  = 0; _campos[4][5] = 59;
  _label[5]  = "";        _campos[5][1]  = settings.presetFrequency[2]; _campos[5][2]  = _colSetup+160; _campos[5][3]  = _linSetup+60;  _campos[5][4]  = 0; _campos[5][5] = 59;
  _label[6]  = "Freq";    _campos[6][1]  = settings.currentFrequency;   _campos[6][2]  = _colSetup;     _campos[6][3]  = _linSetup+80;  _campos[6][4]  = 0; _campos[6][5] = 59;
  _label[7]  = "Volume";  _campos[7][1]  = settings.currentVolume;      _campos[7][2]  = _colSetup;     _campos[7][3]  = _linSetup+100; _campos[7][4]  = 0; _campos[7][5] = 59;
  _label[8]  = "Grave";   _campos[8][1]  = settings.bass;               _campos[8][2]  = _colSetup;     _campos[8][3]  = _linSetup+120; _campos[8][4]  = 0; _campos[8][5] = 59;
  _label[9]  = "Agudo";   _campos[9][1]  = settings.treble;             _campos[9][2]  = _colSetup;     _campos[9][3]  = _linSetup+140; _campos[9][4]  = 0; _campos[9][5] = 59;
  _label[10] = "Balance"; _campos[10][1] = settings.balance;            _campos[10][2] = _colSetup;     _campos[10][3] = _linSetup+160; _campos[10][4] = 0; _campos[20][5] = 59;

  limpaArea();

  monitor.setTextSize(2);
  monitor.setTextColor(WHITE, BLUE);
  imprimeTexto("Setup","E",5);
  monitor.setTextColor(WHITE, BLACK);

  for (byte i=0; i<_qtdeCampos; i++) { // Mostra Campos
    monitor.setCursor(0,_campos[i][3]); monitor.print(_label[i]);
    monitor.setCursor(_campos[i][2],_campos[i][3]); monitor.print(_campos[i][1],DEC);
  }
  
  if (_dadosAlterados) {                          // Efetua a gravação dos valores caso tenham sido alterados
     saveSettings();
  }
  btnSetupState=0;
  delay(5000);
}
///////////////////
// Executa Setup //
///////////////////
void executaSetupClock()
{
  DateTime _datahora             = relogio.now();
  bool     _dadosAlterados       = false;
  byte     _qtdeCampos           = 5;
  int      _campos[_qtdeCampos][5];
  String   _label[_qtdeCampos];
  
  //          Label                    Campo                               Coluna               Linha               limite minimo      limite maximo
  _label[0] = "DATA";  _campos[0][1] = _datahora.day();    _campos[0][2] = 110; _campos[0][3] = 65; _campos[0][4] = 1; _campos[0][5] = 31;
  _label[1] = "/";     _campos[1][1] = _datahora.month();  _campos[1][2] = 155; _campos[1][3] = 65; _campos[1][4] = 1; _campos[1][5] = 12;
  _label[2] = "/";     _campos[2][1] = _datahora.year();   _campos[2][2] = 200; _campos[2][3] = 65; _campos[2][4] = 0; _campos[2][5] = 2099;
  _label[3] = "HORA";  _campos[3][1] = _datahora.hour();   _campos[3][2] = 110; _campos[3][3] = 85; _campos[3][4] = 0; _campos[3][5] = 23;
  _label[4] = ":";     _campos[4][1] = _datahora.minute(); _campos[4][2] = 155; _campos[4][3] = 85; _campos[4][4] = 0; _campos[4][5] = 59;

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

        if (digitalRead(btnSetupPin)){                                    // Executa o modo Setup
           executaSetup(); 
           return;
        }                         
        
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
  executaSetupConfig();
}
///////////////////
// saveSettings //
///////////////////
void saveSettings() {
    byte EEPROMaddress = SETTINGS_ADDRESS;
    const byte* pointer = (const byte*)(const void*)&settings;
    for (unsigned int i = 0; i < sizeof(settings); i++) EEPROM.write(EEPROMaddress++, *pointer++);
}
///////////////////
// loadSettings //
///////////////////
void loadSettings() {
  
    // Try to load the settings.
    byte EEPROMaddress = SETTINGS_ADDRESS;    
    byte* pointer = (byte*)(void*)&settings;
    for (unsigned int i = 0; i < sizeof(settings); i++) *pointer++ = EEPROM.read(EEPROMaddress++);
    
    // O ID das configurações do OCB não foi encontrado, define-os
    if (strcmp(settings.id, SETTINGS_ID_STRING) != 0) {
      
        // Use the default settings...
        strcpy(settings.id, SETTINGS_ID_STRING);
        settings.source             = 0;
        settings.currentPreset      = 1;
        settings.presetFrequency[0] = 10230;
        settings.presetFrequency[1] = 9430;
        settings.presetFrequency[2] = 8930;
        settings.currentFrequency   = 10230;
        settings.currentVolume      = 0;
        settings.bass               = 0;
        settings.treble             = 0;
        settings.balance            = 0;
        
        // Clear the "Last Track" path.
        EEPROMaddress = SETTINGS_ADDRESS + sizeof(settings);
        EEPROM.write(EEPROMaddress, 0);
    }
}

