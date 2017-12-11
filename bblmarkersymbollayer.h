/***************************************************************************
    bubbles.h
    -------------------
    begin                : Jan 21, 2004
    copyright            : (C) 2004 by Tim Sutton
    email                : tim@linfiniti.com

 ***************************************************************************/

#ifndef BBLMARKERSIMBOLLAYER_H
#define BBLMARKERSIMBOLLAYER_H

#include "qgssymbollayerv2.h"

#define LIQUID_PRODUCTION       "1_liquidproduction"
#define LIQUID_INJECTION        "2_liquidinjection"
#define GAS_PRODUCTION          "3_gasproduction"
#define CONDENSAT_PRODUCTION    "4_condensatproduction"

typedef struct
{
  QString name;
  QString code;
  QColor backColor;
  QColor lineColor;
  QColor labelColor;
  bool inPercent;
} FLUID_CODES;

typedef struct
{
  QString code;
  int isFlowing;
  int isPump;
} BBL_LIFT_METHOD;

typedef struct
{
	QString name;
	qreal scale;
	int unitsType;
        int units;
	QVector<qreal> fluids;
} BBL_DIAGRAMM;

typedef QMap<QString, BBL_DIAGRAMM> DiagrammMap;
typedef QMap<QString, qreal> QgsRealMap;
typedef QList<FLUID_CODES> QgsFluidList;
typedef QMap<QString, FLUID_CODES> QgsFluidMap;

class bblMarkerSimbolLayer : public QgsMarkerSymbolLayerV2
{
public:
   
	enum BubbleDiagrammType
	{
		liquid = 0,
		injection,
		gas,
		condensat
	};

    bblMarkerSimbolLayer(const QgsStringMap& properties = QgsStringMap());
    ~bblMarkerSimbolLayer();

    QString layerType() const override;

    static QgsSymbolLayerV2* create( const QgsStringMap& properties = QgsStringMap() );
    static QgsSymbolLayerV2* createFromSld( QDomElement &element );
    bblMarkerSimbolLayer* clone() const override;

	void setProperties(const QgsStringMap& properties);

    /*bool setSubSymbol( QgsSymbolV2* symbol ) override;
    QgsSymbolV2* subSymbol() override;*/

    void setColor( const QColor& color ) override;
    virtual QColor color() const override;

    void renderPoint( QPointF point, QgsSymbolV2RenderContext& context ) override;
    void startRender( QgsSymbolV2RenderContext& context ) override;
    void stopRender( QgsSymbolV2RenderContext& context ) override;

    QgsStringMap properties() const override;

    //! Writes the SLD element following the SLD v1.1 specs
    void toSld( QDomDocument& doc, QDomElement &element, const QgsStringMap& props ) const override;

    //void drawPreviewIcon( QgsSymbolV2RenderContext& context, QSize size ) override;

    QSet<QString> usedAttributes() const override;

	//QRectF bounds( QPointF point, QgsSymbolV2RenderContext& context ) override;

    QgsFluidList &fluidCodes() const;
	DiagrammMap &standardDiagramms();
	//QgsStringMap &diagrammTypes() const;
	//QgsRealMap &diagrammScales() const;


    //setters and getters
	void setDiagrammType(const QString &dType);
	QString diagrammType();

	void setScaleTypeId(int sct);
	int scaleTypeId() const;

	void setMinDiagrammSize(qreal val);
	qreal minDiagrammSize() const;

	void setMaxDiagrammSize(qreal val);
	qreal maxDiagrammSize() const;

        QString labelTemplate() const;
        void setLabelTemplate(const QString &val);

	double labelSize() const;
	void setLabelSize(double val);

	int decimal() const;
	void setDecimal(int val);

	bool showLineout() const;
	void setShowLineout(bool val);

	bool showLabels() const;
	void setShowLabels(bool val);

	void setShowDiagramms(bool val);
	bool showDiagramms() const;
	
	bool dailyProduction() const;
	void setDailyProduction(bool val);
	

    void setOutputUnit( QgsSymbolV2::OutputUnit unit ) override;
    QgsSymbolV2::OutputUnit outputUnit() const override;

    void setMapUnitScale( const QgsMapUnitScale& scale ) override;
    QgsMapUnitScale mapUnitScale() const override;

protected:
	double calculateSize( QgsSymbolV2RenderContext& context, bool& hasDataDefinedSize ) const;

  private:
    class bblMarkerSimbolLayerP *impl;
};

#endif // BBLMARKERSIMBOLLAYER_H
