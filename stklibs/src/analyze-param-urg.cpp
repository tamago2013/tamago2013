/*
 * urg-param.cpp
 *
 *  Created on: 2010/07/14
 *      Author: shikina
 */
#include <string>
#include <map>
#include <cmath>
#include <iostream>

#include "../include/analyze-param.hpp"
#include "../include/analyze-param-urg.hpp"


using namespace std;

bool AnalyzeParamURG::analyzeLine(void *vparam, std::istream &data)
{
	AnalyzeParamURG *param = static_cast<AnalyzeParamURG *>(vparam);

	map<string,ParamURG>::iterator it = param->paramMap.find(param->lastKey);
	string mode;

	// 文字列の解析
	data >> ws >>mode;
	if(data.fail()){return false;}

	if(mode.compare("DEGREE") == 0){param->isDegree = true;}
	else if(mode.compare("RADIAN") == 0){param->isDegree = false;}
	else if(mode.compare("URG") == 0)
	{
		data >> param->lastKey;
		if(!data.fail())
		{
			ParamURG p;
			p.serial = param->lastKey;
			param->paramMap.insert(map<string,ParamURG>::value_type(p.serial, p));
			return true;
		}
	}
	else if(mode.compare("COMMENT") == 0)
	{
		if(it != param->paramMap.end())
		{
			// 残りはコメント
			getline(data,it->second.comment);
		}
	}
	else if(mode.compare("SENSORID") == 0)
	{
		int id;
		data >> id;
		if((it != (param->paramMap.end())) && (!data.fail()))
		{
			it->second.sensorId = id;
		}
	}
	else if(mode.compare("REFLECT") == 0)
	{
		string reflect;
		data >> reflect;
		if((it != (param->paramMap.end())) && reflect.compare("TRUE") == 0)
		{
			it->second.isReflect = true;
		}
	}
	else if(mode.compare("STEP") == 0)
	{
		string step;
		double ang1, ang2;
		int group, cull;
		if((it != (param->paramMap.end())))
		{
			data >> step;
			data >> ang1 >> ang2 >> group >> cull;
			if(!data.fail())
			{
				it->second.group = group;
				it->second.cull = cull;
				if(step.compare("SETANG") == 0)
				{
					if(param->isDegree)
					{
						ang1 *= M_PI / 180.0;
						ang2 *= M_PI / 180.0;
					}
					it->second.viewAng = ang1;
					it->second.middleAng = ang2;
					it->second.isSetAng = true;
					it->second.isSetStep = false;
				}
				else if(step.compare("SETSTEP") == 0)
				{
					it->second.minStep = ang1;
					it->second.maxStep = ang2;
					it->second.isSetStep = true;
					it->second.isSetAng = false;
				}
			}
			else{it->second.isSetAng = false; it->second.isSetStep = false;}
		}

	}
	else if(mode.compare("POSITION") == 0)
	{
		Coordinate3D trans ; // 移動量
		Coordinate3D rotate ; // 回転量
		data >> trans.x >> trans.y >> trans.z >> rotate.x >> rotate.y >> rotate.z;
		if(param->isDegree)
		{
			rotate.x *= M_PI / 180.0;
			rotate.y *= M_PI / 180.0;
			rotate.z *= M_PI / 180.0;
		}

		if((it != (param->paramMap.end())) && (!data.fail()))
		{
			it->second.trans = trans;
			it->second.rotate = rotate;
		}
	}

	/*
	{
		ParamURG p;
		data >> p.serial >> p.trans.x >> p.trans.y >> p.trans.z >> p.rotate.x >> p.rotate.y >> p.rotate.z;
		if(!data.fail())
		{
			// deg -> rad
			if(param->isDegree)
			{
				p.rotate.x *= M_PI / 180.0;
				p.rotate.y *= M_PI / 180.0;
				p.rotate.z *= M_PI / 180.0;
			}
			// 残りはコメント
			getline(data,p.comment);

			//登録
			param->paramMap.insert(map<string,ParamURG>::value_type(p.serial, p));
			return true;
		}
	}*/
	return false;
}

int AnalyzeParamURG::load(const char *fileName)
{
	return paramAnalyze(this, fileName, analyzeLine);
}

int AnalyzeParamURG::getSensorID(std::string serial)
{
	map<string,ParamURG>::iterator it = paramMap.find(serial);
	if(it == paramMap.end())
	{
		std::cerr << "WARNING: cannot find SERIAL '" << serial << "' and set ID to " << UNKNOWN_URG_SENSOR_ID << endl;
		return UNKNOWN_URG_SENSOR_ID;
	}

	std::cerr << endl <<"sensor ID = " << it->second.sensorId << endl;
	std::cerr << it->second.comment << endl << endl;

	return 	it->second.sensorId;
}

ParamURG AnalyzeParamURG::getParam(std::string serial)
{
	map<string,ParamURG>::iterator it = paramMap.find(serial);
	if(it == paramMap.end())
	{
		std::cerr << "WARNING: cannot find SERIAL '" << serial << "'." << endl;

		ParamURG p;
		p.serial = serial;
		p.comment = "unknown";
		p.sensorId;
		p.trans.x = p.trans.y = p.trans.z = 0 ;
		p.rotate.x = p.rotate.y = p.rotate.z = 0 ;
		return p;
	}
	return it->second;
}
