#ifndef WIDGET_MSG_HPP
#define WIDGET_MSG_HPP

#include <QLabel>

class WidgetMSG : public QLabel
{
    Q_OBJECT

    public:

         WidgetMSG();
        ~WidgetMSG();

    public slots:

         void set_message(const std::string &str);
         void add_message(const std::string &str);
};

#endif // WIDGET_MSG_HPP
