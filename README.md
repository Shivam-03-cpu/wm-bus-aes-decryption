# 🔐 W-MBus AES-128 Decryption – OMS Volume 2

## 📘 Overview
This project decrypts a Wireless M-Bus telegram using AES-128-CBC as per OMS Volume 2 (Security Mode 5). It extracts metadata, constructs the IV, decrypts the payload, and outputs human-readable data.

## 🧩 Telegram Structure
- **Meter Address**: 8 bytes (ID + version + device type)
- **Access Number (ACC)**: 1 byte, repeated 8 times in IV
- **Encrypted Payload**: AES-128 encrypted data
- **CI Field**: Indicates encryption (e.g., 0x8C)

## 🔐 AES-128 Decryption Steps
1. Extract meter address and access number
2. Construct IV: `IV = meterAddress + (ACC × 8)`
3. Decrypt using AES-128-CBC with provided key
4. Validate: First two bytes should be `0x2F 0x2F`
5. Parse TLV format for readings and status

## 🛠️ Build Instructions

### Linux/macOS
```bash
sudo apt install libssl-dev g++
g++ main.cpp -o decrypt -lssl -lcrypto
./decrypt