#include "../include/process.h"
#include "../include/queue.h"
#include "../include/config.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define IDX(x, y, w, c) (((y) * (w) + (x)) * (c))

// --- PRIVATE HELPERS ---

double color_distance(unsigned char r1, unsigned char g1, unsigned char b1,
                      unsigned char r2, unsigned char g2, unsigned char b2) {
    return sqrt(pow(r1 - r2, 2) + pow(g1 - g2, 2) + pow(b1 - b2, 2));
}

// Checks if a pixel has a neighbor that has already been removed (visited)
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

// Helper to paint a pixel white
void set_pixel_white(unsigned char *img, int index, int channels) {
    img[index]     = TARGET_R; 
    img[index + 1] = TARGET_G;
    img[index + 2] = TARGET_B;
    if (channels == 4) img[index + 3] = 255;
}

// --- CORE FUNCTIONS ---

// 1. Main Flood Fill (Starts from all 4 corners to catch more background)
void flood_fill_background(unsigned char *img, unsigned char *visited, int width, int height, int channels, 
                           unsigned char bg_r, unsigned char bg_g, unsigned char bg_b, double threshold) {
    Queue* q = createQueue();
    
    // Start from all 4 corners (Top-Left, Top-Right, Bot-Left, Bot-Right)
    int start_points[4][2] = {{0,0}, {width-1, 0}, {0, height-1}, {width-1, height-1}};
    
    for(int i=0; i<4; i++) {
        int x = start_points[i][0];
        int y = start_points[i][1];
        enqueue(q, x, y);
        visited[y * width + x] = 1;
    }

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
                    double dist = color_distance(img[idx], img[idx+1], img[idx+2], bg_r, bg_g, bg_b);

                    if (dist < threshold) {
                        enqueue(q, nx, ny);
                        visited[v_index] = 1;
                    }
                }
            }
        }
    }
    freeQueue(q);
}

// 2. Island Removal (Fixes the gaps inside arms/hair loops)
void remove_isolated_islands(unsigned char *img, unsigned char *visited, int width, int height, int channels, 
                             unsigned char bg_r, unsigned char bg_g, unsigned char bg_b, double threshold) {
    // We use a stricter threshold (0.9x) to avoid deleting parts of the shirt that look like the wall
    double strict_threshold = threshold * 0.9; 

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int v_index = y * width + x;
            
            // If the pixel was NOT reached by the flood fill...
            if (visited[v_index] == 0) { 
                int idx = IDX(x, y, width, channels);
                double dist = color_distance(img[idx], img[idx+1], img[idx+2], bg_r, bg_g, bg_b);

                // ...but it looks exactly like the background? It's an island!
                if (dist < strict_threshold) {
                    set_pixel_white(img, idx, channels);
                    visited[v_index] = 1; 
                }
            }
        }
    }
}

// 3. Edge Erosion (Fixes the colored halo/outline around hair)
void erode_hair_edges(unsigned char *img, unsigned char *visited, int width, int height, int channels, 
                      unsigned char bg_r, unsigned char bg_g, unsigned char bg_b, double threshold) {
    int passes = 2; // How many pixels deep to eat
    
    int *to_remove_x = malloc(width * height * sizeof(int));
    int *to_remove_y = malloc(width * height * sizeof(int));

    if (!to_remove_x || !to_remove_y) return; 

    for (int pass = 0; pass < passes; pass++) {
        int remove_count = 0;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int v_index = y * width + x;
                
                // If it is part of the SUBJECT (not visited)...
                if (visited[v_index] == 0) {
                    // ...but it touches the WHITE background...
                    if (has_visited_neighbor(x, y, width, height, visited)) {
                        int idx = IDX(x, y, width, channels);
                        double dist = color_distance(img[idx], img[idx+1], img[idx+2], bg_r, bg_g, bg_b);
                        
                        // ...and is somewhat similar to the background (Loose threshold 1.4x)
                        if (dist < (threshold * 1.4)) {
                            to_remove_x[remove_count] = x;
                            to_remove_y[remove_count] = y;
                            remove_count++;
                        }
                    }
                }
            }
        }

        // Apply removals for this pass
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

    // Phase 1: Main Flood Fill
    flood_fill_background(img, visited, width, height, channels, bg_r, bg_g, bg_b, threshold);

    // Phase 2: Clean up isolated gaps
    remove_isolated_islands(img, visited, width, height, channels, bg_r, bg_g, bg_b, threshold);

    // Phase 3: Smooth the edges
    erode_hair_edges(img, visited, width, height, channels, bg_r, bg_g, bg_b, threshold);

    free(visited);
}