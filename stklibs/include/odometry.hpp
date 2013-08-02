#ifndef ODOMETRY_HPP__
#define ODOMETRY_HPP__

#include <iostream>
#include <vector>
#include <ssm-log.hpp>

#include <ssmtype/spur-odometry.h>
#include "stkmath.hpp"
#include "stream-access.hpp"

inline Spur_Odometry diffSpurOdm( Spur_Odometry &end, Spur_Odometry &beg )
{
	double dx, dy, dth;
	double cosTh, sinTh;
	Spur_Odometry diff;

	dx = end.x - beg.x;
	dy = end.y - beg.y;
	dth = circularTheta( end.theta - beg.theta );

	cosTh = cos( dth );
	sinTh = sin( dth );

	diff.x = dx * cosTh - dy * sinTh;
	diff.y = dy * sinTh + dy * cosTh;
	diff.theta = dth;
	diff.v = end.v - beg.v;
	diff.w = end.w - beg.w;

	return diff;
}

/// 出力
inline std::ostream& operator<< (std::ostream &os, const Spur_Odometry &odm )
{
	os << odm.x << ' ' << odm.y << ' ' << odm.theta << ' ' << odm.v << ' ' << odm.w;
	return os;
}

/// 入力
inline std::istream& operator>> (std::istream &is, Spur_Odometry &odm )
{
	is >> odm.x >> odm.y >> odm.theta >> odm.v >> odm.w;
	return is;
}

struct OdometryData
{
	ssmTimeT time;
	Spur_Odometry odm;
};

/// 出力
inline std::ostream& operator<< (std::ostream &os, const OdometryData &odm )
{
	os << odm.time << ' ' << odm.odm;
	return os;
}

/// 入力
inline std::istream& operator>> (std::istream &is, OdometryData &odm )
{
	is >>odm.time >> odm.odm;
	return is;
}

class OdometryDataArray : public std::vector<OdometryData>, public StreamReadLine
{
	bool readData( std::istream &data )
	{
		OdometryData odm;
		data >> odm.time >> odm.odm.x >> odm.odm.y >> odm.odm.theta;
		if( data.bad(  ) )
			return false;
		data >> odm.odm.v >> odm.odm.w;
		this->push_back( odm );
		return true;
	}
public:
};

struct OdometryCorrectData : public OdometryData
{
	double dv, dw;
};

typedef std::vector<OdometryCorrectData> OdometryCorrectDataArray;

class OdometryCorrect
{
	void init()
	{
		data.reserve( 200 * 60 * 40 ); // オドメトリデータ 200Hz x 40分の領域を確保
	}
public:
	OdometryCorrect(){ init(  ); }


	OdometryCorrectDataArray data;

	/**
	 * @brief 読み込み
	 * @return 読み込んだデータの数
	 */
	int load( SSMLog<Spur_Odometry> &odm )
	{
		int cnt = 0;
		OdometryCorrectData o;
		o.dv = o.dw = 0;
		data.clear(  );

		while( odm.readNext(  ) )
		{
			o.odm = odm.data(  );
			o.time = odm.time(  );
			data.push_back( o );
			cnt++;
		}
		return cnt;
	}

	/**
	 * @brief 書き込み
	 * @return 書き込んだデータの数
	 */
	int save( SSMLog<Spur_Odometry> &odm )
	{
		int cnt = 0;
		OdometryCorrectDataArray::const_iterator it;
		Spur_Odometry o;

		for( it = data.begin(  ); it != data.end(  ); it++ )
		{
			o = it->odm;
			o.v += it->dv;
			o.w += it->dw;
			odm.data(  ) = o;
			if( !odm.write( it->time ) )
			{
				std::cerr<<"write err" << std::endl;
				break;
			}
			cnt++;
		}
		return cnt;
	}

	/**
	 * @brief 指定した位置に最も近いデータを探す(全探索)
	 */
	OdometryCorrectDataArray::iterator find( double ax, double ay, double aError )
	{
		if( data.size(  ) <= 0 )
			return data.end(  );

		OdometryCorrectDataArray::iterator it, sit;
		double d,dMin = INFINITY;
		double xx, yy;

		for( it = data.begin(); it != data.end(); it++ )
		{
			xx = ax - it->odm.x;
			yy = ay - it->odm.y;
			d = xx * xx + yy * yy;
			if( d < dMin )
			{
				sit = it;
				dMin = d;
			}
		}

		if( dMin <= aError * aError )
			return sit;
		return data.end(  );
	}

	/**
	 * @brief 指定した時刻に最も近いデータを探す
	 */
	OdometryCorrectDataArray::iterator find( const ssmTimeT &time )
	{
		if( data.size(  ) <= 0 )
			return data.end(  );

		size_t beg = 0, end = data.size(  ) - 1;
		ssmTimeT begTime = data.front(  ).time, endTime = data.back(  ).time;
		int d;
		ssmTimeT dTime = 0.0;

		if( time <= begTime )
			return data.begin(  );
		if( time >= endTime )
			return data.end(  ) - 1;

		double rate;
		while( beg != end )
		{
			rate = ( ( time - begTime ) / ( endTime - begTime ) );
			d = round( ( end - beg ) *  rate );
			dTime = data.at( beg + d ).time;
			if( d == 0 || beg + d == end )
			{
				beg += d;
				break;
			}

			if( dTime <= time )
			{
				beg = beg + d;
				begTime = data.at( beg ).time;
			}
			else
			{
				end = beg + d;
				endTime = data.at( end ).time;
			}
		}

		return data.begin(  ) + beg;
	}

	bool correctAngVel( OdometryCorrectDataArray::iterator &sit, OdometryCorrectDataArray::iterator &eit, double eth )
	{
		OdometryCorrectDataArray::iterator it;
		OdometryCorrectData odm;
		ssmTimeT dTime;
		odm = *eit;
		if( sit == eit )
			return false;

		do
		{
			double diffTh;

			diffTh = circularTheta( eth - eit->odm.theta );

			// 角速度に応じた重みを計算
			double rateMax = 0;
			odm = *sit;
			it = sit;
			do
			{
				it++;
				dTime = it->time - odm.time;
				rateMax += std::abs( it->odm.w ) * dTime;

				odm = *it;
			}
			while( it != eit );
			if( rateMax <= 0.0 )
				return false;
			//std::cerr << "rate " << rateMax << std::endl;
			odm = *sit;
			it = sit;
//			cerr << "dTime " << dTime << endl;
			do
			{
				it++;
				dTime = it->time - odm.time;
				//it->odm.v = it->odm.v;//そのまま
				it->dw += std::abs( it->odm.w ) * diffTh / rateMax;
				//it->odm.w += std::abs( it->odm.w ) * diffTh / rateMax;
				it->odm.x = odm.odm.x + ( it->odm.v + it->dv ) * cos( odm.odm.theta ) * dTime;
				it->odm.y = odm.odm.y + ( it->odm.v + it->dv ) * sin( odm.odm.theta ) * dTime;
				it->odm.theta = odm.odm.theta + ( it->odm.w + it->dw ) * dTime;

				odm = *it;
			}while( it != eit );
			//std::cerr << "ref " << eth << ", corr " << odm.odm.theta << std::endl;
		} while( std::abs( circularTheta( eth - eit->odm.theta ) ) > deg2rad( 1 ) );
		return true;
	}

	bool correctVel( OdometryCorrectDataArray::iterator &sit, OdometryCorrectDataArray::iterator &eit, double ex, double ey )
	{
		OdometryCorrectDataArray::iterator it;
		OdometryCorrectData odm;
		ssmTimeT dTime;
		double diffVecAng, diffVecLength;
//		do
		{
			double diffx, diffy;

			diffx = ex - eit->odm.x;
			diffy = ey - eit->odm.y;

			diffVecAng = std::atan2( diffy, diffx );
			diffVecLength = std::sqrt( std::pow( diffx, 2 ) + std::pow( diffy, 2 ) );
//			std::cerr << diffVecLength << std::endl;

			// 角度に応じた重みを計算
			double rateMax = 0;
			odm = *sit;
			it = sit;
			do
			{
				it++;
				dTime = it->time - odm.time;
				rateMax += std::abs( cos( diffVecAng - odm.odm.theta ) * it->odm.v ) * dTime;

				odm = *it;
			}
			while( it != eit );
			if( rateMax <= 0.0 )
				return false;

//			std::cerr << "rateMax" << rateMax <<  std::endl;
			// 重みに応じた分配
			odm = *sit;
			it = sit;
			do
			{
				it++;
				dTime = it->time - odm.time;
				//dTime = 0.005;
				//it->odm.v += cos( diffVecAng - odm.odm.theta ) * std::abs( it->odm.v ) * diffVecLength/ rateMax;
				//it->odm.w = it->odm.w;// そのまま
				it->dv += cos( diffVecAng - odm.odm.theta ) * std::abs( it->odm.v ) * diffVecLength/ rateMax;
				it->odm.x = odm.odm.x + ( it->odm.v + it->dv ) * cos( odm.odm.theta ) * dTime;
				it->odm.y = odm.odm.y + ( it->odm.v + it->dv ) * sin( odm.odm.theta ) * dTime;
				it->odm.theta = odm.odm.theta + ( it->odm.w + it->dw ) * dTime;

				odm = *it;
			}
			while( it != eit );

			//std::cerr << "pos:" << odm.odm.x << ", " << odm.odm.y << std::endl;
		}
//		while( diffVecLength > 0.1 );
		return true;
	}

	bool recalc( OdometryCorrectDataArray::iterator &eit )
	{
		OdometryCorrectDataArray::iterator it;
		OdometryCorrectData odm;
		ssmTimeT dTime;

		if( eit == data.end() )
			return false;
		odm = *eit;
		it = eit;

		while( ++it != data.end() )
		{
			dTime = it->time - odm.time;
			//it->odm.v += it->odm.v;//そのまま
			//it->odm.w = it->odm.w;// そのまま
			it->odm.x = odm.odm.x + ( it->odm.v + it->dv ) * cos( odm.odm.theta ) * dTime;
			it->odm.y = odm.odm.y + ( it->odm.v + it->dv ) * sin( odm.odm.theta ) * dTime;
			it->odm.theta = odm.odm.theta + ( it->odm.w + it->dw ) * dTime;

			odm = *it;
		}

		return true;
	}

	bool recalc(  )
	{
		OdometryCorrectDataArray::iterator it;
		it = find( 0.0 );
		return this->recalc( it );
	}

	bool correct( const ssmTimeT &startTime, const ssmTimeT &endTime, double ex, double ey, double eth )
	{
		OdometryCorrectDataArray::iterator sit, eit, it;
		//ssmTimeT diffTime;
		OdometryCorrectData odm;
		Spur_Odometry saveOdm;

		sit = find( startTime );
		eit = find( endTime );

		if( sit == data.end(  ) || eit == data.end(  ) )
			return false;

		if( eit - sit < 0 )
			return false;

		//diffTime = eit->time - sit->time;

		saveOdm = eit->odm;
		odm = *eit;

		if( eit != data.begin(  ) )
		{
			//--------------------------
			// 角速度の補正
			//std::cerr << "angvel" << std::endl;
			correctAngVel( sit, eit, eth );
			//--------------------------
			// 速度の補正
			//std::cerr << "vel" << std::endl;
			correctVel( sit, eit, ex, ey );

		}
		else
		{
			eit->odm.x = ex;
			eit->odm.y = ex;
			eit->odm.theta = eth;

		}

		//--------------------------
		// つながっている残りの部分の補正
		recalc( eit );
		/*
		double xx, yy, tx, ty, tth;
		double sinTh, cosTh;
		cosTh = cos( eit->odm.theta - saveOdm.theta );
		sinTh = sin( eit->odm.theta - saveOdm.theta );
		tx = eit->odm.x;
		ty = eit->odm.y;
		tth = eit->odm.theta - saveOdm.theta;

		it = eit + 1;
		while( it != data.end(  ) )
		{
			xx = it->odm.x - saveOdm.x;
			yy = it->odm.y - saveOdm.y;

			it->odm.x = xx * cosTh - yy * sinTh + tx;
			it->odm.y = xx * sinTh + yy * cosTh + ty;
			it->odm.theta += tth;

			it++;
		}
		*/
		return true;
	}

	void write( std::ostream &os )
	{
		OdometryCorrectDataArray::const_iterator it;
		for( it = data.begin(); it != data.end(); it++ )
		{
			os << it->time << ' ' << it->odm << std::endl;
		}
	}
};



#endif //ODOMETRY_HPP__
