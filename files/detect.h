#ifndef DETECT_H
#define DETECT_H

// --- SUBJECT DETECTION MODULE ---
// Uses saturation-based detection to identify the subject (person)
// against low-saturation backgrounds, regardless of color cast.

/**
 * Calculate RGB saturation (variance) for a pixel.
 * High saturation = colorful subject (skin, clothes, hair).
 * Low saturation = monotone background (white, beige, gray).
 * 
 * @param r Red channel value (0-255)
 * @param g Green channel value (0-255)
 * @param b Blue channel value (0-255)
 * @return Saturation value (0-255 range)
 */
int calculate_saturation(unsigned char r, unsigned char g, unsigned char b);

/**
 * Determine if a pixel likely belongs to the human subject.
 * Uses saturation threshold instead of color ranges.
 * 
 * @param r Red channel value
 * @param g Green channel value
 * @param b Blue channel value
 * @param min_saturation Minimum saturation to be considered subject (default: 15-25)
 * @return 1 if pixel is likely part of subject, 0 otherwise
 */
int is_subject_pixel(unsigned char r, unsigned char g, unsigned char b, int min_saturation);

/**
 * Check if a pixel is likely part of the human subject.
 * This is a convenience wrapper that uses the default saturation threshold.
 * 
 * @param r Red channel value
 * @param g Green channel value
 * @param b Blue channel value
 * @return 1 if pixel is likely part of subject, 0 otherwise
 */
int is_likely_subject(unsigned char r, unsigned char g, unsigned char b);

#endif
