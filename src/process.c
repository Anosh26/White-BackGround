#include "../include/process.h"
#include "../include/queue.h"
#include "../include/config.h" 
#include <math.h>
#include <stdlib.h>
#include <string.h>


#define IDX(x, y, w, c) (((y) * (w) + (x)) * (c))

double color_distance(unsigned char r1, unsigned char g1, unsigned char b1,
                      unsigned char r2, unsigned char g2, unsigned char b2) {
    return sqrt(pow(r1 - r2, 2) + pow(g1 - g2, 2) + pow(b1 - b2, 2));
}

// Updated: Function signature now matches the header
void remove_background(unsigned char *img, int width, int height, int channels, double threshold) {
    unsigned char bg_r = img[0];
    unsigned char bg_g = img[1];
    unsigned char bg_b = img[2];

    unsigned char *visited = (unsigned char *)calloc(width * height, sizeof(unsigned char));
    if (!visited) return;

    Queue* q = createQueue();
    enqueue(q, 0, 0);
    
    visited[0] = 1; 

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    while (!isQueueEmpty(q)) {
        Point current = dequeue(q);
        int cx = current.x;
        int cy = current.y;

        // Turn pixel WHITE using values from config.h
        int index = IDX(cx, cy, width, channels);
        img[index]     = TARGET_R; 
        img[index + 1] = TARGET_G;
        img[index + 2] = TARGET_B;
        if (channels == 4) img[index + 3] = 255;

        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                int v_index = ny * width + nx;
                
                if (visited[v_index] == 0) {
                    int n_pixel_index = IDX(nx, ny, width, channels);
                    double dist = color_distance(img[n_pixel_index], img[n_pixel_index + 1], img[n_pixel_index + 2], bg_r, bg_g, bg_b);

                    // Updated: Uses the 'threshold' variable passed from main
                    if (dist < threshold) {
                        enqueue(q, nx, ny);
                        visited[v_index] = 1;
                    }
                }
            }
        }
    }

    free(visited);
    freeQueue(q); 
}