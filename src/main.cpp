// [DONE] 1- Create an image and have access to the pixels-array [DONE]
// [DONE] 2- make a function that's able to draw single pixels given the desired coords [DONE]
// [DONE] 3- make darwDot() [DONE]
// [DONE] 4- make drawSegment() [DONE]
// 5- make a function that given the model verticies and edge indicies , project the verticies onto the pic. plane
// 6- make a parser for PLY files , to make the program more versatile
// 7- clean up the code

#include <cstdio>
#include <cstdlib>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "logger.h"

inline void setPixel(unsigned char *data, int xCoord, int yCoord, unsigned int rgba, size_t width, size_t height, size_t comp) __attribute__((always_inline));
void drawDot(unsigned char *data, int xCoord, int yCoord, unsigned int rgba, size_t dotSize, size_t width, size_t height, size_t comp);
void drawSegment(unsigned char *data, int x1Coord, int y1Coord, int x2Coord, int y2Coord, unsigned int rgba, size_t lineThickness, size_t width, size_t height, size_t comp);

int main(int argc, const char *argv[])
{

    const char *filename = "output.png";
    size_t width = 500;
    size_t height = 500;
    size_t comp = 4;
    size_t imgSizeBytes = width * height * comp;

    unsigned char *data = (unsigned char *)malloc(imgSizeBytes);
    // zero out the mem-block
    memset((void *)data, 0xff, imgSizeBytes);

    setPixel(data, 250, 250, 0xff0000ff, width, height, comp);
    setPixel(data, 0, 0, 0xff0000ff, width, height, comp);
    setPixel(data, width - 1, height - 1, 0xff0000ff, width, height, comp);
    drawDot(data, 200, 50, 0xffcc00ff, 5, width, height, comp);

    drawSegment(data, 0, 300, 499, 300, 0xff0000ff, 1, width, height, comp);
    drawSegment(data, 200, 0, 200, 499, 0xff00ff00, 1, width, height, comp);
    drawSegment(data, 0, 0, 499, 499, 0xff00ff00, 1, width, height, comp);
    drawSegment(data, 499, 0, 0, 499, 0xff00ff00, 1, width, height, comp);

    int stb_status = stbi_write_png(filename, width, height, comp, data, width * comp);

    if (stb_status == 0)
    {
        logError("something went wrong!! \n");
    }
    return 0;
}
// rgba is layed out in little endian (i.e : 0xaabbggrr)
inline void setPixel(unsigned char *data, int xCoord, int yCoord, unsigned int rgba, size_t width, size_t height, size_t comp)
{
    assert(xCoord < width);
    assert(0 <= xCoord);
    assert(yCoord < height);
    assert(0 <= yCoord);

    unsigned int *target = (unsigned int *)(data + (yCoord * width * comp) + (xCoord * comp));
    *target = rgba;
}

// [TODO] need to fix off-boundry pixels when choosing dotSize bigger than 1 [TODO]
void drawDot(unsigned char *data, int xCoord, int yCoord, unsigned int rgba, size_t dotSize, size_t width, size_t height, size_t comp)
{
    switch (dotSize)
    {
    case 1:
        setPixel(data, xCoord, yCoord, rgba, width, height, comp);
        break;
    case 3:
        setPixel(data, xCoord, yCoord, rgba, width, height, comp);
        setPixel(data, xCoord - 1, yCoord, rgba, width, height, comp);
        setPixel(data, xCoord + 1, yCoord, rgba, width, height, comp);
        break;
    case 5:
    default:
        setPixel(data, xCoord, yCoord, rgba, width, height, comp);
        setPixel(data, xCoord - 1, yCoord - 1, rgba, width, height, comp);
        setPixel(data, xCoord + 1, yCoord - 1, rgba, width, height, comp);
        setPixel(data, xCoord - 1, yCoord + 1, rgba, width, height, comp);
        setPixel(data, xCoord + 1, yCoord + 1, rgba, width, height, comp);
        break;
    }
}

// [TODO] need to fix off-boundry pixels when choosing lineThickness bigger than 1 [TODO]
void drawSegment(unsigned char *data, int x1Coord, int y1Coord, int x2Coord, int y2Coord, unsigned int rgba, size_t lineThickness, size_t width, size_t height, size_t comp)
{
    // pick the left point to be `p1`
    int p1X, p1Y;
    int p2X, p2Y;

    p1X = x1Coord;
    p1Y = y1Coord;

    p2X = x2Coord;
    p2Y = y2Coord;
    for (int xDiff = abs(p2X - p1X), yDiff = abs(p2Y - p1Y); xDiff + yDiff != 0;)
    {

        int xDirection = (xDiff == 0) ? 0 : (p2X - p1X) / xDiff;
        int yDirection = (yDiff == 0) ? 0 : (p2Y - p1Y) / yDiff;

        if (xDiff >= yDiff)
        {
            drawDot(data, p1X = p1X + xDirection, p1Y, rgba, lineThickness, width, height, comp);
            --xDiff;
        }
        else
        {
            drawDot(data, p1X, p1Y = p1Y + yDirection, rgba, lineThickness, width, height, comp);
            --yDiff;
        }
    }
}
