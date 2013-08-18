#include <cstdio>
#include <cstdarg>
#include <ssm.hpp>
#include "widget-msg.hpp"
#include "ssm-message.hpp"

int sr_state = 0;
WidgetMSG *w_msg = NULL;

void logger(const char* format, ...)
{
    if(!w_msg) return;

    va_list arg;
    char str[256];

    va_start(arg, format);
    vsprintf(str, format, arg);
    w_msg->add_message(str);
    va_end(arg);
}

void smTarget(WidgetMSG *msg)
{
    w_msg = msg;
}

void smInit()
{
    if(sr_state != 0) return;
    sr_state = initSSM();
    if(sr_state) { logger("SSMへの接続に成功しました。\n"); }
    else         { logger("SSMへの接続に失敗しました。\n"); }
}

void smEnd()
{
    if(sr_state == 0) return;
    sr_state = endSSM();

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
    if(sr_state == 0) return;
    if(api->isOpen()) return;

    if(api->open()) { logger("%s [%d] へ接続成功。\n", api->getStreamName(), api->getStreamId()); }
    else            { logger("%s [%d] へ接続失敗。\n", api->getStreamName(), api->getStreamId()); }
}

void smReadNew(SSMApiBase *api)
{
    if(sr_state == 0) return;
    if(api->isOpen()) { api->readNew(); }
}
