#include <iostream>
#include <string>
#include <GL/glut.h>

#include "GLcommon.hpp"
#include <time.h>
#include <ssm.hpp>

bool gIsDrawHelp = false;

const char *help[] =
{
	"[ 1 ] : Horizontal URG",
	"[ 2 ] : Buttom URG",
	"[ 3 ] : Back URG",
	"[ 4 ] : Obs URG",
	"[ 5 ] : Kinect",
	"[ 6 ] : Free Map",
	"[ 7 ] : Road Map",
	"[ 8 ] : Runnable Map",
	"[ 9 ] : Configuration",
	"[ 0 ] : Particle",
	"",
	"[ w ] : draw world map",
	"[ h ] : draw this help",
	"",
	"[ u ] : init cam pos",
	"[ m ] : change drawing color mode",
	"[+/-] : change drawing points num",
	"[b/B] : chage background color"
};

/**
 * @brief ディスプレイへのヘルプ表示
 */
void drawHelp()
{
	const static int HELP_WIDTH = 400;
	const static int LINE_HEIGHT = 32; // 1行毎のオフセット
	if( gIsDrawHelp )
	{
		std::string data;

		glPushMatrix(  );
//		glTranslatef(0,0,0.0 );

		float x = ( gWindowSize[0] - HELP_WIDTH + 20 ), y = ( LINE_HEIGHT );
		// helpの描画
		for( size_t i = 0; i < sizeof( help ) / sizeof( char* ); i++ )
		{
			if( i == 0 || i == 2 )
			{
				if( ( gIsDraw[(i + 1)%10] % 3 ) )
					glColor4f(1.0,1.0,1.0,1.0);
				else
					glColor4f(0.7,0.7,0.8,1.0);
			}
			else if( ( i < 10 && ( !( gIsDraw[(i + 1)%10] % 2 ) ) ) || ( i == 11 && !gIsDrawWorldMap ) )
				glColor4f(0.7,0.7,0.8,1.0);
			else
				glColor4f(1.0,1.0,1.0,1.0);
//			std::cout << help[i] << std::endl;
			drawString( help[i], x, y, 0 );
			y += LINE_HEIGHT;

		}

		// help背景色の描画
		glColor4f(0.0,0.0,0.0,0.5);
		//glColor4f(1,1,1,1);
		glBegin( GL_QUADS );
		{
			glVertex2f( ( gWindowSize[0] - HELP_WIDTH ), 0.0 );
			glVertex2f( gWindowSize[0], 0.0 );
			glVertex2f( gWindowSize[0], gWindowSize[1] );
			glVertex2f( ( gWindowSize[0] - HELP_WIDTH ), gWindowSize[1] );
		}
		glEnd();

		glPopMatrix(  );

	}

}




