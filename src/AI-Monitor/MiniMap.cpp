#include "MiniMap.h"
#include <QtGui/QImage>
#include <math.h>

MiniMap::MiniMap(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers|QGL::AlphaChannel), parent)
{
    setWindowTitle(tr("OpenGL framebuffer objects"));
    makeCurrent();

    if (QGLFramebufferObject::hasOpenGLFramebufferBlit()) {
        QGLFramebufferObjectFormat format;
        format.setSamples(4);
        format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);

        render_fbo = new QGLFramebufferObject(512, 512, format);
    } else {
        render_fbo = new QGLFramebufferObject(1024, 1024);
    }

    tr_x = tr_y = 0.0f;
    scale = 0.1f;
}

MiniMap::~MiniMap()
{
    delete render_fbo;
}

void MiniMap::paintEvent(QPaintEvent *)
{
    draw();
}

void MiniMap::draw()
{
    QPainter p(this); // used for text overlay

    // save the GL state set for QPainter
    saveGLState();

    // draw into the GL widget
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 10, 100);
    glTranslatef(0.0f, 0.0f, -15.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width(), height());
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, render_fbo->texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

// faire les draw par la...

    // restore the GL state that QPainter expects
    restoreGLState();

    // draw the overlayed text using QPainter
    p.setPen(QColor(197, 197, 197, 157));
    p.setBrush(QColor(197, 197, 197, 127));
    p.drawRect(QRect(0, 0, width(), height()));
    p.setPen(Qt::black);
    p.setBrush(Qt::NoBrush);
    const QString str1(tr("Y aura la minimap ici."));
    QFontMetrics fm(p.font());
    p.drawText(width()/2 - fm.width(str1)/2, 20, str1);
}

void MiniMap::mousePressEvent(QMouseEvent *e)
{
    anchor = e->pos();
}

void MiniMap::mouseMoveEvent(QMouseEvent *e)
{
    QPoint diff = e->pos() - anchor;
    if (e->buttons() & Qt::LeftButton) {
        tr_x += diff.y()/5.0f;
        tr_y += diff.x()/5.0f;
    }// else if (e->buttons() & Qt::RightButton) ...

    anchor = e->pos();
    draw();
}

void MiniMap::wheelEvent(QWheelEvent *e)
{
    e->delta() > 0 ? scale += scale*0.1f : scale -= scale*0.1f;
    draw();
}

void MiniMap::saveGLState()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
}

void MiniMap::restoreGLState()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}
