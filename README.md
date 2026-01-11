# LSB Image Steganography in C

This project implements **Least Significant Bit (LSB) Steganography** in C, allowing secret data to be hidden inside BMP images **without affecting visible quality**. 
It demonstrates secure and covert data transmission using bitwise operations and file handling in C.

---

## Features

- Hide secret text or files inside BMP images
- Retrieve hidden data accurately through decoding
- Maintain image quality while performing capacity checks
- Handle file operations efficiently using bitwise operations

---

---

## How It Works

1. **Encoding:**  
   The program reads the BMP image and hides the secret data in the **least significant bit** of each pixel.  
2. **Decoding:**  
   The program extracts the hidden data by reading the LSBs of the image pixels.  
3. **Capacity Check:**  
   Before encoding, the program checks if the image has enough capacity to hold the secret data.  

---

## Requirements

- C compiler (GCC recommended)
- BMP images (24-bit) for testing
- Basic understanding of command line usage

---

## How to Run

### Encode a file into an image
```bash
./encode input.bmp secret.txt output.bmp

