#ifndef __TXTCONFIGURATOR_HPP_
#define __TXTCONFIGURATOR_HPP_

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>

/* **************************
  使い方:

  1 読み込むファイル指定してopen。
　　コンストラクタの引数指定でもいい。
  2 読み込みたいパラメータを第一引数にし、
　　その後の引数に代入したい変数を指定してset。
　　複数代入可。
  3 substituteする。
  4 closeは勝手にやってくれる。自分でやってもいい。

  *************************** */

class TxtConfigurator{
    enum{
        ALL_INT,
        ALL_FLOAT,
        ALL_DOUBLE,
        ALL_LONG,
        ALL_BOOL
    };

    typedef struct{
        const char *cp_name;
        std::vector<void *> values;
        int num;
        int type;
    } ConfigParam;

    std::ifstream ifs;
    const char *filename;
    bool isOpen;
    std::vector<ConfigParam> config_params;

public:
    TxtConfigurator();
    TxtConfigurator(const char *input);
    ~TxtConfigurator();

    void open(const char *input);
    void search_reset();

    void set(const char *key, int *val1);
    void set(const char *key, int *val1, int *val2);
    void set(const char *key, double *val1);
    void set(const char *key, double *val1, double *val2);
    void set(const char *key, long int *val1);
    void set(const char *key, bool *val1);
    void set(const char *key, float *val1);

    void set2(const char *key, int *val1, int *val2);
    void set2(const char *key, int *val1);
    void set2(const char *key, double *val1, double *val2);

    void substitute();
    void close();
};

#endif
