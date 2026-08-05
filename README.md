# 🎯 Inverted Pendulum Stabilization using Arduino (Swing-Up + PID Control)

A real-time **Inverted Pendulum Control System** implemented on **Arduino** using a **Quadrature Encoder**, **DC Motor**, and **L298N Motor Driver**. The controller combines an **Energy-Based Swing-Up Algorithm** with a **PID Balancing Controller** to swing the pendulum from the hanging position and stabilize it around the upright position.

---

## 📌 Project Overview

The inverted pendulum is a classic nonlinear control systems problem widely used in robotics, automation, and control engineering.

This project consists of two operating modes:

- **Swing-Up Mode** – Pumps energy into the pendulum until it reaches the upright position.
- **PID Balance Mode** – Stabilizes the pendulum around the vertical position using a PID controller.

To improve stability, a **Low-Pass Filter** is applied to the angular velocity calculation, reducing encoder noise and measurement jitter.

---

## ✨ Features

- Energy-based Swing-Up Control
- PID Stabilization near Upright Position
- Automatic Switching Between Swing-Up and PID Modes
- Quadrature Encoder Position Tracking
- Low-Pass Filter for Velocity Smoothing
- Motor Dead-Zone Compensation
- Adjustable PID Gains
- Real-Time Serial Monitor Debugging
- Safety Output Saturation

---

## 🛠 Hardware Required

- Arduino Uno / Nano
- DC Motor
- L298N Motor Driver
- Incremental Quadrature Encoder (400 PPR)
- Inverted Pendulum Mechanism
- External Motor Power Supply
- Connecting Wires

---

## 🔌 Pin Connections

| Component | Arduino Pin |
|------------|-------------|
| Encoder A | D2 |
| Encoder B | D3 |
| Motor PWM (ENB) | D10 |
| Motor Direction IN3 | D8 |
| Motor Direction IN4 | D9 |

---

## ⚙ Control Strategy

### 1. Swing-Up Controller

When the pendulum is far from the upright position, the controller injects energy using

```
Output = -K_swing × AngularVelocity × cos(Angle)
```

This gradually increases the pendulum's energy until it approaches the vertical position.

A kick-start function is included if the pendulum is stationary.

---

### 2. PID Controller

Once the pendulum is within **±25°** of the upright position, the controller switches to PID mode.

```
Output = Kp × Error + Ki × Integral + Kd × AngularVelocity
```

Current gains:

```
Kp = 60
Ki = 0
Kd = 3.5
```

---

### 3. Velocity Smoothing

Raw encoder measurements often introduce noise.

A first-order Low-Pass Filter smooths the angular velocity:

```
FilteredVelocity =
0.7 × PreviousVelocity +
0.3 × CurrentVelocity
```

This significantly reduces jitter and improves balancing performance.

---

## 📊 Controller Workflow

```
Encoder
    │
    ▼
Read Angle
    │
    ▼
Calculate Error
    │
    ▼
Compute Angular Velocity
    │
    ▼
Low Pass Filter
    │
    ▼
Is Pendulum Near Upright?
        │
   ┌────┴────┐
   │         │
   ▼         ▼
Swing-Up    PID
   │         │
   └────┬────┘
        ▼
 Motor Output
        ▼
 DC Motor
```

---

## 📈 Serial Output

The Arduino continuously prints

```
Angle : xx.x
Output : xxx
```

Example

```
Ang:-8.3 | Out:142
Ang:-4.7 | Out:86
Ang:0.8 | Out:-12
```

This helps during controller tuning.

---

## 📂 Project Structure

```
.
├── InvertedPendulum.ino
├── README.md
└── images/
    ├── setup.jpg
    └── demo.gif
```

---

## 🚀 How to Run

1. Connect the hardware.
2. Open the Arduino IDE.
3. Select the correct board and COM port.
4. Upload the sketch.
5. Open the Serial Monitor.
6. Set baud rate to **115200**.
7. Place the pendulum in the hanging position.
8. Observe the swing-up and balancing process.

---


```



---

## 📚 Applications

- Control Systems
- Robotics
- Automation
- Mechatronics
- Educational Laboratories
- Research in Nonlinear Control

---

## 🔧 Future Improvements

- LQR Controller
- State Space Control
- Kalman Filter
- Adaptive PID
- Model Predictive Control (MPC)
- Wireless Parameter Tuning
- MATLAB/Simulink Integration
- ROS Integration

---

## 👨‍💻 Author

**Sachin Kumar**

B.Tech in Electronics and Instrumentation Engineering  
National Institute of Technology Silchar

GitHub: https://github.com/Sachin-Kumar540



---

