#include <arduinoFFT.h>
#include <Servo.h>


#define SAMPLES 128              // Must be a power of 2
#define SAMPLING_FREQUENCY 1000  // In Hz, must be over twice the max frequency you expect to measure


arduinoFFT FFT = arduinoFFT();
Servo myServo;  // Create a servo object


unsigned int sampling_period_us;
unsigned long microseconds;


double vReal[SAMPLES];
double vImag[SAMPLES];


const float targetFrequency = 83.57;  // Target frequency for E string
const float tolerance = 0.5;          // Tolerance in Hz for tuning


void setup() {
  Serial.begin(31250);
  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
  myServo.attach(9);  // Attach the servo to pin 9
  myServo.write(90);  // Set initial position of servo to middle
}


void loop() {
  /* Sample the sensor */
  int analogValue;
  for (int i = 0; i < SAMPLES; i++) {
    microseconds = micros();  // Get the current time
    analogValue = analogRead(A0);
    if (analogValue > 1000) {
      i--;  // Ignore this sample and redo this iteration
      continue;
    }
    vReal[i] = analogValue;
    vImag[i] = 0;
    while (micros() < (microseconds + sampling_period_us)) {
      // Wait for the next sample period
    }
  }


  /* Perform FFT */
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);


  /* Find peak frequency */
  double peakFrequency = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
  Serial.print("Peak Frequency: ");
  Serial.println(peakFrequency);


  /* Servo motor adjustment */
  if ((peakFrequency <= 90 && peakFrequency >= 70) && analogValue >= 1) {
    if (peakFrequency > targetFrequency + tolerance) {
      myServo.write(60);  // Example value to loosen the string
      Serial.println("Loosening string...");
    } else if (peakFrequency < targetFrequency - tolerance) {
      myServo.write(120);  // Example value to tighten the string
      Serial.println("Tightening string...");
    } else {
      myServo.write(90);  // Target frequency within tolerance
      Serial.println("In tune!");
    }
  } else {
    myServo.write(90);
    Serial.println("Out of bounds values...");
  }




  delay(1000);  // Delay for stability
}


