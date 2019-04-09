/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   This example is compatible with hardware board version 1c

   This shows the minimal amount of code to setup and measure salinity. 
 */

#include <ECSalinity.h>

EC_Salinity ec;

void setup()
{
  Serial.begin(9600);
  ec.setK(10.0);
}

void loop()
{
  ec.measureSalinity();
  Serial.print("PSU: "); 
  Serial.println(ec.salinityPSU);
  Serial.println("-----");
  delay(1000);
}
