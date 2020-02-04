#ifndef PUBLIC_DIALOG_H
#define PUBLIC_DIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QTranslator>
#include "pointerautodel.h"

class public_Dialog : public QDialog
{
public:
    static void retranslateSystem(const QString &key);
    static void clearCache();
    static void resetPayModeName();
    static QTranslator *get_printTranslate(const QString &languageKey);
    static void del_printTranslate();
private:
    static pointerAutoDel<QTranslator> printTranslate;
    static pointerAutoDel<QTranslator> curTranslate;
//    static QTranslator printTranslate;
//    static QTranslator curTranslate;
public:
    public_Dialog(QWidget *parent);
    virtual void retranslateView() = 0;

    void comIndexSaveClear(QComboBox *com);
    void comIndexRestore(QComboBox *com);
    void comIndexRestoreAll();
    QMap<QComboBox*, QVariant> com_index;
};

#endif // PUBLIC_DIALOG_H
