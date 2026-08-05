// -----------------------------------------------------------------
//       FINAL PENDULUM CODE (WITH JITTER FILTER FIX)
// -----------------------------------------------------------------

#include <Arduino.h>

// --- PIN DEFINITIONS ---
#define encoderPinA 2 
#define encoderPinB 3 

// Motor Driver (Channel B)
#define ENB 10 
#define IN3 8  
#define IN4 9  

// --- ENCODER VARIABLES ---
volatile long pendulumEncoderCount = 0;
const int PPR = 400;     
float CPR = PPR * 4.0;   

// --- TARGETS ---
float uprightAngle = 180.0;
float swingUpThreshold = 25.0; // +/- 25 deg from top -> Switch to PID

// --- PID GAINS (Balancing Logic) ---
float Kp = 60.0;   // Strength to catch the fall
float Ki = 0.0;    // Integral (Start 0)
float Kd = 3.5;    // Damping 

// --- SWING UP GAINS (Energy Pumping) ---
float K_swing = 50.0; // Slightly reduced to prevent violence

// --- STATE VARIABLES ---
float angle = 0;
float lastAngle = 0;
float angularVelocity = 0; 
float error = 0, lastError = 0, integralError = 0;
unsigned long lastTime = 0;
float dt = 0.01; 

// --- FUNCTION PROTOTYPES ---
void updatePendulumEncoder();
void setMotor(float output);

void setup() {
  Serial.begin(115200); // Set Serial Monitor to 115200!

  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(encoderPinA), updatePendulumEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), updatePendulumEncoder, CHANGE);

  lastTime = micros(); 
  Serial.println("System Ready. Hanging Down = 0 degrees.");
}

void loop() {
  // 1. Time Calculation
  unsigned long now = micros();
  dt = (now - lastTime) / 1000000.0; 
  if(dt < 0.005) return; 
  lastTime = now;

  // 2. Angle Calculation
  angle = (pendulumEncoderCount / CPR) * 360.0;
  angle = fmod(angle, 360.0);
  if (angle < 0) angle += 360.0; 

  // 3. Error Calculation (Target is 180)
  error = uprightAngle - angle;
  if (error > 180.0) error -= 360.0;
  else if (error < -180.0) error += 360.0;
  
  // ---------------------------------------------------------
  // 4. VELOCITY CALCULATION (WITH SMOOTHING FIX)
  // ---------------------------------------------------------
  float rawVelocity = (error - lastError) / dt;
  
  // Low Pass Filter: 70% History, 30% New Data.
  // This ignores the single-pixel jitter (-0.2 to -0.5) seen in your screenshot.
  angularVelocity = (0.7 * angularVelocity) + (0.3 * rawVelocity);

  // ---------------------------------------------------------
  // 5. CONTROLLER LOGIC
  // ---------------------------------------------------------
  float output = 0;

  if (abs(error) < swingUpThreshold) {
      // === PID MODE (Near Top) ===
      integralError += error * dt;
      integralError = constrain(integralError, -20, 20); 
      output = (Kp * error) + (Ki * integralError) + (Kd * angularVelocity);
  } else {
      // === SWING UP MODE (Near Bottom) ===
      integralError = 0; 
      
      float angleRad = angle * PI / 180.0;
      
      // Energy Pumping: Move OPPOSITE to swing direction
      output = -1.0 * K_swing * angularVelocity * cos(angleRad);
      
      // Kickstart: Only kick if we are TRULY stuck (Velocity ~ 0)
      // The filter helps here by ensuring velocity is truly 0, not jittering.
      if (abs(error) > 170.0 && abs(angularVelocity) < 0.5) {
         output = 255.0; 
      }
  }

  // Safety Clamp
  output = constrain(output, -255, 255);

  // 6. Motor Output
  setMotor(output);

  // 7. Update State
  lastError = error;
  lastAngle = angle; 

  // 8. Debug (Normalized for easy reading)
  float displayAngle = angle;
  if(displayAngle > 180) displayAngle -= 360; 

  Serial.print("Ang:"); Serial.print(displayAngle, 1);
  Serial.print(" | Out:"); Serial.println(output);
}

// --- HELPER FUNCTIONS ---

void updatePendulumEncoder() {
  int a = digitalRead(encoderPinA);
  int b = digitalRead(encoderPinB);
  
  if (a == b) {
    pendulumEncoderCount++;
  } else {
    pendulumEncoderCount--;
  }
}

void setMotor(float pwmVal) {
  int minPWM = 40; // Motor deadzone
  int speed = abs((int)pwmVal);
  
  if (speed < minPWM && speed > 0) speed = minPWM;
  speed = constrain(speed, 0, 255);

  // ** DIRECTION CONTROL ** // If cart moves LEFT when you tilt RIGHT, swap IN3 HIGH/LOW below.
  if (pwmVal > 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else if (pwmVal < 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    speed = 0;
  }
  
  analogWrite(ENB, speed);
}