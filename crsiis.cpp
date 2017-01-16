#include "crsiis.h"
#include "aboutdlg.h"

// Qt include
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QGridLayout>
#include <QFont>
#include <QLineEdit>
#include <QToolButton>
#include <QMargins>
#include <QProgressBar>
#include <QDockWidget>


// QGis include

#include <qgsvectorlayer.h>
#include <qgsmaplayerregistry.h>
#include <qgsrasterlayer.h>
#include <qgsproject.h>
#include <qgslayertreemodel.h>
#include <qgsapplication.h>
#include <qgslayertreelayer.h>
#include <qgslayertreegroup.h>
#include <qgslayertreeregistrybridge.h>
#include <qgslayertreeviewdefaultactions.h>
#include <qgsmaptoolselectutils.h>
#include <qgsrubberband.h>
#include <qgswkbtypes.h>

#include "qgis_devlayertreeviewmenuprovider.h"

CRSIIS* CRSIIS::sm_instance = 0;

CRSIIS::CRSIIS(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QgsApplication::setPluginPath(QCoreApplication::applicationDirPath() + "/plugins");
	QgsApplication::initQgis();

	//! 初始化map canvas
	m_mapCanvas = new QgsMapCanvas();
	m_mapCanvas->enableAntiAliasing(true);
	m_mapCanvas->setCanvasColor(QColor(255, 255, 255));

	//! 初始化图层管理器
	m_layerTreeView = new QgsLayerTreeView(this);
	initLayerTreeView();

	//! 初始化信息显示条
	m_infoBar = new QgsMessageBar(this);
	m_infoBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);


	//! 布局
	QWidget* centralWidget = this->centralWidget();
	QGridLayout* centralLayout = new QGridLayout(centralWidget);
	//centralLayout->addWidget(m_mapCanvas, 0, 0, 1, 1); // 被“m_stackedWidget->addWidget(m_mapCanvas);”代替
	//centralLayout->addWidget(m_layerTreeView, 0, 1, 1, 1); //用dockWidget窗口的话，就把这句注销掉，“initLayerTreeView();”可以实现图层管理
	//centralLayout->setColumnStretch(0, 3); //设置第一列的伸缩比例
	//centralLayout->setColumnStretch(1, 1); //设置第2列的伸缩比例，和上一句连起来的意思就：第一列和第二列的伸缩比例为3:1

	m_stackedWidget = new QStackedWidget(this);
	m_stackedWidget->setLayout(new QHBoxLayout());
	m_stackedWidget->addWidget(m_mapCanvas);

	centralLayout->addWidget(m_stackedWidget, 0, 0, 1, 1);
	centralLayout->addWidget(m_infoBar, 1, 0, 1, 1);

	createStatusBar();

	setupConnections();

}

CRSIIS::~CRSIIS()
{

}


void CRSIIS::setupConnections(){
	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(about_slot_function()));
	connect(ui.actionOpen_Raster, SIGNAL(triggered()), this, SLOT(addRasterLayers()));
	connect(ui.actionOpen_Vector, SIGNAL(triggered()), this, SLOT(addVectorLayers()));
	connect(ui.actionROI, SIGNAL(triggered()), this, SLOT(ROI_selction()));


	//显示坐标
	connect(m_mapCanvas, SIGNAL(xyCoordinates(const QgsPoint&)), this, SLOT(showMouseCoordinate(const QgsPoint&)));
	// 显示当前图层比例尺
	connect(m_mapCanvas, SIGNAL(scaleChanged(double)), this, SLOT(showScale(double)));


}


void CRSIIS::about_slot_function(){
	aboutDlg *newdlg = new aboutDlg;
	newdlg->show();

}

void CRSIIS::addRasterLayers(){

	QString filename = QFileDialog::getOpenFileName(this, tr("open vector"), QDir::currentPath(), tr("Image File(*.tif *.img *.bmp *.jpg *.png);;All Files(*.*)"));
	QDir::setCurrent(QFileInfo(filename).path());
	QStringList temp = filename.split(QDir::separator());
	QString basename = temp.at(temp.size() - 1);
	QgsRasterLayer* rasterLayer = new QgsRasterLayer(filename, basename, "gdal", false);
	if (!rasterLayer->isValid())
	{
		QMessageBox::critical(this, "error", "layer is invalid");
		return;
	}

	QgsMapLayerRegistry::instance()->addMapLayer(rasterLayer);
	mapCanvasLayerSet.append(rasterLayer);
	m_mapCanvas->setLayerSet(mapCanvasLayerSet);
	m_mapCanvas->setExtent(rasterLayer->extent());
	m_mapCanvas->enableAntiAliasing(true);
	m_mapCanvas->setCanvasColor(QColor(255, 255, 255));
	m_mapCanvas->freeze(false);
	m_mapCanvas->setLayerSet(mapCanvasLayerSet);
	m_mapCanvas->setVisible(true);
	m_mapCanvas->refresh();
}

void CRSIIS::addVectorLayers(){
	QString filename = QFileDialog::getOpenFileName(this, tr("open vector"), "", "*.shp");
	QDir::setCurrent(QFileInfo(filename).path());
	QStringList temp = filename.split(QDir::separator());
	QString basename = temp.at(temp.size() - 1);
	QgsVectorLayer* vecLayer = new QgsVectorLayer(filename, basename, "ogr", false);
	if (!vecLayer->isValid())
	{
		QMessageBox::critical(this, "error", "layer is invalid");
		return;
	}

	QgsMapLayerRegistry::instance()->addMapLayer(vecLayer);
	mapCanvasLayerSet.append(vecLayer);
	m_mapCanvas->setExtent(vecLayer->extent());
	m_mapCanvas->setLayerSet(mapCanvasLayerSet);
	m_mapCanvas->setVisible(true);
	m_mapCanvas->freeze(false);
	m_mapCanvas->refresh();
}

void CRSIIS::initLayerTreeView(){
	QgsLayerTreeModel* model = new QgsLayerTreeModel(QgsProject::instance()->layerTreeRoot(), this);
	model->setFlag(QgsLayerTreeModel::AllowNodeRename);
	model->setFlag(QgsLayerTreeModel::AllowNodeReorder);
	model->setFlag(QgsLayerTreeModel::AllowNodeChangeVisibility);
	model->setFlag(QgsLayerTreeModel::ShowLegendAsTree);
	model->setAutoCollapseLegendNodes(10);
	m_layerTreeView->setModel(model);
	// 添加右键菜单
	m_layerTreeView->setMenuProvider(new qgis_devLayerTreeViewMenuProvider(m_layerTreeView, m_mapCanvas));

	connect(QgsProject::instance()->layerTreeRegistryBridge(), SIGNAL(addedLayersToLayerTree(QList<QgsMapLayer*>)),
		this, SLOT(autoSelectAddedLayer(QList<QgsMapLayer*>)));

	// 设置这个路径是为了获取图标文件
	QString iconDir = "../Resources/images/themes/default/";

	// add group tool button
	QToolButton * btnAddGroup = new QToolButton();
	btnAddGroup->setAutoRaise(true);
	btnAddGroup->setIcon(QIcon(iconDir + "mActionAdd.png"));

	// expand / collapse tool buttons
	QToolButton* btnExpandAll = new QToolButton();
	btnExpandAll->setAutoRaise(true);
	btnExpandAll->setIcon(QIcon(iconDir + "mActionExpandTree.png"));
	btnExpandAll->setToolTip(tr("Expand All"));
	connect(btnExpandAll, SIGNAL(clicked()), m_layerTreeView, SLOT(expandAll()));

	QToolButton* btnCollapseAll = new QToolButton();
	btnCollapseAll->setAutoRaise(true);
	btnCollapseAll->setIcon(QIcon(iconDir + "mActionCollapseTree.png"));
	btnCollapseAll->setToolTip(tr("Collapse All"));
	connect(btnCollapseAll, SIGNAL(clicked()), m_layerTreeView, SLOT(collapseAll()));

	// remove item button
	QToolButton* btnRemoveItem = new QToolButton();
	// btnRemoveItem->setDefaultAction( this->m_actionRemoveLayer );
	btnRemoveItem->setAutoRaise(true);

	// 按钮布局
	QHBoxLayout* toolbarLayout = new QHBoxLayout();
	toolbarLayout->setContentsMargins(QMargins(5, 0, 5, 0));
	toolbarLayout->addWidget(btnAddGroup);
	toolbarLayout->addWidget(btnCollapseAll);
	toolbarLayout->addWidget(btnExpandAll);
	toolbarLayout->addWidget(btnRemoveItem);
	toolbarLayout->addStretch();

	QVBoxLayout* vboxLayout = new QVBoxLayout();
	vboxLayout->setMargin(0);
	vboxLayout->addLayout(toolbarLayout);
	vboxLayout->addWidget(m_layerTreeView);

	// 装进dock widget中
	m_layerTreeDock = new QDockWidget(tr("Layers"), this);
	m_layerTreeDock->setObjectName("Layers");
	m_layerTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	QWidget* w = new QWidget();
	w->setLayout(vboxLayout);
	m_layerTreeDock->setWidget(w);
	addDockWidget(Qt::LeftDockWidgetArea, m_layerTreeDock);

	// 连接地图画布和图层管理器
	m_layerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge(QgsProject::instance()->layerTreeRoot(), m_mapCanvas, this);
	connect(QgsProject::instance(), SIGNAL(writeProject(QDomDocument&)), m_layerTreeCanvasBridge, SLOT(writeProject(QDomDocument&)));
	connect(QgsProject::instance(), SIGNAL(readProject(QDomDocument)), m_layerTreeCanvasBridge, SLOT(readProject(QDomDocument)));

}

void CRSIIS::autoSelectAddedLayer(QList<QgsMapLayer*> layers)
{
	if (layers.count())
	{
		QgsLayerTreeLayer* nodeLayer = QgsProject::instance()->layerTreeRoot()->findLayer(layers[0]->id());

		if (!nodeLayer)
		{
			return;
		}

		QModelIndex index = m_layerTreeView->layerTreeModel()->node2index(nodeLayer);
		m_layerTreeView->setCurrentIndex(index);
	}
}

void CRSIIS::createStatusBar()
{
	statusBar()->setStyleSheet("QStatusBar::item {border: none;}");// 

	//! 添加进度条
	m_progressBar = new QProgressBar(statusBar());
	m_progressBar->setObjectName("m_progressBar");
	m_progressBar->setMaximum(100);
	m_progressBar->hide(); // 事先隐藏，后面用到时再显示
	statusBar()->addPermanentWidget(m_progressBar, 1); //给状态栏添加一个永久窗口部件
	connect(m_mapCanvas, SIGNAL(renderStarting()), this, SLOT(canvasRefreshStarted()));
	connect(m_mapCanvas, SIGNAL(mapCanvasRefreshed()), this, SLOT(canvasRefreshFinished()));

	QFont myFont("Arial", 9);
	statusBar()->setFont(myFont);

	//! 添加坐标显示标签
	m_coordsLabel = new QLabel(QString(), statusBar());
	m_coordsLabel->setObjectName("m_coordsLabel");
	m_coordsLabel->setFont(myFont);
	m_coordsLabel->setMinimumWidth(10);
	m_coordsLabel->setMargin(3);
	m_coordsLabel->setAlignment(Qt::AlignCenter);
	m_coordsLabel->setFrameStyle(QFrame::NoFrame);
	m_coordsLabel->setText(tr("Coordinate:"));
	m_coordsLabel->setToolTip(tr("Current map coordinate"));
	statusBar()->addPermanentWidget(m_coordsLabel, 0);

	//! 坐标编辑标签
	m_coordsEdit = new QLineEdit(QString(), statusBar());
	m_coordsEdit->setObjectName("m_coordsEdit");
	m_coordsEdit->setFont(myFont);
	m_coordsEdit->setMinimumWidth(10);
	m_coordsEdit->setMaximumWidth(300);
	m_coordsEdit->setContentsMargins(0, 0, 0, 0);
	m_coordsEdit->setAlignment(Qt::AlignCenter);
	statusBar()->addPermanentWidget(m_coordsEdit, 0);
	connect(m_coordsEdit, SIGNAL(returnPressed()), this, SLOT(userCenter()));

	m_dizzyTimer = new QTimer(this);
	connect(m_dizzyTimer, SIGNAL(timeout()), this, SLOT(dizzy()));

	//! 比例尺标签
	m_scaleLabel = new QLabel(QString(), statusBar());
	m_scaleLabel->setObjectName("m_scaleLabel");
	m_scaleLabel->setFont(myFont);
	m_scaleLabel->setMinimumWidth(10);
	m_scaleLabel->setMargin(3);
	m_scaleLabel->setAlignment(Qt::AlignCenter);
	m_scaleLabel->setFrameStyle(QFrame::NoFrame);
	m_scaleLabel->setText(tr("Scale"));
	m_scaleLabel->setToolTip(tr("Current map scale"));
	statusBar()->addPermanentWidget(m_scaleLabel, 0);

	m_scaleEdit = new QgsScaleComboBox(statusBar());
	m_scaleEdit->setObjectName("m_scaleEdit");
	m_scaleEdit->setFont(myFont);
	m_scaleEdit->setMinimumWidth(10);
	m_scaleEdit->setMaximumWidth(100);
	m_scaleEdit->setContentsMargins(0, 0, 0, 0);
	m_scaleEdit->setToolTip(tr("Current map scale (formatted as x:y)"));
	statusBar()->addPermanentWidget(m_scaleEdit, 0);
	connect(m_scaleEdit, SIGNAL(scaleChanged()), this, SLOT(userScale()));

}

void CRSIIS::addDockWidget(Qt::DockWidgetArea area, QDockWidget* dockwidget)
{
	QMainWindow::addDockWidget(area, dockwidget);
	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	dockwidget->show();
	m_mapCanvas->refresh();
}


void CRSIIS::showMouseCoordinate(const QgsPoint &p)
{
	if (m_mapCanvas->mapUnits() == QgsUnitTypes::DistanceDegrees) // 坐标度分秒的显示方式
	{
		QString format = QgsProject::instance()->readEntry("PositionPrecision", "/DegreeFormat", "D");
		if (format == "DM") // 只显示度和分
		{
			m_coordsEdit->setText(p.toDegreesMinutes(m_MousePrecisionDecimalPlaces));
		}
		else if (format == "DMS") // 显示度分秒
		{
			m_coordsEdit->setText(p.toDegreesMinutesSeconds(m_MousePrecisionDecimalPlaces));
		}
		else
		{
			m_coordsEdit->setText(p.toString(m_MousePrecisionDecimalPlaces));
		}
	}
	else
	{
		m_coordsEdit->setText(p.toString(m_MousePrecisionDecimalPlaces));
	}

	if (m_coordsEdit->width() > m_coordsEdit->minimumWidth())
	{
		m_coordsEdit->setMinimumWidth(m_coordsEdit->width());
	}
}


void CRSIIS::showScale(double theScale)
{
	m_scaleEdit->setScale(1.0 / theScale);
	if (m_scaleEdit->width() > m_scaleEdit->minimumWidth())
	{
		m_scaleEdit->setMinimumWidth(m_scaleEdit->width());
	}
}


void CRSIIS::userScale()
{
	m_mapCanvas->zoomScale(1.0 / m_scaleEdit->scale());
}



// 待完善,目前只能显示固定的临时图形
void CRSIIS::ROI_selction( ){
	
	QgsRubberBand *pRubber = new QgsRubberBand(m_mapCanvas, QgsWkbTypes::LineGeometry);

	pRubber->setColor(QColor(0, 255, 0));
	pRubber->setWidth(2);
	const QgsMapToPixel*pM2PTransform = m_mapCanvas->getCoordinateTransform();
	QgsPoint pt1 = pM2PTransform->toMapCoordinates(0, 0);
	QgsPoint pt2 = pM2PTransform->toMapCoordinates(100, 0);
	QgsPoint pt3 = pM2PTransform->toMapCoordinates(100, 100);
	QgsPoint pt4 = pM2PTransform->toMapCoordinates(0, 100);
	pRubber->addPoint(pt1, false);
	pRubber->addPoint(pt2, false);
	pRubber->addPoint(pt3, false);
	pRubber->addPoint(pt4, true);



}