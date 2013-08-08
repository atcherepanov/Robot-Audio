int motor_pin1 = 4;
int motor_pin2 = 5;
int motor_pin3 = 6;
int motor_pin4 = 7;

#define FASTADC 1

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Define the mics
int mic_FR = A0;
int mic_BR = A1;
int mic_BL = A2;
int mic_FL = A3;

//Set the size of the arrays
const int reads = 150;
//The arrays for the data collection
int FRarray[reads]; 
int FLarray[reads]; 
int BRarray[reads]; 

//Back left mic gives incorrect readings
// int BLarray[reads]; 

// set the averages of the collected sound
double avgFR = 0;
double avgFL = 0;
double avgBR  = 0;
int i;

// setup the pins
void setup() {
	pinMode(motor_pin1,OUTPUT);
	pinMode(motor_pin2,OUTPUT);
	pinMode(motor_pin3,OUTPUT);
	pinMode(motor_pin4,OUTPUT);
	pinMode(mic_FR,INPUT);
	pinMode(mic_BR,INPUT);
	pinMode(mic_BL,INPUT);
	pinMode(mic_FL,INPUT);
	Serial.begin(9600) ;
}
// The function to get the data and process it and act on it
int quickread()
{
	for(;;)
	{
		//int avgBL = 0;

		//reset the sums and avg
		double sumFR = 0;
		double sumFL = 0;
		double sumBR  = 0;
		avgFR = 0;
		avgFL = 0;
		avgBR  = 0;


		//code to make the pins read faster
		#if FASTADC
		// set prescale to 16
		sbi(ADCSRA,ADPS2) ;
		cbi(ADCSRA,ADPS1) ;
		cbi(ADCSRA,ADPS0) ;
		#endif

		//preform the reads FR->FL->BR->FR with a frquency of 77KHz

		for (i = 0 ; i < reads ; i++)
		{
			FRarray[i] = analogRead(mic_FR ) ;
			FLarray[i] = analogRead(mic_FL) ;
			BRarray[i] = analogRead(mic_BR ) ;
			//     BLarray[i] = analogRead(mic_BL) ;
		}

		//print out method
		/*
        for (i = 0 ; i < reads ; i++)
  {
    Serial.print(FRarray[i]);
    Serial.print("         ");
    Serial.print(FLarray[i]);
    Serial.print("         ");
    Serial.print(BRarray[i]);
    Serial.println();
  }
		 */


		// collect the sum of the array
		for (i = 0 ; i < reads ; i++)
		{
			avgFR = avgFR + FRarray[i];
			avgFL = avgFL + FLarray[i];
			avgBR = avgBR + BRarray[i];
			//     avgBL = avgBL + BLarray[i];
		}

		//divide to get the average
		avgFR = avgFR/reads;
		avgFL = avgFL/reads;
		avgBR = avgBR/reads;

		// more printout
		/*
       Serial.print(avgFR);
    Serial.print("         ");
    Serial.print(avgFL);
    Serial.print("         ");
    Serial.print(avgBR);
    Serial.println();
		 */

		//get the absolute differnece from the average
		for (i = 0 ; i < reads ; i++)
		{
			FRarray[i] = abs(FRarray[i] - avgFR);
			FLarray[i] = abs(FLarray[i] - avgFL);
			BRarray[i] = abs(BRarray[i] - avgBR);

		}
		// square the results
		for (i = 0 ; i < reads ; i++)
		{
			FRarray[i] = FRarray[i]*FRarray[i];
			FLarray[i] = FLarray[i]*FLarray[i];
			BRarray[i] = BRarray[i]*BRarray[i];
		}
		//add up the sum of the transform array
		for (i = 0 ; i < reads ; i++)
		{
			sumFR = sumFR + FRarray[i];
			sumFL = sumFL + FLarray[i];
			sumBR = sumBR + BRarray[i];
		}
		//divide by reads
		sumFR = sumFR / reads;
		sumFL = sumFL / reads;
		sumBR = sumBR / reads;

		//square the result to get the variance
		sumFR = sqrt(sumFR);
		sumFL = sqrt(sumFL);
		sumBR = sqrt(sumBR);
		/*
      Serial.print(sumFR);
    Serial.print("         ");
    Serial.print(sumFL);
    Serial.print("         ");
    Serial.print(sumBR);
    Serial.println();
		 */

		// if else statments controling the direction of travel based on the sumFL,sumFR,sumBR

		if(sumFR > 20 || sumFL > 20 || sumBR > 20)
		{

			Serial.print(sumFR);
			Serial.print("         ");
			Serial.print(sumFL);
			Serial.print("         ");
			Serial.print(sumBR);
			Serial.println();

			if(((sumFL+sumFR)/2) - sumFL < 2 && ((sumFL+sumFR)/2) - sumFL > -2 && sumFR > sumBR && sumFL > sumBR)
			{
				Serial.println("Forward");
				forward();
				return 0;
			}
			else if(sumFR > sumFL && sumFR > sumBR)
			{
				Serial.println("Left");
				turnleftby((sumFR-sumFL)/2);
				/*
  if(sumBR > sumFL)
  {
    turnleftby(sumFR-sumBR);
  }
  else
  {
    turnleftby(sumFR-sumFL);
  }
				 */
			}

			else if(sumBR > sumFL && sumBR > sumFR)
			{
				Serial.println("Left"); 
				turnleft();
				turnleft();
				// turnleftby(sumBR/2);
			}

			else if(sumFL > sumFR && sumFL > sumBR)
			{
				Serial.println("Right");
				turnrightby((sumFL-sumFR)/2);
				// turnrightby(sumFL-sumFR);
			}


		}
		else
		{
			return 1;  
		}
	}

	/*
Serial.print((sumFR/sumFL)*17);
Serial.print("         ");
Serial.print((sumFL/sumFR)*17);
Serial.println();
	 */


	/*
if(sumFR > 20 || sumFL > 20 || sumBR > 20)
{
 if(sumFR > sumFL && sumFR > sumBR)
 {
   Serial.println("FR");
 }
   if(sumFL > sumFR && sumFL > sumBR)
 {
   Serial.println("FL");
 }
  if(sumBR > sumFL && sumBR > sumFR)
 {
   Serial.println("BR");
 }
 if(((sumFL+sumFR)/2) - sumFL < 2 && ((sumFL+sumFR)/2) - sumFL > -2)
 {
   Serial.println("Forward");
 }

}
	 */  
}

void loop() {
	delay(1000);
	Serial.println(quickread());

}
void turnleftby (int x)
{
	digitalWrite(motor_pin1,LOW);       //use the combination which works for you
	digitalWrite(motor_pin2,HIGH);    //left motor rotates forward and right motor backward
	digitalWrite(motor_pin3,HIGH);
	digitalWrite(motor_pin4,LOW);
	delay(50*x);       
	halt();
	return; 
}

void turnrightby (int x)
{
	digitalWrite(motor_pin1,HIGH);       //use the combination which works for you
	digitalWrite(motor_pin2,LOW);      //right motor rotates forward and left motor backward
	digitalWrite(motor_pin3,LOW);
	digitalWrite(motor_pin4,HIGH);
	delay(50*x);       
	halt();
	return; 
}

void turnleft () {
	digitalWrite(motor_pin1,LOW);       //use the combination which works for you
	digitalWrite(motor_pin2,HIGH);    //left motor rotates forward and right motor backward
	digitalWrite(motor_pin3,HIGH);
	digitalWrite(motor_pin4,LOW);
	delay(500);                              // wait for the robot to make the turn
	halt();
	return;
}
void forward() {                            // use combination which works for you
	digitalWrite(motor_pin1,HIGH);
	digitalWrite(motor_pin2,LOW);
	digitalWrite(motor_pin3,HIGH);
	digitalWrite(motor_pin4,LOW);
	delay(500);
	halt();
	return;
}


void backward() {
	digitalWrite(motor_pin1,LOW);
	digitalWrite(motor_pin2,HIGH);                      
	digitalWrite(motor_pin3,LOW);
	digitalWrite(motor_pin4,HIGH);
	delay(750);
	halt();
	return;
}
void turnright () {
	digitalWrite(motor_pin1,HIGH);       //use the combination which works for you
	digitalWrite(motor_pin2,LOW);      //right motor rotates forward and left motor backward
	digitalWrite(motor_pin3,LOW);
	digitalWrite(motor_pin4,HIGH);
	delay(500);                     // wait for the robot to make the turn
	halt();
	return;
}

void halt () {
	digitalWrite(motor_pin1,LOW);
	digitalWrite(motor_pin2,LOW);
	digitalWrite(motor_pin3,LOW);
	digitalWrite(motor_pin4,LOW);
	delay(500);                          //wait after stopping
	return;
}
