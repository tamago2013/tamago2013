/*

	SSM Adjuster Base 2D
	二次元のアジャストの基本クラス

	2012/10/11	MTM

*/

#ifndef MTM_ADJUSTER_BASE_2D
#define MTM_ADJUSTER_BASE_2D

#include<iostream>
#include<vector>
#include"Points_and_Lines.hpp"

namespace mtm{

	//2次元のアジャスト結果
	typedef struct{
		double x;
		double y;
		double theta;
	}Adjust2D;

	//２次元のアジャスト結果(評価つき)
	typedef struct{
		double x;
		double y;
		double theta;
		double value;
	}Adjust2D_withValue;


	class AdjusterBase2D{
		public:
			//コンストラクタ、デストラクターーーーーーーーーーーーーーーーー
			AdjusterBase2D(void){
				ResetPoints();
			};
			~AdjusterBase2D(void){
			};
			//点の入出力ーーーーーーーーーーーーーーーーーーーーーーーー
			void ResetPoints(void){			//urgから取得した点を0に
				points.resize(0);
			}
			int GetPointSize(void){	//サイズを返す
				return points.size();
			}
			void GetPoints(std::vector<PointXY>& input){
				input = points;
			}
			void SetPointSize(int size){		//サイズをセット
				if(size < 0)size = 0;
				points.resize(size);
			}
			void PushBackPoints(double urg_x, double urg_y){			//点を1つ追加
				mtm::PointXY point;
				point.x = urg_x;
				point.y = urg_y;
				points.push_back(point);
			}
			void SetPoint(int num, double urg_x, double urg_y){		//指定した要素番号に点を入力　指定された要素番号に点がなければ、push_backして点を追加
				if(num < 0 || num >= points.size())num = -1;
				if(num == -1)PushBackPoints(urg_x,urg_y);
				else{
					points[num].x = urg_x;
					points[num].y = urg_y;
				}
			}
			void AllPointsFS2GL(double Odom_x, double Odom_y, double Odom_th){		//全ての点をFS座標からGL座標に変える
				if(points.size() >= 1){
					for(int i=0;i<points.size();i++)mtm::FS2GL(&points[i],Odom_x,Odom_y,Odom_th);
				}
			}
		protected:
			std::vector<mtm::PointXY> 	points;
	};

}

#endif
