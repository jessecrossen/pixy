#ifndef SIGEDITDIALOG_H
#define SIGEDITDIALOG_H

#include <QDialog>
#include <QPixmap>
#include <QAbstractButton>
#include "monmodule.h"
#include "colorlut.h"

class Interpreter;

namespace Ui {
    class SigEditDialog;
    class SigModule;
}

// a peristant singleton to monitor and manage parameter changes
class SigModule : public MonModule
{
public:
    SigModule(Interpreter *interpreter);
    ~SigModule();
    // implement MonModule
    virtual bool render(uint32_t fourcc, const void *args[]);
    virtual void paramChange();
    // current signatures
    ColorSignature m_signatures[CL_NUM_SIGNATURES];
    // current signature range multipliers
    float m_ranges[CL_NUM_SIGNATURES];
    // minimum brightness
    int32_t m_yMin;
    // store the singleton instance of this class
    static SigModule *instance;
    // safely access properties from the static instance
    static ColorSignature getSignature(int i);
    static float getRange(int i);
    static int32_t getYMin();
    // update signatures
    void updateSignature(int i, ColorSignature sig, float range);
    void deleteSignature(int i);
};

class SigEditDialog : public QDialog
{
    Q_OBJECT

public:
    SigEditDialog(QWidget *parent, Interpreter *interpreter);
    ~SigEditDialog();

protected slots:
    // receive frame images from the renderer
    void handleImage(QImage image, uchar renderFlags);
    // save or discard edits
    void apply(QAbstractButton *button);
    virtual void accept();
    virtual void reject();

protected:
    // drawing
    virtual void paintEvent(QPaintEvent *event);
    void paintBackground(QPainter &painter);
    void paintSignature(QPainter &painter, int i);
    // mouse tracking
    QRect sigRect(ColorSignature sig);
    QRect effectiveSigRect(ColorSignature sig, float range);
    QRect outerSigRect(ColorSignature sig, float range);
    void rect2sig(QRect r, ColorSignature *sig);
    void xy2uv(QPointF xy, int32_t *u, int32_t *v);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    void updateHover(int32_t u, int32_t v);
    void updateDrag(int32_t u, int32_t v);
    virtual void keyPressEvent(QKeyEvent *event);
    // drag and selectiong state
    bool m_dragging;
    int m_dragIndex;
    bool m_moving;
    int32_t m_uOffset, m_vOffset;
    int m_selectIndex;

private:
    Ui::SigEditDialog *ui;

    // the connection to pixy
    Interpreter *m_interpreter;

    // signatures and ranges stored when the dialog loads
    ColorSignature m_initSignatures[CL_NUM_SIGNATURES];
    float m_initRanges[CL_NUM_SIGNATURES];
    void storeSignatures();
    void recallSignatures();

    // a list of video frames mapped into the uv colorspace
    QList<QImage> m_images;
    // the maximum number of trailing frames to show
    static const int MAX_IMAGES = 4;
    // the number of frames received from the renderer
    //  since the last flush
    int m_framecount;
};

#endif // SIGEDITDIALOG_H
