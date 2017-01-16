#include "qgis_devlayertreeviewmenuprovider.h"
#include "crsiis.h"

#include <QMenu>
#include <QModelIndex>
#include <QIcon>
#include <QAction>
#include <QMap>

// QGis include
#include <qgslayertreeviewdefaultactions.h>
#include <qgslayertreenode.h>
#include <qgslayertreemodel.h>
#include <qgslayertree.h>
#include <qgsrasterlayer.h>

qgis_devLayerTreeViewMenuProvider::qgis_devLayerTreeViewMenuProvider( QgsLayerTreeView *view, QgsMapCanvas* canvas )
    : mView( view )
    , mCanvas( canvas )
{
}

qgis_devLayerTreeViewMenuProvider::~qgis_devLayerTreeViewMenuProvider()
{

}

QMenu* qgis_devLayerTreeViewMenuProvider::createContextMenu()
{
    QMenu* menu = new QMenu;

    QgsLayerTreeViewDefaultActions* actions = mView->defaultActions();

    QModelIndex idx = mView->currentIndex();

    // global menu
    if ( !idx.isValid() )
    {
        menu->addAction( actions->actionAddGroup( menu ) );
        //menu->addAction( CRSIIS::getThemeIcon( "mActionExpandTree.png" ), tr( "&Expand All" ), mView, SLOT( expandAll() ) );
        //menu->addAction( CRSIIS::getThemeIcon( "mActionCollapseTree.png" ), tr( "&Collapse All" ), mView, SLOT( collapseAll() ) );
    }
    else if ( QgsLayerTreeNode* node = mView->layerTreeModel()->index2node( idx ) )
    {
        // layer or group selected
        if ( QgsLayerTree::isGroup( node ) )
        {
            menu->addAction( actions->actionZoomToGroup( mCanvas, menu ) );
            //menu->addAction( CRSIIS::getThemeIcon( "mActionRemoveLayer.svg" ), tr( "&Remove" ), CRSIIS::instance(), SLOT( removeLayer() ) );
            menu->addAction( actions->actionRenameGroupOrLayer( menu ) );
            if ( mView->selectedNodes( true ).count() >= 2 )
            {
                menu->addAction( actions->actionGroupSelected( menu ) );
            }
            menu->addAction( actions->actionAddGroup( menu ) );
        }
        else if ( QgsLayerTree::isLayer( node ) )
        {
            QgsMapLayer* layer = QgsLayerTree::toLayer( node )->layer();
            menu->addAction( actions->actionZoomToLayer( mCanvas, menu ) );
            menu->addAction( actions->actionShowInOverview( menu ) );

            // 属性表菜单
            //menu->addAction( CRSIIS::getThemeIcon( "mActionOpenTable.png" ), tr( "&Open Attribute Table" ), CRSIIS::instance(), SLOT( openAttributeTableDialog() ) );

            // 如果选择的是栅格图层
            if ( layer && layer->type() == QgsMapLayer::RasterLayer )
            {
                menu->addAction( tr( "&Zoom to Best Scale (100%)" ), CRSIIS::instance(), SLOT( legendLayerZoomNative() ) );

                QgsRasterLayer* rasterLayer =  qobject_cast<QgsRasterLayer *>( layer );
                if ( rasterLayer && rasterLayer->rasterType() != QgsRasterLayer::Palette )
                {
                    menu->addAction( tr( "&Stretch Using Current Extent" ), CRSIIS::instance(), SLOT( legendLayerStretchUsingCurrentExtent() ) );
                }
            }

            //menu->addAction( CRSIIS::getThemeIcon( "mActionRemoveLayer.svg" ), tr( "&Remove" ), CRSIIS::instance(), SLOT( removeLayer() ) );

            // 样式表
            //menu->addAction( CRSIIS::getThemeIcon( "mActionRemoveLayer.svg" ), tr( "&Property" ), CRSIIS::instance(), SLOT( showProperty() ) );
        }
    }

    return menu;
}

void qgis_devLayerTreeViewMenuProvider::addLegendLayerAction( QAction * action, QString menu, QString id, QgsMapLayer::LayerType type, bool allLayers )
{
    mLegendLayerActionMap[type].append( LegendLayerAction( action, menu, id, allLayers ) );
}

void qgis_devLayerTreeViewMenuProvider::addLegendLayerActionForLayer( QAction * action, QgsMapLayer * layer )
{
    if ( !action || !layer ) { return; }

    legendLayerActions( layer->type() );
    if ( !mLegendLayerActionMap.contains( layer->type() ) ) {return;}

    QMap<QgsMapLayer::LayerType, QList<LegendLayerAction>>::iterator it = mLegendLayerActionMap.find( layer->type() );
    for ( int i = 0; i < it->count(); i++ )
    {
        if ( ( *it )[i].action == action )
        {
            ( ( *it )[i] ).layers.append( layer );
            return;
        }
    }
}

void qgis_devLayerTreeViewMenuProvider::removeLegendLayerActionsForLayer( QgsMapLayer * layer )
{
    if ( !layer || !mLegendLayerActionMap.contains( layer->type() ) ) {return;}

    QMap<QgsMapLayer::LayerType, QList<LegendLayerAction>>::iterator it = mLegendLayerActionMap.find( layer->type() );
    for ( int i = 0; i < it->count(); i++ )
    {
        ( *it )[i].layers.removeAll( layer );
    }


}

QList< LegendLayerAction > qgis_devLayerTreeViewMenuProvider::legendLayerActions( QgsMapLayer::LayerType type ) const
{
    return mLegendLayerActionMap.contains( type ) ? mLegendLayerActionMap.value( type ) : QList<LegendLayerAction>();
}
