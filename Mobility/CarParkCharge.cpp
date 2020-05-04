#include <Grove_LED_Bar.h>
#include <Ultrasonic.h>
#include <stdlib.h>

Grove_LED_Bar bar1(9, 8, 0);
Grove_LED_Bar bar2(8, 7, 0);
Ultrasonic ultrasonic1(4);
Ultrasonic ultrasonic2(3);

void setup()
{
  Serial.begin(9600);
  bar1.begin();
}

int p1=0,p2=0;
float i1=0,i2=0;
String us1s = "Slot 1 Free";
String us2s = "Slot 2 Free";

void loop()
{ 
  int us1 = ultrasonic1.MeasureInInches(); 
  int us2 = ultrasonic2.MeasureInInches(); 
  p1=0,p2=0;
  char buffer[20];
  bar1.setGreenToRed(false);
  if(us1 < 3) p1=1;
  if(us2 < 3) p2=1;

  if(p1==1)
  {    
      bar1.setLevel(i1);
      if(i1<10.1)
      {
          us1s = itoa(i1*10,buffer,10);
          us1s= us1s + "% charged";
          i1=i1+1;
      }
      if(i1>10)
      {
        us1s = "Car 1 Available";
      }
  }
  if(p2==1)
  {    
      bar2.setLevel(i2);
      if(i2<10.1)
      {
          us2s = itoa(i2*10,buffer,10);
          us2s= us2s + "% charged";
          i2=i2+1;
      }
      if(i2>10)
      {
        us2s = "Car 2 Available";
      }
  }

  if(p1==0)
  {   i1=0;
      bar1.setLevel(i1);
      us1s = "Slot 1 Free";
  }
  if(p2==0)
  {   i2=0;
      bar2.setLevel(i2);
      us2s = "Slot 2 Free";
  }
  delay(1000);
  Serial.println(us1s + " : " + us2s);
}
