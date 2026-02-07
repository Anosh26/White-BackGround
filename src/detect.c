#include "../include/detect.h"
#include "../include/config.h"
#include <stdlib.h>

#define IDX(x, y, w, c) (((y) * (w) + (x)) * (c))

// --- SATURATION-BASED DETECTION ---

/**
 * Calculate RGB saturation (variance).
 * Formula: max(R,G,B) - min(R,G,B)
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
 * Check if a region is uniform (solid color) by examining neighbors.
 * 
 * KEY INSIGHT: 
 * - Solid red background: All pixels similar → LOW variance → uniform
 * - Human face: Skin texture, shadows, pores → HIGH variance → NOT uniform
 * 
 * This distinguishes vibrant solid backgrounds from colorful subjects.
 */
int is_uniform_region(unsigned char *img, int x, int y, int width, int height, 
                      int channels, int max_variance) {
    // Get center pixel color
    int center_idx = IDX(x, y, width, channels);
    int center_r = img[center_idx];
    int center_g = img[center_idx + 1];
    int center_b = img[center_idx + 2];
    
    // Check 3x3 neighborhood (8 surrounding pixels)
    int total_variance = 0;
    int count = 0;
    
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue; // Skip center pixel
            
            int nx = x + dx;
            int ny = y + dy;
            
            // Bounds check
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                int neighbor_idx = IDX(nx, ny, width, channels);
                int nr = img[neighbor_idx];
                int ng = img[neighbor_idx + 1];
                int nb = img[neighbor_idx + 2];
                
                // Calculate color difference from center
                int diff_r = abs(nr - center_r);
                int diff_g = abs(ng - center_g);
                int diff_b = abs(nb - center_b);
                int variance = diff_r + diff_g + diff_b;
                
                total_variance += variance;
                count++;
            }
        }
    }
    
    if (count == 0) return 0; // Edge case
    
    // Average variance across neighbors
    int avg_variance = total_variance / count;
    
    // If average variance is low, it's a uniform region (background)
    return (avg_variance <= max_variance);
}

/**
 * Determine if a pixel belongs to the subject using BOTH saturation AND uniformity.
 * 
 * LOGIC:
 * 1. Calculate saturation
 * 2. If saturation is LOW → definitely background
 * 3. If saturation is HIGH:
 *    a. Check if it's a uniform region (solid color)
 *    b. If uniform → background (even though colorful)
 *    c. If NOT uniform → subject (colorful AND textured)
 */
int is_subject_pixel(unsigned char *img, int x, int y, int width, int height, 
                     int channels, int min_saturation) {
    int idx = IDX(x, y, width, channels);
    unsigned char r = img[idx];
    unsigned char g = img[idx + 1];
    unsigned char b = img[idx + 2];
    
    int saturation = calculate_saturation(r, g, b);
    
    // Brightness check to avoid pure black
    int brightness = (r + g + b) / 3;
    if (brightness < 20) return 0;
    
    // LOW saturation → definitely background
    if (saturation < min_saturation) {
        return 0;
    }
    
    // HIGH saturation → need to check uniformity
    // Solid colored backgrounds (like red backdrop) are uniform
    // Human subjects (skin, clothes) have texture/variation
    
    // Default max variance: 15 (configurable)
    #ifndef UNIFORMITY_THRESHOLD
    #define UNIFORMITY_THRESHOLD 15
    #endif
    
    int is_uniform = is_uniform_region(img, x, y, width, height, channels, UNIFORMITY_THRESHOLD);
    
    // If it's uniform, it's a solid background (even if colorful)
    if (is_uniform) {
        return 0; // Background
    }
    
    // If it's NOT uniform AND has high saturation → it's the subject
    return 1;
}

/**
 * Convenience wrapper using the default threshold from config.
 */
int is_likely_subject(unsigned char *img, int x, int y, int width, int height, int channels) {
    #ifndef SUBJECT_SATURATION_THRESHOLD
    #define SUBJECT_SATURATION_THRESHOLD 18
    #endif
    
    return is_subject_pixel(img, x, y, width, height, channels, SUBJECT_SATURATION_THRESHOLD);
}