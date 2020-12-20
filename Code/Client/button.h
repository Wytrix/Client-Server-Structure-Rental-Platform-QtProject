#ifndef BUTTON_H
#define BUTTON_H

#include "Header.h"

class Button : public QGraphicsObject
{
    Q_OBJECT
public:
    Button(QString text,int x,int y,int w,int h,int s = 14,int tpye = 0,bool vari = true,int r = LightSkyBlue.red(),int g = LightSkyBlue.green(),int b = LightSkyBlue.blue(),int a = 255,int lr = RoyalBlue.red(),int lg = RoyalBlue.green(),int lb = RoyalBlue.blue(), bool isEnglish = false, bool Random = false);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
    void setFontSize(int s);
    void setSize(int width, int height);
    void setText(QString s);
    void setBackColor(QColor c);
    void setLineColor(QColor c);
    void setEnglish(bool);
    void setHasVal(bool b);
    void setvari(bool v);
    void setRandom(bool);
    bool toColor(int a,int b,int c);
    void ColorRandom();
    void setPosition(const QPoint);
    QPoint getPosition() const;
    void setEdit();
    void setRound(bool);
    void setImage(QString s);
    void removeImage();
    QGraphicsTextItem *text;
    QString imageName;
signals:
    void ButtonIsReleased();
    void sendVal(Button *val);

private:
    QString texts;
    QGraphicsDropShadowEffect *eff;
    QGraphicsScene *scene;
    QPoint pos;

    int X,Y,w,h;
    int size;
    int type;
    int rr,gg,bb,aa,liner,lineg,lineb,red,green,blue;
    bool hasval, vari, isEnglish, random, edit, round, hasImage;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void advance(int step);
};

#endif // BUTTON_H
