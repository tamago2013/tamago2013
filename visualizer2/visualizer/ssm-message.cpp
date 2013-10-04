#include <cstdio>
#include <ssm.hpp>
#include "widget-msg.hpp"
#include "ssm-message.hpp"
#include "tkg-utility.hpp"

int sm_state = 0;
WidgetMSG *w_msg = NULL;

void logger(const std::string &str)
{
    if(w_msg) w_msg->add_message(str);
}

void smTarget(WidgetMSG *msg)
{
    w_msg = msg;
}

void smInit()
{
    if(sm_state != 0) return;
    sm_state = initSSM();
    if(sm_state) { logger("SSMへの接続に成功しました。\n"); }
    else         { logger("SSMへの接続に失敗しました。\n"); }
}

void smEnd()
{
    if(sm_state == 0) return;
    sm_state = endSSM();

    /*
    for(int i=0; i<ssm_count; i++)
    {
        if(ssm[i]->isOpen())
        {
            ssm[i]->close();
        }
        delete ssm[i];
    }
    ssm_state = endSSM();
    */
}

void smConnect(SSMApiBase *api)
{
    if(sm_state == 0) return;
    if(api->isOpen()) return;

    if(api->open()) { logger( tkg::strf("%s [%d] へ接続成功。\n", api->getStreamName(), api->getStreamId()) ); }
    else            { logger( tkg::strf("%s [%d] へ接続失敗。\n", api->getStreamName(), api->getStreamId()) ); }
}

bool smState(SSMApiBase *api)
{
    if(sm_state == 0) return false;
    return api->isOpen();
}

void smReadLast(SSMApiBase *api)
{
    if(sm_state == 0) return;
    if(api->isOpen()) { api->readLast(); }
}

void smReadTime(SSMApiBase *api, double time)
{
    if(sm_state == 0) return;
    if(api->isOpen()) { api->readTime(time); }
}
