#ifndef XHTML11_H
#define XHTML11_H

namespace xhtml11 {

const QString htmlns = QStringLiteral("http://www.w3.org/1999/xhtml");
const QString htmlTag = QStringLiteral("html");
const QString headTag = QStringLiteral("head");
const QString bodyTag = QStringLiteral("body");
const QString titleTag = QStringLiteral("title");
const QString divTag = QStringLiteral("div");
const QString pTag = QStringLiteral("p");
const QString idTag = QStringLiteral("id");
const QString classTag = QStringLiteral("class");
const QString empty;

using HtmlTag = XmlTag<QString,&htmlns, &htmlTag>;
using HeadTag = XmlTag<QString,&htmlns, &headTag>;
using BodyTag = XmlTag<QString,&htmlns, &bodyTag>;
using TitleTag = XmlTag<QString,&htmlns, &titleTag>;
using DivTag = XmlTag<QString,&htmlns, &divTag>;
using PTag = XmlTag<QString,&htmlns, &pTag>;

static HtmlTag html;
static HeadTag head;
static TitleTag title;
static BodyTag body;
static DivTag div;
static PTag p;

struct XHtmlDocument {
};

struct HtmlType {
    using Tag = HtmlTag;
};
struct HeadType {
    using Tag = HeadTag;
};
struct BodyType {
    using Tag = BodyTag;
};
struct TitleType {
    using Tag = TitleTag;
};
struct DivType {
    using Tag = DivTag;
};
struct PType {
    using Tag = PTag;
};

}

template <>
struct allowed_child_types<xhtml11::XHtmlDocument> {
    using types = std::tuple<xhtml11::HtmlType>;
};

template <>
struct allowed_child_types<xhtml11::HtmlType> {
    using types = std::tuple<xhtml11::HeadType, xhtml11::BodyType>;
};

template <>
struct allowed_child_types<xhtml11::HeadType> {
    using types = std::tuple<xhtml11::TitleType>;
};

template <>
struct allowed_child_types<xhtml11::BodyType> {
    using types = std::tuple<xhtml11::DivType,xhtml11::PType>;
};

template <>
struct allowed_child_types<xhtml11::DivType> {
    using types = std::tuple<xhtml11::DivType,xhtml11::PType>;
};

template <>
struct allowed_child_types<xhtml11::PType> {
    using types = std::tuple<>;
};

#endif
