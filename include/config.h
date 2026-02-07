#ifndef CONFIG_H
#define CONFIG_H

// --- ALGORITHM SETTINGS ---
// How distinct the background must be (Higher = cuts more shadows)
// Default: 80.0
#define COLOR_THRESHOLD 80.0 

// --- SUBJECT DETECTION SETTINGS ---

// Minimum saturation (RGB variance) to consider a pixel as part of the subject
// Formula: max(R,G,B) - min(R,G,B)
// 
// Values:
//   - Low (10-15): More aggressive protection, may save some background noise
//   - Medium (16-22): Balanced, recommended for most passport photos
//   - High (23-30): Stricter, may remove dark hair edges or shadows
// 
// Default: 18 (works well for scanned passport photos with color cast)
#define SUBJECT_SATURATION_THRESHOLD 18

// Maximum color variance in a 3x3 neighborhood to be considered "uniform"
// Uniform regions are solid backgrounds (even if colorful like red/blue)
// 
// How it works:
//   - Solid red background: Neighbors are all similar → variance < 15 → uniform → removed
//   - Human face/clothes: Skin texture, shadows → variance > 15 → NOT uniform → protected
// 
// Values:
//   - Low (8-12): Stricter, only very solid backgrounds are uniform
//   - Medium (13-18): Balanced, recommended for solid colored backgrounds
//   - High (19-25): Looser, may incorrectly mark textured backgrounds as uniform
// 
// Default: 15 (distinguishes solid backgrounds from textured subjects)
#define UNIFORMITY_THRESHOLD 15

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