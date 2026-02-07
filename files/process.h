#ifndef PROCESS_H
#define PROCESS_H

// Updated: Now accepts 'double threshold' as the last argument
void remove_background(unsigned char *img, int width, int height, int channels, double threshold);

#endif