
#ifndef FINDBRANCHESDIALOG_H
#define FINDBRANCHESDIALOG_H

#include <QDialog>

class QLineEdit;
class QComboBox;

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
    void test();

private:
    void getSelectedClone();

    Canvas *m_canvas;
    QLineEdit *m_endpointEdit;
    QComboBox *m_endpointCBox;
    QComboBox *m_layoutCBox;
    QString m_endpointIDString;
    DSBClone *m_endpointClone;

};

}

#endif
