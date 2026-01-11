# 🔐 Personal Password Vault - Pattern & PIN Manager

![Language](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=for-the-badge&logo=cplusplus)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge&logo=windows)
![Security](https://img.shields.io/badge/Security-Encrypted-2ECC71?style=for-the-badge)
![Pattern](https://img.shields.io/badge/Pattern-Lock-9B59B6?style=for-the-badge)
![PIN](https://img.shields.io/badge/PIN-Supported-E67E22?style=for-the-badge)
![Vault](https://img.shields.io/badge/Storage-Binary%20Vault-C0392B?style=for-the-badge)


## 🎯 What Is This?

Welcome to your **Personal Password Vault** – a secure, offline password manager designed specifically for you and your friends! This isn't your typical password manager; it's built to handle the **unique ways** we remember secrets:

- 🔢 **Patterns** (like phone unlock patterns)
- #️⃣ **PINs** (4-6 digit codes)
- 📝 **Text Passwords** (traditional passwords)
- 👥 **Dual Vault System** (separate storage for you and friends)

## ✨ Why You'll Love It

### 🎮 Interactive Pattern Entry
Forget typing! Draw patterns on a 3x3 grid just like your phone. Watch live visual feedback as you connect the dots.

### 👁️ Smart Privacy Controls
- **Tab to toggle** password visibility
- **Auto-clearing clipboard** after 10 seconds
- **Pattern replay** to review your drawn patterns

### 🔒 Military-Grade Security
- **AES-like XOR encryption** with your master key
- **Double-encrypted master key** (`.key` file)
- **Encrypted data storage** (`.dat` file)
- **Three-attempt limit** with lockout protection

### 📁 Dual Vault System
- **My Passwords** (Vault 1) - Your personal secrets
- **Friend's Passwords** (Vault 2) - Shared/trusted access

## 🚀 Getting Started

### Prerequisites
- **Windows OS** (uses Windows.h for console colors)
- **C++ Compiler** (MinGW, Visual Studio, etc.)
- **Basic C++ knowledge** to compile

### Compilation
```bash
# Using g++
g++ password_vault.cpp -o vault.exe -std=c++11

# Using Visual Studio
# Create a new Console Application and add the .cpp file
```

### First Run Setup
1. **Run the program** - `vault.exe`
2. **Create your Master Password** (minimum 4 characters)
3. **Remember it!** - There's NO recovery if you forget
4. Two files will be generated:
   - `master.key` - Your encrypted master key
   - `vault.dat` - Your encrypted password database

## 📊 How It Works

### 🔐 The Encryption Magic
```
Your Password → XOR with Master Key → Encrypted Storage
              ↳ Uses rolling key technique
              ↳ Each character XORed with master key characters
```

### 🗂️ File Structure
```
Project Folder/
├── vault.exe          # Your compiled program
├── master.key         # Encrypted master password
├── vault.dat          # Encrypted password database
└── password_vault.cpp # Source code
```

### 📝 Entry Structure
Each saved entry contains:
- **Title** (e.g., "Bank Account Pattern")
- **Type** (Pattern=1, PIN=2, Text=3)
- **Encrypted Password** (XOR encrypted)
- **Modification Timestamp**
- **Vault Number** (1=Yours, 2=Friend's)

## 🎮 Features Deep Dive

### 🖼️ Pattern System (The Star Feature!)
```
Visual Grid:          Your Pattern:
 (1) (2) (3)          1 → 5 → 9
 (4) (5) (6)          ↓
 (7) (8) (9)          4 → 2 → 3
```
- **Live drawing** with visual feedback
- **Minimum 2 points** requirement
- **Animation replay** to verify patterns
- **Backspace support** for corrections

### 🔍 Smart Search & Sort
- **Case-insensitive search** across vaults
- **Four sorting methods**:
  1. A → Z (Alphabetical)
  2. Z → A (Reverse)
  3. Newest First
  4. Oldest First

### 📋 Clipboard Management
- **Auto-copy** passwords to clipboard
- **10-second timer** then auto-clear
- **Windows API integration** for reliability

### 🎨 Beautiful Console Interface
- **Color-coded sections** for easy navigation
- **Clear headers** and separators
- **Intuitive menus** with counts

## 🛡️ Security Architecture

### 🔑 Master Key Protection
```
Your Password → XOR with "V@ultK3y#2024" → master.key
```
- **Double encryption** for the master key
- **Static salt** adds another layer
- **No plaintext storage** ever

### 🚫 Anti-Brute Force
- **3 attempt limit** per session
- **Immediate exit** on failure
- **No delay clues** (consistent timing)

### 🔄 Real-time Encryption
- **Memory encryption** - passwords encrypted in RAM
- **File encryption** - entire .dat file is encrypted
- **No temporary files** - all operations in memory

## 📖 Usage Guide

### Adding a New Entry
1. Choose "My Passwords" or "Friend's Passwords"
2. Select "New Entry"
3. Enter a descriptive title
4. Choose password type:
   - **Pattern**: Draw on the 3x3 grid
   - **PIN**: Numbers only with confirmation
   - **Text**: Traditional password
5. Confirm your entry

### Viewing & Managing
- **Tab toggle** to show/hide passwords
- **Pattern replay** for visual confirmation
- **Update/Delete** individual entries
- **Copy to clipboard** with auto-clear

### Vault Management
- **Separate counts** for each vault
- **Independent sorting** per vault
- **Targeted search** within each vault

## ⚠️ Important Security Notes

### 🚨 CRITICAL WARNINGS
1. **NO PASSWORD RECOVERY** - If you forget the master password, your data is permanently inaccessible
2. **BACKUP YOUR FILES** - Regularly copy `master.key` and `vault.dat` to a secure location
3. **OFFLINE ONLY** - This is designed as an offline tool; never expose these files online
4. **LOCAL SECURITY** - Secure your computer physically and with login passwords

## 🔧 Technical Details

### Platform Specifics
- **Windows-only** due to `windows.h` and `conio.h`
- **Console application** - no GUI dependencies
- **Lightweight** - single executable after compilation

### Dependencies
- **Standard C++11** libraries only
- **Windows API** for clipboard and colors
- **No external libraries** required

### Code Structure
- **Modular functions** for each feature
- **Color-coded console output**
- **Error handling** for file operations
- **Input validation** throughout

## 🆘 Troubleshooting

### Common Issues
1. **"Error saving data file"** - Check write permissions in the folder
2. **Clipboard not working** - Run as Administrator if needed
3. **Colors not displaying** - Use Windows Command Prompt or PowerShell
4. **Pattern not showing** - Ensure you're using the correct console

### Compilation Errors
- **'GetStdHandle' not found** - Compile on Windows or adjust for Linux/Mac
- **'_getch' undefined** - Use Windows compiler or replace with getchar()
- **Color constants error** - Ensure windows.h is included

## 🚀 Future Enhancements (You Can Add!)

### Potential Upgrades
1. **Export/Import** encrypted backups
2. **Password strength meter** for text passwords
3. **Auto-lock** after inactivity
4. **Multiple user profiles**
5. **Cloud backup** with encryption

### Code Modification Tips
- **Change encryption key** in `saveMaster()` and `checkMaster()`
- **Add new password types** by extending the `type` system
- **Modify grid size** for patterns in `drawInputGrid()`

## 👤 Author

**Utkarsh Gupta**  
Aspiring Software Engineer | C++ Enthusiast
