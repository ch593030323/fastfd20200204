#include "label_img_code128.h"

label_img_code128::label_img_code128(QString text,QString p)
{
    barcode = text;
    path = p;
}

//开始生成条码图片
void label_img_code128::start()
{
    QString barstring = BuildBarString(barcode);
    KiCode128C(barstring);
}

// 建立条码字符串
// <param name="tex">条码内容</param>
QString label_img_code128::BuildBarString(QString tex)
{
    QString barstart = "bbsbssbssss";    //码头

    QString barbody = "";                //码身

    QString barcheck = "";               //码检

    QString barend = "bbsssbbbsbsbb";    //码尾

    int checkNum = 104;

    //循环添加码身,计算码检

    for (int i = 1; i <= tex.length(); i++)
    {

        //int index = (int)tex[i - 1] - 32;

        int index = (int)(tex.at(i - 1).unicode()) - 32;

        checkNum += (index * i);

        barbody += AddSimpleTag(index);//加入字符值的条码标记

    }

    //码检值计算

    //barcheck = AddSimpleTag(int.Parse(Convert.ToDouble(checkNum % 103).ToString("0")));
    barcheck = AddSimpleTag((int)(QString::number(checkNum % 103).toDouble()));

    QString barstring = barstart + barbody + barcheck + barend;

    return barstring;
}

//增加一个条码标记

QString label_img_code128::AddSimpleTag(int CodeIndex)
{

    QString res = "";

    // <summary>1-4的条的字符标识 </summary>

    QStringList TagB;
    TagB << "" << "b" << "bb" << "bbb" << "bbbb";

    // <summary>1-4的空的字符标识 </summary>

    QStringList TagS;
    TagS << "" << "s" << "ss" << "sss" << "ssss";

    QStringList Code128List;
    Code128List << "212222"<<"222122"<<"222221"<<"121223"<<"121322"<<"131222"<<"122213"<<"122312"<<"132212"<<"221213"<<

            "221312"<<"231212"<<"112232"<<"122132"<<"122231"<<"113222"<<"123122"<<"123221"<<"223211"<<"221132"<<

            "221231"<<"213212"<<"223112"<<"312131"<<"311222"<<"321122"<<"321221"<<"312212"<<"322112"<<"322211"<<

            "212123"<<"212321"<<"232121"<<"111323"<<"131123"<<"131321"<<"112313"<<"132113"<<"132311"<<"211313"<<

            "231113"<<"231311"<<"112133"<<"112331"<<"132131"<<"113123"<<"113321"<<"133121"<<"313121"<<"211331"<<

            "231131"<<"213113"<<"213311"<<"213131"<<"311123"<<"311321"<<"331121"<<"312113"<<"312311"<<"332111"<<

            "314111"<<"221411"<<"431111"<<"111224"<<"111422"<<"121124"<<"121421"<<"141122"<<"141221"<<"112214"<<

            "112412"<<"122114"<<"122411"<<"142112"<<"142211"<<"241211"<<"221114"<<"413111"<<"241112"<<"134111"<<

            "111242"<<"121142"<<"121241"<<"114212"<<"124112"<<"124211"<<"411212"<<"421112"<<"421211"<<"212141"<<

            "214121"<<"412121"<<"111143"<<"111341"<<"131141"<<"114113"<<"114311"<<"411113"<<"411311"<<"113141"<<

            "114131"<<"311141"<<"411131"<<"211412"<<"211214"<<"211232";

    QString tag = Code128List.at(CodeIndex);

    for (int i = 0; i < tag.length(); i++)

    {

        QString temp = "";

        int num = QString(tag.at(i)).toInt();

        if (i % 2 == 0)

        {

            temp = TagB.at(num);

        }

        else

        {

            temp = TagS.at(num);

        }

        res += temp;

    }

    return res;

}

// <summary>生成条码 </summary>

// <param name="BarString">条码模式字符串</param> //Format32bppArgb

// <returns>条码图形</returns>
void label_img_code128::KiCode128C(QString BarString)
{

    int MAXWIDTH;
    int MAXHEIGHT=30;
    int FONTHEIGHT=10;

    //设定条码线条宽度px
    int LineWidth=1;
    //预留10px的左右空白；
    MAXWIDTH=LineWidth*BarString.size()+5;

    //计算条线宽度时，精度丢失造成条码右边空白，进行右移居中
    int move=(MAXWIDTH-LineWidth*BarString.size())/2;

    QSize size(MAXWIDTH,MAXHEIGHT+FONTHEIGHT);
    QImage image(size,QImage::Format_ARGB32);
    image.fill(qRgba(255, 255, 255, 0));

    //QBitmap bmp(MAXWIDTH,MAXHEIGHT+FONTHEIGHT);
    QPainter painter(&image);
    QPen white,black;
//    painter.setRenderHint(QPainter::Antialiasing,true);//弧线圆润
    white.setColor(QColor(255,255,255));
    black.setColor(QColor(0,0,0));
    white.setWidth(LineWidth);
    black.setWidth(LineWidth);

    //背景充填白色
    painter.setPen(white);
    painter.drawRect(0,0,MAXWIDTH,MAXHEIGHT+FONTHEIGHT);

    //画线
    for(int i=0;i<BarString.size();i++)
    {
        if(BarString.at(i)=='b') painter.setPen(black);
        else painter.setPen(white);
        painter.drawLine(i*LineWidth+LineWidth/2+move,0,i*LineWidth+LineWidth/2+move,MAXHEIGHT);
    }

    //添加底部条形码字符
    painter.setPen(black);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setRenderHint(QPainter::TextAntialiasing,true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing,true);
    QFont font;
    font.setFamily("宋体");
    font.setPixelSize(10);
#ifdef Q_OS_WIN
    font.setWeight(QFont::ExtraLight);
#endif
    painter.setFont(font);
    painter.drawText(5,30,MAXWIDTH,10,Qt::AlignCenter,"S/N:" + barcode);
    image.save(path + barcode + ".bmp");

}

void label_img_code128::reset(QString text,QString p)
{
    barcode = text;
    path = p;
}

void label_img_code128::paintBar(QPainter *painter, const QString &bar)
{
    QString text = BuildBarString(bar);
    int MAXWIDTH;
    int MAXHEIGHT=30;
    int FONTHEIGHT=10;

    //设定条码线条宽度px
    int LineWidth=1;
    //预留10px的左右空白；
    MAXWIDTH=LineWidth*text.size()+5;

    //计算条线宽度时，精度丢失造成条码右边空白，进行右移居中
    int move=(MAXWIDTH-LineWidth*text.size())/2;

//    QSize size(MAXWIDTH,MAXHEIGHT+FONTHEIGHT);
//    QImage image(size,QImage::Format_ARGB32);
//    image.fill(qRgba(255, 255, 255, 0));

    //QBitmap bmp(MAXWIDTH,MAXHEIGHT+FONTHEIGHT);
    QPen white,black;
//    painter->setRenderHint(QPainter::Antialiasing,true);//弧线圆润
    white.setColor(QColor(255,255,255));
    black.setColor(QColor(0,0,0));
    white.setWidth(LineWidth);
    black.setWidth(LineWidth);

    //背景充填白色
//    painter->setPen(white);
//    painter->drawRect(0,0,MAXWIDTH,MAXHEIGHT+FONTHEIGHT);

    //画线
    for(int i=0;i<text.size();i++)
    {
        if(text.at(i)=='b') painter->setPen(black);
        else painter->setPen(white);
        painter->drawLine(i*LineWidth+LineWidth/2+move,0,i*LineWidth+LineWidth/2+move,MAXHEIGHT);
    }
}
