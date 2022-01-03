#ifndef MODEL_H
#define MODEL_H

struct point2d
{
    float x;
    float y;
};

struct point3d
{
    float x;
    float y;
    float z;
};
struct pair
{
    unsigned int item1;
    unsigned int item2;
};

struct model
{
    unsigned int nVerticies;
    unsigned int nEdges;
    point3d *verticies;
    pair *edgeIndices;
};

#endif