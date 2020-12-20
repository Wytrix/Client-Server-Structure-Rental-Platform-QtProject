#include "mainwindow.h"
#include "button.h"
#include <QFontDatabase>
#include "controller.h"

class QObject;
class Controller;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    view->setViewport(new QOpenGLWidget);
    controller = new Controller(*scene, *view, this);
    setCentralWidget(view);
    resize(SCENE_WIDTH * 2, SCENE_HEIGHT * 2);
    view->setFixedSize(SCENE_WIDTH * 2, SCENE_HEIGHT * 2);
    setFixedSize(SCENE_WIDTH * 2, SCENE_HEIGHT * 2);
    initScene();
    initSceneBackground();
    QTimer::singleShot(0, this, SLOT(adjustViewSize()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::adjustViewSize()
{
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
}

void MainWindow::initScene()
{
    scene->setSceneRect(-SCENE_WIDTH, -SCENE_HEIGHT, SCENE_WIDTH * 2, SCENE_HEIGHT * 2);
}

void MainWindow::initSceneBackground()
{
    view->setBackgroundBrush(QColor(0xBF,0xEF,0xFF));
}

