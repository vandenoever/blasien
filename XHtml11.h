/****************************************************************************
**
** Copyright (C) 2015 Jos van den Oever
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 3 as published by the Free Software Foundation and appearing
** in the file lgpl-3.0.txt included in the packaging of this file.  Please
** review the following information to ensure the GNU Lesser General Public
** License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/
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
const QString imgTag = QStringLiteral("img");
const QString idTag = QStringLiteral("id");
const QString classTag = QStringLiteral("class");
const QString srcTag = QStringLiteral("src");
const QString altTag = QStringLiteral("alt");
const QString empty;

using HtmlTag = XmlTag<QString,&htmlns, &htmlTag>;
using HeadTag = XmlTag<QString,&htmlns, &headTag>;
using BodyTag = XmlTag<QString,&htmlns, &bodyTag>;
using TitleTag = XmlTag<QString,&htmlns, &titleTag>;
using DivTag = XmlTag<QString,&htmlns, &divTag>;
using PTag = XmlTag<QString,&htmlns, &pTag>;
using ImgTag = XmlTag<QString,&htmlns, &imgTag>;
using IdTag = XmlTag<QString,&empty, &idTag>;
using ClassTag = XmlTag<QString,&empty, &classTag>;
using SrcTag = XmlTag<QString,&empty, &srcTag>;
using AltTag = XmlTag<QString,&empty, &altTag>;

static HtmlTag html;
static HeadTag head;
static TitleTag title;
static BodyTag body;
static DivTag div;
static PTag p;
static ImgTag img;

struct XHtmlDocument {
};

struct HtmlType {
    using Tag = HtmlTag;
    using allowedAttributes = std::tuple<xhtml11::IdTag,xhtml11::ClassTag>;
};
struct HeadType {
    using Tag = HeadTag;
    using allowedAttributes = std::tuple<xhtml11::IdTag,xhtml11::ClassTag>;
};
struct BodyType {
    using Tag = BodyTag;
    using allowedAttributes = std::tuple<xhtml11::IdTag,xhtml11::ClassTag>;
};
struct TitleType {
    using Tag = TitleTag;
    using allowedAttributes = std::tuple<xhtml11::IdTag,xhtml11::ClassTag>;
};
struct DivType {
    using Tag = DivTag;
    using allowedAttributes = std::tuple<xhtml11::IdTag,xhtml11::ClassTag>;
};
struct PType {
    using Tag = PTag;
    using allowedAttributes = std::tuple<xhtml11::IdTag,xhtml11::ClassTag>;
};
struct ImgType {
    using Tag = ImgTag;
    using allowedAttributes = std::tuple<xhtml11::IdTag,xhtml11::ClassTag>;
    using requiredAttributes = std::tuple<xhtml11::SrcTag,xhtml11::AltTag>;
};

}

xhtml11::IdTag id;
xhtml11::ClassTag class_;
xhtml11::SrcTag src;
xhtml11::AltTag alt;

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
struct allowed_child_types<xhtml11::TitleType> {
    using types = std::tuple<TextType>;
};

template <>
struct allowed_child_types<xhtml11::BodyType> {
    using types = std::tuple<xhtml11::DivType,xhtml11::PType,xhtml11::ImgType>;
};

template <>
struct allowed_child_types<xhtml11::DivType> {
    using types = std::tuple<xhtml11::DivType,xhtml11::PType,xhtml11::ImgType>;
};

template <>
struct allowed_child_types<xhtml11::PType> {
    using types = std::tuple<xhtml11::ImgType,TextType>;
};

template <>
struct allowed_child_types<xhtml11::ImgType> {
    using types = std::tuple<>;
};

#endif
