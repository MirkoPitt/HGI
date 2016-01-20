int Address = 3;
int Clock = 6;
int Direction = 9;
int PositionPin = A0;
int PositionValue = 0;
int LowLimit = 5;
int HighLimit = 750;
int ClockDelay = 1; //sembra buono 1.6
//int ClockDelay = 0.6; //sembra buono 1.6


void setup(){
  //Set the Address, Clock and Direction pin as output
  pinMode(Address, OUTPUT); //il pin 9 è una porta analogica che la posso settare sia come in che out
  pinMode(Clock, OUTPUT);
  pinMode(Direction, OUTPUT);
  //Initialise the serial port to print some data to the console
  Serial.begin(9600);
  //Set the Voltage reference for the ADC to the internal one -> BandGap del silicio (trick consigliato da Ave)
  analogReference(DEFAULT); 
  //Define an initial direction
  digitalWrite(Direction, LOW);
}

void loop() {
  // Enable step motor
  //digitalWrite(Address, LOW);
  //Read the position
  PositionValue = analogRead(PositionPin);
  Serial.print("Posizione letta dall'arduino: ");
  Serial.println(PositionValue);
  
  //Cambio della direzione de vado troppo fine corsa
  if ( (PositionValue < LowLimit) && (digitalRead(Direction) == 1 )) digitalWrite(Direction, LOW);
  if ( (PositionValue > HighLimit) && (digitalRead(Direction) == 0 )) digitalWrite(Direction, HIGH); 
  
  //Dagli un pacchetto di impulsi compatibilmente con la posizione
  for (int i=0; i <= 2000; i++){
      digitalWrite(Clock, HIGH);
      delay(ClockDelay);
      digitalWrite(Clock, LOW);
      delay(ClockDelay);
      } 
      
  
  // Disable step motor:
  //digitalWrite(Address, HIGH);  
  
}
