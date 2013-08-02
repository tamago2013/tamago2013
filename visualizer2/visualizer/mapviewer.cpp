#include "mapviewer.hpp"
#include "gnd-bmp.hpp"
#include "gnd-psm.hpp"
using namespace gnd;
using namespace gnd::psm;

MapViewer::MapViewer()
{
    width  = -1;
    height = -1;
    data   = NULL;
}

MapViewer::~MapViewer()
{
    delete data;
}

bool MapViewer::read_cmap(const char *dirname)
{
    if( dirname == NULL) return false;
    if(*dirname == '\0') return false;


    map_t  psm_map;
    cmap_t cnt_map;
    bmp8_t bmp_map;

    // read  map raw data
    if( read_counting_map(&cnt_map, dirname) < 0)
    {
        return false;
    }

    // convert?
    if( gnd::psm::build_map(&psm_map, &cnt_map, gnd_mm2dist(1)) < 0 ) {
        //::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to build map\n");
        return false;
    }

    if( build_bmp(&bmp_map, &psm_map, gnd_m2dist(1.0/10)) < 0 )
    {
        //::fprintf(stderr, " ... \x1b[1m\x1b[31mERROR\x1b[39m\x1b[0m: fail to convert bmp\n");
        return false;
    }

    width  = bmp_map.column();
    height = bmp_map.row();
    base_x = bmp_map.xorg();
    base_y = bmp_map.yorg();
    unit_x = bmp_map.xrsl();
    unit_y = bmp_map.yrsl();

    data = new char[width*height];
    for(int y=0; y<height; y++)
    for(int x=0; x<width;  x++)
    {
        data[y*width+x] = bmp_map.value(y, x);
    }

    return true;
}

#include <GL/gl.h>
void MapViewer::draw()
{
    glPointSize(10);
    glBegin(GL_POINTS);
    for(int y=0; y<height; y++)
    for(int x=0; x<width;  x++)
    {
        int v = data[y*width+x];
        if(v==0) continue;
        glColor4d(v/255.0, v/255.0, v/255.0, 1-v/255.0);
        glVertex2d(x*unit_x+base_x, y*unit_y+base_y);
    }
    glEnd();
    glPointSize(1);
}
