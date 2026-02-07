#ifndef DETECT_H
#define DETECT_H

// --- SUBJECT DETECTION MODULE ---
// Uses saturation + uniformity to identify the subject (person)
// against backgrounds, even if the background is a solid vibrant color.

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
 * Check if a pixel is part of a uniform colored region.
 * Uniform regions (solid backgrounds) have low neighbor variance.
 * 
 * @param img Image data
 * @param x X coordinate
 * @param y Y coordinate
 * @param width Image width
 * @param height Image height
 * @param channels Number of channels (3 or 4)
 * @param max_variance Maximum allowed variance to be considered uniform
 * @return 1 if uniform (likely background), 0 if varied (likely subject)
 */
int is_uniform_region(unsigned char *img, int x, int y, int width, int height, 
                      int channels, int max_variance);

/**
 * Determine if a pixel likely belongs to the human subject.
 * Uses BOTH saturation AND uniformity checks.
 * 
 * @param img Image data (needed for uniformity check)
 * @param x X coordinate
 * @param y Y coordinate
 * @param width Image width
 * @param height Image height
 * @param channels Number of channels
 * @param min_saturation Minimum saturation to be considered subject
 * @return 1 if pixel is likely part of subject, 0 otherwise
 */
int is_subject_pixel(unsigned char *img, int x, int y, int width, int height, 
                     int channels, int min_saturation);

/**
 * Check if a pixel is likely part of the human subject.
 * This is a convenience wrapper that uses the default saturation threshold.
 * 
 * @param img Image data
 * @param x X coordinate
 * @param y Y coordinate
 * @param width Image width
 * @param height Image height
 * @param channels Number of channels
 * @return 1 if pixel is likely part of subject, 0 otherwise
 */
int is_likely_subject(unsigned char *img, int x, int y, int width, int height, int channels);

#endif