#include "w_bt_dish_kitchenprinter_printermanager_tool.h"
#include "ui_w_bt_dish_kitchenprinter_printermanager_tool.h"
#include "printer_pos.h"
#include "lds_messagebox.h"

w_bt_dish_Kitchenprinter_printermanager_tool::w_bt_dish_Kitchenprinter_printermanager_tool(const QString &printport, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_bt_dish_Kitchenprinter_printermanager_tool),
    printport(printport)
{
    ui->setupUi(this);
    /*
  ESC t n 选择国际扩展字符码表
  [名称]	Select character code table
  [格式]	ASCII	ESC	t	n
  Hex 	1B	74	n
  Decimal	27	116	n
  [范围]	0 ￡ n ￡ 10, 16 ￡ n ￡20
  [描述]	从下列表中选择由n确定的国际扩展字符表。
  n	扩展字库名称
  0	PC437:Standard-Europe	欧洲标准
  1	Katakana	日本片假名
  2	PC850:Multilingual	多语言
  3	PC860:Portuguese	葡萄牙文
  4	PC863:Canadian-French	加拿大、法国
  5	PC865:Nordic	北欧
  6	West-Europe	西欧
  7	Greek	希腊文
  8	Hebrew	希伯来文
  9	PC755:East-Eruope	东欧
  10	Iran	伊朗文
  16	WPC1252
  17	PC866:Cyrillic#2	古斯拉夫语
  18	PC852:Latin2	拉丁文
  19	PC858

  [注释]
  [默认值]	n = 0
  [参考]
  */
    QList< QPair<QString, QVariant> > code_page_list;
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF00", "(PC437:Standard-Europe)"));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF01", "(Katakana)"             ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF02", "(PC850:Multilingual)"   ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF03", "(PC860:Portuguese)"     ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF04", "(PC863:Canadian-French)"));

    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF05", "(PC865:Nordic)"         ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF06", "(West-Europe)"          ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF07", "(Greek)"                ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF08", "(Hebrew)"               ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF09", "(PC755:East-Eruope)"    ));

    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF0A", "(Iran)"                 ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF0D", "(PC857:Turkish)"        ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF10", "(WPC1252)"              ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF11", "(PC866:Cyrillic#2)"     ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF12", "(PC852:Latin2)"         ));

    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF13", "(PC858)"                ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF2E", "(WPC1251:Cyrillic)"     ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF33", "(WPC1257:Baltic-Rim)"   ));
    code_page_list.append(QPair<QString, QVariant>("1F1B1FFF30", "(WPC1254:Turkish)"      ));

    QList< QPair<QString, QVariant> > beep_list;
    beep_list.append(QPair<QString, QVariant>("1f 1b 1f 50 42", "Yes"));
    beep_list.append(QPair<QString, QVariant>("1f 1b 1f 50 40", "No"));

    QList< QPair<QString, QVariant> > chiness_character_mode_list;
    chiness_character_mode_list.append(QPair<QString, QVariant>("1f 1b 1f fe 00", "Yes"));
    chiness_character_mode_list.append(QPair<QString, QVariant>("1f 1b 1f fe 01", "No"));


    ui->comboBox_beeper->addItemsByPairList(beep_list);
    ui->comboBox_chinese_mode->addItemsByPairList(chiness_character_mode_list);
    ui->comboBox_code_page->addItemsByPairList(code_page_list);

    connect(ui->pushButton_app_beeper, SIGNAL(clicked()), this, SLOT(toapp_beeper()));
    connect(ui->pushButton_app_chinese_mode, SIGNAL(clicked()), this, SLOT(toapp_chinese_mode()));
    connect(ui->pushButton_app_code_page, SIGNAL(clicked()), this, SLOT(toapp_code_page()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));

    this->setWindowTitle("tool");
}


w_bt_dish_Kitchenprinter_printermanager_tool::~w_bt_dish_Kitchenprinter_printermanager_tool()
{
    delete ui;
}

void w_bt_dish_Kitchenprinter_printermanager_tool::toapp_beeper()
{
    towriteHex(this->sender());
}

void w_bt_dish_Kitchenprinter_printermanager_tool::toapp_chinese_mode()
{
    towriteHex(this->sender());
}

void w_bt_dish_Kitchenprinter_printermanager_tool::toapp_code_page()
{
    towriteHex(this->sender());
}

void w_bt_dish_Kitchenprinter_printermanager_tool::toexit()
{
    this->reject();
}

void w_bt_dish_Kitchenprinter_printermanager_tool::towriteHex(QObject *obj)
{
    lds_ComboBox *com = 0;
    if(obj == ui->pushButton_app_beeper) com = ui->comboBox_beeper;
    if(obj == ui->pushButton_app_chinese_mode) com = ui->comboBox_chinese_mode;
    if(obj == ui->pushButton_app_code_page) com = ui->comboBox_code_page;

    if(com != 0) {
        QSharedPointer<Printer_POS> printer(Printer_POS::get_pos_printer_by_path(printport));
        if(false == printer->tryOpen(printport)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, printer->lastError());
            return;
        }
        printer->writeHex(com->curusrdata().toByteArray());
        printer->close();
    }
}
