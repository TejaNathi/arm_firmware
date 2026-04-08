# 🤖 ARM Firmware (STM32 + Raspberry Pi)

Firmware for a multi-axis robotic arm using STM32, designed for high-performance motion control with support for trajectory execution from a Raspberry Pi.

---

## 🚀 Overview

This project implements:

- 🧠 Real-time motion control on STM32
- 📡 UART communication with Raspberry Pi
- ⚙️ Multi-axis stepper motor control
- 📈 Support for S-curve / spline-based trajectories
- ⚡ Designed for Timer + DMA-based execution (future-ready)

---

## 🏗️ System Architecture

Raspberry Pi
    ↓
(Trajectory planning, IK, spline)
    ↓ UART
STM32 (This Firmware)
    ↓
Timer / DMA / GPIO
    ↓
Stepper Drivers → Robotic Arm

---

## 🧠 Key Concepts

- STM32 handles precise timing
- Raspberry Pi handles:
  - Inverse kinematics
  - Trajectory generation
  - Motion planning
- Firmware executes motion deterministically

---

## 📂 Project Structure

src/
 ├── main.c
 ├── communication.c
 ├── motor.c
 ├── queue.c
include/
 ├── motor.h
 ├── communication.h

---

## ⚙️ Requirements

- STM32 board (STM32F4 series recommended)
- Raspberry Pi
- Stepper motor drivers
- PlatformIO (VS Code)

---

## 🛠️ Installation (PlatformIO)

1. Install VS Code  
2. Install PlatformIO IDE extension  
3. Clone repo:
   git clone https://github.com/TejaNathi/arm_firmwar.git
   cd arm_firmwar
4. Open in VS Code  
5. Build:
   pio run
6. Upload:
   pio run --target upload

---

## 🧪 Features

- UART command parsing
- Multi-motor structure
- Waypoint queue system
- Timer-based stepping
- DMA system (in progress)

---

## 🧠 Future Roadmap

- Timer + DMA step generation
- Double buffering
- Jerk-controlled motion
- Real-time feedback

---

## 📡 Example Command

MOVE J1 90 SA1 4999 TA1 500 J2 45 SA2 4999 TA2 1000 T200

---

## 👨‍💻 Author

Teja Nathi

---

## 📜 License

MIT License
