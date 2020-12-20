#include "button.h"

Button::Button(QString txt,int x,int y,int ww,int hh,int s,int typ,bool varii,int r,int g, int b,int a, int lr,int lg,int lb,bool isEng,bool Random)
{
    setPos(x,y);
    X = x, Y = y;
    texts = txt;
    w = ww;
    h = hh;
    type = typ;
    size = s;
    vari = varii, random = Random;
    rr = r, gg = g, bb = b, aa = a, liner = lr, lineg =lg, lineb = lb, red = 0, green = 0, blue = 0;
    eff = new QGraphicsDropShadowEffect;
    eff->setBlurRadius(20);
    eff->setOffset(10);
    eff->setColor(QColor(r,g,b,100));
    this->setGraphicsEffect(eff);
    hasval = false;
    isEnglish = isEng;
    round = true;
    hasImage = false;
    switch(type){
    case 0:
        setBackColor(LightSkyBlue);
        setLineColor(RoyalBlue);
        break;
    case 1:
        setBackColor(LightPink);
        setLineColor(Crimson);
        break;
    case 2:
        setBackColor(NavajoWhite);
        setLineColor(Coral);
        break;
    case 3:
        setBackColor(PaleGreen);
        setLineColor(ForestGreen);
        break;
    case 4:
        setBackColor(Plum);
        setLineColor(MediumOrchid);
        break;
    case 5:
        setBackColor(Khaki);
        setLineColor(Orange);
        break;
    case 6:
        setBackColor(PaleTurquoise);
        setLineColor(SteelBlue);
        break;
    case 7:
        setBackColor(Silver);
        setLineColor(DimGray);
        break;
    case 8:
        setBackColor(Wheat);
        setLineColor(DarkOrange);
        break;
    case 9:
        setBackColor(LightPink);
        setLineColor(Indigo);
        break;
    case 10:
        setBackColor(PaleGodenrod);
        setLineColor(DimGray);
        break;
    case 11:
        setBackColor(LightSteelBlue);
        setLineColor(LightSlateGray);
        break;

    }
}
QRectF Button::boundingRect() const
{
    return QRectF(- w / 2 - 5, - h / 2 -5, w + 10, h + 10);
}
QPainterPath Button::shape() const
{
    QPainterPath path;
    if(round)path.addRoundedRect(- w / 2, - h / 2, w, h , qMin(w/4, h/4), qMin(w/4, h/4));
    else path.addRect(- w / 2, - h / 2, w, h );
    return path;
}
void Button::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setWidthF(2);
    QColor color(rr,gg,bb,aa);
    QColor ecolor(255, 251, 240, aa);
    //QColor mcolor(rr*0.5 +127, gg*0.5 + 125, bb*0.5 + 120, aa);
    painter->setPen(pen);

    QLinearGradient ligrat;
    ligrat.setStart(-w, 0);
    ligrat.setFinalStop(w, 0);
    ligrat.setColorAt(0, color);
    //ligrat.setColorAt(0.5, mcolor);
    ligrat.setColorAt(1, ecolor);
    //ligrat.setSpread(QGradient::RepeatSpread);
    if(vari)painter->fillPath(shape(), ligrat);
    else painter->fillPath(shape(), color);
    //painter->drawPath(shape());

    color.setRgb(liner,lineg,lineb);
    pen.setColor(color);
    painter->setPen(pen);

    QFont font;
    if(isEnglish)font.setFamily("Verdana");
    else font.setFamily("微软雅黑");
    //font.setBold(false);
    font.setPixelSize(size);
    //font.setWeight(1);

    painter->setFont(font);
    painter->setRenderHint(QPainter::TextAntialiasing,true);
    painter->drawText(QRectF(- w / 2, - h / 2, w, h),Qt::AlignCenter,texts);
    if(hasImage) painter->drawImage(QRectF(-w/2,-h/2,w,h),QImage(imageName));
    painter->restore();
}

void Button::setFontSize(int s)
{
    size = s;
}

void Button::setSize(int width, int height)
{
    w = width;
    h = height;
}

void Button::setText(QString s)
{
    texts = s;
}

void Button::setHasVal(bool b)
{
    hasval = b;
}

void Button::setvari(bool v)
{
    vari = v;
}

void Button::setRound(bool b)
{
    round = b;
}

void Button::setBackColor(QColor c)
{
    rr = c.red();
    gg = c.green();
    bb = c.blue();
    aa = c.alpha();
    eff->setColor(QColor(rr,gg,bb,100));
    this->setGraphicsEffect(eff);
}

void Button::setLineColor(QColor c)
{
    liner = c.red();
    lineg = c.green();
    lineb = c.blue();
}

void Button::setEnglish(bool b){
    isEnglish = b;
}

void Button::setRandom(bool b){
    random  = b;
}

bool Button::toColor(int a, int b, int c)
{
    if(rr != a)rr += (a-rr)/qAbs(rr-a);
    if(gg != b)gg += (b-gg)/qAbs(gg-b);
    if(bb != c)bb += (c-bb)/qAbs(bb-c);
    if(rr == a && gg == b && bb == c)return true;
    else return false;
}

void Button::ColorRandom()
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    if(red == 0 && blue == 0 && green == 0)
    {
        red = qrand()%256;
        blue = qrand()%256;
        green = qrand()%256;
    }
    bool reach = false;
    reach = toColor(red, green, blue);
    if(reach)
    {
        red = qrand()%256;
        blue = qrand()%256;
        green = qrand()%256;
    }
    setLineColor(QColor(qMax(0,rr-70),qMax(0,gg-70),qMax(0,bb-70)));
    eff->setColor(QColor(rr,gg,bb,100));
    this->setGraphicsEffect(eff);
    update();
}

void Button::setPosition(const QPoint a)
{
    X=a.x();
    Y=a.y();
    setPos(X,Y);
}

QPoint Button::getPosition() const
{
    QPoint temp(X,Y);
    return temp;
}

void Button::setEdit()
{
    setBackColor(White);
    setSize(w,EDIT_LINE_HEIGHT);
    setRound(false);
    text = new QGraphicsTextItem(this);
    QFont font;
    font.setFamily("微软雅黑");
    font.setPixelSize(36);
    text->setFont(font);
    text->setTextInteractionFlags(Qt::TextEditable);
    text->setTextWidth(w);
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignCenter);
    QTextCursor cursor = text->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
    cursor.clearSelection();
    text->setTextCursor(cursor);
    text->setPos(-w/2, -h/2);
    text->setTextInteractionFlags(Qt::TextEditorInteraction);
    text->setTabChangesFocus(true);
    text->setDefaultTextColor(CornflowerBlue);
}

void Button::setImage(QString s)
{
    hasImage = true;
    imageName = s;
    update();
}
void Button::removeImage()
{
    hasImage = false;
    update();
}
void Button::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(contains(QPointF(event->pos())))
        {
            QPropertyAnimation *animation = new QPropertyAnimation(this, "scale");
            animation->setDuration(200);
            animation->setKeyValueAt(0,1);
            animation->setKeyValueAt(1,0.9);
            animation->setEasingCurve(QEasingCurve::OutExpo);
            animation->start(QAbstractAnimation::DeleteWhenStopped);
            //delete animation;
        }
    }
    //QGraphicsItem::mousePressEvent(event);
}

void Button::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "scale");
        animation->setDuration(200);
        animation->setKeyValueAt(0,0.9);
        animation->setKeyValueAt(1,1);
        animation->setEasingCurve(QEasingCurve::OutExpo);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        if(contains(event->pos()))
        {
            emit ButtonIsReleased();
            if(hasval)emit sendVal(this);
        }
    }
    //QGraphicsItem::mouseReleaseEvent(event);
}

void Button::advance(int step)
{
    if(!step){
        return ;
    }
    if(random)ColorRandom();
}
