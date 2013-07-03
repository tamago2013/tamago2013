#include <cstdio>
#include <cstdlib>
#include <signal.h>
#include <vector>
#include <map>
#include <fstream>
#include <GL/freeglut.h>
#include <ssm.hpp>
#include <ssmtype/spur-odometry.h>
#include "ssm-laser.hpp"
#include "qstream.hpp"
#include "ssmType.hpp"
#include "tkgutil.hpp"
using namespace std;
using namespace tkg;

//#define MERGEDEBUG
#ifdef MERGEDEBUG
#include "tkg/mergedline.hpp"
MergedLine mergedline;
point mergecenter;
bool testscreen = false;
#endif

#define SSM_NAME_SOKUIKI_3D_FS_EDGE "sokuiki_fs_edge"

// -----------------------------------------------------------------------------

int wid1 = 750;
int wid2 =  50;
int hei  = 600;

oqstream qout;
bool syncpp = true;
bool view2d = false;
bool load2d = false;

string fn_config = "Visualizer.conf";
string fn_map;
vector<int>    routeinfo;
vector<triple> route;
vector<triple> gridmap;
vector<triple> linemap;
vector<triple> edgemap;

int grounding   = 1;
int disp_laser  = 0;
int camera_mode = 0;
double vh[] = {-1.0, 1.0};
double farDist = 100.0;

bool setgl = false;

triple robot;
triple camCent;
double camDist = 20.0;
double camRotH = 0.0;
double camRotV = 0.0;

int btn,pmx,pmy;
double map_a=0.2, map_b=0.0;

map<string, vector<double> > config;

const int  stream_max  = 4;
int        stream_num  = 0;
bool        disp_sdata[stream_max];
SSMSOKUIKIData3D sdata[stream_max];

SSMApi<Spur_Odometry> glpos;   bool disp_glpos=true;
SSMApi<YSD_Pinfo>     pinfo;   bool disp_pinfo=true;
SSMApi<MTM_Minfo>     linfo;   bool disp_linfo=true;
SSMApi<MTM_Minfo>     einfo;   bool disp_einfo=true;
SSMApi<MTM_Adjust>    lajst;   bool disp_lajst=true;
SSMApi<MTM_Adjust>    eajst;   bool disp_eajst=true;
SSMApi<MTM_Line>      ldata;   bool disp_ldata=true;
SSMApi<MTM_Line>      edata;   bool disp_edata=true;

pair<const char*, int> stream[stream_max] = 
{
	pair<const char*, int>(SSM_NAME_SOKUIKI_3D_FS, 3),
	pair<const char*, int>(SSM_NAME_SOKUIKI_3D_FS, 4),
	pair<const char*, int>(SSM_NAME_SOKUIKI_3D_FS, 5),
	pair<const char*, int>(SNAME_YSD_GRAND_EDGE, SID_YSD_GRAND_EDGE)
};

bool disp_map=true;
bool disp_way=true;
bool disp_lmap=true;
bool disp_emap=true;
bool* menu_var_map[10];
char  menu_str_map[10][256];
bool* menu_var_stream[stream_max+10];
char  menu_str_stream[stream_max+10][256];
const char *menu_str_camera[] = {"*free", "*robot", "*robot-behind"};
const char *menu_str_switch[] = {"*off", "*on"};

double testcolor[7][3] = 
{
	{1,0,0}, {1,1,0},{0,1,0},{0,1,1},{0,0,1},{1,0,1},{1,1,1}
};

double amodecolor[4][3] = 
{
	{0.5,0.5,0.5}, {0.0,0.5,0.5},{0.0,0.5,0.0},{0.5,0.5,0.0} 
};


// -----------------------------------------------------------------------------

void myString(double x, double y, double z, const char *str)
{
	glRasterPos3d(x, y, z);
	for(int i=0; str[i]; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}
}

void myString(triple p, const char *str)
{
	myString(p.x, p.y, p.z, str);
}

void myColorHue(double h)
{
	double r=0,g=0,b=0;
	     if(h<=  0) { r=1.0;         g=0.0;         b=0.0;         }
	     if(h<= 60) { r=1.0;         g=+(h-  0)/60; b=0.0;         }
	else if(h<=120) { r=-(h-120)/60; g=1.0;         b=0.0;         }
	else if(h<=180) { r=0.0;         g=1.0;         b=+(h-120)/60; }
	else if(h<=240) { r=0.0;         g=-(h-240)/60; b=1.0;         }
	else if(h<=300) { r=+(h-240)/60; g=0.0;         b=1.0;         }
	else if(h<=360) { r=1.0;         g=0.0;         b=-(h-360)/60; }
	else            { r=1.0;         g=0.0;         b=0.0;         }
	glColor3d(r,g,b);
}


void setColor(const vector<double> &color, double h=0)
{
	if(color.empty())
	{
		h=h*240;
		h=min(max(0.0,h),240.0);
		myColorHue(h);
	}
	else
	{
		glColor4dv(&color[0]);
	}		
}

void myLookAt()
{
	triple c = polar(camDist, camRotV,        camRotH) + camCent;
	triple v = polar(camDist, camRotV-M_PI/2, camRotH);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(c.x, c.y, c.z,  camCent.x, camCent.y, camCent.z, v.x, v.y, v.z);
}

void drowCross(triple p, double theta, double r)
{
	glBegin(GL_LINES);
	glVertex3dv( (p + polar(r, M_PI/2, theta + 0.0*M_PI)).vec );
	glVertex3dv( (p + polar(r, M_PI/2, theta + 1.0*M_PI)).vec );
	glVertex3dv( (p + polar(r, M_PI/2, theta + 0.5*M_PI)).vec );
	glVertex3dv( (p + polar(r, M_PI/2, theta + 1.5*M_PI)).vec );
	glEnd();
}

void display1()
{
	vector<double> color;
	double low  = (vh[0]+0.9)/map_a/1.8 - map_b;
	double high = (vh[1]+0.9)/map_a/1.8 - map_b;

	// style
	glLineWidth(1);
	glPointSize(3);

	// ground
	const double gsize = 50.0;
	double gr_x = floor(robot.x);
	double gr_y = floor(robot.y);

	setColor(config["ground-panel"]);
	glBegin(GL_POLYGON);
	glVertex3d(gr_x+gsize, gr_y+gsize, 0.0);
	glVertex3d(gr_x-gsize, gr_y+gsize, 0.0);
	glVertex3d(gr_x-gsize, gr_y-gsize, 0.0);
	glVertex3d(gr_x+gsize, gr_y-gsize, 0.0);
	glEnd();

	setColor(config["ground-line"]);
	glBegin(GL_LINES);
	for(int i=-gsize; i<=gsize; i++)
	{
		glVertex3d(gr_x+i*1.0, gr_y-gsize, 0.0);
		glVertex3d(gr_x+i*1.0, gr_y+gsize, 0.0);
		glVertex3d(gr_x-gsize, gr_y+i*1.0, 0.0);
		glVertex3d(gr_x+gsize, gr_y+i*1.0, 0.0);
	}
	glEnd();

	// style
	glLineWidth(3);

	// robot
	glColor3d(1.0, 1.0, 1.0);
	drowCross(robot, glpos.data.theta, 1.0);

	// adjust
	if(disp_lajst)
	{
		glColor3d(0.0, 1.0, 1.0);
		drowCross(triple(lajst.data.x, lajst.data.y, 0.0), lajst.data.theta, 0.5);		
	}
	if(disp_eajst)
	{
		glColor3d(0.0, 1.0, 0.0);
		drowCross(triple(eajst.data.x, eajst.data.y, 0.0), eajst.data.theta, 0.5);		
	}

#ifdef MERGEDEBUG
	glColor3d(1.0, 0.0, 1.0);
	drowCross(triple(mergecenter.real(), mergecenter.imag(), 0.0), eajst.data.theta, 0.5);		
#endif


	// gridmap
	if(disp_map)
	{
		color = config["map-points"];
		glBegin(GL_POINTS);
		for(int i=0; i<gridmap.size(); i++)
		{
			if(low<=gridmap[i].z && gridmap[i].z<=high)
			{
				setColor(color, map_a*(map_b+gridmap[i].z));
				if(view2d) glVertex2dv(gridmap[i].vec);
				else       glVertex3dv(gridmap[i].vec);
			}
		}
		glEnd();
	}

	// scan data
	for(int i=0; i<stream_num; i++)
	{
		// scan point
		if(!disp_sdata[i]) continue;

		glBegin(GL_POINTS);
		color = config[strprintf("urg-points-%d",i)];
		for(int j=0; j<sdata[i].property.numPoints; j++)
		{
			if(sdata[i].data[j].isError()) continue;

			triple ref(sdata[i].data[j].reflect.vec);
			if(low<=ref.z && ref.z<=high)
			{
				ref.rotZ(glpos.data.theta);
				setColor(color, map_a*(map_b+ref.z));
				glVertex3dv( (robot+ref).vec );
			}
		}
		glEnd();

		// scan laser
		if(!disp_laser) continue;

		glBegin(GL_LINES);
		for(int i=0; i<stream_num; i++)
		{
			color = config[strprintf("urg-beams-%d",i)];
			for(int j=0; j<sdata[i].property.numPoints; j++)
			{
				if(sdata[i].data[j].isError()) continue;

				triple ref(sdata[i].data[j].reflect.vec);
				triple ori(sdata[i].data[j].origin.vec);				
				if(low<=ref.z && ref.z<=high)
				{
					ref.rotZ(glpos.data.theta);
					ori.rotZ(glpos.data.theta);
					setColor(color, map_a*(map_b+ref.z));
					glVertex3dv( (robot+ref).vec );
					glVertex3dv( (robot+ori).vec );
				}
			}
		}
		glEnd();
	}

	// linemap
	if( disp_lmap )
	{
		glColor3d(0.0, 0.4, 0.4);
		glBegin(GL_LINES);
		for(int i=0; i<linemap.size(); i++)
		{
			glVertex3dv( linemap[i].vec );
		}
		glEnd();
	}

	// edgemap
	if( disp_emap )
	{
		glColor3d(0.0, 0.4, 0.0);
		glBegin(GL_LINES);
		for(int i=0; i<edgemap.size(); i++)
		{
			glVertex3dv( edgemap[i].vec );
		}
		glEnd();
	}

	// linedata
	if( ldata.isOpen() && disp_ldata )
	{
		glColor3d(0.0, 1.0, 1.0);
		glBegin(GL_LINES);
		for(int i=0; i<ldata.data.numpoints; i++)
		{
			glVertex3d( ldata.data.start_x[i],  ldata.data.start_y[i],  0.5 );
			glVertex3d( ldata.data.finish_x[i], ldata.data.finish_y[i], 0.5 );
		}
		glEnd();
	}

	// edgedata
	if( edata.isOpen() && disp_edata )
	{
		glColor3d(0.0, 1.0, 0.0);
		glBegin(GL_LINES);
		for(int i=0; i<edata.data.numpoints; i++)
		{
			glVertex3d( edata.data.start_x[i],  edata.data.start_y[i],  0.5 );
			glVertex3d( edata.data.finish_x[i], edata.data.finish_y[i], 0.5 );
		}
		glEnd();
	}


#ifdef MERGEDEBUG
	glColor3d(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	glVertex3d(0,0,0);
	glVertex3d(10,0,0);
	glVertex3d(0,0,0);
	glVertex3d(0,10,0);
	glVertex3d(0,0,0);
	glVertex3d(0,0,10);
	glEnd();

	//glColor3d(1.0, 1.0, 1.0);

	glBegin(GL_LINES);
/*
	for(int i=0; i<mergedline.data.size(); i++)
	{
		point p1(0, 100), p2(0, -100);
		p1 *= polar(1.0,  mergedline.data[i].rad); p1 += polar(mergedline.data[i].dist, mergedline.data[i].rad);
		p2 *= polar(1.0,  mergedline.data[i].rad); p2 += polar(mergedline.data[i].dist, mergedline.data[i].rad);

		glColor3dv( testcolor[mergedline.data[i].group%7]);
		glVertex3d( p1.real(), p1.imag(), 1.5 );
		glVertex3d( p2.real(), p2.imag(), 1.5 );
	}
*/
	for(int i=0; i<mergedline.data.size(); i++)
	{
		//glColor3dv( testcolor[mergedline.data[i].group%7]);
		glColor4d(1, 0, 0, 0.5);
		glVertex3d( mergedline.data[i][0].real(), mergedline.data[i][0].imag(), 0.5 );
		glVertex3d( mergedline.data[i][1].real(), mergedline.data[i][1].imag(), 0.5 );
	}

	glEnd();


#endif

	// style
	glPointSize(7);

	// route
	if(disp_way)
	{
		// route line
		glBegin(GL_LINES);
		for(int i=1; i<route.size(); i++)
		{
			//glColor3d(1.0, 1.0, 1.0);
			//if(pinfo.isOpen()) if(i==pinfo.data.waypoint) glColor3d(0.0, 1.0, 0.0);
			glColor3dv(amodecolor[routeinfo[i-1]]);
			glVertex3dv(route[i-1].vec);
			glVertex3dv(route[i  ].vec);
		}
		glEnd();	

		// route point
		glColor3d(1.0, 1.0, 0.0);
		glBegin(GL_POINTS);
		for(int i=0; i<route.size(); i++)
		{
			glVertex3dv(route[i].vec);
		}
		glEnd();

		// string
		glColor3d(1.0, 1.0, 1.0);
		for(int i=0; i<route.size(); i++)
		{
			//glColor3dv(amodecolor[routeinfo[i]]);
			myString(route[i]+triple(0,0,0.2), strprintf("%d",i).c_str());
		}		
	}
}


void display2()
{
	glColor3d(0.2, 0.2, 0.2);
	glBegin(GL_POLYGON);
	glVertex2d(-1, -1);
	glVertex2d(+1, -1);
	glVertex2d(+1, +1);
	glVertex2d(-1, +1);
	glEnd();

	glLineWidth(3);

	glBegin(GL_LINE_STRIP);
	for(int i=0; i<7; i++) {
		myColorHue(240*i/6);
		glVertex2d( 0.0,  -0.9 + 1.8*i/6 );
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3d(1.0, 1.0, 1.0);
	for(int i=0; i<2; i++) {
		glVertex2d( -0.6, vh[i]*0.9 );
		glVertex2d(  0.6, vh[i]*0.9 );
	}
	glEnd();

	glLineWidth(1);
}

#ifdef MERGEDEBUG
void displayT()
{
	mergedline.prepare();
	glPointSize(3);
	glBegin(GL_POINTS);
	for(int i=0; i<mergedline.data.size(); i++)
	{
		glColor3dv(testcolor[mergedline.data[i].group%7]);
		glVertex2d(mergedline.data[i].rad, mergedline.data[i].dist);
	}
	glEnd();
}
#endif

void display()
{
	if(camera_mode >= 1) camCent = robot;
	if(camera_mode >= 2) camRotH = glpos.data.theta + M_PI;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef MERGEDEBUG
	if(testscreen)
	{
		const double md=100.0;
		glViewport(0,0,wid1+wid2,hei);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, 2*M_PI, 0, md, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0,0,1, 0,0,0, 0,1,0);
		displayT();
		myString(0, 0, 0, strprintf("%d", mergedline.data.size()).c_str() );
		glutSwapBuffers();
		return;
	}
#endif

	glViewport(0,0,wid1,hei);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)wid1/hei, 0.1, farDist);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	myLookAt();
	display1();

	glViewport(wid1,0,wid2,hei);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,0,1, 0,0,0, 0,1,0);
	display2();

	glViewport(0,0,wid1+wid2,hei);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,0,1, 0,0,0, 0,1,0);

	if(linfo.isOpen())
	myString(-0.98, -0.98, 0, strprintf("%s [%d]", linfo.data.mapname, linfo.data.mapnum).c_str() );
	if(einfo.isOpen())
	myString(-0.98, -0.90, 0, strprintf("%s [%d]", einfo.data.mapname, einfo.data.mapnum).c_str() );
	if(setgl)
	myString(-0.98,  0.90, 0, "Restart Mode");

	glutSwapBuffers();
}


void reshape(int w, int h)
{
	wid1=w-wid2; hei=h;
}

int mergecnt = 0;
void idle()
{
	bool update = false;

	if(setgl)
	{
		qout << "r";
	}

#ifdef MERGEDEBUG
	if(edata.isOpen()) if(edata.readNew())
	{
		double minX=1e10, maxX=-1e10; 
		double minY=1e10, maxY=-1e10;
		for(int i=0; i<edata.data.numpoints; i++)
		{
			minX = min(minX, edata.data. start_x[i]);
			minX = min(minX, edata.data. finish_x[i]);
			minY = min(minY, edata.data. start_y[i]);	
			minY = min(minY, edata.data. finish_y[i]);
			maxX = max(maxX, edata.data. start_x[i]);
			maxX = max(maxX, edata.data. finish_x[i]);
			maxY = max(maxY, edata.data. start_y[i]);		
			maxY = max(maxY, edata.data. finish_y[i]);	

			point s(edata.data. start_x[i], edata.data. start_y[i]);
			point t(edata.data.finish_x[i], edata.data.finish_y[i]);
			mergedline.push(line(s,t));
		}

		mergecenter = point( (minX+maxX)/2.0, (minY+maxY)/2.0 );

		if(true) //mergecnt==0)
		{
			mergedline.shift(-mergecenter);
			mergedline.prepare();
			mergedline.clustering_line();
			mergedline.clustering_segment();
			mergedline.merge();
			mergedline.shift(+mergecenter);
		}

		update=true;
	}
#endif

	if(ldata.isOpen()) if(ldata.readNew()) { update=true; }
	if(edata.isOpen()) if(edata.readNew()) { update=true; }
	if(lajst.isOpen()) if(lajst.readNew()) { update=true; }
	if(eajst.isOpen()) if(eajst.readNew()) { update=true; }
	if(pinfo.isOpen()) if(pinfo.readNew()) { update=true; }
	if(glpos.isOpen()) if(glpos.readNew())
	{
		robot = triple(glpos.data.x, glpos.data.y, 0);
		update=true;
	}

	triple t;
	if(linfo.isOpen())
	{
		if(linfo.readNew())
		{
			linemap.clear();
			ifstream fin(linfo.data.mapname);
			if(fin) while(fin >> t.x >> t.y) { linemap.push_back(t); }
		}
	}

	if(einfo.isOpen())
	{
		if(einfo.readNew())
		{
			edgemap.clear();
			ifstream fin(einfo.data.mapname);
			if(fin) while(fin >> t.x >> t.y) { edgemap.push_back(t); }
		}
	}

	for(int i=0; i<stream_num; i++)
	{
		if(sdata[i].readNew()) { update=true; }
	}

	if(update) glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	if(key == 'x') { glutLeaveMainLoop(); }
	if(key == 'd') { view2d=!view2d; glutPostRedisplay(); }

#ifdef MERGEDEBUG
	if(key == 'r') { mergedline.data.clear(); }

	if(key == 's') { testscreen=!testscreen; glutPostRedisplay();}
	if(key == 'c') { mergedline.clustering_line();    glutPostRedisplay(); }
	if(key == 'v') { mergedline.clustering_segment(); glutPostRedisplay(); }
	if(key == 'b') { mergedline.merge();              glutPostRedisplay(); }
	if(key == 'a')
	{
		mergedline.clustering_line();
		mergedline.clustering_segment();
		mergedline.merge();
		glutPostRedisplay();
	}
#endif
}



int hidx=-1;
void mouse(int button, int state, int mx, int my)
{
	if(mx<wid1)
	{
		if(button==0) { btn=button; pmx=mx; pmy=my; }
		if(button==2) { btn=button; pmx=mx; pmy=my; }
		if(button==3) { camDist*=1.1; glutPostRedisplay(); }
		if(button==4) { camDist/=1.1; glutPostRedisplay(); }
	}
	else
	{
		double dy = -(2.0*my/hei-1.0)/0.9;

		if(state==1) hidx=-1;
		if(state==0)
		{
			for(int i=0; i<2; i++) if(abs(vh[i]-dy) < 0.05) hidx=i;
		}

		if(button==3) { farDist += 20; }
		if(button==4) { farDist -= 20; }
		if(button==3 || button==4)
		{
			glutPostRedisplay();
		}
	}
}

void motion(int mx, int my)
{
	if(mx<wid1)
	{
		double dx = (double) (pmx - mx) / wid1;
		double dy = (double) (pmy - my) / hei;

		if(btn==0)
		{
			triple tmp(dy*camDist/2, dx*camDist/2, 0);
			if(grounding==0) tmp.rotY(camRotV);
			tmp.rotZ(camRotH);
			camCent = camCent + tmp;
			if(grounding==1) { camCent.z=0; }
			pmx=mx; pmy=my;
		}
		if(btn==2)
		{
			camRotH += dx;
			camRotV += dy;
			pmx=mx; pmy=my;
		}
	}
	else
	{
		double dy = -(2.0*my/hei-1.0)/0.9;

		if(hidx!=-1)
		{
			if(hidx==0 && dy<     -1.00) dy=     -1.00;
			if(hidx==0 && dy>vh[1]-0.05) dy=vh[1]-0.05;
			if(hidx==1 && dy<vh[0]+0.05) dy=vh[0]+0.05; 
			if(hidx==1 && dy>     +1.00) dy=     +1.00;
			vh[hidx] = dy;
		}
	}

	glutPostRedisplay();
}

void menu_template(const char **menu_str, int &var, int val)
{
	glutChangeToMenuEntry(var+1, menu_str[var]+1, var+1);
	var = val-1;
	glutChangeToMenuEntry(var+1, menu_str[var]+0, var+1);
	glutPostRedisplay();
}

void menu(int val)
{
	if(val == 1) { camRotV=0;                  glutPostRedisplay(); }
	if(val == 2) { setgl = true;               glutPostRedisplay(); }
	if(val == 3) { setgl = false; qout << "g"; glutPostRedisplay(); }
}

void menu_camera(int val)
{ menu_template(menu_str_camera, camera_mode, val); }

void menu_ground(int val)
{ menu_template(menu_str_switch, grounding, val); }

void menu_laser(int val)
{ menu_template(menu_str_switch, disp_laser, val); }

void menu_stream(int val)
{
	(*menu_var_stream[val-1]) = !(*menu_var_stream[val-1]);
	glutChangeToMenuEntry(val, menu_str_stream[val-1]+(*menu_var_stream[val-1]?0:1), val);
	glutPostRedisplay();
}

void menu_map(int val)
{
	(*menu_var_map[val-1]) = !(*menu_var_map[val-1]);
	glutChangeToMenuEntry(val, menu_str_map[val-1]+(*menu_var_map[val-1]?0:1), val);
	glutPostRedisplay();
}

void resetConfig()
{
/*
	setColor("ground-panel", "101010");
	setColor("ground-line",  "808080");
	setColor("map-points",   "00FFFF");
	for(int i=0; i<stream_max; i++)
	{
		setColor(strprintf("urg-points-%d",i), "00FF00");
		setColor(strprintf("urg-beams-%d", i), "FF000050");
	}
*/
}

void loadConfig()
{
	if(fn_config == "") return;

	ifstream fin(fn_config.c_str());
	if(fin)
	{
		string key,val;
		while(fin >> key >> val)
		{
			if(val == "rainbow")	config[key] = vector<double>();
			else                 config[key] = convert_color(val);
		}
	}
}

bool optAnalyze(int argc, char **argv)
{
	int opt;
	while((opt = getopt(argc, argv, "c:m:r:dsh")) != -1)
	{
		switch(opt)
		{
			case 'c':
				fn_config = optarg;
				break;

			case 'm':
				fn_map = string("../../data/") + optarg;
				if(fn_map[fn_map.size()-1]!='/') fn_map+='/';
				break;

			case 's':
				syncpp = false;
				break;

			case 'd':
				load2d = true;
				break;

			case 'h':	
				cerr << endl
				<< "\t[options]"  << endl
				<< "\ts            : stand-alone"  << endl
				<< "\tc <FileName> : config file." << endl
				<< "\tm <FileName> : gridmap.map"  << endl
				<< "\tr <FileName> : keiro.route"  << endl << endl
				<< "\t[manual]" << endl
				<< "\tleft click     : camera position." << endl
				<< "\tright click    : camera angle." << endl
				<< "\tmiddle click   : setting menu." << endl << endl;
				return false;

			default: // (opt=='?')
				cerr << "help : " << argv[0] << " -h" << endl;
				return false;
		}
	}
	return true;
}

template <typename T> void ssm_safe_close(SSMApi<T> &ssmapi)
{
	if( ssmapi.isOpen() )
	{
		ssmapi.close();
		cerr << "SSMApi.close " << ssmapi.getStreamName() << "[" << ssmapi.getStreamId() << "]" << endl;
	}
}

void ssm_safe_close(SSMSOKUIKIData3D &ssmapi)
{
	if( ssmapi.isOpen() )
	{
		ssmapi.close();
		cerr << "SSMApi.close " << ssmapi.getStreamName() << "[" << ssmapi.getStreamId() << "]" << endl;
	}
}

int main(int argc, char **argv)
{
	bool ret_ssm = false;

	if(!optAnalyze(argc, argv)) return 1;
	resetConfig();
	loadConfig();

	if(syncpp)
	{
		cout << "qstream connect ... " << flush;
		qout.open("../../data/pipe");
		cout << "ok" << endl;
	}

	try
	{
		ret_ssm = initSSM();
		if(ret_ssm)
		{
			camera_mode = 2;

			//glpos.open(SNAME_GLOBAL);
			glpos.open(SNAME_GLOBAL);
			pinfo.open(SNAME_YSD_PINFO,  SID_YSD_PINFO);
			linfo.open(SNAME_MTM_MINFO,  SID_MTM_MINFO_LINE);
			einfo.open(SNAME_MTM_MINFO,  SID_MTM_MINFO_EDGE);
			lajst.open(SNAME_MTM_ADJUST, SID_MTM_ADJUST_LINE);
			eajst.open(SNAME_MTM_ADJUST, SID_MTM_ADJUST_GRAND_EDGE);
			ldata.open(SNAME_MTM_LINE,   SID_MTM_LINE);
			edata.open(SNAME_MTM_LINE,   SID_MTM_GRAND_EDGE);

			if(ldata.isOpen()) { ldata.data.numpoints = 0; }
			if(edata.isOpen()) { edata.data.numpoints = 0; }

			for(int i=0; i<stream_max; i++)
			{
				sdata[stream_num].open(stream[i].first, stream[i].second);
				if(!sdata[stream_num].getProperty())
				{
					sdata[stream_num].close();
					continue;
				}

				disp_sdata[stream_num]=true;
				sprintf(menu_str_stream[stream_num], "*%s[%d]", stream[i].first, stream[i].second);
				menu_var_stream[stream_num] = &disp_sdata[stream_num];
				stream_num++;
			}
		}

		if(fn_map != "")
		{
			ifstream fin;

			if(load2d) fin.open( (fn_map+"grid2d.map").c_str() );
			else       fin.open( (fn_map+"grid3d.map").c_str() );
			if(fin)
			{
				triple t;
				while(fin >> t.x >> t.y >> t.z)
				{	
					gridmap.push_back(t);
				}
				fin.close();
			}

			fin.open( (fn_map+"keiro.dat").c_str() );
			if(fin)
			{
				triple t; char ch;
				while(fin >> ch >> t.x >> t.y)
				{
					route.push_back(t);
				}
				fin.close();
			}

			int adjustmode=0;
			fin.open( (fn_map+"adjust_change.conf").c_str() );
			if(fin)
			{
				string str; int s=0,t=0;
				while(fin >> str)
				{
					if(str=="ChangePoint:")
					{
						fin >> t;
						while(s<t) { routeinfo.push_back(adjustmode); s++; }
					}
					if(str=="AdjustMode:")
					{
						fin >> str;
						if(str == "Odometry") adjustmode=0;
						if(str == "LineOnly") adjustmode=1;
						if(str == "EdgeOnly") adjustmode=2;
						if(str == "Both"    ) adjustmode=3;
					}
				}
				fin.close();
			}

			while(routeinfo.size()<route.size()) { routeinfo.push_back(adjustmode); }
		}

		glutInit(&argc, argv);
		glutInitWindowSize(wid1+wid2,hei);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE /*| GLUT_DEPTH*/);
		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

		glutCreateWindow("Visualizer");
		glClearColor(0.0, 0.0, 0.0, 0.0);

		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutIdleFunc(idle);
		glutKeyboardFunc(keyboard);
		glutMouseFunc(mouse);
		glutMotionFunc(motion);

		int sc,mc=0,MENU_ID[5];

		MENU_ID[0] = glutCreateMenu(menu_camera);
		for(int i=0; i<3; i++) glutAddMenuEntry(menu_str_camera[i]+(i==camera_mode?0:1), i+1);

		MENU_ID[1] = glutCreateMenu(menu_ground);
		for(int i=0; i<2; i++) glutAddMenuEntry(menu_str_switch[i]+(i==grounding?0:1), i+1);

		MENU_ID[2] = glutCreateMenu(menu_laser);
		for(int i=0; i<2; i++) glutAddMenuEntry(menu_str_switch[i]+(i==disp_laser?0:1), i+1);

		MENU_ID[3] = glutCreateMenu(menu_stream);
		for(sc=0; sc<stream_num; sc++)
		{
			glutAddMenuEntry(menu_str_stream[sc], sc+1);
		}
		//if(glpos.isOpen()) { strcpy(menu_str_stream[sc], "*spur-gl"); menu_var_stream[sc]=&disp_glpos; glutAddMenuEntry(menu_str_stream[sc], sc+1); sc++; }
		//if(pinfo.isOpen()) { strcpy(menu_str_stream[sc], "*pinfo"    ); menu_var_stream[sc]=&disp_pinfo; glutAddMenuEntry(menu_str_stream[sc], sc+1); sc++; }
		//if(minfo.isOpen()) { strcpy(menu_str_stream[sc], "*minfo"    ); menu_var_stream[sc]=&disp_minfo; glutAddMenuEntry(menu_str_stream[sc], sc+1); sc++; }		
		if(ldata.isOpen()) { strcpy(menu_str_stream[sc], "*line"       ); menu_var_stream[sc]=&disp_ldata; glutAddMenuEntry(menu_str_stream[sc], sc+1); sc++; }
		if(lajst.isOpen()) { strcpy(menu_str_stream[sc], "*line-adjust"); menu_var_stream[sc]=&disp_lajst; glutAddMenuEntry(menu_str_stream[sc], sc+1); sc++; }
		if(edata.isOpen()) { strcpy(menu_str_stream[sc], "*edge"       ); menu_var_stream[sc]=&disp_edata; glutAddMenuEntry(menu_str_stream[sc], sc+1); sc++; }
		if(eajst.isOpen()) { strcpy(menu_str_stream[sc], "*edge-adjust"); menu_var_stream[sc]=&disp_eajst; glutAddMenuEntry(menu_str_stream[sc], sc+1); sc++; }

		MENU_ID[4] = glutCreateMenu(menu_map);
		if(gridmap.size()) { strcpy(menu_str_map[mc], "*gridmap"  ); menu_var_map[mc]=&disp_map;  glutAddMenuEntry(menu_str_map[mc], mc+1); mc++; }
		if(  route.size()) { strcpy(menu_str_map[mc], "*waypoint" ); menu_var_map[mc]=&disp_way;  glutAddMenuEntry(menu_str_map[mc], mc+1); mc++; }
		if(linfo.isOpen()) { strcpy(menu_str_map[mc], "*line"     ); menu_var_map[mc]=&disp_lmap; glutAddMenuEntry(menu_str_map[mc], mc+1); mc++; }
		if(einfo.isOpen()) { strcpy(menu_str_map[mc], "*edge"     ); menu_var_map[mc]=&disp_emap; glutAddMenuEntry(menu_str_map[mc], mc+1); mc++; }



		glutCreateMenu(menu);
		glutAddMenuEntry("ViewPoint2D", 1);
		if(sc) glutAddSubMenu("stream",    MENU_ID[3]);
		if(mc) glutAddSubMenu("mapinfo",    MENU_ID[4]);
		glutAddSubMenu("camera",    MENU_ID[0]);
		glutAddSubMenu("grounding", MENU_ID[1]);
		glutAddSubMenu("laser",     MENU_ID[2]);
		glutAddMenuEntry("Restart Mode", 2);
		glutAddMenuEntry("!! Start !!",  3);
		glutAttachMenu(GLUT_MIDDLE_BUTTON);

#ifdef MERGEDEBUG
	camera_mode=0;
	for(int i=0; i<sc; i++) (*menu_var_stream[i]) = false;
	for(int i=0; i<mc; i++) (*menu_var_map[i])    = false;
	disp_edata = true;
	disp_sdata[2] = true;
#endif

		idle();
		glutMainLoop();
	}
	catch(Exception e) { cerr << "Exception : " << e.str << endl; }
	catch( ... )       { cerr << "Unkown Exception" << endl; }

	for(int i=0; i<stream_num; i++)
	{
		ssm_safe_close(sdata[i]);
	}

	ssm_safe_close(glpos);
	ssm_safe_close(pinfo);
	ssm_safe_close(linfo);
	ssm_safe_close(einfo);
	ssm_safe_close(ldata);
	ssm_safe_close(lajst);
	ssm_safe_close(edata);
	ssm_safe_close(eajst);

	if(ret_ssm)
	{
		endSSM();
		cerr << "endSSM" << endl;
	}

	return 0;
}
