#include <Adafruit_LiquidCrystal.h>

Adafruit_LiquidCrystal lcd_1(0);

int tonePin = 2;
int toneFreq = 1000;
int ledPin = 13;
int buttonPin = 8;
int debounceDelay = 30;

int dotLength = 100; 
// dotLength = basic unit of speed in milliseconds
// 240 gives 5 words per minute (WPM) speed.
// WPM = 1200/dotLength.
// For other speeds, use dotLength = 1200/(WPM)
//
// Other lengths are computed from dot length
int dotSpace = dotLength;
int dashLength = dotLength*3;
int letterSpace = dotLength*3;
int wordSpace = dotLength*7; 
float wpm = 1200./dotLength;
  
int t1, t2, onTime, gap;
bool newLetter, newWord, letterFound, keyboardText;
int lineLength = 0;
int maxLineLength = 16; // Match LCD width
int lcdRow = 0;
int lcdCol = 0;

char* letters[] = 
{
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", // A-I
  ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // J-R 
  "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." // S-Z
};

char* numbers[] = 
{
  "-----", ".----", "..---", "...--", "....-", //0-4
  ".....", "-....", "--...", "---..", "----." //5-9
};

String dashSeq = "";
char keyLetter, ch;
int i, index;

void setup() 
{
  delay(500);
  pinMode(ledPin, OUTPUT);
  pinMode(tonePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println();
  Serial.println("-------------------------------");
  Serial.println("Morse Code decoder/encoder");
  Serial.print("Speed=");
  Serial.print(wpm);
  Serial.print("wpm, ");
  Serial.print("dot=");
  Serial.print(dotLength);
  Serial.println("ms");   
  
  // Test the LED and tone
  tone(tonePin, toneFreq);
  digitalWrite(ledPin, HIGH);
  delay(2000);
  digitalWrite(ledPin, LOW);
  noTone(tonePin);
  delay(600);

  // Initialize the LCD
  lcd_1.begin(16, 2);
  lcd_1.clear();
  lcd_1.print("Morse Code");
  lcd_1.setCursor(0, 1);
  lcd_1.print("Translator");
  delay(2000);
  lcd_1.clear();
  
  newLetter = false;
  newWord = false;
  keyboardText = false;
  
  Serial.println("Enter text or Key in:");
  Serial.println("-------------------------------");
}

void loop() 
{
  // Check to see if something has been entered on the keyboard
  if (Serial.available() > 0)
  {
    if (keyboardText == false) 
    {
      Serial.println();
      Serial.println("-------------------------------");
      lcd_1.clear();
      lcdRow = 0;
      lcdCol = 0;
      lcd_1.setCursor(lcdCol, lcdRow);
    }
    keyboardText = true;
    ch = Serial.read();
    if (ch >= 'a' && ch <= 'z')
    { ch = ch-32; }
    
    if (ch >= 'A' && ch <= 'Z')
    {
      Serial.print(ch); 
      Serial.print(" ");
      Serial.println(letters[ch-'A']);
      lcd_1.print(ch);
      updateLCDCursor();
      flashSequence(letters[ch-'A']);
      delay(letterSpace);
    }
    if (ch >= '0' && ch <= '9')
    {
      Serial.print(ch);
      Serial.print(" ");
      Serial.println(numbers[ch-'0']);
      lcd_1.print(ch);
      updateLCDCursor();
      flashSequence(numbers[ch-'0']);
      delay(letterSpace);
    }
    if (ch == ' ')
    {
      Serial.println("_");
      lcd_1.print(" ");
      updateLCDCursor();
      delay(wordSpace);    
    } 

    // Print a header after last keyboard text    
    if (Serial.available() <= 0) 
    {
      Serial.println();
      Serial.println("Enter text or Key in:");
      Serial.println("-------------------------------");
      keyboardText = false;
    }
  }
 
  if (digitalRead(buttonPin) == LOW ) // Button is pressed
  {
    newLetter = true; 
    newWord = true;
    t1 = millis(); // Time at button press
    digitalWrite(ledPin, HIGH); // Turn on LED and tone
    tone(tonePin, toneFreq);
    delay(debounceDelay);     
    while (digitalRead(buttonPin) == LOW ) // Wait for button release
    {
      delay(debounceDelay);
    }
    delay(debounceDelay);
       
    t2 = millis();  // Time at button release
    onTime = t2 - t1;  // Length of dot or dash keyed in
    digitalWrite(ledPin, LOW); // Turn off LED and tone
    noTone(tonePin); 
    
    // Check if dot or dash 
    if (onTime <= dotLength * 1.5) // Allow for 50% longer 
    {
      dashSeq = dashSeq + "."; // Build dot/dash sequence
      Serial.print("."); // Print dot to Serial Monitor
    }
    else 
    {
      dashSeq = dashSeq + "-"; // Build dot/dash sequence
      Serial.print("-"); // Print dash to Serial Monitor
    }
  }  // End button press section
  
  // Look for a gap >= letterSpace to signal end letter
  gap = millis() - t2; 
  if (newLetter == true && gap >= letterSpace)  
  { 
    Serial.print(" "); // Separate letters in Serial output
    
    // Check through letter sequences to find matching dash sequence
    letterFound = false;
    keyLetter = 63; // char 63 is "?"
    
    // Check letters
    for (i = 0; i <= 25; i++)
    {
      if (dashSeq == letters[i]) 
      {
        keyLetter = i + 65; // Convert to ASCII A-Z
        letterFound = true;   
        break; // Don't keep checking if letter found  
      }
    }
    
    // Check numbers if no letter match
    if (letterFound == false)
    {
      for (i = 0; i <= 9; i++) // Fixed bound to match array size
      {
        if (dashSeq == numbers[i]) 
        {
          keyLetter = i + 48; // Convert to ASCII 0-9
          letterFound = true;   
          break; // Don't keep checking if number found  
        }
      }
    }
    
    // Output the found letter/number
    lcd_1.print(keyLetter);
    updateLCDCursor();
    Serial.print(keyLetter); // Print decoded character
    
    if (letterFound == false) // Buzz for unknown key sequence
    {
      tone(tonePin, 100, 500);
    }
    
    newLetter = false; // Reset
    dashSeq = "";
    lineLength = lineLength + 1;
  }  
  
  // When gap is >= wordSpace, insert space between words
  if (newWord == true && gap >= wordSpace * 1.5)
  { 
    newWord = false; 
    Serial.print(" "); // Space between words
    lcd_1.print(" ");
    updateLCDCursor();
    lineLength = lineLength + 1;
     
    // Flash to indicate new word
    digitalWrite(ledPin, HIGH);
    delay(25);
    digitalWrite(ledPin, LOW);       
  } 

  // Insert linebreaks for Serial output
  if (lineLength >= maxLineLength) 
  {
    Serial.println();
    lineLength = 0;
  }      
}

void updateLCDCursor()
{
  lcdCol++;
  if (lcdCol >= 16) // End of first line
  {
    lcdCol = 0;
    lcdRow++;
    if (lcdRow >= 2) // End of display
    {
      lcdRow = 0;
      lcd_1.clear();
    }
    lcd_1.setCursor(lcdCol, lcdRow);
  }
}

void flashSequence(char* sequence)
{
  int i = 0;
  while (sequence[i] == '.' || sequence[i] == '-')
  {
    flashDotOrDash(sequence[i]);
    i++;
  }
}

void flashDotOrDash(char dotOrDash)
{
  digitalWrite(ledPin, HIGH);
  tone(tonePin, toneFreq);
  if (dotOrDash == '.')
  { 
    delay(dotLength); 
  }
  else
  { 
    delay(dashLength); 
  }

  digitalWrite(ledPin, LOW);
  noTone(tonePin);
  delay(dotLength); 
}
//--- end of sketch ---
