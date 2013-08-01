/*
 * analyze-param.hpp
 *
 *  Created on: 2010/07/14
 *      Author: shikina
 */

#ifndef ANALYZEPARAM_HPP_
#define ANALYZEPARAM_HPP_

#include <iostream>


/**
 * @brief パラメータファイルの解析
 * streamから１行読み込んでanalyzeLine(istream)に渡すプログラム
 * @param fileName[in] パラメータファイル名
 * @param (*analyzeLine)[in] 解析プログラム
 * @return 読み込めたパラメータの数
 */
int paramAnalyze(void *param, std::istream &input, bool (*analyzeLine)(void *, std::istream &));

/**
 * @brief パラメータファイルの解析
 * ファイルから１行読み込んでanalyzeLine(istream)に渡すプログラム
 * @param fileName[in] パラメータファイル名
 * @param (*analyzeLine)[in] 解析プログラム
 * @return 読み込めたパラメータの数
 */
int paramAnalyze(void *param, const char *fileName, bool (*analyzeLine)(void *, std::istream &));

#endif /* ANALYZEPARAM_HPP_ */
