#include <QString>
#include <QXmlStreamReader>
#include <QFile>
#include <QDomDocument>
#include <QDebug>
#include <QUrl>
#include <rngtest.h>

QDomDocument loadDOM(const QString& url);

namespace {

/**
 * Helper class for writing fatal messages of the form fatal() << "error!";
 */
class fatal
{
private:
    QString msg;
public:
    fatal() {}
    ~fatal()
    {
        qFatal("%s", qPrintable(msg));
    }
    template<class T>
    fatal& operator<<(T s)
    {
        msg += s;
        return *this;
    }
};

struct ParseState {
    const ParseState* const parent;
    const QString ns;
    const QUrl datatypeLibrary;
};

const QString rngns = "http://relaxng.org/ns/structure/1.0";



bool isRng(const QDomNode& n, const char* localName) {
    return n.namespaceURI() == rngns && n.localName() == localName;
}

bool isRng(const QDomNode& n, const QList<QString> localNames) {
    return n.namespaceURI() == rngns && localNames.contains(n.localName());
}

QDomElement getStart(const QDomElement& grammar) {
    Q_ASSERT(isRng(grammar, "grammar"));
    QDomElement e = grammar.firstChildElement("start");
    while (!e.isNull() && !isRng(e, "start")) {
        e = e.nextSiblingElement("start");
    }
    QDomElement start = e;
    e = e.nextSiblingElement("start");
    while (!e.isNull() && !isRng(e, "start")) {
        e = e.nextSiblingElement("start");
    }
    Q_ASSERT(e.isNull());
    return start;
}

ParseState
parseCommonAttributes(const QDomElement& e, const ParseState* parentState = 0) {
    auto ns = e.attribute("ns", parentState ?parentState->ns :QString());
    auto dl = e.attribute("datatypeLibrary");
    auto datatypeLibrary = dl.isNull() ?dl :(parentState ?parentState->datatypeLibrary :QUrl());
    ParseState state{parentState, ns, datatypeLibrary};
    return state;
}

struct Attributes {

};

struct Element {
    // if ns.isNull() then any element name is allowed
    // if !ns.isNull() && localName().isNull then only elements not from ns are allowed
    // otherwise only elements with ns and localName are allowed
    const QString ns;
    const QString localName;
    const Attributes atts;
};

std::pair<Attributes, QList<QDomElement>>
handleAttributes(const QString& localName, const QList<QDomElement>& list) {
    Q_ASSERT(list.size() >= 2);
    QDomElement first = list.first();
    //qDebug() << first.localName();
    Q_ASSERT(localName == "group" || localName == "choice" || localName == "interleave");
    // interleave: there can be no attributes
    // group: first item could be the set of attributes
    // choice: this is the set of attributes
    if (first.localName() == "interleave") {
        return std::make_pair(Attributes{}, list);
    } else if (first.localName() == "choice") {
        return std::make_pair(Attributes{}, QList<QDomElement>{});
    } else { // group
        //qDebug() << first.localName();
        //Q_ASSERT(isRng(first,"attribute"));
    }
    //Attributes atts{};
    QList<QDomElement> a;
    return std::make_pair(Attributes{}, QList<QDomElement>{});
}

QList<QDomElement>
getContents(const QDomElement& content) {
    Q_ASSERT(isRng(content,{"group","choice","interleave"}));
    QDomElement one = content.firstChildElement();
    QDomElement two = one.nextSiblingElement();
    Q_ASSERT(!one.isNull() && !two.isNull());
    QList<QDomElement> contents;
    if (content.localName() == one.localName()) {
        contents.append(getContents(one));
    } else {
        contents.append(one);
    }
    contents.append(two);
    return contents;
}

// define	  ::=  	<define name="NCName"> <element> nameClass top </element> </define>
QDomElement
parseDefine(const QDomElement& define) {
    Q_ASSERT(isRng(define, "define"));
    QDomElement element = define.firstChildElement("element");
    Q_ASSERT(isRng(element, "element"));
    Q_ASSERT(element.nextSiblingElement().isNull());

    // parse name
    QDomElement name = element.firstChildElement();
    Q_ASSERT(isRng(name,{"name","anyName"}));
    // The element name is either
    // 1) <anyName/>,
    // 2) or nsname with rng ns and empty ns,
    // 3) just a qname
    // In
    QString ns;
    QString localName;
    if (name.localName() == "name") {
        Q_ASSERT(name.childNodes().length() == 1);
        Q_ASSERT(name.firstChild().nodeType() == QDomNode::TextNode);
        ns = name.attribute("ns");
        localName = name.text();
    } else if (name.childNodes().count() == 1) {
        Q_ASSERT(isRng(name, "anyName"));
        QDomElement except = name.firstChildElement();
        Q_ASSERT(isRng(except, "except"));
        QDomElement nsName = except.firstChildElement();
        Q_ASSERT(isRng(nsName, "nsName"));
        Q_ASSERT(nsName.childNodes().count() == 0);
        ns = name.attribute("ns");
    } else {
        Q_ASSERT(name.childNodes().count() == 0);
    }

    QDomElement content = name.nextSiblingElement();
    Q_ASSERT(!content.isNull());
    Q_ASSERT(content.nextSiblingElement().isNull());
    QList<QDomElement> contents = getContents(content);
    auto p = handleAttributes(content.localName(), contents);

    Element e{ns, localName, p.first};
    return element;
}

bool
isNCName(const QString& ncname) {
    return !ncname.isEmpty();
}

void
parseGrammar(const QDomElement& grammar) {
    Q_ASSERT(isRng(grammar, "grammar"));
    QDomElement start = grammar.firstChildElement("start");
    Q_ASSERT(isRng(start, "start"));
    QMap<QString, QDomElement> defines;
    QDomElement define = start.nextSiblingElement("define");
    while (!define.isNull() && isRng(define, "define")) {
        QString name = define.attribute("name");
        Q_ASSERT(isNCName(name));
        Q_ASSERT(!defines.contains(name));
        defines[name] = parseDefine(define);
        define = define.nextSiblingElement("define");
    }
    Q_ASSERT(define.isNull());

    ParseState parseState = parseCommonAttributes(grammar);
}

QString
getName(const QDomElement& e) {
    return e.attribute("name").remove("-");
}

struct QName {
    QString ns;
    QString localName;
    bool operator<(const QName& o) const {
        return ns != o.ns ? ns < o.ns : localName < o.localName;
    }
};

void
getQNames(const QDomElement& e, QMap<QString,QString>& nsmap, QMap<QString,QString>& namemap, QMap<QName,QString>& qnamemap) {
    if (isRng(e, "name")) {
        QString ns = e.attribute("ns");
        QString nsvar;
        if (!ns.isNull()) {
            if (!nsmap.contains(ns)) {
                nsvar = QString("ns%1").arg(nsmap.size());
                nsmap.insert(ns, nsvar);
            } else {
                nsvar = nsmap[ns];
            }
        }
        QString name = e.text();
        QString namevar;
        if (!name.isNull()) {
            if (!namemap.contains(name)) {
                namevar = QString("%1LocalName").arg(name);
                namemap.insert(name, namevar);
            } else {
                namevar = namemap[name];
            }
        }
        QName qname = QName{ns,name};
        if (!nsvar.isNull() && !namevar.isNull() && !qnamemap.contains(qname)) {
            QString qnamevar = nsvar + "_" + name + "QName";
            qnamemap.insert(qname, qnamevar);
        }
    }
    QDomElement c = e.firstChildElement();
    while (!c.isNull()) {
        getQNames(c, nsmap, namemap, qnamemap);
        c = c.nextSiblingElement();
    }
}
QString
writeNames(const QMap<QString,QString>& map) {
    QString h;
    for (auto key: map.keys()) {
        h += "constexpr char " + map[key] + "[] = \"" + key + "\";\n";
    }
    return h;
}
QString
writeQNames(const QMap<QString,QString>& nsmap, const QMap<QString,QString>& namemap, const QMap<QName,QString>& map) {
    QString h;
    for (auto key: map.keys()) {
        h += QString("using %1 = rng::types::QName<%2,%3>;\n").arg(map[key]).arg(nsmap[key.ns]).arg(namemap[key.localName]);
    }
    return h;
}


QString
createHeader(const QDomElement& grammar) {

    QString h;
    QMap<QString,QString> nsmap;
    QMap<QString,QString> namemap;
    QMap<QName,QString> qnamemap;
    getQNames(grammar, nsmap, namemap, qnamemap);
    h += writeNames(nsmap);
    h += writeNames(namemap);
    h += writeQNames(nsmap, namemap, qnamemap);

    QDomElement c = grammar.firstChildElement("define");
    while (!c.isNull()) {
        h += "using " + getName(c) + " = int;\n";
        c = c.nextSiblingElement("define");
    }
    return h;
}
#include "rng.h"

void
convert(const QString& rngfile, const QString& /*outdir*/) {

    QDomDocument d = loadDOM(rngfile);

    parseGrammar(d.documentElement());

    QString h = createHeader(d.documentElement());
    QFile f("/home/oever/src/blasien/rng2cpp/rng.h");
    f.open(QIODevice::WriteOnly);
    f.write(h.toUtf8());
    f.close();
}

}

int main(int argc, char *argv[])
{
    QString rngfile;
    QString outdir;
    if (argc != 3) {
        fatal() << "Usage " << argv[0] << " rngfile outputdir";
    } else {
        rngfile = argv[1];
        outdir = argv[2];
    }
    convert(rngfile, outdir);

    return 0;
}

