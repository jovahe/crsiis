#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <QDialog>
#include "ui_aboutdlg.h"

class aboutDlg : public QDialog
{
	Q_OBJECT

public:
	aboutDlg(QWidget *parent = 0);
	~aboutDlg();

private:
	Ui::aboutDlg ui;
};

#endif // ABOUTDLG_H
