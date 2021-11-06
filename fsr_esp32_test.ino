int fsrPin = 4;
int ledPin = 2;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  int analogReading = analogRead(fsrPin);

  Serial.print("Force sensor reading = ");
  Serial.print(analogReading); // print the raw analog reading

  if (analogReading < 10)       // from 0 to 9
  {
    Serial.println(" -> no pressure");
    digitalWrite(ledPin, LOW);
  }
  else if (analogReading < 200) // from 10 to 199
  {
    digitalWrite(ledPin, LOW);
    Serial.println(" -> light touch");
  }
  else if (analogReading < 500) // from 200 to 499
  {
    Serial.println(" -> light squeeze");
    digitalWrite(ledPin, HIGH);
  }
  else if (analogReading < 800) // from 500 to 799
  {
    Serial.println(" -> medium squeeze");
    digitalWrite(ledPin, HIGH);
  }
  else // from 800 to 1023
  {
    Serial.println(" -> big squeeze");
    digitalWrite(ledPin, HIGH);
  }
}
