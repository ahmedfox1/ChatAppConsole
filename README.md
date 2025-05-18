# ChatAppConsole

A simple **WhatsApp-style chat simulator** built with **C++** and designed for the console. This project demonstrates core Object-Oriented Programming (OOP) concepts like classes, inheritance, data encapsulation, and dynamic memory usage.

## 🚀 Features

- 👤 User Registration & Login  
- 📇 Manage Contacts (Add / Remove)  
- 💬 One-to-One and Group Chat Rooms  
- 📚 Message History per Chat Room  
- 📸 Story System (Create & View Stories)  
- 🧠 Console-based Menus with Clean Navigation

## 🧱 Structure

| File / Folder       | Description                             |
|---------------------|-----------------------------------------|
| `Main.cpp`          | Entry point with main menu navigation   |
| `ChatApplication.*` | Core application logic & user handling  |
| `ChatRoom.*`        | Chat room management (group or 1-to-1)  |
| `User.*`            | (Assumed) User data and contact logic   |
| `Message.*`         | (Assumed) Chat message structure        |
| `Story.*`           | (Assumed) Story creation & viewing      |

> Note: Actual files like `User`, `Message`, and `Story` might be defined in `ChatApplication.cpp` or split across other headers.

## 🧰 Requirements

- C++17 or above  
- Built using Visual Studio (tested with `.vcxproj`)  
- Any platform with a C++ compiler (Windows, Linux, etc.)

## 🧪 How to Run

1. Clone the repository:
   ```bash
   git clone https://github.com/ahmedfox1/ChatAppConsole.git
Open the solution (.sln) in Visual Studio, or compile via terminal:

bash
Copy
Edit
g++ -std=c++17 -o ChatApp Main.cpp ChatApplication.cpp ChatRoom.cpp
./ChatApp
Follow the instructions in the console menu.

📚 Educational Purpose
This app is not connected to a network (no TCP/IP) and is intended for offline simulation and learning:

Great for practicing OOP in C++

Demonstrates how user interaction can be structured via console interfaces

Simulates real app logic without complex backend

📌 Notes
Data is stored in memory only (no file or database persistence)

No multithreading or networking — all logic is single-user/local

Can be extended to support file-based saving or network chat

👨‍💻 Author

Made with ❤️ by ahmedfox1
