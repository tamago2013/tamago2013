#ifndef SSM_MESSAGE_HPP
#define SSM_MESSAGE_HPP

class WidgetMSG;
class SSMApiBase;

void smTarget(WidgetMSG*);
void smInit();
void smEnd();
void smConnect(SSMApiBase*);
void smReadNew(SSMApiBase*);

#endif // SSM_MESSAGE_HPP
