/**
 * ファイル書き込み・読み込みクラス
 */
#ifndef STREAM_ACCESS_HPP_
#define STREAM_ACCESS_HPP_

#include <sstream>
#include <iostream>
#include <fstream>
class StreamRead
{
public:

	// ファイル読み込み
	virtual int read( std::istream &input ) = 0;

	int read( const char *fileName )
	{
		std::ifstream file;
		file.open( fileName );
		if( !file.is_open() )
		{
			std::cerr << "cannot open file [" << fileName << "]." << std::endl;
			return -1;
		}

		return read( file );
	}
};

class StreamReadLine : public StreamRead
{
public:
	//１行読み込み
	virtual bool readData( std::istream &data ) = 0;
	using StreamRead::read;

	int read( std::istream &input )
	{
		int cnt = 0;
		std::string line;
		while( std::getline(input, line) != 0 )
		{
			// '#'以降の削除
			std::string::size_type t = line.find('#');
			std::istringstream data((t != std::string::npos ? (line.substr(0,t).c_str()) : (line.c_str())));

			if( readData( data ) )
				cnt++;
		}
		return cnt;

	}

};


class StreamWrite
{
public:
	// ファイル書き込み
	virtual int write( std::ostream &output ) const = 0;

	int write( const char * fileName ) const
	{
		std::ofstream file;
		file.open( fileName );
		if( !file.is_open() )
		{
			std::cerr << "cannot open file [" << fileName << "]." << std::endl;
			return -1;
		}

		return write( file );
	}

};


#endif /* STREAM_ACCESS */
