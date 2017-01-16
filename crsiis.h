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

	//! 保持单一实例
	static inline CRSIIS *instance() { return sm_instance; };

public slots:
	void about_slot_function();
	void addRasterLayers();
	void addVectorLayers();
	void ROI_selction(); //感兴趣区域选取

	void autoSelectAddedLayer(QList<QgsMapLayer*> layers);

private:
	Ui::CRSIISClass ui;
	static CRSIIS* sm_instance;

	void setupConnections();

	QgsMapCanvas* m_mapCanvas;// 地图画布
	QList<QgsMapCanvasLayer> mapCanvasLayerSet;// 地图画布所用的图层集合

	//! 图层管理
	QgsLayerTreeView* m_layerTreeView;
	QgsLayerTreeMapCanvasBridge *m_layerTreeCanvasBridge;

	QDockWidget *m_layerTreeDock;
	QDockWidget *m_layerOrderDock;

	QLabel* m_scaleLabel; // 在状态栏中显示“scale 1:”
	QgsScaleComboBox* m_scaleEdit; //! 在状态栏中显示比例尺值
	QProgressBar* m_progressBar;
	QLabel* m_coordsLabel; //! 在状态栏显示"Coordinate / Extent"
	QLineEdit* m_coordsEdit; //! 在状态栏显示地理坐标

	QgsMessageBar* m_infoBar; // 在地图窗口上显示信息

	QStackedWidget* m_stackedWidget; // 用于mapcanvas和mapComposor的切换
	//! Helps to make people dizzy
	QTimer* m_dizzyTimer;

	unsigned int m_MousePrecisionDecimalPlaces; // 指定鼠标坐标小数点位数

	void initLayerTreeView();

	void createStatusBar();

private slots:
	//! 显示鼠标位置地理坐标
	void showMouseCoordinate(const QgsPoint &);
	void showScale(double );
	void userScale();

};

#endif // CRSIIS_H
