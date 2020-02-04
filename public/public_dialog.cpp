#include "public_dialog.h"
#include <QApplication>
#include <QFontDatabase>
#include <QtDebug>
#include "w_scr_dish_pay_module_select.h"
#include "w_bt_dish.h"
#include "w_i_inventory.h"
#include "public_sql.h"
#include <QApplication>
#include "w_m_member.h"
#include "w_sys_manage_outer_pay_set.h"
#include "restauranttableview.h"
#include "w_rpt_report_frame.h"
#include "w_bt_dish_paymode.h"

pointerAutoDel<QTranslator> public_Dialog::curTranslate;
pointerAutoDel<QTranslator> public_Dialog::printTranslate;

public_Dialog::public_Dialog(QWidget *parent)
    : QDialog(parent)
{
}

void public_Dialog::comIndexSaveClear(QComboBox *com)
{
    int idx = com->currentIndex();
    if(com->isEditable()) {
        if(com->itemText(idx) != com->currentText()) {
            com_index.insert(com, com->currentText());
            return;
        }
    }
    com_index.insert(com, idx);
    com->clear();
}

void public_Dialog::comIndexRestore(QComboBox *com)
{
    QVariant d = com_index.value(com);
    if(d.type() == QVariant::Int) {
        com->setCurrentIndex(d.toInt());
        return;
    }
    if(d.type() == QVariant::String) {
        com->setEditText(d.toString());
        return;
    }
    com->setCurrentIndex(-1);
}

void public_Dialog::comIndexRestoreAll()
{
    for(QMap<QComboBox*, QVariant>::iterator k = com_index.begin();
        k != com_index.end();
        k++) {
        comIndexRestore(k.key());
    }
}

void public_Dialog::clearCache()
{
    w_scr_dish_pay_module_select::clearCache();
    w_bt_dish::clearCache();
    w_i_inventory::clearCache();
    w_m_member::clearCache();
    w_sys_manage_outer_pay_set::clearCache();
    restraurantDelegate::clearCache();
    w_rpt_report_frame::clearCache();
    lds::reportModelMapClear();
    lds::skin_name_map_reset();

    resetPayModeName();
}

void public_Dialog::resetPayModeName()
{
    lds_query_hddpos  query;
    if(!query.db.isOpen()) {
        return;
    }
    lds_query_hddpos::tran_saction();
    w_bt_dish_paymode::update_tanslate();
    lds_query_hddpos::com_mit();
}

QTranslator *public_Dialog::get_printTranslate(const QString &languageKey)
{
    static QString preKey;

    if(preKey != languageKey) {
        preKey = languageKey;
        printTranslate.delPointer();
    }
    if( 0 == printTranslate.getPointer() ) {
        printTranslate.setPointer(new QTranslator);
    }
    if(printTranslate.getPointer()->isEmpty()) {
        printTranslate.getPointer()->load("userdata/settings/language/"+languageKey+"_fastfd.qm");
    }
    return printTranslate.getPointer();
}

void public_Dialog::del_printTranslate()
{
    printTranslate.delPointer();
}

void public_Dialog::retranslateSystem(const QString &key)
{
    if(0 != curTranslate.getPointer()
            &&!curTranslate.getPointer()->isEmpty()) {
        qApp->removeTranslator(curTranslate.getPointer());
        curTranslate.delPointer();
    }
    if( 0 == curTranslate.getPointer() ) {
        curTranslate.setPointer(new QTranslator);
    }
    //userdata/settings/language/KO_fastfd.qm
    QString qmpath = QString("userdata/settings/language/") + key + "_fastfd.qm";
    QString font_path_language = "userdata/settings/language/" + key + ".ttf";
    if(QFile::exists(font_path_language)) {
        int fid = QFontDatabase::addApplicationFont(font_path_language);
        if(fid != -1) {
            QString family = QFontDatabase::applicationFontFamilies(fid).value(0);
            qApp->setFont(ldsObjFont(qApp, family));

            lds::DOUBLE_DATA ddata;
            ddata.udpKey = lds::udp_font_update_request;
            ddata.font_path = font_path_language;
            public_sql::doublescreensenddataByconf(ddata);
        }
    } else {
        qApp->setFont(ldsObjFont(qApp, lds::FONT_DEFAULT_Family));
    }
    if(curTranslate.getPointer()->load(qmpath)) {
        qApp->installTranslator(curTranslate.getPointer());
        qDebug() << __FILE__ << __LINE__ << "installTranslator";//QT_TR_NOOP
    }
    public_Dialog::clearCache();
}
