
#ifndef FINDBRANCHESDIALOG_H
#define FINDBRANCHESDIALOG_H

#include <QDialog>

class QLineEdit;

namespace dunnart {

class Canvas;
class DSBSpecies;

class FindBranchesDialog : public QDialog
{
    Q_OBJECT

public:
    FindBranchesDialog(Canvas *canvas, QWidget *parent = 0);

private slots:
    void findBranches();
    void canvasSelectionChanged();

private:
    void getSelectedSpecies();

    Canvas *m_canvas;
    QLineEdit *m_endpointEdit;
    QString m_endpointIDString;
    DSBSpecies *m_endpointSpecies;

};

}

#endif
