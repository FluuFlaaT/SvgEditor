#pragma once
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QGraphicsSvgItem>
#include <QStringList>
#include <QLoggingCategory>

// Q_DECLARE_LOGGING_CATEGORY(xmlParserLog)

class XMLParser {
public:
    // Singleton instance
    static XMLParser& getInstance() {
        static XMLParser instance;
        return instance;
    }

    // Function to parse SVG file and return QGraphicsSvgItem
    QStringList getIds(const QString& fileName);

    private:
        XMLParser() = default;
        ~XMLParser() = default;
        XMLParser(const XMLParser&) = delete;
        XMLParser& operator=(const XMLParser&) = delete;
};