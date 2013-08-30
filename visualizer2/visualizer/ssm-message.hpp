#ifndef SSM_MESSAGE_HPP
#define SSM_MESSAGE_HPP

class WidgetMSG;
class SSMApiBase;

void smTarget(WidgetMSG*);
void smInit();
void smEnd();

bool smState   (SSMApiBase*);
void smConnect (SSMApiBase*);
void smReadLast(SSMApiBase*);
void smReadTime(SSMApiBase*, double);

#endif // SSM_MESSAGE_HPP
