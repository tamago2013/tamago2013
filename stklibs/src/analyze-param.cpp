/*
 * param.cpp
 *
 *  Created on: 2010/07/13
 *      Author: shikina
 */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cerrno>


#include "../include/analyze-param.hpp"

using namespace std;

/**
 * @brief パラメータファイルの解析
 * streamから１行読み込んでanalyzeLine(istream)に渡すプログラム
 * @param fileName[in] パラメータファイル名
 * @param (*analyzeLine)[in] 解析プログラム
 * @return 読み込めたパラメータの数
 */
int paramAnalyze(void *param, std::istream &input, bool (*analyzeLine)(void * ,std::istream &))
{
	int cnt = 0;
	string line;
	while( getline(input, line) != 0 )
	{
		// '#'以降の削除
		string::size_type t = line.find('#');
		istringstream data((t != string::npos ? (line.substr(0,t).c_str()) : (line.c_str())));

		if( analyzeLine(param, data) ){cnt++;}
	}
	return cnt;
}

/**
 * @brief パラメータファイルの解析
 * ファイルから１行読み込んでanalyzeLine(istream)に渡すプログラム
 * @param fileName[in] パラメータファイル名
 * @param (*analyzeLine)[in] 解析プログラム
 * @return 読み込めたパラメータの数
 */
int paramAnalyze(void *param, const char *fileName, bool (*analyzeLine)(void *, std::istream &))
{
	ifstream file;
	file.open(fileName);

	if(file.is_open())
	{
		return paramAnalyze(param, file, analyzeLine);
	}
	else
	{
		string str("paramAnalyze() : cannot open file : ");
		str += fileName;
		throw runtime_error(str);
	}
	return 0;
}
