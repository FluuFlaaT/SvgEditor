#include "XMLParser.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(xmlParserLog)
Q_LOGGING_CATEGORY(xmlParserLog, "xmlParser")

QStringList XMLParser::getIds(const QString& fileName) {
    // Goal: Get ids of elements within SVG file.
    QStringList ids;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(xmlParserLog) << "Failed to open file:" << fileName;
        return ids;
    }
    QXmlStreamReader xml(&file);
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == "svg") {
                continue;
            }
            if (xml.attributes().hasAttribute("id")) {
                ids.append(xml.attributes().value("id").toString());
                // qCDebug output id
                qCDebug(xmlParserLog) << "Found id:" << xml.attributes().value("id").toString();
            }
        }
    }
    return ids;
}