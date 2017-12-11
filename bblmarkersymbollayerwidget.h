#ifndef BBLMARKERSYMBOLLAYERWIDGET_H
#define BBLMARKERSYMBOLLAYERWIDGET_H

#include "qgssymbollayerv2widget.h"
#include "ui_bubbleswidgetbase.h"

class bbMarkerSymbolLayerWidgetP;

class bbMarkerSymbolLayerWidget : public QgsSymbolLayerV2Widget, private Ui::BubblesSetupWidget
{
        Q_OBJECT
public:
        bbMarkerSymbolLayerWidget(const QgsVectorLayer* vl, QWidget* parent = nullptr );
        ~bbMarkerSymbolLayerWidget(void);

        static QgsSymbolLayerV2Widget* create( const QgsVectorLayer* vl ) { return new bbMarkerSymbolLayerWidget( vl ); }

        // from base class
    virtual void setSymbolLayer( QgsSymbolLayerV2* layer ) override;
    virtual QgsSymbolLayerV2* symbolLayer() override;

private:
        bbMarkerSymbolLayerWidgetP *impl;
		friend class bbMarkerSymbolLayerWidgetP;

public slots:
        /*void scaleEdit_editingFinished();
        void titleEdit_editingFinished();
        void diagrammTypeChanged(int row);
        void diagrammType_EditTextChanged(QString text);
        void scaleTypeChanged(int);
        void maxDiagrammSize_ValueChanged(double);
        void minDiagrammSize_ValueChanged(double);
        void fluidItemClicked(QListWidgetItem*);
        void fluidItemRowChanged(int);
        void fluidBackColorClicked(QColor);
        void fluidLineColorClicked(QColor);

        void labelColorChanged(QColor color);*/
        void showLineoutsToggled(bool);
		void showLabelsToggled(bool);
		void showDiagrammsToggled(bool val);
        /*void perDayProdToggled(bool);
        void updateDataDefinedProperty();

        void expressionFieldChanged();
		void labelSizeValueChanged(double val);
		void decimalEditValueChanged(int val);
		void addToTemplateClicked();*/
};


#endif // BBLMARKERSYMBOLLAYERWIDGET_H
