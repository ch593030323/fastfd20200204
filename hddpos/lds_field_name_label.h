#ifndef LDS_FIELD_NAME_LABEL_H
#define LDS_FIELD_NAME_LABEL_H

#include <QLabel>

class lds_field_name_label : public QLabel
{
    Q_OBJECT
public:
    explicit lds_field_name_label(QWidget *parent = 0);

    void setText(const QString &text);
};

#endif // LDS_FIELD_NAME_LABEL_H
