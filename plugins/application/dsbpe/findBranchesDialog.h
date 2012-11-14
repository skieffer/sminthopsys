
#ifndef FINDBRANCHESDIALOG_H
#define FINDBRANCHESDIALOG_H

#include <QDialog>

class QLineEdit;

namespace dunnart {

class Canvas;
class DSBClone;

class FindBranchesDialog : public QDialog
{
    Q_OBJECT

public:
    FindBranchesDialog(Canvas *canvas, QWidget *parent = 0);

private slots:
    void findBranches();
    void canvasSelectionChanged();

private:
    void getSelectedClone();

    Canvas *m_canvas;
    QLineEdit *m_endpointEdit;
    QString m_endpointIDString;
    DSBClone *m_endpointClone;

};

}

#endif
