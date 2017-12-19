///////////////////////
// Introdução Radio  //
///////////////////////
void iniciaTFT() 
{
#ifdef DEBUG
  Serial.print("Inicializando monitor... ");
#endif  
  monitor.reset();                                		// Reinicia o tft
  monitor.begin(vg_identifier);                   		// Inicializa tft 
  monitor.setRotation(3);                         		// Ajusta o tft para ficar alinhado na horizontal
#ifdef DEBUG
  Serial.println("OK!");
#endif  
}

///////////////////////
// Introducao Radio  //
///////////////////////
void telaIntroducao() 
{
#ifdef DEBUG
  Serial.print("Apresentando Tela Introducao... ");
#endif  
//  bmpDraw("logo.bmp", 0, 0);
  
  monitor.setCursor(0,5);
  monitor.setTextColor(BLUE); 
  monitor.setTextSize(2);
//  monitor.println( "  OnBoard Computer Beetle");  

  delay(5000);
#ifdef DEBUG
  Serial.println("OK!");
#endif    
}  

///////////////////////
// Prepara o tft     //
///////////////////////
void preparaTFT() 
{  
#ifdef DEBUG
  Serial.print("Preparando monitor... ");
#endif  

  // Preenche monitor com Preto
  monitor.fillScreen(BLACK);                      

  // Desenha cabecalho
  monitor.fillRect(0,  0, monitor.width()-1, 25, BLUE);   
  monitor.fillRect(0, 25, monitor.width()-1,  1, DARKGREY);   
  monitor.fillRect(0, 26, monitor.width()-1,  1, WHITE);   
  monitor.fillRect(0, 27, monitor.width()-1,  1, DARKGREY);   
  monitor.fillRect(0, 28, monitor.width()-1,  5, BLUE);   

  // Desenha Rodape
  monitor.fillRect(0, monitor.height()-25, monitor.width()-1, 25, BLUE);
  
#ifdef DEBUG
  Serial.println("OK!");
#endif    
} 

///////////////////
// Imprime Texto //
///////////////////
void imprimeTexto(String pTexto, String pAlinhamento, int pLinha)
// pAlinhamento = "E"squerda (DEFAULT) - "C"entro - "D"ireita
{
  int _vcol     = 5;
  int _tamFonte = 8;
  
  switch(monitor.getTextSize())
  {
    case 2:
     _tamFonte = 12;
     break;
    case 3:
     _tamFonte = 16;
     break;
    case 4:
     _tamFonte = 20;
     break;
  }   
  if (pAlinhamento=="C")
     _vcol = ((monitor.width()-(pTexto.length()*_tamFonte))/2);
  else if (pAlinhamento=="D")
     _vcol = (monitor.width()-pTexto.length()*_tamFonte);
  else if (pAlinhamento=="E")
     _vcol = 5;

  monitor.setCursor(_vcol, pLinha);
  monitor.println( pTexto );
}
////////////////////////////////////////////////////////////////
void limpaArea()
{
  // Limpa a area util
  monitor.fillRect(0, 33,  monitor.width(), monitor.height()-58, BLACK);  
}

/////////////////////////////////////////////////////////////
// Termometro Generico                                     //
// p_titulo      - Titulo do Termometro (3 ou 4 caracteres // 
// p_valor       - Valor de medicao representado em branco //
// p_tamanho     - Tamanho termometro repres. cinza claro  //
// p_referencia  - Vlr limite para pintar de vermelho      //
// p_col_baixo   - coluna de baixo para inicio termometro  //
// p_lin_baixo   - linha de baixo do termometro            //
/////////////////////////////////////////////////////////////
void mostraTermometro(String p_titulo, int16_t p_valor, int16_t p_tamanho, int16_t p_referencia, int16_t p_col_baixo, int16_t p_lin_baixo, int16_t p_largura )
{
  float   _lin   = p_lin_baixo;
  int16_t _cor   = WHITE;
  float   _passo = 15;

  monitor.setTextSize(2);
  int16_t _linTit = p_col_baixo - (((p_titulo.length()*12) - p_largura-6) / 2);
  monitor.setCursor(_linTit, _lin);
  monitor.println(p_titulo);
 
  _lin   = _lin - 15;
  _passo = 15 / p_tamanho;
  for (byte i = 0; i < 15; i++)   
  {
    if (i < p_valor) 
      if ( i >= p_referencia )
         _cor = RED;
      else
         _cor = WHITE;
    else
       _cor = LIGHTGREY;
    monitor.fillRect( p_col_baixo, _lin-_passo, p_largura, 10-_passo, _cor);
    monitor.drawFastHLine(p_col_baixo, _lin-_passo, p_largura+5, WHITE);
    _lin-=(10-_passo);
  }
}

/////////////////////
// Mostra Relogio  //
/////////////////////
void mostra_relogio()
{
  DateTime _datahora = relogio.now();
  
  int vcol = monitor.width()-((5*12));
  int vlin = 5;
  char _daysOfTheWeek[7][12] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"};

#ifdef DEBUG
  Serial.print( "RELOGIO   : " );  
  if ( _datahora.day()<10)
     Serial.print("0");  
  Serial.print( _datahora.day());Serial.print( "/");
  if ( _datahora.month()<10)
     Serial.print("0");
  Serial.print( _datahora.month());Serial.print( "/");
  Serial.print( _datahora.year());Serial.print( " ");
  Serial.print( _daysOfTheWeek[_datahora.dayOfTheWeek()]);Serial.print( " ");
  if ( _datahora.hour()<10)
     Serial.print("0");
  Serial.print( _datahora.hour());Serial.print( ":");
  if ( _datahora.minute()<10)
     Serial.print("0");
  Serial.println( _datahora.minute());
#endif

  // Mostra a data 
  monitor.setFont();
  monitor.setTextSize(2);
  monitor.setTextColor(WHITE,BLUE);  
  monitor.setCursor(vcol, vlin);

  if ( _datahora.hour()<10)
     monitor.print("0");
  monitor.print( _datahora.hour(),DEC);
  monitor.print( ":");
  if ( _datahora.minute()<10)
     monitor.print("0");
  monitor.print( _datahora.minute(),DEC);

  monitor.setTextColor(WHITE);
}


/*
#define BUFFPIXEL 20

void bmpDraw(char *filename, int x, int y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  boolean  first = true;

  if((x >= monitor.width()) || (y >= monitor.height())) return;

#ifdef DEBUG
  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.println(filename);
#endif  
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
#ifdef DEBUG
    Serial.println(F("File not found"));
#endif  
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.println(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= monitor.width())  w = monitor.width()  - x;
        if((y+h-1) >= monitor.height()) h = monitor.height() - y;

        // Set TFT address window to clipped image bounds
        monitor.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if(lcdidx > 0) {
                monitor.pushColors(lcdbuffer, lcdidx, first);
                lcdidx = 0;
                first  = false;
              }
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx++] = monitor.color565(r,g,b);
          } // end pixel
        } // end scanline
        // Write any remaining data to LCD
        if(lcdidx > 0) {
          monitor.pushColors(lcdbuffer, lcdidx, first);
        } 
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
*/
