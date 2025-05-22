#include "XMLParser.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(xmlParserLog)
Q_LOGGING_CATEGORY(xmlParserLog, "xmlParser")

namespace {
static const QSet<QString> kTargetTags = {
    QStringLiteral("path"),
    QStringLiteral("rect"),
    QStringLiteral("ellipse"),
    QStringLiteral("polygon"),
    QStringLiteral("polyline"),
    QStringLiteral("circle")
};

void walk(QDomElement elem,
          QHash<QString,int> &counters,
          QStringList &ids)
{
    // 只关心可识别标签
    if (kTargetTags.contains(elem.tagName())) {
        QString idAttr  = QStringLiteral("id");
        if (elem.attribute(idAttr).isEmpty()) {
            // 生成 id
            QString newId = QStringLiteral("%1_%2")
                                .arg(elem.tagName())
                                .arg(++counters[elem.tagName()]);
            elem.setAttribute(idAttr, newId);
            qCDebug(xmlParserLog) << "Created new ID:" << newId << "for tag:" << elem.tagName();
            ids.append(newId);
        } else {
            QString existingId = elem.attribute(idAttr);
            qCDebug(xmlParserLog) << "Found existing ID:" << existingId << "for tag:" << elem.tagName();
            ids.append(existingId);
        }
    }

    // 递归处理子节点
    for (QDomNode n = elem.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (n.isElement())
            walk(n.toElement(), counters, ids);
    }
}
} // unnamed namespace

QStringList XMLParser::processIds(const QString &fileName)
{
    qCInfo(xmlParserLog) << "Starting XML processing for file:" << fileName;
    
    QStringList ids;
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open" << fileName;
        return ids;
    }
    qCDebug(xmlParserLog) << "File opened successfully for reading";

    QDomDocument doc;
    QString  errMsg;
    int      errLine = 0, errCol = 0;
    if (!doc.setContent(&f, &errMsg, &errLine, &errCol)) {
        qWarning() << "Parse error:" << errMsg << "at" << errLine << ":" << errCol;
        return ids;
    }
    qCDebug(xmlParserLog) << "XML document parsed successfully";
    f.close();                                   // 关闭，只读完成

    QHash<QString,int> counters;                 // 计数器
    qCDebug(xmlParserLog) << "Starting DOM tree traversal";
    walk(doc.documentElement(), counters, ids);  // 深度优先遍历
    qCDebug(xmlParserLog) << "DOM traversal complete, found/created" << ids.size() << "IDs";

    // 写回文件（覆盖）
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qWarning() << "Cannot rewrite" << fileName;
        return ids;
    }
    qCDebug(xmlParserLog) << "File opened successfully for writing";
    
    QTextStream out(&f);
    doc.save(out, 2);                            // 缩进 2 空格，可根据需要调整
    f.close();
    qCDebug(xmlParserLog) << "File saved successfully";

    qCInfo(xmlParserLog) << "XML processing complete for file:" << fileName << "with" << ids.size() << "IDs";
    return ids;
}