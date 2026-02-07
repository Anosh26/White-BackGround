#include "../include/detect.h"
#include "../include/config.h"

// --- SATURATION-BASED DETECTION ---
// This approach works correctly even when photos have a global color cast
// (red/yellow tint from phone camera or scanning artifacts).

/**
 * Calculate RGB saturation (variance).
 * Formula: max(R,G,B) - min(R,G,B)
 * 
 * Why this works:
 * - Human subjects (skin, hair, clothes) have HIGH saturation (varied RGB values)
 * - Passport backgrounds (white, beige, light blue) have LOW saturation (similar RGB values)
 * - This remains true even if the entire image has a red/yellow tint
 * 
 * Examples:
 * - Pure white background: (255, 255, 255) → saturation = 0
 * - Beige with red tint: (240, 235, 230) → saturation = 10
 * - Human skin: (210, 160, 140) → saturation = 70
 * - Dark hair: (50, 40, 35) → saturation = 15
 */
int calculate_saturation(unsigned char r, unsigned char g, unsigned char b) {
    int max_val = r;
    int min_val = r;
    
    if (g > max_val) max_val = g;
    if (b > max_val) max_val = b;
    
    if (g < min_val) min_val = g;
    if (b < min_val) min_val = b;
    
    return max_val - min_val;
}

/**
 * Determine if a pixel belongs to the subject using saturation.
 * 
 * @param min_saturation Threshold for subject detection
 *        - Too low (< 10): Background noise gets protected
 *        - Too high (> 30): Dark hair and shadows get removed
 *        - Recommended: 15-25 for passport photos
 */
int is_subject_pixel(unsigned char r, unsigned char g, unsigned char b, int min_saturation) {
    int saturation = calculate_saturation(r, g, b);
    
    // Additional brightness check to avoid protecting pure black pixels
    // (which might have saturation due to noise but aren't part of the subject)
    int brightness = (r + g + b) / 3;
    
    // Pixel is subject if:
    // 1. It has sufficient saturation (colorful/varied)
    // 2. It's not pure black (brightness > 20)
    return (saturation >= min_saturation && brightness > 20);
}

/**
 * Convenience wrapper using the default threshold from config.
 */
int is_likely_subject(unsigned char r, unsigned char g, unsigned char b) {
    // Use configurable threshold with sensible default
    #ifndef SUBJECT_SATURATION_THRESHOLD
    #define SUBJECT_SATURATION_THRESHOLD 18
    #endif
    
    return is_subject_pixel(r, g, b, SUBJECT_SATURATION_THRESHOLD);
}
