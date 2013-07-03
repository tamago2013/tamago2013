/*

	SSM Type
	SSMの名前ここで定義しておく
	
	2012/10/11	MTM

*/

#ifndef MTM_SSM_TYPE
#define MTM_SSM_TYPE

#include"ssm-laser.hpp"

//Line_GL=============================================================
//MTM Line_Data(自作判定された線分の始点・終点データ(GL座標系))
#define SNAME_MTM_LINE "Line_Data"
#define SID_MTM_LINE 2
#define SID_MTM_GRAND_EDGE 5
//直線データ型の定義
typedef struct{
	int numpoints;
	int points_on_the_line[100];		//その直線上に乗った点の数
	double start_x[100];			//その直線の始点x座標
	double start_y[100];			//その直線の始点y座標
	double finish_x[100];		//その直線の終点x座標
	double finish_y[100];		//その直線の終点y座標
}MTM_Line;

//Pinfo(YSD)==========================================================
#define SNAME_YSD_PINFO "Pinfo"
#define SID_YSD_PINFO 1
typedef struct{
	int route;
	int waypoint;
}YSD_Pinfo;

//MapChangerの流すMap番号===============================================
#define SNAME_MTM_MINFO "Minfo"
#define SID_MTM_MINFO_LINE 0
#define SID_MTM_MINFO_EDGE 1
#define MTM_MINFO_CHAR_SIZE 300
typedef struct{
	char mapname[MTM_MINFO_CHAR_SIZE];		//マップのフルパス（最大200文字）
	int mapnum;										//マップ番号
}MTM_Minfo;

//Adjusterが流すそれぞれのアジャスト情報======================================
#define SNAME_MTM_ADJUST "MTM_Adjust"
#define SID_MTM_ADJUST_LINE 1
#define SID_MTM_ADJUST_GRAND_EDGE 2
typedef struct{
	double x;
	double y;
	double theta;
	bool valid;		//有効かどうか(アジャスト失敗時はこれをtrueに)
}MTM_Adjust;
//YSDのGrandEdgeDetectorの情報==========================================
#define SNAME_YSD_GRAND_EDGE SSM_NAME_SOKUIKI_3D_FS_EDGE
#define SID_YSD_GRAND_EDGE 1
/*
構造体はSSM_SOKUIKI_3D
*/

/*
//GrandEdgeGetterが流す蓄積されたEdge情報=================================
//YSD_GRAND_EDGEがあるのでおそらくデバッグ用
#define SNAME_MTM_GRAND_EDGE "Heaped_Grand_Edge"
#define SID_MTM_GRAND_EDGE 2
#define MTM_GRAND_EDGE_SIZE 500
typedef struct{
	int numpoints;
	float edge_x_gl[MTM_GRAND_EDGE_SIZE];
	float edge_y_gl[MTM_GRAND_EDGE_SIZE];	
}MTM_GrandEdge;

//GrandEdgeGetterが流す蓄積されたEdge情報=================================
//YSD_GRAND_EDGEがあるのでおそらくデバッグ用
#define SNAME_MTM_GRAND_EDGE_LINE "Heaped_Grand_Edge_Line"
#define SID_MTM_GRAND_EDGE_LINE 2
typedef struct{
	int numpoints;
	double start_x[100];			//その直線の始点x座標
	double start_y[100];			//その直線の始点y座標
	double finish_x[100];		//その直線の終点x座標
	double finish_y[100];		//その直線の終点y座標
}MTM_GrandEdgeLine;
*/
#endif
