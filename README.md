# 🚀 Space Shooter C++ Collection

Welcome to the **Space Shooter Collection**! This repository features two distinct implementations of a classic space shooter game: a fully featured **Graphical Version** powered by SFML and a retro-style **Terminal Version** built for the Windows console.

---

## 🌌 1. Graphical Space War (SFML)

This version delivers a modern arcade experience with sprite-based graphics, sound effects, and smooth physics.

### 📂 Game Structure & Dependencies
To make the game portable and easy to run, the folder structure is critical:
* **`lib/` Folder:** This directory contains the necessary **DLL files** required by SFML. The executable checks this folder to load the game libraries dynamically.
* **`Play.bat`:** A batch file created to simplify the launch process.

### 🎮 How to Run

#### Option A: Quick Play (No Setup Required)
If you do not have C++, a compiler, or SFML installed on your system, you can still play the game easily:
1.  Navigate to the game folder.
2.  Double-click the **`Play.bat`** file.
    * *This automatically launches the game with all required dependencies linked correctly.*

#### Option B: Compile from Source
For developers who want to modify or rebuild the game, use the following compilation command (ensure SFML 3.0.2 paths are correct for your system):

```bash
g++ graphic_main.cpp resource.o -I C:\SFML-3.0.2\include -L C:\SFML-3.0.2\lib -o graphic_main.exe -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system -mwindows
```
### ✨ Features
* **Weapon System:** Upgrade your firepower by collecting Power-Ups (levels 1-5).
* **Enemy Variety:** Fight against 4 different types of enemy ships with unique movement and attack patterns.
* **Audio Engine:** Immersive sound effects for explosions, shooting, and UI interactions.

---

### 🛠️ Project Structure Overview
```
Root Directory/
├── lib/                  # Critical DLL files for SFML
├── Play.bat              # Launcher for easy access
├── graphic_main.cpp      # Source code for Graphical Version
├── [Terminal_Code].cpp   # Source code for Terminal Version
├── font.ttf              # Game fonts
├── *.png                 # Sprite assets (Enemies, Player, Bullets)
└── *.wav / *.mp3         # Audio assets
```
---
## 💻 2. Terminal Space Shooter
A nostalgic, ASCII-art based shooter that runs purely in the command line window.

### ⚠️ System Requirements: Windows Only
This version is built using the `<windows.h>` library to handle console cursor positioning, colors, and real-time keyboard input.

      Note: This code is strictly for Windows. It will not compile or run on Linux or macOS environments.

#### 🕹️ Controls
```
Key	Action
A	Move Ship Left
D	Move Ship Right
SPACE	Fire Weapon
X	Exit Game
```

### 📦 Unique Mechanics
* **Question Boxes (Q B):** Random event triggers! Hitting these can either:

    * ❤️ Restore Health

    * 💔 Damage you

    * 🔫 Upgrade your Weapon

    * ⭐ Give Bonus Points

* Rendering: Uses custom frame timing to ensure smooth ASCII animations (targeting ~30 FPS).

Enjoy the game!


<img width="597" height="386" alt="game1" src="https://github.com/user-attachments/assets/ea7511e0-86e1-4705-8151-d8706d18037c" />



<img width="598" height="399" alt="image" src="https://github.com/user-attachments/assets/0fb27edd-1bcf-4576-accd-8c5bd120ce5c" />
