#pragma once
#include <QString>
#include "../ShapeToolBar.h"

/**
 * @brief Get a human-readable name for a shape type
 * @param type The shape type
 * @return A human-readable name for the shape type
 */
inline QString getShapeTypeName(ShapeType type)
{
    switch (type) {
        case ShapeType::Line: return QObject::tr("Line");
        case ShapeType::Rectangle: return QObject::tr("Rectangle");
        case ShapeType::Ellipse: return QObject::tr("Ellipse");
        case ShapeType::Pentagon: return QObject::tr("Pentagon");
        case ShapeType::Hexagon: return QObject::tr("Hexagon");
        case ShapeType::Star: return QObject::tr("Star");
        case ShapeType::Freehand: return QObject::tr("Freehand");
        case ShapeType::Text: return QObject::tr("Text");
        default: return QObject::tr("Shape");
    }
}
