#include "crsiis.h"
#include <QtWidgets/QApplication>
#include<qgsapplication.h>  
#include<qgsproviderregistry.h>  
#include<qgssinglesymbolrenderer.h>  
#include<qgsmaplayerregistry.h>  
#include<qgsvectorlayer.h>  
#include<qgsmapcanvas.h>  
#include<QString>  
#include<QApplication>  
#include<QWidget>  

#include<QMessageBox>  
#include<QObject>  
#include <QList>  
#include <QFileInfoList>  
#include <QDir>  
#include <QLibrary>  
#include <QDebug>  

int main(int argc, char *argv[])
{
	//QApplication a(argc, argv);
	QgsApplication myApp(argc, argv, true);
	//QgsApplication::setPrefixPath("D:/coding/PublicLibs/QGISDev", true); // 注意：把这里的路径改成你电脑上Qgis的路径！！！
	QgsApplication::setPluginPath(QCoreApplication::applicationDirPath() + "/plugins");
	QgsApplication::initQgis();

	
	

	/*CRSIIS w;
	w.show();*/
	//return a.exec();
	CRSIIS *myQgis = new CRSIIS;
	myQgis->show();
	return myApp.exec();

	//QgsApplication myApp(argc, argv, true);
	////QgsApplication::setPrefixPath("D:/coding/PublicLibs/QGISDev", true);
	//QgsApplication::setPluginPath(QCoreApplication::applicationDirPath() + "/plugins");
	//QString pluginPath = QgsApplication::pluginPath();
	//QgsApplication::initQgis();

	//QgsProviderRegistry* provider = QgsProviderRegistry::instance();

	//QString myLayerPath = "D:\\data\\forPaper\\Landsat\\Beijing\\Beijing061503.shp";
	//QString myLayerBaseName = "airports"; //图层名称;  

	//QList<QgsMapLayer*> myList;
	//QgsVectorLayer* mypLayer = new QgsVectorLayer(myLayerPath, myLayerBaseName, "ogr", false);
	//if (!mypLayer)
	//{
	//	return 0;
	//}
	//if (mypLayer->isValid())
	//{
	//	QMessageBox::information(0, "", "layer is valid");
	//	mypLayer->setProviderEncoding("System");
	//	myList << mypLayer;
	//}
	//QgsMapLayerRegistry::instance()->addMapLayer(mypLayer);
	//QList<QgsMapCanvasLayer> myLayerSet;
	//myLayerSet.append(QgsMapCanvasLayer(mypLayer));

	//QgsMapCanvas* mypMapCanvas = new QgsMapCanvas(0);
	//mypMapCanvas->setExtent(mypLayer->extent());
	//mypMapCanvas->enableAntiAliasing(true);
	//mypMapCanvas->setCanvasColor(QColor(255, 255, 255));
	//mypMapCanvas->freeze(false);
	//mypMapCanvas->setLayerSet(myLayerSet);
	//mypMapCanvas->setVisible(true);
	//mypMapCanvas->refresh();

	//return myApp.exec();




}
