#ifndef LDS_MESSAGEBOX_H
#define LDS_MESSAGEBOX_H

#include <QObject>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include "lds_roundeddialog_rect_align.h"
#include <QAbstractButton>
#include <QStyle>
#include <QHBoxLayout>

#define _TITLE_WIN(object)		+( (qobject_cast<QWidget *>(object))?(qobject_cast<QWidget *>(object)->windowTitle()):("")  )
#define _TEXT_SEND(object)		 ( qobject_cast<QAbstractButton *>(object)?  qobject_cast<QAbstractButton *>(object)->text() : "" )
#define _TEXT_SLOT(object)		( object == 0 ? "" :  _TEXT_SEND(object->sender() ) )
#define _SHOW_LOADING(parent)     lds_messagebox_loading_show loading(parent, "", "LOADING..."); loading.show();
#define MESSAGE_TITLE_VOID QString()

class lds_msgbox_virtual : public QDialog {
    Q_OBJECT
public:
    lds_msgbox_virtual(QStyle::StandardPixmap icon,
               const QString &title,
               const QString &text,
               QStringList buttonTexts,
               QWidget *parent
            );
    QPushButton *btnok;
    QList<QObject *> btnlist;

public slots:
    void updateTimeframe(int index);
    void todone();
signals:
    void btnok_finished(int index);
protected:
    void keyPressEvent(QKeyEvent *e);
};

class lds_msgbox:public lds_msgbox_virtual{
    Q_OBJECT
public:
    lds_msgbox(QStyle::StandardPixmap icon,
               const QString &title,
               const QString &text,
               QStringList buttonTexts,
               QWidget *parent
            );
};

class lds_messagebox_textedit : public lds_msgbox_virtual{
    Q_OBJECT
public:
    lds_messagebox_textedit(QWidget *parent, const QString &title,
                           const QString& text,
                           QStringList buttonTexts = QStringList());
};

class lds_messagebox : public QObject
{
    Q_OBJECT
public:
    static QPoint messge_pos;
public:
    explicit lds_messagebox(QObject *parent = 0);

    ////////////////////////////////////////////////////1
    static int warning(QWidget *parent, const QString &title,
                       const QString& text,
                       const QString& button0Text,
                       const QString& button1Text = QString(),
                       const QString& button2Text = QString(),
                       bool delayclose=false
            );
    static int warning(QWidget *parent, const QString &title,
                       const QString& text,
                       bool delayclose=false);
    ////////////////////////////////////////////////////2
    static int information(QWidget *parent, const QString &title,
                           const QString& text,
                           const QString& button0Text,
                           const QString& button1Text = QString(),
                           const QString& button2Text = QString(),
                           bool delayclose=false
            );
    static int information(QWidget *parent, const QString &title,
                           const QString& text,
                           bool delayclose=false);
    ////////////////////////////////////////////////////3
    static int question(QWidget *parent, const QString &title,
                        const QString& text,
                        const QString& button0Text,
                        const QString& button1Text = QString(),
                        const QString& button2Text = QString(),
                        const QString& button3Text = QString(),
                        bool delayclose=false
            );
    static int question(QWidget *parent, const QString &title,
                        const QString& text,
                        bool delayclose=false);
    ////////////////////////////////////////////////////4
    static int critical(QWidget *parent, const QString &title,
                        const QString& text,
                        const QString& button0Text,
                        const QString& button1Text = QString(),
                        const QString& button2Text = QString(),
                        bool delayclose=false);

    static int critical(QWidget *parent, const QString &title,
                        const QString& text,
                        bool delayclose=false);
    ////////////////////////////////////////////////////5
private:
    static int showOldMessageBox(QWidget *parent, QStyle::StandardPixmap icon,
                                 const QString &title, const QString &text,
                                 QString button0Text,
                                 const QString &button1Text,
                                 const QString &button2Text,
                                 const QString &button3Text,
                                 bool delayclose);
};

class lds_messagebox_loading : public QDialog{
    Q_OBJECT
public:
    lds_messagebox_loading(QWidget *parent, const QString &title,
                           const QString& text="");
    ~lds_messagebox_loading();
    void image_dynamic_run();
    QString text();

    QPushButton* layoutAddButton(const QString &btn_txt);
    int layoutButtonIndexOf(QObject *obj);
    QObject *layoutButton(int index);
public slots:
    void setText(const QString &text);
    void update_image();
private:
    QMovie *mv;
    QLabel *label_image;
    QLabel *label_text;    
    QHBoxLayout*hlayout;
    QList<QObject *> btnlist;
};

class lds_messagebox_loading_show : public QObject{
    Q_OBJECT
public:
    lds_messagebox_loading_show(QWidget *parent, const QString &title,
                                const QString& text="");
    ~lds_messagebox_loading_show();
    void show();
    void hide();
    lds_roundeddialog_rect_align *widget();
    void show_delay();
    void image_dynamic_run();

    QString text();
    void setTile(const QString &title);
    QString title();

    void addButton(const QString &btn_txt);
    void hideButton(int index);
    void setButtonText(int index, const QString &text);

    void setMSGWindowFlags(Qt::WindowFlags flags);
    int result();
signals:
    void signal_btn_clicked(int);
public slots:
    void updateProgress(qint64 bytesStep, qint64 bytesTotal);
    void updateProgress(qint64 bytesStep, qint64 bytesTotal, const QString &title);
    void setText(const QString &text, int delay);
    void setText(const QString &text);
    void showMessage(const QString &text);

    void toBtnClicked();
private:
    lds_roundeddialog_rect_align *dialog;
    lds_messagebox_loading *loading;
    QString _title;
    QString _text;
    int btn_click_index;
};
#endif // LDS_MESSAGEBOX_H
