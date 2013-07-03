/*点と直線のデータ型とそれらに関する演算のまとめ	*/
/*2011/9/19 MTM							*/

#ifndef MTM_POINTS_AND_LINES
#define MTM_POINTS_AND_LINES

#include <iostream>
#include <math.h>
#include "DegRad.hpp"

namespace mtm{

	//構造体の定義=================================================================================================
	//点データ
	typedef struct{
		double x;
		double y;
	}PointXY;

	//直線データ
	typedef struct{
		PointXY start;
		PointXY finish;
	}LineXY;

	//基本========================================================================================================
	//Point2つからLineを作成
	void MakeLine(LineXY *result, PointXY point1, PointXY point2){
		result->start.x = point1.x;
		result->start.y = point1.y;
		result->finish.x = point2.x;
		result->finish.y = point2.y;
	}
	//Lineの中心点を取得
	void GetCenter(PointXY *result, LineXY line){
		result->x = (line.start.x + line.finish.x)/2;
		result->y = (line.start.y + line.finish.y)/2;	
	}
	//ポイントをコピー
	void CopyPoint(PointXY *result, PointXY point){
		result->x = point.x;
		result->y = point.y;
	}
	//ポイントの和
	void AddPoint(PointXY *result, PointXY point1, PointXY point2){
		result->x = point1.x + point2.x;
		result->y = point1.y + point2.y;
	}
	//ラインをコピー
	void CopyLine(LineXY *result, LineXY line){
		CopyPoint(&result->start,line.start);
		CopyPoint(&result->finish,line.finish);
	}
	//PointXYを表示
	void DispPoint(PointXY point){
		std::cout << "(x,y) = (" << point.x << "," << point.y << ")\n";  
	}
	//LineXYを表示
	void DispLine(LineXY line){
		std::cout << "Start  Point";	DispPoint(line.start);
		std::cout << "Finish Point";	DispPoint(line.finish);
	}

	//ゼロ点の作成
	void Zero(PointXY *point){
		point->x = 0.0;
		point->y = 0.0;
	}
	//始点・終点共にゼロの直線の作成
	void ZeroLine(LineXY *line){
		Zero(&line->start);
		Zero(&line->finish);
	}
	//移動回転===========================================================================================================
	//0,0中心の回転
	void ZeroRotate(PointXY *point, double rad){
		double x = point->x * cos(rad) - point->y * sin(rad);
		double y = point->x * sin(rad) + point->y * cos(rad);
		point->x = x;
		point->y = y;
	}
	//点の平行移動
	void Trans(PointXY *point, PointXY trans){
		point->x = point->x + trans.x;
		point->y = point->y + trans.y;
	}
	//回転中心を指定した点の回転
	void Rotate(PointXY *point, PointXY center, double rad){
		PointXY minus_center;
		minus_center.x = -center.x;
		minus_center.y = -center.y;
		Trans(point,minus_center);
		ZeroRotate(point,rad);
		Trans(point,center);
	}
	//直線の平行移動
	void LineTrans(LineXY *line, PointXY trans){
		Trans(&line->start,trans);
		Trans(&line->finish,trans);
	}
	//直線の中心指定回転
	void LineRotate(LineXY *line, PointXY center, double rad){
		Rotate(&line->start,center,rad);
		Rotate(&line->finish,center,rad);
	}

	//distanceの計算====================================================================================================
	//点と点の距離を計算
	double Distance(PointXY point1, PointXY point2){
		return sqrt((point1.x-point2.x)*(point1.x-point2.x)+(point1.y-point2.y)*(point1.y-point2.y));
	}
	//直線の長さの計算
	double Length(LineXY line){
		return Distance(line.start,line.finish);
	}

	//FS-GLの変換========================================================================================================
	//FS→GL
	void FS2GL(PointXY *point, double odomGL_x, double odomGL_y, double odomGL_th){
		double theta = atan2(point->y,point->x);
		double radius = sqrt(point->x * point->x + point->y * point->y);
		point->x = radius * cos(theta + odomGL_th) + odomGL_x;
		point->y = radius * sin(theta + odomGL_th) + odomGL_y;
	}
	void LineFS2GL(LineXY *line, double odomGL_x, double odomGL_y, double odomGL_th){
		FS2GL(&line->start, odomGL_x, odomGL_y, odomGL_th);
		FS2GL(&line->finish, odomGL_x, odomGL_y, odomGL_th);
	}
	//GL→FS
	void GL2FS(PointXY *point, double odomGL_x, double odomGL_y, double odomGL_th){
		double theta = atan2((point->y - odomGL_y),(point->x - odomGL_x));
		double radius = sqrt((point->x - odomGL_x)*(point->x - odomGL_x)+(point->y - odomGL_y)*(point->y - odomGL_y));
		point->x = radius * cos(theta - odomGL_th);
		point->y = radius * sin(theta - odomGL_th);
	}
	void LineGL2FS(LineXY *line, double odomGL_x, double odomGL_y, double odomGL_th){
		GL2FS(&line->start, odomGL_x, odomGL_y, odomGL_th);
		GL2FS(&line->finish, odomGL_x, odomGL_y, odomGL_th);
	}
	//Ax+By+C=0の係数の計算================================================================================================
	//傾きθ
	double LineTheta(LineXY line){
		if(fabs(line.start.x - line.finish.x) < 0.0001){
			if(line.start.y - line.finish.y < 0.0)return RAD(-90.0);
			else return RAD(90.0);
		}
		else return atan((line.start.y - line.finish.y)/(line.start.x - line.finish.x));
	}
	//係数A
	double LineA(LineXY line){
		return sin(LineTheta(line));
	}
	//係数B
	double LineB(LineXY line){
		return -cos(LineTheta(line));
	}
	//係数C
	double LineC(LineXY line){
		return - line.start.y * LineB(line) - line.start.x * LineA(line);
	}

	//垂直二等分線を返す(resultにはinputと同じ長さの直線が代入される)========================================================
	void GetPerpendicularBisector(LineXY *result, LineXY input){
		double theta = LineTheta(input);
		double center_x = (input.start.x + input.finish.x)/2;
		double center_y = (input.start.y + input.finish.y)/2;
		double length = Distance(input.start,input.finish);
		result->start.x = center_x + length / 2 * cos(theta + RAD(90.0));
		result->start.y = center_y + length / 2 * sin(theta + RAD(90.0));
		result->finish.x = center_x + length / 2 * cos(theta + RAD(-90.0));
		result->finish.y = center_y + length / 2 * sin(theta + RAD(-90.0));
	}

	//点と直線の距離を返す===================================================================================================
	//d = |a x0 + b y0 + c|で表される点(x0,y0)と線ax+by+c=0の距離dを返す
	double GetPointLineDistance(PointXY point, LineXY line){
		return fabs(point.x * LineA(line) + point.y * LineB(line) + LineC(line));
	}
	//点XYからLineに向かって、lineに垂直に伸びるベクトルの成分を返す
	void GetPointLineVector(double *x, double *y, PointXY point, LineXY line){
		double a,b,c;
		a = LineA(line);
		b = LineB(line);
		c = LineC(line);
		if(a == 0.0 && b == 0.0 && c == 0.0){
			*x = 0.0;
			*y = point.y;
		}
		else{
			*x = -1/(a*a + b*b)*(a*a*point.x + a*b*point.y + a*c);
			*y = -1/(a*a + b*b)*(a*b*point.x + b*b*point.y + b*c);
		}
	}


	//直線AとBの角度の相関を計算==============================================================================================
	double LineAngleDifference(LineXY line1, LineXY line2){	//直線1の角度-直線2の角度を返す。ただし直線なので180度回したら同じ直線になることを考え、角度の差の絶対値が小さくなる向きで返す。
		double theta1 = LineTheta(line1);
		double theta2 = LineTheta(line2);
		if(fabs(theta1 - theta2) <= RAD(90.0))return theta1 - theta2;
		else{
			if(theta1 - theta2 >= 0)return (theta1 - theta2) - RAD(180.0);
			else return RAD(180.0) + (theta1 - theta2);
		}	
	}

	//点と直線の関係を計算===================================================================================================
	bool PointOnLineSegment(LineXY line, PointXY point){		//「直線」line上にある点が、「線分」line上に乗っているか調べる
		if(Length(line) <= Distance(point,line.start))return false;
		if(Length(line) <= Distance(point,line.finish))return false;
		return true;
	}
	bool PointOnWideLine(LineXY line, PointXY point, double width){	//線分lineを幅wにした長方形上に、点point乗るかを調べる
		PointXY center;
		GetCenter(&center,line);
		if(Distance(center,point) <= Length(line) / 2){				//長方形ではなく、線分の長さ方向の制限を円で近似したほうが早いっぽい
			if(GetPointLineDistance(point,line) <= width / 2){
				return true;
			}
		}
		return false;	
	}
	//そのほか=============================================================================================================
	bool LineInCircle(LineXY line, PointXY center, double radius){	//線分lineが一部分でも、centerを中心に半径radiusの円の領域内に入っていればtrue,そうでなければfalse
		double x,y;
		GetPointLineVector(&x,&y,center,line);
		if(radius <= sqrt(x*x + y*y))return false;							//中心点と直線の距離が半径以上なら、どうやってもその線分と円は交わらない
		if(radius >= Distance(center,line.start ))return true;		//線分の始点または終点が円の中に入っていれば、確実にその線分は円の中に入っている
		if(radius >= Distance(center,line.finish))return true;
		PointXY point;
		point.x = center.x + x;
		point.y = center.y + y;														//中心点からlineにおろした垂線の交点がpointとなる
		if(PointOnLineSegment(line,point))return true;					//pointがline上に乗っていれば、lineは円をまたぐような格好で円と交わる
		return false;																		//そうでなければ、lineは直線であれば円と交わるが、線分であるため交わらないような位置にいる
	}
	//線分の当たり判定(tempを傾きとは垂直な向きに平行移動させて、targに重なるかどうか)
	bool HitLine(LineXY& temp, LineXY& targ){
		//(1)tempの両端から伸びるtempに垂直な直線を張る
		double theta = LineTheta(temp);
		LineXY l1, l2;
		l1.start.x  = temp.start.x  + 1.0 * cos(theta + RAD(90.0));	//l1：tempに垂直な、temp.startを通る線
		l1.start.y  = temp.start.y  + 1.0 * sin(theta + RAD(90.0));
		l1.finish.x = temp.start.x  + 1.0 * cos(theta - RAD(90.0));
		l1.finish.y = temp.start.y  + 1.0 * sin(theta - RAD(90.0));
		l2.start.x  = temp.finish.x + 1.0 * cos(theta + RAD(90.0));	//l2:tempに垂直な、temp.finishを通る線
		l2.start.y  = temp.finish.y + 1.0 * sin(theta + RAD(90.0));
		l2.finish.x = temp.finish.x + 1.0 * cos(theta - RAD(90.0));
		l2.finish.y = temp.finish.y + 1.0 * sin(theta - RAD(90.0));
		//(2)targの両端が、この２本の直線のどちら側にいるかを調べる
		double ds1 = GetPointLineDistance(targ.start,l1);		//targ.startとl1の「点と直線の距離」を取得
		double df1 = GetPointLineDistance(targ.finish,l1);		//targ.finishとl1の「点と直線の距離」を取得
		double ds2 = GetPointLineDistance(targ.start,l2);		//targ.startとl2の「点と直線の距離」を取得
		double df2 = GetPointLineDistance(targ.finish,l2);		//targ.finishとl2の「点と直線の距離」を取得
		int patern_ts, patern_tf;											//tempのstartとfinishがどこにいるか？→「-1」：l1よりも手前側にいる　「0」:l1とl2に挟まれている　「1」：l2よりも奥にいる
		double length = Length(temp);										//tempの長さ
		if(ds1 <= length && ds2 <= length)patern_ts = 0;		//targ.startについて、l1とl2との「点と直線の距離」がtempの長さ以下だったら、l1とl2に挟まれている
		else if(ds1 > ds2)patern_ts = 1;								//それ以外で、l1との距離のほうが、l2との距離よりも長ければ、l2よりも奥にいる
		else patern_ts = -1;													//それ以外なら、l1よりも手前にいる
		if(df1 <= length && df2 <= length)patern_tf = 0;
		else if(df1 > df2)patern_tf = 1;
		else patern_tf = -1;
		if(patern_ts == 0 || patern_tf == 0)return true;		//targ線分の両端のどっちかが、l1とl2に挟まれていればOK
		if(patern_ts == -1 && patern_tf == 1)return true;		//どっちも挟まれていないとき→片方がl1より手前、もう一方がl2より奥にいるならOK
		if(patern_ts == 1 && patern_tf == -1)return true;
		return false;
	}

}
#endif

