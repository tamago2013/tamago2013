#include "map-viewer.hpp"
#include "gnd-bmp.hpp"
#include "gnd-opsm.hpp"
#include "tkg-opengl.hpp"
#include "tkg-debug.hpp"

MapLoader::MapLoader()
{
    tkg::debug("new MapLoader\n");
    data = NULL;
}

MapLoader::~MapLoader()
{
    tkg::debug("delete MapLoader\n");
    delete data;
}

void MapLoader::file(std::string path)
{
    fname = path;
}

void MapLoader::load()
{
    const char *dirname = fname.c_str();
    if( dirname == NULL) { emit send_make(); return; }
    if(*dirname == '\0') { emit send_make(); return; }

    gnd::opsm::map_t  opsm_map;
    gnd::opsm::cmap_t cnt_map;
    gnd::bmp8_t       bmp_map;

    // read  map raw data
    if( gnd::opsm::read_counting_map(&cnt_map, dirname) < 0)
    {
        //window->message()->add_message("マップの読込に失敗しました。[");
        //window->message()->add_message(dirname);
        //window->message()->add_message("]\n");
        emit send_make(); return;
    }

    if( gnd::opsm::build_map(&opsm_map, &cnt_map, gnd_mm2dist(1)) < 0 ) {
        //window->message()->add_message("マップの作成に失敗しました。\n");
        emit send_make(); return;
    }

    if( gnd::opsm::build_bmp(&bmp_map, &opsm_map, gnd_m2dist(1.0/10)) < 0 )
    {
        //window->message()->add_message("マップの画像化に失敗しました。\n");
        emit send_make(); return;emit send_make(); return;
    }
    //window->message()->add_message("マップの読込に成功しました。\n");

    minfo.width  = bmp_map.column();
    minfo.height = bmp_map.row();
    minfo.base_x = bmp_map.xorg();
    minfo.base_y = bmp_map.yorg();
    minfo.unit_x = bmp_map.xrsl();
    minfo.unit_y = bmp_map.yrsl();

    data = new uchar[minfo.width * minfo.height * 3];

    for(int y=0; y<minfo.height; y++)
    for(int x=0; x<minfo.width;  x++)
    {
        data[(y*minfo.width+x)*3+0] = bmp_map.value(y, x);
        data[(y*minfo.width+x)*3+1] = bmp_map.value(y, x);
        data[(y*minfo.width+x)*3+2] = bmp_map.value(y, x);
    }

    emit send_make();
}



MapViewer::MapViewer(std::string path)
{
    tkg::debug("new MapViewer\n");
    state  = false;
    fname  = path;
    loader = NULL;
    thread = NULL;
}

MapViewer::~MapViewer()
{
    tkg::debug("delete MapViewer\n");
    delete loader;
    delete thread;
}

bool MapViewer::good()
{
    return state;
}

void MapViewer::draw()
{
    if(!state) return;

    static const int dx[] = {0,1,1,0};
    static const int dy[] = {0,0,1,1};

    glColor3d(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, image);

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_POLYGON);
    for(int i=0; i<4; i++)
    {
        double pos_x = minfo.base_x + minfo.width  * minfo.unit_x * dx[i];
        double pos_y = minfo.base_y + minfo.height * minfo.unit_y * dy[i];
        glTexCoord2d(dx[i], dy[i]);
        glVertex3d  (pos_x, pos_y, 0.0);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);

    double sx = floor(minfo.base_x), gx = ceil(minfo.base_x + minfo.width  * minfo.unit_x);
    double sy = floor(minfo.base_y), gy = ceil(minfo.base_y + minfo.height * minfo.unit_y);

    glColor3d(0.2,0.2,0.2);
    glBegin(GL_LINES);
    for(double x=sx; x<=gx; x+=1.0)
    {
        glVertex3d(x, sy, 0.0);
        glVertex3d(x, gy, 0.0);
    }
    for(double y=sy; y<=gy; y+=1.0)
    {
        glVertex3d(sx, y, 0.0);
        glVertex3d(gx, y, 0.0);
    }
    glEnd();
}

void MapViewer::load()
{
    loader = new MapLoader;
    thread = new QThread;

    connect(this,   SIGNAL(send_load()), loader, SLOT(load()));
    connect(loader, SIGNAL(send_make()), this,   SLOT(make()));

    loader->file(fname);
    loader->moveToThread(thread);
    thread->start();

    emit send_load();
}

void MapViewer::make()
{
    thread->quit();

    if(loader->data == NULL) return;

    minfo = loader->minfo;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &image);
    glBindTexture(GL_TEXTURE_2D, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, minfo.width, minfo.height, GL_RGB, GL_UNSIGNED_BYTE, loader->data);

    state = true;

    //delete loader; loader = NULL;
    //delete thread; thread = NULL;
}
