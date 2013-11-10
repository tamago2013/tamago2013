//============================================================================
// Name        : TxtConfigurator.cpp
// Author      : Shun Kawamoto (KAW)
// Version     : 0.10
// Copyright   : Your copyright notice
// Description :　テキスト設定ファイル用クラス
//============================================================================

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

#include "TxtConfigurator.hpp"

using namespace std;

void TxtConfigurator::search_reset(){
    ifs.clear();
    ifs.seekg(0);
}

TxtConfigurator::TxtConfigurator(){
    isOpen=false;
}

TxtConfigurator::TxtConfigurator(const char *input){
    this->open(input);
}

TxtConfigurator::~TxtConfigurator(){
    if(isOpen)this->close();
}

void TxtConfigurator::open(const char *input){
    filename = input;
    ifs.open(filename);
    if(!ifs){
        cout << "Cannot open " << filename << endl;
        isOpen = false;
    }else{
        cout << "Found " << filename << endl;
        isOpen = true;
    }
}

void TxtConfigurator::set(const char *key, int *val1){
    ConfigParam cp;

    cp.cp_name = key;
    cp.values.push_back(val1);
    cp.num = 1;
    cp.type = ALL_INT;
    config_params.push_back(cp);
    //this->substitute();

    //cout << "set : " << key << endl;
}

void TxtConfigurator::set(const char *key, int *val1, int *val2){
    ConfigParam cp;

    cp.cp_name = key;
    cp.values.push_back(val1);
    cp.values.push_back(val2);
    cp.num = 2;
    cp.type = ALL_INT;
    config_params.push_back(cp);
    //this->substitute();

    //cout << "set : " << key << endl;
}

void TxtConfigurator::set(const char *key, double *val1){
    ConfigParam cp;

    cp.cp_name = key;
    cp.values.push_back(val1);
    cp.num = 1;
    cp.type = ALL_DOUBLE;
    config_params.push_back(cp);
    //this->substitute();

    //cout << "set : " << key << endl;
}

void TxtConfigurator::set(const char *key, double *val1, double *val2){
    ConfigParam cp;

    cp.cp_name = key;
    cp.values.push_back(val1);
    cp.values.push_back(val2);
    cp.num = 2;
    cp.type = ALL_DOUBLE;
    config_params.push_back(cp);
    //this->substitute();

    //cout << "set : " << key << endl;
}

void TxtConfigurator::set(const char *key, float *val1){
    ConfigParam cp;

    cp.cp_name = key;
    cp.values.push_back(val1);
    cp.num = 1;
    cp.type = ALL_FLOAT;
    config_params.push_back(cp);
    //this->substitute();

    //cout << "set : " << key << endl;
}

void TxtConfigurator::set(const char *key, long int *val1){
    ConfigParam cp;

    cp.cp_name = key;
    cp.values.push_back(val1);
    cp.num = 1;
    cp.type = ALL_LONG;
    config_params.push_back(cp);
    //this->substitute();

    //cout << "set : " << key << endl;
}

void TxtConfigurator::set(const char *key, bool *val1){
    ConfigParam cp;

    cp.cp_name = key;
    cp.values.push_back(val1);
    cp.num = 1;
    cp.type = ALL_BOOL;
    config_params.push_back(cp);
    //this->substitute();

    //cout << "set : " << key << endl;
}

// set2

void TxtConfigurator::set2(const char *key, int *val1, int *val2){
    ConfigParam cp;

    cp.cp_name = key;
    cp.values.push_back(val1);
    cp.values.push_back(val2);
    cp.num = 2;
    cp.type = ALL_INT;
    config_params.push_back(cp);
    this->substitute();

    search_reset();
}

void TxtConfigurator::set2(const char *key, int *val1){
    ConfigParam cp;

    cp.cp_name = key;
    cp.values.push_back(val1);
    cp.num = 1;
    cp.type = ALL_INT;
    config_params.push_back(cp);
    this->substitute();

    search_reset();
}


void TxtConfigurator::set2(const char *key, double *val1, double *val2){
    ConfigParam cp;

    cp.cp_name = key;
    cp.values.push_back(val1);
    cp.values.push_back(val2);
    cp.num = 2;
    cp.type = ALL_DOUBLE;
    config_params.push_back(cp);
    this->substitute();

    search_reset();
}

void TxtConfigurator::substitute(){
    string str;

    for(ifs >> str;!ifs.eof();ifs >> str){
        for(int i = 0; i < (int)config_params.size(); i++){
            if(str == config_params[i].cp_name){
                switch(config_params[i].type){
                case ALL_INT:
                    for(int k = 0; k < config_params[i].num; k++){
                        int *tmp;

                        ifs >> str;
                        tmp = (int *)config_params[i].values[k];
                        *tmp = atoi(str.c_str());
                    }
                    break;
                case ALL_FLOAT:
                    for(int k = 0; k < config_params[i].num; k++){
                        float *tmp;

                        ifs >> str;
                        tmp = (float *)config_params[i].values[k];
                        *tmp = (float) atof(str.c_str());
                    }
                    break;
                case ALL_DOUBLE:
                    for(int k = 0; k < config_params[i].num; k++){
                        double *tmp;

                        ifs >> str;
                        tmp = (double *)config_params[i].values[k];
                        *tmp = atof(str.c_str());
                    }
                    break;
                case ALL_LONG:
                    for(int k = 0; k < config_params[i].num; k++){
                        long *tmp;

                        ifs >> str;
                        tmp = (long *)config_params[i].values[k];
                        *tmp = atol(str.c_str());
                    }
                    break;
                case ALL_BOOL:
                    for(int k = 0; k < config_params[i].num; k++){
                        int t;
                        bool *tmp;

                        ifs >> str;
                        tmp = (bool *)config_params[i].values[k];
                        t = atoi(str.c_str());
                        if(t){
                            *tmp=true;
                        }else{
                            *tmp =false;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
    //config_params.clear();
}

void TxtConfigurator::close(){
    if(isOpen){
        ifs.close();
        cout << "Closed " << filename << endl;
        isOpen = false;
    }else{
        cout << "No file opened" << endl;
    }
}

