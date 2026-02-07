#include <stdio.h>
#include <stdlib.h> // For atof, atoi
#include <string.h>
#include "../include/stb_image.h"
#include "../include/stb_image_write.h"
#include "../include/process.h"
#include "../include/config.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: whitebg <image_path> [threshold] [quality]\n");
        return 1;
    }

    // 1. Setup Defaults (from config.h)
    double threshold = COLOR_THRESHOLD; 
    int quality = JPEG_QUALITY;

    // 2. Overwrite if user typed extra numbers
    if (argc >= 3) threshold = atof(argv[2]);
    if (argc >= 4) quality = atoi(argv[3]);

    printf("Processing with Threshold: %.0f, Quality: %d\n", threshold, quality);

    // 3. Load
    int width, height, channels;
    unsigned char *img = stbi_load(argv[1], &width, &height, &channels, 0);
    if (img == NULL) {
        printf("Error loading image.\n");
        return 1;
    }

    // 4. Process (Pass the threshold!)
    remove_background(img, width, height, channels, threshold);

    // 5. Generate New Filename
    char out_name[1024];
    strcpy(out_name, argv[1]);

    char *last_slash = strrchr(out_name, '\\');
    if (!last_slash) last_slash = strrchr(out_name, '/');

    char temp_filename[256];
    if (last_slash) {
        strcpy(temp_filename, last_slash + 1); // Copy just "photo.jpg"
        // Write new name AFTER the slash: "C:\path\white_T50_Q90_photo.jpg"
        sprintf(last_slash + 1, "%sT%.0f_Q%d_%s", OUTPUT_PREFIX, threshold, quality, temp_filename);
    } else {
        strcpy(temp_filename, out_name);
        sprintf(out_name, "%sT%.0f_Q%d_%s", OUTPUT_PREFIX, threshold, quality, temp_filename);
    }

    // 6. Save (Pass the quality!)
    if (stbi_write_jpg(out_name, width, height, channels, img, quality) == 0) {
        printf("FAILED to save image!\n");
    } else {
        printf("Saved: %s\n", out_name);
    }

    stbi_image_free(img);
    return 0;
}