#ifndef CRSIIS_H
#define CRSIIS_H

#include <QtWidgets/QMainWindow>
#include "ui_crsiis.h"
// Qt include
#include <QStackedWidget>



// QGis include
#include "qgspoint.h"
#include "qgsapplication.h"
#include <qgsmapcanvas.h>
#include <qgsmaplayer.h>
#include <qgslayertreeview.h>
#include <qgslayertreemapcanvasbridge.h>
#include <qgsscalecombobox.h>

class CRSIIS : public QMainWindow
{
	Q_OBJECT

public:
	CRSIIS(QWidget *parent = 0);
	~CRSIIS();

	void addDockWidget(Qt::DockWidgetArea area, QDockWidget* dockwidget);

	//! ���ֵ�һʵ��
	static inline CRSIIS *instance() { return sm_instance; };

public slots:
	void about_slot_function();
	void addRasterLayers();
	void addVectorLayers();
	void ROI_selction(); //����Ȥ����ѡȡ

	void autoSelectAddedLayer(QList<QgsMapLayer*> layers);

private:
	Ui::CRSIISClass ui;
	static CRSIIS* sm_instance;

	void setupConnections();

	QgsMapCanvas* m_mapCanvas;// ��ͼ����
	QList<QgsMapCanvasLayer> mapCanvasLayerSet;// ��ͼ�������õ�ͼ�㼯��

	//! ͼ�����
	QgsLayerTreeView* m_layerTreeView;
	QgsLayerTreeMapCanvasBridge *m_layerTreeCanvasBridge;

	QDockWidget *m_layerTreeDock;
	QDockWidget *m_layerOrderDock;

	QLabel* m_scaleLabel; // ��״̬������ʾ��scale 1:��
	QgsScaleComboBox* m_scaleEdit; //! ��״̬������ʾ������ֵ
	QProgressBar* m_progressBar;
	QLabel* m_coordsLabel; //! ��״̬����ʾ"Coordinate / Extent"
	QLineEdit* m_coordsEdit; //! ��״̬����ʾ��������

	QgsMessageBar* m_infoBar; // �ڵ�ͼ��������ʾ��Ϣ

	QStackedWidget* m_stackedWidget; // ����mapcanvas��mapComposor���л�
	//! Helps to make people dizzy
	QTimer* m_dizzyTimer;

	unsigned int m_MousePrecisionDecimalPlaces; // ָ���������С����λ��

	void initLayerTreeView();

	void createStatusBar();

private slots:
	//! ��ʾ���λ�õ�������
	void showMouseCoordinate(const QgsPoint &);
	void showScale(double );
	void userScale();

};

#endif // CRSIIS_H
