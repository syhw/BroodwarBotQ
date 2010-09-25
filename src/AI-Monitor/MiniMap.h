#ifdef BW_QT_DEBUG
#ifndef MINIMAP_H
#define MINIMAP_H

#include <QtOpenGL/QtOpenGL>
#include <QtGui/QImage>
#include <QtCore/QTimeLine>

class MiniMap : public QGLWidget
{
    Q_OBJECT

public:
    MiniMap(QWidget *parent);
    ~MiniMap();

    void saveGLState();
    void restoreGLState();

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);

public slots:
    void draw();

private:
    QPoint anchor;
    float scale;
    float tr_x, tr_y;
    QGLFramebufferObject *render_fbo;
};

#endif // MINIMAP_H
#endif