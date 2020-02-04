#ifndef LDS_DOUBLESPINBOX_H
#define LDS_DOUBLESPINBOX_H

#include <QDoubleSpinBox>
#define DBOX_MIN -1000000000000000
#define DBOX_MAX 1000000000000000

class lds_doublespinbox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit lds_doublespinbox(QWidget *parent = 0);

    void expandMAXMIN();
    QSize sizeHint() const;

    void setValue(double v);
    void setTrueValue(double v);
    double value();
    double trueValue();

    //方便折扣和折让的互相转换，基础算法是折扣
    void enableValueMaxMinus();

    bool isEmpty();
signals:
    void returnPressedDelay();
protected:
    void focusOutEvent(QFocusEvent *e);
    void focusInEvent(QFocusEvent *e);
    void keyPressEvent(QKeyEvent *event);

    // 将输入的文本解读为适当的值
    virtual double valueFromText(const QString &text) const;
    // 根据输入的值返回文本
    virtual QString textFromValue(double value1) const;

    QString text() const;

private slots:
    void toSelectAll();
signals:
    void getfocus();
    void outfocus();
private:
    bool bValuemaxminus;
};

#endif // LDS_DOUBLESPINBOX_H
