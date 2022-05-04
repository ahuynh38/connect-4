int latch0 = 22; int latch1 = 26; int latch2 = 30; int latch3 = 34; int latch4 = 38;
int clock0 = 23; int clock1 = 27; int clock2 = 31; int clock3 = 35; int clock4 = 39;
int data0 = 25;  int data1 = 29;  int data2 = 33;  int data3 = 37;  int data4 = 41;

int column = 0;
int playerNum = 1;
byte leds = 0;
int row = 0;
bool hasPlayed = false;
bool gameEnd = false;
int numPiecesPlaced = 0;

int latchPin;  // GREEN Latch pin of 74HC595
int clockPin; // BLUE Clock pin of 74HC595
int dataPin;  // YELLOW Data pin of 74HC595

void updateShiftRegister(int column, byte leds) {
  
  if (column == 0) {
    latchPin = latch0;
    dataPin = data0;
    clockPin = clock0;
  } else if (column == 1) {
    latchPin = latch1;
    dataPin = data1;
    clockPin = clock1;
  } else if (column == 2) {
    latchPin = latch2;
    dataPin = data2;
    clockPin = clock2;
  } else if (column == 3) {
    latchPin = latch3;
    dataPin = data3;
    clockPin = clock3;
  } else if (column == 4) {
    latchPin = latch4;
    dataPin = data4;
    clockPin = clock4;
  }
  
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, leds);
  digitalWrite(latchPin, HIGH);
}

int board[4][5];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("start");

  for (int i = 0; i < 4; i++) { //initialize board to all 0's
    for (int j = 0; j < 5; j++) {
      board[i][j] = 0;
      Serial.print(board[i][j]);
      
    }
    Serial.println("");
    
  }
  Serial.println("");
  byte clearAll = 0;
  updateShiftRegister(0, clearAll);
  updateShiftRegister(1, clearAll);
  updateShiftRegister(2, clearAll);
  updateShiftRegister(3, clearAll);
  updateShiftRegister(4, clearAll);

}

void loop() {
  for (int i = 0; i < 4; i++) { //initialize board to all 0's
    for (int j = 0; j < 5; j++) {
      board[i][j] = 0;
    }
  }
  
  playerNum = 1; //1 = red, 2 = blue
  gameEnd = false;
  row = 0;
  column = 0;
  numPiecesPlaced = 0;
  play();
  
  delay(1500);
}

void play() {
  while (!gameEnd) {
    while (!hasPlayed) {
      int C0 = analogRead(A0);
      int C1 = analogRead(A1);
      int C2 = analogRead(A2);
      int C3 = analogRead(A3);
      int C4 = analogRead(A4);
      
      //wait for piece to be inserted
      while((analogRead(A0) - C0 <= 10) || (analogRead(A1) - C1 <= 10) || (analogRead(A2) - C2 <= 10)
        || (analogRead(A3) - C3 <= 10) || (analogRead(A4) - C4 <= 10)) {
    
          
        if ((analogRead(A0) - C0 >= 10) || (analogRead(A1) - C1 >= 10) || (analogRead(A2) - C2 >= 10)
        || (analogRead(A3) - C3 >= 10) || (analogRead(A4) - C4 >= 10)) {
          Serial.println("Move detected. Current state of photoresistors:");
          Serial.print(C0); Serial.print(" "); Serial.print(C1); Serial.print(" "); 
          Serial.print(C2); Serial.print(" "); Serial.print(C3); Serial.print(" ");
          Serial.print(C4); Serial.println(" ");
          break;
        }
        C0 = analogRead(A0);
        C1 = analogRead(A1);
        C2 = analogRead(A2);
        C3 = analogRead(A3);
        C4 = analogRead(A4);
        Serial.print(C0); Serial.print(" "); Serial.print(C1); Serial.print(" "); 
        Serial.print(C2); Serial.print(" "); Serial.print(C3); Serial.print(" ");
        Serial.print(C4); Serial.println(" ");
        delay(500);
      }
    
      Serial.println("!!");
      //Find the column that the player has selected
      if (analogRead(A0) - C0 >= 10) { //light has decreased aka player has placed a piece
        while (analogRead(A0) - C0 >= 10) { //wait until piece is no longer blocking the light
          delay(100);
        }
        column = 0;
      } else if (analogRead(A1) - C1 >= 10) {
        while (analogRead(A1) - C1 >= 10) {
          delay(100);
        }
        column = 1;
      } else if (analogRead(A2) - C2 >= 10) {
        while (analogRead(A2) - C2 >= 10) {
          delay(100);
        }
        column = 2;
      } else if (analogRead(A3) - C3 >= 10) {
        while (analogRead(A3) - C3 >= 10) {
          delay(100);
        }
        column = 3;
      } else if (analogRead(A4) - C4 >= 10) {
        while (analogRead(A4) - C4 >= 10) {
          delay(100);
        }
        column = 4;
      }
    
      Serial.print("Column ");Serial.println(column);
      if (fullColumn()) { //verify valid column
        Serial.println("Choose another column");
        continue;
      }
      addToBoard(); 
      hasPlayed = true;
    }
    hasPlayed = false;
//    printBoard();
    
    if (checkWin()) {
      Serial.print("Player "); Serial.print(playerNum); Serial.println(" wins");
      win();
      gameEnd = true;
      break;
    }
  
    numPiecesPlaced += 1;
    if (numPiecesPlaced == 20 && !gameEnd) {
      Serial.println("Tie");
      draw();
      gameEnd = true;
      break;
    }

    if (playerNum == 1) {
      playerNum = 2;
    } else {
      playerNum = 1;
    }
  }
  gameEnd = true;
}

//checks if a column selected already has too many pieces 
bool fullColumn() {
  if (board[0][column] != 0) { 
    return true;
  }
  return false;
}
void dropPiece() {
  if (playerNum == 1) {
    for (int i = 0; i < row; i++) {
      bitSet(leds, i);
      updateShiftRegister(column, leds);
      delay(150);
      bitClear(leds, i);
      updateShiftRegister(column, leds);
      delay(50);
    }
    bitSet(leds, row);
    updateShiftRegister(column, leds);
    delay(150);
    
  } else if (playerNum == 2) {
    for (int i = 4; i < 4 + row; i++) {
      bitSet(leds, i);
      updateShiftRegister(column, leds);
      delay(150);
      bitClear(leds, i);
      updateShiftRegister(column, leds);
      delay(50);
    }
    bitSet(leds, 4 + row);
    updateShiftRegister(column, leds);
    delay(150);
    
  }
}
void draw() {
  for (int j = 0; j <= 3; j++) {
    leds = 0;
    for (int i = 0; i < 8; i++) {
      bitSet(leds, i);
    }

    
    for (int i = 0; i < 5; i++) {
      updateShiftRegister(i, leds);
    }
  
    delay(300);
    for (int i = 0; i < 5; i++) {
      byte clearAll = 0;
      updateShiftRegister(i, clearAll);
    }     
    delay(300);
  }
}
void win() {
  byte clearAll = 0;
  for (int j = 0; j <= 3; j++) {
    leds = 0;
    if (playerNum == 1) {
      for (int i = 0; i < 4; i++) {
        bitSet(leds, i);
      }
    } else if (playerNum == 2) {
      for (int i = 4; i < 8; i++) {
        bitSet(leds, i);
      }
    }

    
    for (int i = 0; i < 5; i++) {
      updateShiftRegister(i, leds);
    }
  
    delay(300);
    for (int i = 0; i < 5; i++) {
      updateShiftRegister(i, clearAll);
    }     
    delay(300);
  }
  Serial.println("GAME END");
}
//add piece to board
void addToBoard() { 
  for(int i = 3; i >= 0; i--) { //check from bottom to top
    if (board[i][column] == 0) {

        leds = 0;

        //make sure leds pattern does not override existing board
        for (int i = 0; i < 4; i++) { 
          if (board[i][column] == 1) {
            bitSet(leds, i);
          }
          if (board[i][column] == 2) {
            bitSet(leds, 4 + i);
          }
          updateShiftRegister(column, leds);
        }
        
        Serial.println(leds);

      
      board[i][column] = playerNum;
      row = i;
      dropPiece();
      
      break;
    }
  }
}
//checks win condition once a piece is dropped
bool checkWin() {
  int count = 0;

  //check win by row   
  for (int i = 0; i < 5; i++) {
    if (board[row][i] != playerNum) {
      if (count > 0) {
        count -= 1;
      }
    } else if (board[row][i] == playerNum) {
      count += 1;
      if (count >= 4) {
        return true;
      }
    }
  }
  

  //check win by column
  count = 0;
  for (int i = 0; i < 4; i++) {
    if (board[i][column] == playerNum) {
      count += 1;
      if (count >= 4) {
        return true;
      }
    }
  }

  //check win by \ diagonal
  for (int k = 0; k < 2; k++) {
    count = 0;
    int j = k;
    for (int i = 0; i < 4; i++) {
      if (board[i][j] == playerNum) {
        count += 1;
        if (count >=4) {
          return true;
        }
      }
      j += 1;
    }
  }
  
  //check win by / diagonal
  for (int k = 0; k < 2; k++) {
    count = 0;
    int j = k;
    for (int i = 3; i >= 0; i--) {
      if (board[i][j] == playerNum) {
        count += 1;
        if (count >=4) {
          return true;
        }
      }
      j += 1;
    }
  }
  return false;
}
