#ifndef LDS_DOUBLESPINBOX_LABEL_H
#define LDS_DOUBLESPINBOX_LABEL_H

#include <QLabel>

class lds_doublespinbox_label : public QLabel
{
    Q_OBJECT
public:
    explicit lds_doublespinbox_label(QWidget *parent = 0);

    double value();
    void setValue(double value);
protected Q_SLOTS:
    void setText(const QString &text);
protected:
    QString text() const;

};

#endif // LDS_DOUBLESPINBOX_LABEL_H
