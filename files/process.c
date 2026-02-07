#include "../include/process.h"
#include "../include/queue.h"
#include "../include/config.h"
#include "../include/detect.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define IDX(x, y, w, c) (((y) * (w) + (x)) * (c))

// --- PRIVATE HELPERS ---

double color_distance(unsigned char r1, unsigned char g1, unsigned char b1,
                      unsigned char r2, unsigned char g2, unsigned char b2) {
    return sqrt(pow(r1 - r2, 2) + pow(g1 - g2, 2) + pow(b1 - b2, 2));
}

int has_visited_neighbor(int x, int y, int width, int height, unsigned char *visited) {
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            if (visited[ny * width + nx] == 1) return 1;
        }
    }
    return 0;
}

void set_pixel_white(unsigned char *img, int index, int channels) {
    img[index]     = TARGET_R; 
    img[index + 1] = TARGET_G;
    img[index + 2] = TARGET_B;
    if (channels == 4) img[index + 3] = 255;
}

// --- CORE ALGORITHMS ---

// 1. Flood Fill with Center Guard + Saturation Shield
void flood_fill_background(unsigned char *img, unsigned char *visited, int width, int height, int channels, 
                           unsigned char bg_r, unsigned char bg_g, unsigned char bg_b, double threshold) {
    Queue* q = createQueue();
    
    // Start from 4 corners
    int start_points[4][2] = {{0,0}, {width-1, 0}, {0, height-1}, {width-1, height-1}};
    for(int i=0; i<4; i++) {
        enqueue(q, start_points[i][0], start_points[i][1]);
        visited[start_points[i][1] * width + start_points[i][0]] = 1;
    }

    // DEFINE SAFE ZONE (where the subject is likely to be)
    int safe_x_min = width * 0.20; 
    int safe_x_max = width * 0.80;
    int safe_y_min = height * 0.15; 

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    while (!isQueueEmpty(q)) {
        Point current = dequeue(q);
        int cx = current.x;
        int cy = current.y;

        set_pixel_white(img, IDX(cx, cy, width, channels), channels);

        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                int v_index = ny * width + nx;
                
                if (visited[v_index] == 0) {
                    int idx = IDX(nx, ny, width, channels);
                    
                    // --- SUBJECT PROTECTION LOGIC ---
                    int in_safe_zone = (nx > safe_x_min && nx < safe_x_max && ny > safe_y_min);
                    
                    // 1. SATURATION SHIELD: If pixel has high saturation (colorful subject)
                    //    and is inside the frame, PROTECT IT.
                    // 
                    // KEY IMPROVEMENT: This works even with red/yellow color cast because
                    // saturation measures RGB variance, not specific color values.
                    if (in_safe_zone && is_likely_subject(img[idx], img[idx+1], img[idx+2])) {
                        // Skip this pixel. Do NOT queue it. Do NOT turn it white.
                        continue; 
                    }

                    // 2. COLOR DISTANCE CHECK (background similarity)
                    double dist = color_distance(img[idx], img[idx+1], img[idx+2], bg_r, bg_g, bg_b);
                    
                    // 3. ADAPTIVE THRESHOLD
                    double effective_threshold = threshold;
                    if (in_safe_zone) {
                        // Stricter inside the body zone
                        effective_threshold = threshold * 0.35; 
                    }

                    if (dist < effective_threshold) {
                        enqueue(q, nx, ny);
                        visited[v_index] = 1;
                    }
                }
            }
        }
    }
    freeQueue(q);
}

// 2. Island Removal
void remove_isolated_islands(unsigned char *img, unsigned char *visited, int width, int height, int channels, 
                             unsigned char bg_r, unsigned char bg_g, unsigned char bg_b, double threshold) {
    double strict_threshold = threshold * 0.9; 
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int v_index = y * width + x;
            if (visited[v_index] == 0) { 
                int idx = IDX(x, y, width, channels);
                
                // SATURATION CHECK: Don't remove islands if they're part of the subject
                // This prevents holes in the face/body if the flood fill missed a spot.
                if (is_likely_subject(img[idx], img[idx+1], img[idx+2])) continue;

                double dist = color_distance(img[idx], img[idx+1], img[idx+2], bg_r, bg_g, bg_b);
                if (dist < strict_threshold) {
                    set_pixel_white(img, idx, channels);
                    visited[v_index] = 1; 
                }
            }
        }
    }
}

// 3. Edge Erosion
void erode_hair_edges(unsigned char *img, unsigned char *visited, int width, int height, int channels, 
                      unsigned char bg_r, unsigned char bg_g, unsigned char bg_b, double threshold) {
    int passes = 2; 
    int *to_remove_x = malloc(width * height * sizeof(int));
    int *to_remove_y = malloc(width * height * sizeof(int));
    if (!to_remove_x || !to_remove_y) return;

    for (int pass = 0; pass < passes; pass++) {
        int remove_count = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int v_index = y * width + x;
                if (visited[v_index] == 0) {
                    if (has_visited_neighbor(x, y, width, height, visited)) {
                        int idx = IDX(x, y, width, channels);

                        // SATURATION PROTECTION: Do not erode pixels that are part of the subject.
                        // This preserves the chin, cheek edges, and clothing boundaries.
                        if (is_likely_subject(img[idx], img[idx+1], img[idx+2])) continue;

                        double dist = color_distance(img[idx], img[idx+1], img[idx+2], bg_r, bg_g, bg_b);
                        if (dist < (threshold * 1.4)) {
                            to_remove_x[remove_count] = x;
                            to_remove_y[remove_count] = y;
                            remove_count++;
                        }
                    }
                }
            }
        }
        for (int k = 0; k < remove_count; k++) {
            int rx = to_remove_x[k];
            int ry = to_remove_y[k];
            set_pixel_white(img, IDX(rx, ry, width, channels), channels);
            visited[ry * width + rx] = 1;
        }
    }
    free(to_remove_x);
    free(to_remove_y);
}

// --- MAIN WRAPPER ---
void remove_background(unsigned char *img, int width, int height, int channels, double threshold) {
    unsigned char bg_r = img[0];
    unsigned char bg_g = img[1];
    unsigned char bg_b = img[2];

    unsigned char *visited = (unsigned char *)calloc(width * height, sizeof(unsigned char));
    if (!visited) return;

    flood_fill_background(img, visited, width, height, channels, bg_r, bg_g, bg_b, threshold);
    remove_isolated_islands(img, visited, width, height, channels, bg_r, bg_g, bg_b, threshold);
    erode_hair_edges(img, visited, width, height, channels, bg_r, bg_g, bg_b, threshold);

    free(visited);
}
