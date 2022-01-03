// [DONE] 1- Create an image and have access to the pixels-array [DONE]
// [DONE] 2- make a function that's able to draw single pixels given the desired coords [DONE]
// [DONE] 3- make darwDot() [DONE]
// [DONE] 4- make drawSegment() [DONE]
// 5- make a function that given the model verticies and edge indicies , project the verticies onto the pic. plane
// 5.1 struct for models contains verticies and edges defined in model space coord
// 5.2 camera position in world space coord
// 5.3 scene struct that contain list of objects and a camera
// 5.4 make sure to take into consideration the transformation between object space to world space ,,, and from normilized device coord to pic. coord

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
#include "model.h"

inline void setPixel(unsigned char *data, int xCoord, int yCoord, unsigned int rgba, size_t width, size_t height, size_t comp) __attribute__((always_inline));
void drawDot(unsigned char *data, int xCoord, int yCoord, unsigned int rgba, size_t dotSize, size_t width, size_t height, size_t comp);
void drawSegment(unsigned char *data, int x1Coord, int y1Coord, int x2Coord, int y2Coord, unsigned int rgba, size_t lineThickness, size_t width, size_t height, size_t comp);
void drawModel(unsigned char *data, model *objectModel, int picWidth, int picHeight);

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
#if 0
    setPixel(data, 250, 250, 0xff0000ff, width, height, comp);
    setPixel(data, 0, 0, 0xff0000ff, width, height, comp);
    setPixel(data, width - 1, height - 1, 0xff0000ff, width, height, comp);
    drawDot(data, 200, 50, 0xffcc00ff, 5, width, height, comp);

    drawSegment(data, 0, 300, 499, 300, 0xff0000ff, 1, width, height, comp);
    drawSegment(data, 200, 0, 200, 499, 0xff00ff00, 1, width, height, comp);
    drawSegment(data, 0, 0, 499, 499, 0xff00ff00, 1, width, height, comp);
    drawSegment(data, 499, 0, 0, 499, 0xff00ff00, 1, width, height, comp);
#endif

    // point3d camPos = {0.0f,0.0f,0.0f};

    model cube;
    cube.nVerticies = 8;
    cube.verticies = (point3d *)malloc(sizeof(point3d) * cube.nVerticies);
    (cube.verticies)[0] = {0.5f, 0.5f, 2.5f};
    (cube.verticies)[1] = {0.5f, 1.5f, 2.5};
    (cube.verticies)[2] = {1.5f, 1.5f, 2.5f};
    (cube.verticies)[3] = {1.5f, 0.5f, 2.5};
    (cube.verticies)[4] = {0.5f, 0.5f, 3.5f};
    (cube.verticies)[5] = {0.5f, 1.5f, 3.5};
    (cube.verticies)[6] = {1.5f, 1.5f, 3.5f};
    (cube.verticies)[7] = {1.5f, 0.5f, 3.5f};

    cube.nEdges = 12;
    cube.edgeIndices = (pair *)malloc(sizeof(pair) * cube.nEdges);
    (cube.edgeIndices)[0] = {0, 1};
    (cube.edgeIndices)[1] = {1, 2};
    (cube.edgeIndices)[2] = {2, 3};
    (cube.edgeIndices)[3] = {3, 0};
    (cube.edgeIndices)[4] = {0, 4};
    (cube.edgeIndices)[5] = {1, 5};
    (cube.edgeIndices)[6] = {2, 6};
    (cube.edgeIndices)[7] = {3, 7};
    (cube.edgeIndices)[8] = {4, 5};
    (cube.edgeIndices)[9] = {5, 6};
    (cube.edgeIndices)[10] = {6, 7};
    (cube.edgeIndices)[11] = {7, 4};

    drawModel(data, &cube, width, height);

    int stb_status = stbi_write_png(filename, width, height, comp, data, width * comp);

    if (stb_status == 0)
    {
        logError("something went wrong!! \n");
    }

    free(cube.edgeIndices);
    free(cube.verticies);
    free(data);
    return 0;
}

void drawModel(unsigned char *data, model *objectModel, int picWidth, int picHeight)
{
    unsigned int nVerts = objectModel->nVerticies;
    unsigned int nEdges = objectModel->nEdges;
    point2d *projectedVerts = (point2d *)malloc(sizeof(point2d) * nVerts);
    for (int i = 0; i < nVerts; ++i)
    {
        float x = (objectModel->verticies)[i].x;
        float y = (objectModel->verticies)[i].y;
        float z = (objectModel->verticies)[i].z;
        float xPrime = x / z;
        float yPrime = y / z;
        // projectedVerts[i] = {1 - (xPrime - (picWidth - 1)) / (picWidth - 1), (yPrime - picHeight) / picHeight};
        projectedVerts[i] = {picWidth * xPrime, picHeight * yPrime};

        drawDot(data, projectedVerts[i].x, projectedVerts[i].y, 0xffff00ff, 1, picWidth, picHeight, 4);
    }
    for (int j = 0; j < nEdges; ++j)
    {
        pair endPointsIndicies = (objectModel->edgeIndices)[j];
        point2d p1 = projectedVerts[endPointsIndicies.item1];
        point2d p2 = projectedVerts[endPointsIndicies.item2];
        drawSegment(data, p1.x, p1.y, p2.x, p2.y, 0xff00ccff, 1, picWidth, picHeight, 4);
    }
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
