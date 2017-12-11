#include "bblmarkersymbollayerwidget.h"
#include "bblmarkersymbollayer.h"
#include "qgssymbollayerv2utils.h"

#include <QHash>
#include <QSettings>
//#include <qgisapp.h>

class bbMarkerSymbolLayerWidgetP
{
public:
    bblMarkerSimbolLayer *mLayer;
    bbMarkerSymbolLayerWidget *form;

    bbMarkerSymbolLayerWidgetP(bbMarkerSymbolLayerWidget *f) :
            mLayer(nullptr), form(f)
    {
    }

	void initWidgets()
	{
		if(!mLayer) return;

		//form->componentsList->clear();

		////                QVector<qreal> checks;
		//QgsFluidList fluidCodes = mLayer->fluidCodes();
		//int i = 1;
		//Q_FOREACH(FLUID_CODES value, fluidCodes)
		//{
		//	QListWidgetItem *item = new QListWidgetItem(form->componentsList);
		//	item->setText(QString("%1. %2").arg(i).arg(value.name));
		//	item->setData(Qt::UserRole, value.code);
		//	item->setCheckState(Qt::Unchecked);

		//	form->componentsList->addItem(item);
		//	i++;
		//}

		//                form->templateExpression->registerGetExpressionContextCallback( &bbMarkerSymbolLayerWidgetP::_getExpressionContext, form->vectorLayer() );
		//                form->templateExpression->setLayer( const_cast<QgsVectorLayer*>(form->mVectorLayer) );
		//                form->templateExpression->setField(mLayer->labelTemplate());
		/*form->templateExpression->setText(mLayer->labelTemplate());

		form->labelSize->blockSignals(true);
		form->labelSize->setValue(mLayer->labelSize());
		form->labelSize->blockSignals(false);

		form->decimalEdit->blockSignals(true);
		form->decimalEdit->setValue(mLayer->decimal());
		form->decimalEdit->blockSignals(false);

		form->dailyProduction->blockSignals(true);
		form->dailyProduction->setChecked(mLayer->dailyProduction());
		form->dailyProduction->blockSignals(false);*/

		form->showLineouts->blockSignals(true);
		form->showLineouts->setChecked(mLayer->showLineout());
		form->showLineouts->blockSignals(false);

		form->showLabels->blockSignals(true);
		form->showLabels->setChecked(mLayer->showLabels());
		form->showLabels->blockSignals(false);

		form->showDiagramms->blockSignals(true);
		form->showDiagramms->setChecked(mLayer->showDiagramms());
		form->showDiagramms->blockSignals(false);

		/*QObject::connect(form->templateExpression,  SIGNAL(editingFinished()), form, SLOT(expressionFieldChanged()));*/
		QObject::connect(form->showLineouts,  SIGNAL(toggled(bool)), form, SLOT(showLineoutsToggled(bool)));
		QObject::connect(form->showLabels,  SIGNAL(toggled(bool)), form, SLOT(showLabelsToggled(bool)));
		QObject::connect(form->showDiagramms,  SIGNAL(toggled(bool)), form, SLOT(showDiagrammsToggled(bool)));
		/*QObject::connect(form->dailyProduction,  SIGNAL(toggled(bool)), form, SLOT(perDayProdToggled(bool)));
		QObject::connect(form->labelSize,  SIGNAL(valueChanged(double)), form, SLOT(labelSizeValueChanged(double)));
		QObject::connect(form->decimalEdit,  SIGNAL(valueChanged(int)), form, SLOT(decimalEditValueChanged(int)));
		QObject::connect(form->addToTemplate,  SIGNAL(clicked()), form, SLOT(addToTemplateClicked()));*/
	}


	void diagrammTypeChanged(QString code)
	{
//		if(!mLayer) return;

//		QVector<qreal> checks = mLayer->standardDiagramms().value(code, QVector<qreal>(mLayer->fluidCodes().count(), 0));
//		for(int i=0; i<checks.count(); i++)
//		{
//			bool state = checks[i];
//			form->componentsList->item(i)->setCheckState(state? Qt::Checked : Qt::Unchecked);
//		}
//        form->scaleEdit->setValue(mLayer->diagrammScales()[code]);
	}

//        static QgsExpressionContext _getExpressionContext( const void* context )
//        {

//        }

//        void registerDataDefinedButton( QgsDataDefinedButton * button, const QString & propertyName, QgsDataDefinedButton::DataType type, const QString & description )
//        {
//          const QgsDataDefined* dd = mLayer->getDataDefinedProperty( propertyName );
//          button->init( form->vectorLayer(), dd, type, description );
//          button->setProperty( "propertyName", propertyName );
//          QObject::connect( button, SIGNAL( dataDefinedChanged( const QString& ) ), form, SLOT( updateDataDefinedProperty() ) );
//          QObject::connect( button, SIGNAL( dataDefinedActivated( bool ) ), form, SLOT( updateDataDefinedProperty() ) );

//          button->registerGetExpressionContextCallback( &bbMarkerSymbolLayerWidgetP::_getExpressionContext, this );
//        }

};

bbMarkerSymbolLayerWidget::bbMarkerSymbolLayerWidget(const QgsVectorLayer* vl, QWidget* parent) :
        QgsSymbolLayerV2Widget( parent, vl ),
        impl(new bbMarkerSymbolLayerWidgetP(this))
{
    setupUi( this );
}


bbMarkerSymbolLayerWidget::~bbMarkerSymbolLayerWidget(void)
{
    delete impl;
}

void bbMarkerSymbolLayerWidget::setSymbolLayer( QgsSymbolLayerV2* layer )
{
	if ( !layer || (layer && layer->layerType() != "BubbleMarker") )
	{
		return;
	}

	impl->mLayer = static_cast<bblMarkerSimbolLayer*>( layer );

    impl->initWidgets();
}

QgsSymbolLayerV2* bbMarkerSymbolLayerWidget::symbolLayer()
{
    return impl->mLayer;
}

//void bbMarkerSymbolLayerWidget::scaleEdit_editingFinished()
//{
////        QString code = diagrammType->itemData(diagrammType->currentIndex()).toString();
//
////        impl->mLayer->diagrammScales()[code] = scaleEdit->value();
//
////        emit changed();
//}
//
//void bbMarkerSymbolLayerWidget::titleEdit_editingFinished()
//{
//        emit changed();
//}
//
//void bbMarkerSymbolLayerWidget::diagrammTypeChanged(int row)
//{
////	if(row >=0 && row < diagrammType->count() && impl->mLayer)
////	{
////		QString code = diagrammType->itemData(row).toString();
////		impl->diagrammTypeChanged(code);
////		impl->mLayer->setDiagrammType(code);
//
////		emit changed();
////	}
//}
//
//void bbMarkerSymbolLayerWidget::diagrammType_EditTextChanged(QString text)
//{
////        QString code = diagrammType->itemData(diagrammType->currentIndex()).toString();
////        impl->mLayer->diagrammTypes()[code] = text;
//
////	emit changed();
//}
//
//void bbMarkerSymbolLayerWidget::scaleTypeChanged(int row)
//{
////	if(impl->mLayer)
////	{
////		impl->mLayer->setScaleTypeId(row);
////		emit changed();
////	}
//}
//
//void bbMarkerSymbolLayerWidget::maxDiagrammSize_ValueChanged(double val)
//{
////	if(impl->mLayer)
////	{
////		impl->mLayer->setMaxDiagrammSize(val);
////		emit changed();
////	}
//}
//
//void bbMarkerSymbolLayerWidget::minDiagrammSize_ValueChanged(double val)
//{
////	if(impl->mLayer)
////	{
////		impl->mLayer->setMinDiagrammSize(val);
////		emit changed();
////	}
//}
//
//void bbMarkerSymbolLayerWidget::fluidItemClicked(QListWidgetItem *item)
//{
//	if(!item || impl->mLayer) return;
//
//
//}
//
//void bbMarkerSymbolLayerWidget::fluidItemRowChanged(int row)
//{
//	if(!impl->mLayer) return;
//
//        fluidBackColor->blockSignals(true);
//        fluidLineColor->blockSignals(true);
//        labelColor->blockSignals(true);
//
//        fluidBackColor->setColor(impl->mLayer->fluidCodes()[row].backColor);
//        fluidLineColor->setColor(impl->mLayer->fluidCodes()[row].lineColor);
//        labelColor->setColor(impl->mLayer->fluidCodes()[row].labelColor);
//
//        fluidBackColor->blockSignals(false);
//        fluidLineColor->blockSignals(false);
//        labelColor->blockSignals(false);
//}
//
//void bbMarkerSymbolLayerWidget::fluidBackColorClicked(QColor color)
//{
//        if(!impl->mLayer) return;
//
//        if(componentsList->currentRow() >= 0)
//        {
//                impl->mLayer->fluidCodes()[componentsList->currentRow()].backColor = color;
//                emit changed();
//        }
//        QgsDebugMsg(QgsSymbolLayerV2Utils::encodeColor(color));
//}
//
//void bbMarkerSymbolLayerWidget::fluidLineColorClicked(QColor color)
//{
//        if(!impl->mLayer) return;
//
//        if(componentsList->currentRow() >= 0)
//        {
//                impl->mLayer->fluidCodes()[componentsList->currentRow()].lineColor = color;
//                emit changed();
//        }
//
//        QgsDebugMsg(QgsSymbolLayerV2Utils::encodeColor(color));
//}
//
//void bbMarkerSymbolLayerWidget::labelColorChanged(QColor color)
//{
//    if(!impl->mLayer) return;
//
//    if(componentsList->currentRow() >= 0)
//    {
//        impl->mLayer->fluidCodes()[componentsList->currentRow()].labelColor = color;
//        emit changed();
//    }
//}


void bbMarkerSymbolLayerWidget::showLineoutsToggled(bool val)
{
	if(!impl->mLayer) return;

	impl->mLayer->setShowLineout(val);
	emit changed();
}

void bbMarkerSymbolLayerWidget::showLabelsToggled(bool val)
{
	if(!impl->mLayer) return;

	impl->mLayer->setShowLabels(val);
	emit changed();
}

void bbMarkerSymbolLayerWidget::showDiagrammsToggled(bool val)
{
	if(!impl->mLayer) return;

	impl->mLayer->setShowDiagramms(val);
	emit changed();
}

//void bbMarkerSymbolLayerWidget::perDayProdToggled(bool val)
//{
//	if(!impl->mLayer) return;
//
//	impl->mLayer->setDailyProduction(val);
//	emit changed();
//}
//
//
//void bbMarkerSymbolLayerWidget::updateDataDefinedProperty()
//{
//	if(!impl->mLayer) return;
//}
//
//void bbMarkerSymbolLayerWidget::expressionFieldChanged()
//{
//    if(!impl->mLayer) return;
//
//    impl->mLayer->setLabelTemplate(templateExpression->text());
//    emit changed();
//}
//
//void bbMarkerSymbolLayerWidget::labelSizeValueChanged(double val)
//{
//	if(!impl->mLayer) return;
//
//	impl->mLayer->setLabelSize(val);
//	emit changed();
//}
//
//void bbMarkerSymbolLayerWidget::decimalEditValueChanged(int val)
//{
//	if(!impl->mLayer) return;
//
//	impl->mLayer->setDecimal(val);
//	emit changed();
//}
//
//void bbMarkerSymbolLayerWidget::addToTemplateClicked()
//{
//	if(!impl->mLayer) return;
//}