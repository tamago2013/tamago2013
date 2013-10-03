//
//ssm-cluster.hpp
//
//YSD 2013/9/30
//

#include <ssm.hpp>

#define SNAME_CLUSTER "cluster"
#define NUM_CLUSTER 32

namespace ysd
{	//---> name space ysd

class _rect //要素
{
public:
    double x_g, y_g;    //重心(GL)
    double x1, y1;  //角(GL)
    double x2, y2;  //角(GL)

    _rect()
    {
        this->x_g = 0.0;
        this->y_g = 0.0;
        this->x1 = 0.0;
        this->y1 = 0.0;
        this->x2 = 0.0;
        this->y2 = 0.0;
    }

    void set(double x_g, double y_g, double x1, double y1, double x2, double y2)
    {
        this->x_g = x_g;
        this->y_g = y_g;
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
    }
};

class cluster   //ssmに流れるやつ
{
private:
    int _num_clusters;

public:
    _rect rect[NUM_CLUSTER];

    int num_clusters(void)
    {
        return _num_clusters;
    }

    void set_num_clusters(int num_clusters)
    {
        if(num_clusters > num_clusters)
        {
            fprintf(stderr, "[ERROR]: %d コ以上は入んねえよ!!\n", NUM_CLUSTER);
        }
        else
        {
            this->_num_clusters = num_clusters;
        }
    }

};

}	//<--- name space ysd
