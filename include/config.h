#ifndef CONFIG_H
#define CONFIG_H

// --- ALGORITHM SETTINGS ---
// How distinct the background must be (Higher = cuts more shadows)
// Default: 80.0
#define COLOR_THRESHOLD 80.0 

// --- OUTPUT SETTINGS ---
// Quality of the saved JPG (1-100)
#define JPEG_QUALITY 90

// The text added to the start of the new filename
#define OUTPUT_PREFIX "white_"

// --- COLOR SETTINGS ---
// The target color to replace the background with (RGB)
// 255, 255, 255 is Pure White
#define TARGET_R 255
#define TARGET_G 255
#define TARGET_B 255

#endif