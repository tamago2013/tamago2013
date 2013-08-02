/*
 * capture.hpp
 *
 *  Created on: 2011/12/02
 *      Author: shikina
 */

#ifndef CAPTURE_HPP_
#define CAPTURE_HPP_

#include <string>
bool captureInit( std::string fileName, int fps );

bool captureWriteFrame();

#endif /* CAPTURE_HPP_ */
