#ifndef STKTABLE_HPP__
#define STKTABLE_HPP__

#include <cmath>
#include <vector>
#include <GL/glut.h>


namespace indexcolor
{

template< int SIZE = 256>
class IndexColor
{
protected:
public:
	unsigned char _data[SIZE][4];

	unsigned char *get( int n )
	{
		if( n < 0 )
			n = 0;
		else if( n >= SIZE )
			n = SIZE - 1;
		return _data[n];
	}

	unsigned char *get( float rate )
	{
		return get( static_cast<int>( rate * SIZE ) );
	}
};


/**
 * グレースケール
 */
template< int SIZE = 256>
class GrayScale : public IndexColor<SIZE>
{
public:
	GrayScale()
	{
		for( int i= 0; i < SIZE; i++ )
		{
			IndexColor<SIZE>::_data[i][0] = IndexColor<SIZE>::_data[i][1] = IndexColor<SIZE>::_data[i][2] = IndexColor<SIZE>::_data[i][3] = static_cast<float>( i ) / ( SIZE - 1 ) * 255.0;
		}
	}
};

/**
 * 疑似カラー黒青緑赤白
 */
template< int SIZE = 256>
class PseudoColorKBGRW : public IndexColor<SIZE>
{
public:
	PseudoColorKBGRW()
	{
		double rate;
		for( int i= 0; i < SIZE; i++ )
		{
			IndexColor<SIZE>::_data[i][0] = IndexColor<SIZE>::_data[i][1] = IndexColor<SIZE>::_data[i][2] = 0;
			IndexColor<SIZE>::_data[i][3] = 255;
			rate = ( double ) i / SIZE;
			switch( static_cast<int>( rate * 6 ) % 6 )
			{
			case 0:
			{
				IndexColor<SIZE>::_data[i][2] = 255 * sin( ( rate * 6.0 ) * M_PI / 2.0 );
			}break;
			case 1:
			{
				IndexColor<SIZE>::_data[i][2] = 255;
				IndexColor<SIZE>::_data[i][1] = 255 * sin( ( rate * 6.0 - 1 ) * M_PI / 2.0 );
			}break;
			case 2:
			{
				IndexColor<SIZE>::_data[i][1] = 255;
				IndexColor<SIZE>::_data[i][2] = 255 * cos( ( rate * 6.0 - 2 ) * M_PI / 2.0 );
			}break;
			case 3:
			{
				IndexColor<SIZE>::_data[i][1] = 255;
				IndexColor<SIZE>::_data[i][0] = 255 * sin( ( rate * 6.0 - 3 ) * M_PI / 2.0 );
			}break;
			case 4:
			{
				IndexColor<SIZE>::_data[i][0] = 255;
				IndexColor<SIZE>::_data[i][1] = 255 * cos( ( rate * 6.0 - 4 ) * M_PI / 2.0 );
			}break;
			case 5:
			{
				IndexColor<SIZE>::_data[i][0] = 255;
				IndexColor<SIZE>::_data[i][1] = 255 * sin( ( rate * 6.0 - 5 ) * M_PI / 2.0 );
				IndexColor<SIZE>::_data[i][2] = IndexColor<SIZE>::_data[i][1];
			}break;
			}
		}
	}
};

/**
 * 疑似カラー黒青緑赤白
 */
template< int SIZE = 256>
class PseudoColorKBGW : public IndexColor<SIZE>
{
public:
	PseudoColorKBGW()
	{
		double rate;
		for( int i= 0; i < SIZE; i++ )
		{
			IndexColor<SIZE>::_data[i][0] = IndexColor<SIZE>::_data[i][1] = IndexColor<SIZE>::_data[i][2] = 0;
			IndexColor<SIZE>::_data[i][3] = 255;
			rate = ( double ) i / SIZE;
			switch( static_cast<int>( rate * 4 ) % 4 )
			{
			case 0:
			{
				IndexColor<SIZE>::_data[i][2] = 255 * rate * 4.0;
			}break;
			case 1:
			{
				IndexColor<SIZE>::_data[i][2] = 255;
				IndexColor<SIZE>::_data[i][1] = 255 * ( rate * 4.0 - 1);
			}break;
			case 2:
			{
				IndexColor<SIZE>::_data[i][1] = 255;
				IndexColor<SIZE>::_data[i][2] = 255 - 255 * ( rate * 4.0 - 2);
			}break;
			case 3:
			{
				IndexColor<SIZE>::_data[i][1] = 255;
				IndexColor<SIZE>::_data[i][0] = 255 * ( rate * 4.0 -3 );
				IndexColor<SIZE>::_data[i][2] = IndexColor<SIZE>::_data[i][0];
			}break;
			}
		}
	}
};


}

#endif //STKTABLE_HPP__
