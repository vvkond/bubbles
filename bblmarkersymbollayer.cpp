#include "bblmarkersymbollayer.h"

#include <qdebug.h>
#include <qgsdatadefined.h>
#include <qgsexpressioncontext.h>
#include <qgslabelingenginev2.h>
#include <qgsmarkersymbollayerv2.h>
#include <qgsunittypes.h>
#include <qgsvectorlayer.h>
#include <qstatictext.h>
#include <qtextdocument.h>
#include <qdom.h>

class bblMarkerSimbolLayerP {
public:
    QgsMarkerSymbolV2* mMarkerSymbol;
    QString diagrammType;
    QgsFluidList fluidCodes;
    DiagrammMap standardDiagramms;
    //QgsStringMap diagrammTypes;
    //QgsRealMap diagrammScales;
    QStringList fields;
    QList<int> fieldIndices;
    int scaleTypeId; //0-mass, 1-volume
    qreal minDiagrammSize; //map unit
    qreal maxDiagrammSize; //map unit
    int mXIndex;
    int mYIndex;
    int mWIndex;
    QString labelTemplate;
    double labelSize;
    int decimal; //Знаков после запятой
    bool showLineout;
	bool showLabels;
	bool showDiagramms;
    bool dailyProduction;

    bblMarkerSimbolLayerP()
        : diagrammType(LIQUID_PRODUCTION)
        , scaleTypeId(0)
        , minDiagrammSize(1)
        , maxDiagrammSize(100)
        , labelTemplate("%1-%3")
        , labelSize(7)
        , decimal(2)
        , showLineout(true)
		, showLabels(true)
		, showDiagramms(true)
        , dailyProduction(false)
    {
        mMarkerSymbol = new QgsMarkerSymbolV2();

        {
            FLUID_CODES code = { "", "crude oil", QColor(235, 149, 51), QColor(Qt::black), QColor(Qt::black), false };
            fluidCodes << code;
        }
        {
            FLUID_CODES code = { "", "natural gas", QColor(Qt::darkYellow), QColor(Qt::black), QColor(Qt::black), false };
            fluidCodes << code;
        }
        {
            FLUID_CODES code = { "", "produced water", QColor(Qt::blue), QColor(Qt::black), QColor(Qt::black), false };
            fluidCodes << code;
        }
        {
            FLUID_CODES code = { "", "condensate", QColor(Qt::gray), QColor(Qt::black), QColor(Qt::black), false };
            fluidCodes << code;
        }
        {
            FLUID_CODES code = { "", "injected gas", QColor(Qt::yellow), QColor(Qt::black), QColor(Qt::black), false };
            fluidCodes << code;
        }
        {
            FLUID_CODES code = { "", "injected water", QColor(0, 160, 230), QColor(Qt::black), QColor(Qt::black), false };
            fluidCodes << code;
        }
        {
            FLUID_CODES code = { "", "lift gas", QColor(Qt::yellow), QColor(Qt::black), QColor(Qt::black), false };
            fluidCodes << code;
        }
        {
            FLUID_CODES code = { "", "free gas", QColor(Qt::darkYellow), QColor(Qt::black), QColor(Qt::black), false };
            fluidCodes << code;
        }

        {

            BBL_DIAGRAMM diag = { "", 300000, 0, 0, QVector<qreal>() << 1 << 0 << 1 << 0 << 0 << 0 << 0 << 0 };
            standardDiagramms[LIQUID_PRODUCTION] = diag;
        }
        {
            BBL_DIAGRAMM diag = { "", 300000, 0, 0, QVector<qreal>() << 0 << 0 << 0 << 0 << 1 << 1 << 0 << 0 };
            standardDiagramms[LIQUID_INJECTION] = diag;
        }
        {
            BBL_DIAGRAMM diag = { "", 300000, 1, 10, QVector<qreal>() << 0 << 1 << 0 << 0 << 0 << 0 << 0 << 0 };
            standardDiagramms[GAS_PRODUCTION] = diag;
        }
        {
            BBL_DIAGRAMM diag = { "", 3000000, 0, 0, QVector<qreal>() << 0 << 0 << 0 << 1 << 0 << 0 << 0 << 0 };
            standardDiagramms[CONDENSAT_PRODUCTION] = diag;
        }
    }

    FLUID_CODES fluidByCode(QString code)
    {
        Q_FOREACH (FLUID_CODES f, fluidCodes) {
            if (f.code == code)
                return f;
        }

        FLUID_CODES fluid;
        return fluid;
    }

    bool setSubSymbol(QgsSymbolV2* symbol)
    {
        if (symbol->type() == QgsSymbolV2::Marker) {
            delete mMarkerSymbol;
            mMarkerSymbol = static_cast<QgsMarkerSymbolV2*>(symbol);

            return true;
        }
        return false;
    }

    QString attrFluid(int index)
    {
        switch (index) {
        case 0:
            return " (mass)";
        case 1:
            return " (volume)";
        }
        return "";
    }

    void drawPreview(QPainter* p, QPointF point, QSize size)
    {
        QRectF rect(point, size);

		if(showDiagramms) {
			p->setPen(Qt::black);
			p->setBrush(QBrush(Qt::red));
			p->drawPie(rect, 90 * 16, 180 * 16);
			p->setBrush(QBrush(Qt::blue));
			p->drawPie(rect, 270 * 16, 90 * 16);
			p->setBrush(QBrush(Qt::green));
			p->drawPie(rect, 360 * 16, 90 * 16);
		}
		QPointF pt1(rect.center());
		QPointF pt2(pt1);
		QPointF pt3(rect.right(), pt1.y());
		if(showLineout) {
			QPen pen(Qt::black);
			pen.setWidth(1);
			pen.setCosmetic(true);
			p->setPen(pen);
			
			pt3 = QPointF(rect.topRight());
			pt2 = QPointF((pt1.x()+pt3.x())/2, (pt1.y()+pt3.y())/2);
			pt3.setY(pt2.y());
			p->drawLine(pt1, pt2);
			p->drawLine(pt2, pt3);
		 
			QFont font("arial");
			font.setPointSizeF(qAbs(rect.top()-pt2.y()));
			p->setFont(font);
			p->drawText(pt2, "1P");
		}
    }

    double changeUnits(double val, int index)
    {
        switch(index)
        {
            case 0: //кг
                break;
            case 1: //г
                val *= 1000.0;
                break;
            case 2: //tonn
                val /= 1000.0;
                break;
			case 3: //t. tonn
                val /= 1000000.0;
                break;
            case 4: //UK ton (short)
                val /= 907.18474;
                break;
            case 5: //UK ton (long)
                val /= 1016.0469088;
                break;
            case 10: //м3
                break;
            case 11: //дм3
                val *= 1000.0;
                break;
            case 12: //см3
                val *= 1000000.0;
                break;
            case 13: //км3
                val /= 1000000000.0;
                break;
            default:
                break;
        }
        return val;
    }
};

bblMarkerSimbolLayer::bblMarkerSimbolLayer(const QgsStringMap& properties)
    : QgsMarkerSymbolLayerV2()
    , impl(new bblMarkerSimbolLayerP)
{
    setProperties(properties);

    QgsDataDefined* dd = new QgsDataDefined(true, false, "", "SymbolID");
    QgsSvgMarkerSymbolLayerV2* lay = new QgsSvgMarkerSymbolLayerV2();
    lay->setDataDefinedProperty("name", dd);

    /*lay->setSize(impl->maxDiagrammSize/2);
    lay->setSizeUnit(QgsSymbolV2::MapUnit);*/

    setSubSymbol(new QgsMarkerSymbolV2(QgsSymbolLayerV2List() << lay));
}

bblMarkerSimbolLayer::~bblMarkerSimbolLayer()
{
    delete impl;
}

QString bblMarkerSimbolLayer::layerType() const
{
    return "BubbleMarker";
}

void bblMarkerSimbolLayer::setOutputUnit(QgsSymbolV2::OutputUnit unit)
{
    QgsMarkerSymbolLayerV2::setOutputUnit(unit);
}

QgsSymbolV2::OutputUnit bblMarkerSimbolLayer::outputUnit() const
{
    return QgsMarkerSymbolLayerV2::outputUnit();
}

void bblMarkerSimbolLayer::setMapUnitScale(const QgsMapUnitScale& scale)
{
    QgsMarkerSymbolLayerV2::setMapUnitScale(scale);
}

QgsMapUnitScale bblMarkerSimbolLayer::mapUnitScale() const
{
    return QgsMapUnitScale();
}

QgsSymbolLayerV2* bblMarkerSimbolLayer::create(const QgsStringMap& properties)
{
    bblMarkerSimbolLayer* symbolLayer = new bblMarkerSimbolLayer();

    symbolLayer->setProperties(properties);

    //data defined properties
    symbolLayer->restoreDataDefinedProperties(properties);

    //	Q_FOREACH(QString key, properties.keys())
    //	{
    //		QgsDebugMsg(properties[key]);
    //	}

    return symbolLayer;
}

void bblMarkerSimbolLayer::setProperties(const QgsStringMap& properties)
{
    bblMarkerSimbolLayer* symbolLayer = this;

    if (properties.contains("size")) {
        symbolLayer->setSize(properties["size"].toDouble());
    }
    if (properties.contains("size_unit")) {
        symbolLayer->setSizeUnit(QgsSymbolLayerV2Utils::decodeOutputUnit(properties["size_unit"]));
    }
    if (properties.contains("size_map_unit_scale")) {
        symbolLayer->setSizeMapUnitScale(QgsSymbolLayerV2Utils::decodeMapUnitScale(properties["size_map_unit_scale"]));
    }
    if (properties.contains("offset")) {
        symbolLayer->setOffset(QgsSymbolLayerV2Utils::decodePoint(properties["offset"]));
    }
    if (properties.contains("offset_unit")) {
        symbolLayer->setOffsetUnit(QgsSymbolLayerV2Utils::decodeOutputUnit(properties["offset_unit"]));
    }
    if (properties.contains("offset_map_unit_scale")) {
        symbolLayer->setOffsetMapUnitScale(QgsSymbolLayerV2Utils::decodeMapUnitScale(properties["offset_map_unit_scale"]));
    }

    for (DiagrammMap::iterator d = symbolLayer->standardDiagramms().begin(); d != symbolLayer->standardDiagramms().end(); d++) {
        QString propName = "diagramm_name_" + d.key();
        if (properties.contains(propName)) {
            d.value().name = properties[propName];
        }

        propName = "diagramm_scale_" + d.key();
        if (properties.contains(propName)) {
            d.value().scale = properties[propName].toDouble();
        }

        propName = "diagramm_unitsType_" + d.key();
        if (properties.contains(propName)) {
            d.value().unitsType = properties[propName].toInt();
        }

        propName = "diagramm_units_" + d.key();
        if (properties.contains(propName)) {
            d.value().units = properties[propName].toInt();
        }

        propName = "diagramm_fluids_" + d.key();
        if (properties.contains(propName)) {
            QVector<qreal> vect = QgsSymbolLayerV2Utils::decodeRealVector(properties[propName]);
            d.value().fluids = vect;
        }
    }

    for (QgsFluidList::iterator d = symbolLayer->fluidCodes().begin(); d != symbolLayer->fluidCodes().end(); d++) {
        QString propName = "fluid_background_" + (*d).code;
        if (properties.contains(propName))
            (*d).backColor = QgsSymbolLayerV2Utils::decodeColor(properties[propName]);

        propName = "fluid_line_color_" + (*d).code;
        if (properties.contains(propName))
            (*d).lineColor = QgsSymbolLayerV2Utils::decodeColor(properties[propName]);

        propName = "fluid_label_color_" + (*d).code;
        if (properties.contains(propName))
            (*d).labelColor = QgsSymbolLayerV2Utils::decodeColor(properties[propName]);

        propName = "fluid_inPercent_" + (*d).code;
        if (properties.contains(propName))
            (*d).inPercent = properties[propName].toInt();
    }

    if (properties.contains("diagrammType")) {
        symbolLayer->setDiagrammType(properties["diagrammType"]);
    }

    if (properties.contains("scaleTypeId")) {
        symbolLayer->setScaleTypeId(properties["scaleTypeId"].toInt());
    }

    if (properties.contains("minDiagrammSize"))
        symbolLayer->setMinDiagrammSize(properties["minDiagrammSize"].toDouble());

    if (properties.contains("maxDiagrammSize"))
        symbolLayer->setMaxDiagrammSize(properties["maxDiagrammSize"].toDouble());

    if (properties.contains("labelTemplate")) {
        symbolLayer->setLabelTemplate(properties["labelTemplate"]);
    }

    if (properties.contains("labelSize"))
        symbolLayer->setLabelSize(properties["labelSize"].toDouble());

    if (properties.contains("decimal"))
        symbolLayer->setDecimal(properties["decimal"].toInt());

    if (properties.contains("showLineout"))
        symbolLayer->setShowLineout(properties["showLineout"].toInt());

	if (properties.contains("showLabels"))
		symbolLayer->setShowLabels(properties["showLabels"].toInt());

	if (properties.contains("showDiagramms"))
		symbolLayer->setShowDiagramms(properties["showDiagramms"].toInt());

    if (properties.contains("dailyProduction"))
        symbolLayer->setDailyProduction(properties["dailyProduction"].toInt());
}

//bool bblMarkerSimbolLayer::setSubSymbol( QgsSymbolV2* symbol )
//{
//    return impl->setSubSymbol( symbol );
//}
//
//QgsSymbolV2 *bblMarkerSimbolLayer::subSymbol()
//{
//    return impl->mMarkerSymbol;
//}

struct DiagrammSlice {
	QColor backColor;
	QColor lineColor;
	QString fieldName;
	double percent;
};

class DiagrammDesc {
public:
	double mDiagrammSize;
	QList<DiagrammSlice> mSlices;

	DiagrammDesc() {}
};

bool diagrammLessThan(const DiagrammDesc &s1, const DiagrammDesc &s2)
{
    return s1.mDiagrammSize > s2.mDiagrammSize;
}

void bblMarkerSimbolLayer::renderPoint(QPointF point, QgsSymbolV2RenderContext& context)
{
    if (!impl->mMarkerSymbol) {
        return;
    }

    QgsRenderContext& ctx = context.renderContext();

    QPainter* p = context.renderContext().painter();
    if (!p) {
        return;
    }

    const QgsFeature* f = context.feature();
    if (!f) {
        //preview
        qreal labelSize = QgsSymbolLayerV2Utils::convertToPainterUnits(context.renderContext(), mSize, sizeUnit());
        /*	QFont font;
		font.setPixelSize(labelSize);
		p->setFont( font );
		QRect textRect = p->fontMetrics().boundingRect(impl->standardDiagramms[impl->diagrammType].name);
		QRect rect(0,0, labelSize, textRect.width());
		p->drawText(point, impl->standardDiagramms[impl->diagrammType].name);*/

        impl->drawPreview(p, QPointF(point.x() - labelSize / 2, point.y() - labelSize / 2), QSize(labelSize, labelSize));
        ////qreal labelSize = QgsSymbolLayerV2Utils::convertToPainterUnits(context.renderContext(), 10, QgsSymbolV2::Pixel, QgsSymbolV2::MM);
        //qreal y = 0;
        //Q_FOREACH(FLUID_CODES code, impl->fluidCodes)
        //{
        //	p->setBrush(QBrush(code.backColor));
        //	p->drawEllipse(0, y, labelSize, labelSize);
        //	y += labelSize;
        //}
        return;
    }

    QString scaleType = f->attribute("ScaleType").toString();

	QList<DiagrammDesc> diagramms;
	QString labelTemplate;

    QString fields = f->attribute("BubbleFields").toString();
    if (!fields.isEmpty()) {
		QDomDocument doc;
		if(doc.setContent(fields)) {
			QDomElement docElem = doc.documentElement();
			QDomNodeList diagrammList = docElem.elementsByTagName( "diagramm" );
			for ( int i = 0; i < diagrammList.count(); ++i )
			{
				QDomNode elementNode = diagrammList.at( i );
				if ( !elementNode.isElement() )
					continue;

				QString size = elementNode.toElement().attribute( "size" );
				qreal diagrammSize = QgsSymbolLayerV2Utils::convertToPainterUnits(ctx, size.toDouble(), QgsSymbolV2::MM, QgsSymbolV2::MM);

				if(diagrammSize > 0) {
					DiagrammDesc desc;
					desc.mDiagrammSize = diagrammSize;

					QDomNodeList valueList = elementNode.toElement().elementsByTagName("value");
					for ( int j = 0; j < valueList.count(); ++j ) {
						QDomNode valNode = valueList.at(j);
						if(!valNode.isElement())
							continue;

						QDomElement valElement = valNode.toElement();
						DiagrammSlice slice;
						slice.backColor = QgsSymbolLayerV2Utils::decodeColor(valElement.attribute("backColor"));
						slice.lineColor = QgsSymbolLayerV2Utils::decodeColor(valElement.attribute("lineColor"));
						slice.percent = valElement.text().toDouble();
						slice.fieldName = valElement.attribute("fieldName");
						desc.mSlices.append(slice);
					}
					diagramms.append(desc);
				}
			}

			QDomNodeList labelsList = docElem.elementsByTagName( "label" );
			if(labelsList.count()) {
				labelTemplate = labelsList.at(0).toElement().attribute("labelText");
			}
		}

        //impl->fields = fields.split(',', QString::SkipEmptyParts);
    }

	qSort(diagramms.begin(), diagramms.end(), diagrammLessThan);

	if(impl->showDiagramms) {
		Q_FOREACH(DiagrammDesc desc, diagramms){
			double startAngle = 90;
			int count = desc.mSlices.count();
			double diagrammSize = desc.mDiagrammSize;
			QRectF rect(point, QSizeF(diagrammSize, diagrammSize));
			rect.translate(-diagrammSize / 2, -diagrammSize / 2);

			Q_FOREACH (DiagrammSlice slice, desc.mSlices) {
				QColor color = slice.backColor; 
				p->setBrush(QBrush(color));

				color = slice.lineColor;
				p->setPen(color);

				qreal spanAngle = 360 * slice.percent;

				if (count > 1)
					p->drawPie(rect, startAngle * 16, spanAngle * 16);
				else
					p->drawEllipse(rect);
				startAngle += spanAngle;
			}
		}
	}

    /*if (!impl->fields.count())
        return;*/

    //    qreal maxDiagrammSize = QgsSymbolLayerV2Utils::convertToPainterUnits(ctx, impl->maxDiagrammSize, QgsSymbolV2::MapUnit);
    //    qreal minDiagrammSize = QgsSymbolLayerV2Utils::convertToPainterUnits(ctx, impl->minDiagrammSize, QgsSymbolV2::MapUnit);

    //    qreal dScale = impl->diagrammScales[impl->diagrammType];
    //    if(dScale == 0)
    //        return;

    //    qreal koef = (maxDiagrammSize-minDiagrammSize) / dScale;

    /*qreal sum = 0;
    int unitsIndex = impl->standardDiagramms[impl->diagrammType].units;
    Q_FOREACH (QString field, impl->fields) {
        QString varName = field + scaleType;
        sum += impl->changeUnits(f->attribute(varName).toDouble(), unitsIndex);
    }*/

    //    qreal diagrammSize = minDiagrammSize + sum * koef;
	/*qreal diagrammSize = QgsSymbolLayerV2Utils::convertToPainterUnits(ctx, f->attribute("BubbleSize").toDouble(), QgsSymbolV2::MM, QgsSymbolV2::MM);*/
    qreal labelSize = QgsSymbolLayerV2Utils::convertToPainterUnits(ctx, impl->labelSize, QgsSymbolV2::Pixel, QgsSymbolV2::MM);
    //    if(diagrammSize > maxDiagrammSize)
    //        diagrammSize = maxDiagrammSize;

    QFont font;
	font.setPointSizeF(labelSize);
    p->setFont(font);

    /*QRectF rect(point, QSizeF(diagrammSize, diagrammSize));
    rect.translate(-diagrammSize / 2, -diagrammSize / 2);*/

    //if (sum != 0.0) {
    //    qreal startAngle = 90;
    //    int count = impl->fields.count();
    //    Q_FOREACH (QString field, impl->fields) {
    //        FLUID_CODES fluid = impl->fluidByCode(field);
    //        QColor color = fluid.backColor; //impl->fluidCodes[impl->fieldIndices[i]].backColor;
    //        p->setBrush(QBrush(color));

    //        color = fluid.lineColor;
    //        p->setPen(color);

    //        QString varName = field + scaleType;
    //        qreal value = impl->changeUnits(f->attribute(varName).toDouble(), unitsIndex) / sum;

    //        qreal spanAngle = 360 * value;

    //        if (count > 1)
    //            p->drawPie(rect, startAngle * 16, spanAngle * 16);
    //        else
    //            p->drawEllipse(rect);
    //        startAngle += spanAngle;
    //    }
    //}

    //Draw lineout and production text
    if (impl->mXIndex >= 0 && impl->mYIndex >= 0 && impl->mWIndex >= 0) {
        double xVal = QgsSymbolLayerV2Utils::convertToPainterUnits(ctx, f->attribute(impl->mXIndex).toDouble(), QgsSymbolV2::MM, QgsSymbolV2::MM);//f->attribute(impl->mXIndex).toDouble();
        double yVal = QgsSymbolLayerV2Utils::convertToPainterUnits(ctx, f->attribute(impl->mYIndex).toDouble(), QgsSymbolV2::MM, QgsSymbolV2::MM);//f->attribute(impl->mYIndex).toDouble();
        double widthVal = 10;//QgsSymbolLayerV2Utils::convertToPainterUnits(ctx, f->attribute(impl->mWIndex).toDouble(), QgsSymbolV2::MM, QgsSymbolV2::MM);//f->attribute(impl->mWIndex).toDouble();
        double days = f->attribute("Days").toDouble();
        if (days != 0)
            days = 1.0 / days;

        if (xVal != 0 || yVal != 0) {
            QPointF pt1 = point + QPointF(xVal, yVal); //ctx.mapToPixel().transform(xVal, yVal).toQPointF();

            /*QString labelTemplate = impl->labelTemplate;
            for (int i = 0; i < impl->fluidCodes.count(); i++) {
                QString arg = "%" + QString::number(i + 1);

                if (labelTemplate.contains(arg)) {
                    QString attrName = impl->fluidCodes[i].code + scaleType;
                    double val = impl->changeUnits(f->attribute(attrName).toDouble(), unitsIndex);

                    QString strVal = QString::number(val, 'f', impl->decimal);
                    if (impl->fluidCodes[i].inPercent && sum != 0.0) {
                        val = val / sum * 100.0;
                        strVal = QString::number(val, 'f', 1) + "%";
                    } else if (impl->dailyProduction) {
                        val *= days;
                        strVal = QString::number(val, 'f', impl->decimal);
                    }

                    QString colorStr = impl->fluidCodes[i].labelColor.name();
                    labelTemplate = labelTemplate.replace(arg,
                        QString("<span><font color=\"%1\">%2</font></span>").arg(colorStr).arg(strVal));
                }
            }*/
            QStaticText st(labelTemplate);
			QTextOption opt = st.textOption();
			opt.setWrapMode(QTextOption::NoWrap);
			st.setTextOption(opt);
			st.prepare(p->transform(), p->font());
			widthVal = st.size().width();

            QPointF pt2 = point + QPointF(xVal+widthVal, yVal);//ctx.mapToPixel().transform(xVal + widthVal, yVal).toQPointF();

            QPen pen(Qt::black);
            pen.setWidth(2);
            p->setPen(pen);
            if (point.x() < (pt1.x() + pt2.x()) / 2) {
                if (impl->showLineout) {
                    p->drawLine(point, pt1);
                    p->drawLine(pt1, pt2);
                }
				if(!labelTemplate.isNull() && impl->showLabels)
					p->drawStaticText(pt1.x(), pt1.y(), st);
            } else {
                if (impl->showLineout) {
                    p->drawLine(point, pt2);
                    p->drawLine(pt2, pt1);
                }
				if(!labelTemplate.isNull() && impl->showLabels)
					p->drawStaticText(pt1.x(), pt1.y(), st);
            }
        }
    }

    //if(impl->mMarkerSymbol)
    //{
    //    impl->mMarkerSymbol->renderPoint(point, f, context.renderContext() );
    //}
}

void bblMarkerSimbolLayer::startRender(QgsSymbolV2RenderContext& context)
{
    if (impl->mMarkerSymbol) {
        impl->mMarkerSymbol->startRender(context.renderContext(), context.fields());
    }

    impl->fields.clear();
    impl->fieldIndices.clear();

    //QVector<qreal> checks = impl->standardDiagramms.value(impl->diagrammType, QVector<qreal>());
    //if (!checks.count()) {
    //    return;
    //}

    //    for(int i=0; i<checks.count(); i++)
    //    {
    //        if(checks[i])
    //        {
    //            impl->fields << impl->fluidCodes[i].code + impl->attrFluid(impl->scaleTypeId);
    //            impl->fieldIndices << i;
    //        }
    //    }

    const QgsFields* fields = context.fields();
    if (fields) {
        impl->mXIndex = fields->fieldNameIndex("LablOffX");
        impl->mYIndex = fields->fieldNameIndex("LablOffY");
        impl->mWIndex = fields->fieldNameIndex("LablWidth");
    } else {
        impl->mXIndex = -1;
        impl->mYIndex = -1;
        impl->mWIndex = -1;
    }

    /*   if (context.renderContext().expressionContext().hasVariable("Lineout"))
        impl->showLineout = context.renderContext().expressionContext().variable("Lineout").toBool();

    if (context.renderContext().expressionContext().hasVariable("LablSize"))
        impl->labelSize = context.renderContext().expressionContext().variable("LablSize").toDouble();

    if (context.renderContext().expressionContext().hasVariable("LablDecimal"))
        impl->decimal = context.renderContext().expressionContext().variable("LablDecimal").toInt();

    if (context.renderContext().expressionContext().hasVariable("LablTmpl"))
        impl->labelTemplate = context.renderContext().expressionContext().variable("LablTmpl").toString();

    if (context.renderContext().expressionContext().hasVariable("DailyProd"))
        impl->dailyProduction = context.renderContext().expressionContext().variable("DailyProd").toBool();

    for (int i = 0; i < impl->fluidCodes.count(); i++) {
        QString propName = impl->fluidCodes[i].code + "_background";
        if (context.renderContext().expressionContext().hasVariable(propName))
            impl->fluidCodes[i].backColor = context.renderContext().expressionContext().variable(propName).value<QColor>();

        propName = impl->fluidCodes[i].code + "_line_color";
        if (context.renderContext().expressionContext().hasVariable(propName))
            impl->fluidCodes[i].lineColor = context.renderContext().expressionContext().variable(propName).value<QColor>();

        propName = impl->fluidCodes[i].code + "_label_color";
        if (context.renderContext().expressionContext().hasVariable(propName))
            impl->fluidCodes[i].labelColor = context.renderContext().expressionContext().variable(propName).value<QColor>();

        propName = impl->fluidCodes[i].code + "_inPercent";
        if (context.renderContext().expressionContext().hasVariable(propName))
            impl->fluidCodes[i].inPercent = context.renderContext().expressionContext().variable(propName).toBool();
    }*/
}

void bblMarkerSimbolLayer::stopRender(QgsSymbolV2RenderContext& context)
{
    if (impl->mMarkerSymbol) {
        impl->mMarkerSymbol->stopRender(context.renderContext());
    }
}

double bblMarkerSimbolLayer::calculateSize(QgsSymbolV2RenderContext& context, bool& hasDataDefinedSize) const
{
    double scaledSize = mSize;
    hasDataDefinedSize = context.renderHints() & QgsSymbolV2::DataDefinedSizeScale || hasDataDefinedProperty(QgsSymbolLayerV2::EXPR_SIZE);

    bool ok = true;
    if (hasDataDefinedProperty(QgsSymbolLayerV2::EXPR_SIZE)) {
        context.setOriginalValueVariable(mSize);
        scaledSize = evaluateDataDefinedProperty(QgsSymbolLayerV2::EXPR_SIZE, context, mSize, &ok).toDouble();
    }

    return scaledSize;
}

bblMarkerSimbolLayer* bblMarkerSimbolLayer::clone() const
{
    QgsSymbolLayerV2* clonedLayer = bblMarkerSimbolLayer::create(properties());
    if (impl->mMarkerSymbol) {
        static_cast<bblMarkerSimbolLayer*>(clonedLayer)->impl->setSubSymbol(impl->mMarkerSymbol->clone());
    }

    copyDataDefinedProperties(clonedLayer);
    copyPaintEffect(clonedLayer);

    return static_cast<bblMarkerSimbolLayer*>(clonedLayer);
}

QgsStringMap bblMarkerSimbolLayer::properties() const
{
    QgsStringMap properties;

    for (DiagrammMap::iterator d = impl->standardDiagramms.begin(); d != impl->standardDiagramms.end(); d++) {
        properties["diagramm_name_" + d.key()] = d.value().name;
        properties["diagramm_scale_" + d.key()] = QString::number(d.value().scale, 'f', 4);
        properties["diagramm_unitsType_" + d.key()] = QString::number(d.value().unitsType);
        properties["diagramm_units_" + d.key()] = QString::number(d.value().units);
        properties["diagramm_fluids_" + d.key()] = QgsSymbolLayerV2Utils::encodeRealVector(d.value().fluids);
    }

    for (QgsFluidList::iterator d = impl->fluidCodes.begin(); d != impl->fluidCodes.end(); d++) {
        properties["fluid_background_" + (*d).code] = QgsSymbolLayerV2Utils::encodeColor((*d).backColor);
        properties["fluid_line_color_" + (*d).code] = QgsSymbolLayerV2Utils::encodeColor((*d).lineColor);
        properties["fluid_label_color_" + (*d).code] = QgsSymbolLayerV2Utils::encodeColor((*d).labelColor);
        properties["fluid_inPercent_" + (*d).code] = QString::number((*d).inPercent);
    }

    properties["diagrammType"] = impl->diagrammType;
    properties["scaleTypeId"] = QString::number(impl->scaleTypeId);
    properties["minDiagrammSize"] = QString::number(impl->minDiagrammSize, 'f');
    properties["maxDiagrammSize"] = QString::number(impl->maxDiagrammSize, 'f');
    properties["labelTemplate"] = impl->labelTemplate;
    properties["labelSize"] = QString::number(impl->labelSize, 'f');
    properties["decimal"] = QString::number(impl->decimal);
    properties["showLineout"] = QString::number(impl->showLineout);
	properties["showLabels"] = QString::number(impl->showLabels);
	properties["showDiagramms"] = QString::number(impl->showDiagramms);
    properties["dailyProduction"] = QString::number(impl->dailyProduction);

    properties["size"] = QString::number(mSize);
    properties["size_unit"] = QgsSymbolLayerV2Utils::encodeOutputUnit(mSizeUnit);
    properties["size_map_unit_scale"] = QgsSymbolLayerV2Utils::encodeMapUnitScale(mSizeMapUnitScale);
    properties["offset"] = QgsSymbolLayerV2Utils::encodePoint(mOffset);
    properties["offset_unit"] = QgsSymbolLayerV2Utils::encodeOutputUnit(mOffsetUnit);
    properties["offset_map_unit_scale"] = QgsSymbolLayerV2Utils::encodeMapUnitScale(mOffsetMapUnitScale);

    saveDataDefinedProperties(properties);

    return properties;
}

void bblMarkerSimbolLayer::toSld(QDomDocument& doc, QDomElement& element, const QgsStringMap& props) const
{
    element.appendChild(doc.createComment("VectorField not implemented yet..."));

    impl->mMarkerSymbol->toSld(doc, element, props);
}

QgsSymbolLayerV2* bblMarkerSimbolLayer::createFromSld(QDomElement& element)
{
    QgsDebugMsg("createFromSld");
    Q_UNUSED(element);
    return nullptr;
}

//void bblMarkerSimbolLayer::drawPreviewIcon( QgsSymbolV2RenderContext& context, QSize size )
//{
//    QPainter *p = context.renderContext().painter();
//	impl->drawPreview(p, QPointF(0, 0), QSize(size.width()/2, size.height()/2));
//	/*qreal labelSize = QgsSymbolLayerV2Utils::convertToPainterUnits(context.renderContext(), 10, QgsSymbolV2::Pixel, QgsSymbolV2::MM);
//	qreal y = 0;
//	Q_FOREACH(FLUID_CODES code, impl->fluidCodes)
//	{
//		p->setBrush(QBrush(code.backColor));
//		p->drawEllipse(0, y, labelSize, labelSize);
//		y += labelSize;
//	}*/
//}

QSet<QString> bblMarkerSimbolLayer::usedAttributes() const
{
    QSet<QString> attributes = QgsMarkerSymbolLayerV2::usedAttributes();

    for (DiagrammMap::iterator d = impl->standardDiagramms.begin(); d != impl->standardDiagramms.end(); d++) {
        attributes.insert("diagramm_name_" + d.key());
    }
    attributes.insert("diagrammType");
    attributes.insert("scaleTypeId");
    attributes.insert("minDiagrammSize");
    attributes.insert("maxDiagrammSize");

    if (impl->mMarkerSymbol) {
        attributes.unite(impl->mMarkerSymbol->usedAttributes());
    }
    return attributes;
}

//QRectF bblMarkerSimbolLayer::bounds( QPointF point, QgsSymbolV2RenderContext& context )
//{
//  QSizeF size(20, 20);//= calculateSize( context );
//
//  bool hasDataDefinedRotation = false;
//  QPointF offset;
//  double angle = 0;
//  //calculateOffsetAndRotation( context, size.width(), size.height(), hasDataDefinedRotation, offset, angle );
//
//  double pixelSize = 1.0 / context.renderContext().rasterScaleFactor();
//
//  QMatrix transform;
//
//  // move to the desired position
//  transform.translate( point.x() + offset.x(), point.y() + offset.y() );
//
//  if ( !qgsDoubleNear( angle, 0.0 ) )
//    transform.rotate( angle );
//
//  double penWidth = 0.0;
//
//
//  //antialiasing
//  penWidth += pixelSize;
//
//  QRectF symbolBounds = transform.mapRect( QRectF( -size.width() / 2.0,
//                        -size.height() / 2.0,
//                        size.width(),
//                        size.height() ) );
//
//  //extend bounds by pen width / 2.0
//  symbolBounds.adjust( -penWidth / 2.0, -penWidth / 2.0,
//                       penWidth / 2.0, penWidth / 2.0 );
//
//  return symbolBounds;
//}

void bblMarkerSimbolLayer::setColor(const QColor& color)
{
    if (impl->mMarkerSymbol)
        impl->mMarkerSymbol->setColor(color);

    mColor = color;
}

QColor bblMarkerSimbolLayer::color() const
{
    return impl->mMarkerSymbol ? impl->mMarkerSymbol->color() : mColor;
}

QgsFluidList& bblMarkerSimbolLayer::fluidCodes() const
{
    return impl->fluidCodes;
}

DiagrammMap& bblMarkerSimbolLayer::standardDiagramms()
{
    return impl->standardDiagramms;
}

//QgsStringMap& bblMarkerSimbolLayer::diagrammTypes() const
//{
//    return impl->diagrammTypes;
//}
//
//QgsRealMap& bblMarkerSimbolLayer::diagrammScales() const
//{
//    return impl->diagrammScales;
//}

//setters and getters
void bblMarkerSimbolLayer::setDiagrammType(const QString& dType)
{
    impl->diagrammType = dType;
}

QString bblMarkerSimbolLayer::diagrammType()
{
    return impl->diagrammType;
}

void bblMarkerSimbolLayer::setScaleTypeId(int sct)
{
    impl->scaleTypeId = sct;
}

int bblMarkerSimbolLayer::scaleTypeId() const
{
    return impl->scaleTypeId;
}

void bblMarkerSimbolLayer::setMaxDiagrammSize(qreal val)
{
    impl->maxDiagrammSize = val;
}

qreal bblMarkerSimbolLayer::maxDiagrammSize() const
{
    return impl->maxDiagrammSize;
}

QString bblMarkerSimbolLayer::labelTemplate() const
{
    return impl->labelTemplate;
}

void bblMarkerSimbolLayer::setLabelTemplate(const QString& val)
{
    impl->labelTemplate = val;
}

void bblMarkerSimbolLayer::setMinDiagrammSize(qreal val)
{
    impl->minDiagrammSize = val;
}

qreal bblMarkerSimbolLayer::minDiagrammSize() const
{
    return impl->minDiagrammSize;
}

double bblMarkerSimbolLayer::labelSize() const
{
    return impl->labelSize;
}

void bblMarkerSimbolLayer::setLabelSize(double val)
{
    impl->labelSize = val;
}

int bblMarkerSimbolLayer::decimal() const
{
    return impl->decimal;
}

void bblMarkerSimbolLayer::setDecimal(int val)
{
    impl->decimal = val;
}

bool bblMarkerSimbolLayer::showLineout() const
{
    return impl->showLineout;
}

void bblMarkerSimbolLayer::setShowLineout(bool val)
{
    impl->showLineout = val;
}

bool bblMarkerSimbolLayer::showLabels() const
{
	return impl->showLabels;
}

void bblMarkerSimbolLayer::setShowLabels(bool val)
{
	impl->showLabels = val;
}

bool bblMarkerSimbolLayer::showDiagramms() const
{
	return impl->showDiagramms;
}

void bblMarkerSimbolLayer::setShowDiagramms(bool val)
{
	impl->showDiagramms = val;
}

bool bblMarkerSimbolLayer::dailyProduction() const
{
    return impl->dailyProduction;
}

void bblMarkerSimbolLayer::setDailyProduction(bool val)
{
    impl->dailyProduction = val;
}
