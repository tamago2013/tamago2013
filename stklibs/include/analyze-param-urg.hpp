/*
 * analyze-param-urg.hpp
 *
 *  Created on: 2010/07/14
 *      Author: shikina
 */

#ifndef ANALYZEPARAMURG_HPP_
#define ANALYZEPARAMURG_HPP_

#include <iostream>
#include <string>
#include <map>

#include "analyze-param.hpp"

static const int UNKNOWN_URG_SENSOR_ID = 9;

#define var2string(var) #var

// 3次元座標系クラス
class Coordinate3D
{
public:
	double x,y,z;
	Coordinate3D(){}
	Coordinate3D(double x,double y, double z):x(x),y(y),z(z){}
};

class ParamURG
{
	void init()
	{
		sensorId = 0;
		isReflect = false;
		isSetAng = false;
		isSetStep = false;
		group = 1;
		cull = 0;
	}
public:
	std::string serial, comment;
	int sensorId;
	bool isReflect, isSetAng, isSetStep;
	double viewAng, middleAng;
	int group, cull;
	int minStep, maxStep;
	ParamURG(){init();}
	Coordinate3D trans ; // @param 移動量
	Coordinate3D rotate ; // @param 回転量
};

class AnalyzeParamURG
{
	void init()
	{
		isDegree = false;
	}
	static bool analyzeLine(void *param, std::istream &data);
public :
	std::map<std::string, ParamURG> paramMap;
	bool isDegree;
	std::string lastKey;
	AnalyzeParamURG(){init();}

	void clear(){paramMap.clear();}
	int load(const char *fileName);
	int getSensorID(std::string serial);
	ParamURG getParam(std::string serial);
};

#endif /* ANALYZEPARAMURG_HPP_ */
