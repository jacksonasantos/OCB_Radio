//////////////////////
// Inicializa MP3   //
/////////////////////
bool iniciaMP3()
{
  mySoftwareSerial.begin(9600);                          // Inicializa softwareSerial
  if (!myDFPlayer.begin(mySoftwareSerial)) {             // Usa softwareSerial para comunicacar com mp3.
    return -1;
  }
  else {
    myDFPlayer.reset();                                  // Reset the module
    myDFPlayer.setTimeOut(500);                          // Define o time out (500ms) da comunicação serial
    myDFPlayer.volume(FIX_VOLUME);                       // Define o volume inicial
    myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);                      // Define a equalização do som
    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);         // Configura o tipo de device usar - SD as default
    myDFPlayer.outputSetting(true, 15);                  // output setting, enable the output and set the gain to 15
    return 1;
  }
}

///////////////////
// Executa MP3   //
///////////////////
void executaMp3()
{

#ifdef DEBUG
  Serial.print("Inicializando MP3 Player... ");
#endif  
  if (!iniciaMP3()){
#ifdef DEBUG
     Serial.println("Falha");
#endif  
     return;
  }
  else {
#ifdef DEBUG
      Serial.println("OK!");
#endif  
  }
  
  int                       lastPosVol         = -1;
  
  #define                   ROTARYSTEPS        1
  #define                   ROTARYMIN          0
  #define                   ROTARYMAX          30

  encoderVol.setPosition(myDFPlayer.readVolume() / ROTARYSTEPS);

/*  
  //----Mp3 control----
  myDFPlayer.sleep();     //sleep
  myDFPlayer.enableDAC();  //Enable On-chip DAC
  myDFPlayer.disableDAC();  //Disable On-chip DAC
  myDFPlayer.outputSetting(true, 15); //output setting, enable the output and set the gain to 15
  myDFPlayer.volumeUp(); //Volume Up
  myDFPlayer.volumeDown(); //Volume Down  
  
  //----Mp3 play----
  myDFPlayer.next();  //Play next mp3
  myDFPlayer.previous();  //Play previous mp3
  myDFPlayer.play(1);  //Play the first mp3
  myDFPlayer.loop(1);  //Loop the first mp3
  myDFPlayer.pause();  //pause the mp3
  myDFPlayer.start();  //start the mp3 from the pause
  myDFPlayer.playFolder(15, 4);  //play specific mp3 in SD:/15/004.mp3; Folder Name(1~99); File Name(1~255)
  myDFPlayer.enableLoopAll(); //loop all mp3 files.
  myDFPlayer.disableLoopAll(); //stop loop all mp3 files.
  myDFPlayer.playMp3Folder(4); //play specific mp3 in SD:/MP3/0004.mp3; File Name(0~65535)
  myDFPlayer.advertise(3); //advertise specific mp3 in SD:/ADVERT/0003.mp3; File Name(0~65535)
  myDFPlayer.stopAdvertise(); //stop advertise
  myDFPlayer.playLargeFolder(2, 999); //play specific mp3 in SD:/02/004.mp3; Folder Name(1~10); File Name(1~1000)
  myDFPlayer.loopFolder(5); //loop all mp3 files in folder SD:/05.
  myDFPlayer.randomAll(); //Random play all the mp3.
  myDFPlayer.enableLoop(); //enable loop.
  myDFPlayer.disableLoop(); //disable loop.

    static unsigned long timer = millis();
  
    monitor.setCursor(50,40);
    monitor.println(myDFPlayer.readCurrentFileNumber());
    
    if (myDFPlayer.readType() == DFPlayerPlayFinished) {
      myDFPlayer.next();  //Play next mp3 every 3 second.
    }
    if (millis() - timer > 30000) {
      timer = millis();
      Serial.println("Tocando ... ");
      Serial.print("No. Arq Corrente "); Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
      myDFPlayer.next();  //Play next mp3 every 3 second.
    }
*/
  
#ifdef DEBUG  
  Serial.print("Estado do MP3      "); Serial.println(myDFPlayer.readState()); //read mp3 state
  Serial.print("Volume             "); Serial.println(myDFPlayer.readVolume()); //read current volume
  Serial.print("Equalizador        "); Serial.println(myDFPlayer.readEQ()); //read EQ setting
  Serial.print("Qtde Arquivos      "); Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
  Serial.print("No. Arq Corrente   "); Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
  Serial.print("Qtde Arq Pasta MP3 "); Serial.println(myDFPlayer.readFileCountsInFolder(0)); //read fill counts in folder SD:/03
#endif

  monitor.setTextColor(WHITE,BLACK);  
  monitor.setTextSize(2);
  
  while (!digitalRead(btnModoPin))
  {
    
    encoderVol.tick();                                             // Verifica o encoder do Volume
    int newPosVol = encoderVol.getPosition() * ROTARYSTEPS;        // captura a posição fisica atual e calcula a posição lógica
    if (newPosVol < ROTARYMIN) {
      encoderVol.setPosition(ROTARYMIN / ROTARYSTEPS);
      newPosVol = ROTARYMIN;
    } else if (newPosVol > ROTARYMAX) {
      encoderVol.setPosition(ROTARYMAX / ROTARYSTEPS);
      newPosVol = ROTARYMAX;
    } 
    if (lastPosVol != newPosVol) {
#ifdef DEBUG
      Serial.print("Rotary Volume : ");
      Serial.println(newPosVol);
#endif    
      lastPosVol = newPosVol;
      myDFPlayer.volume(lastPosVol);
      mostraVolumeMP3();    
    } 
  
    mostraEQMP3();                                                 // Mostra a equalização do som
    
    
    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read());       // Imprime a mensagem detalhada do DFPlayer para verificar com diferentes erros e/ou estados.
    }
    
  }
  myDFPlayer.stop();     //Reset the module
}
////////////////////////////////////////////////////////////////
void mostraEQMP3()
{
  String                    DESC_EQ[]         = {"Normal ", "Pop    ", "Rock   ", "Jazz   ", "Classic", "Bass   "};

#ifdef DEBUG
  Serial.print(" Equalizacao  : ");Serial.println(myDFPlayer.readEQ());
#endif

  monitor.setTextColor(WHITE,BLACK);  
  monitor.setTextSize(1);

  imprimeTexto(DESC_EQ[myDFPlayer.readEQ()],"D",60);
}
////////////////////////////////////////////////////////////////
void mostraVolumeMP3()
{
  int16_t vlinMax = monitor.height();
#ifdef DEBUG
  Serial.print(" Volume  : ");Serial.println(myDFPlayer.readVolume());
#endif
  mostraTermometro("Vol", myDFPlayer.readVolume(), 30, 28, 10, vlinMax-43, 20 );
}
//////////////////////////////////
// Apresenta detalhes do Player //
//////////////////////////////////
void printDetail(uint8_t type, int value){
  monitor.setTextColor(WHITE,BLACK);  
  monitor.setTextSize(2);
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      imprimeTexto("Tempo Esgotado!","C",80);
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      imprimeTexto("Erro de Empilhamento!","C",80);
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      imprimeTexto("Cartão Inserido!","C",80);
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      imprimeTexto("Cartão Removido!","C",80);
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      imprimeTexto("Cartão OnLine!","C",80);
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayer Erro: "));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          imprimeTexto("DFPlayer Erro: Cartão não encontrado...","C",80);
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          imprimeTexto("DFPlayer Erro: Dormindo...","C",80);
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          imprimeTexto("DFPlayer Erro: Get Wrong Stack...","C",80);
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          imprimeTexto("DFPlayer Erro: Check Sum Not Match...","C",80);
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          imprimeTexto("DFPlayer Erro: Indice do Arquivo não encontrado...","C",80);
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          imprimeTexto("DFPlayer Erro: Arquivo não encontrado...","C",80);
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          imprimeTexto("DFPlayer Erro: Em Anuncio...","C",80);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
