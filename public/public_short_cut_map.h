#ifndef PUBLIC_SHORT_CUT_MAP_H
#define PUBLIC_SHORT_CUT_MAP_H

#include <QDialog>
#include <QItemDelegate>

namespace Ui {
class public_short_cut_map;
}

class public_short_cut_map : public QDialog
{
    Q_OBJECT
public:
    enum {
        QT_BEGIN = 1,
        QT_END = 7,
        PAY_BEGIN = 8,
        PAY_END = 11,
    };
public:
    explicit public_short_cut_map(QWidget *parent = 0);
    ~public_short_cut_map();
    //F4 -- 34123213
    static QString get_desc_by_keyvalue(int keyvalue, int row0, int row1);
    static void OPER_KEY_MODEL_INIT();

    void showqt();
    void showpay();

    //Qt
    static bool keyPressQt(QWidget *qt, QKeyEvent *e);
    static void shortCutFunctionQt(QObject *qt, const QString &desc);
signals:
    void signalShortCutFunction(const QString &k);
    void signalKey(int k);
private:
    void invokeMethod(const char *slot);
    static void invokeMethod(QObject *o, const char *slot);

    //~Qt
protected:
    bool eventFilter(QObject *o, QEvent *e);
private slots:
    void took();
    void toexit();
    void todefault();
private:
    static QString defaultData(const QString &key = "");
    static void default2(const QString &desc, const QString text_value);
    static void saveData2();
    static int _row0;
    static int _row1;
private:
    Ui::public_short_cut_map *ui;

    class delegate : public QItemDelegate{
    public:
        delegate(QObject *parent = 0) : QItemDelegate(parent) {}
        ~delegate() {}
        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    };
};

#endif // PUBLIC_SHORT_CUT_MAP_H
