#ifndef RELAXNG_H
#define RELAXNG_H


namespace rng {

const QString rngns = QStringLiteral("http://www.w3.org/1999/xhtml");
const QString grammarTag = QStringLiteral("grammar");
const QString defineTag = QStringLiteral("define");
const QString startTag = QStringLiteral("start");
const QString optionalTag = QStringLiteral("optional");
const QString dataLibraryTag = QStringLiteral("dataLibrary");
const QString empty;

using GrammarTag = XmlTag<QString,&rngns, &grammarTag>;
using DefineTag = XmlTag<QString,&rngns, &defineTag>;
using StartTag = XmlTag<QString,&rngns, &startTag>;
using OptionalTag = XmlTag<QString,&rngns, &optionalTag>;
using DataLibraryTag = XmlTag<QString,&empty, &dataLibraryTag>;

const GrammarTag grammar;
const DefineTag define;
const StartTag start;
const OptionalTag optional;
const DataLibraryTag dataLibrary;

struct RelaxNGDocument {
};

struct GrammarType {
    using Tag = GrammarTag;
    using allowedAttributes = std::tuple<rng::DataLibraryTag>;
};
struct DefineType {
    using Tag = DefineTag;
    using allowedAttributes = std::tuple<>;
};
struct StartType {
    using Tag = StartTag;
    using allowedAttributes = std::tuple<>;
};

}

const rng::DataLibraryTag dataLibrary;

template <>
struct allowed_child_types<rng::RelaxNGDocument> {
    using types = std::tuple<rng::GrammarType>;
};

template <>
struct allowed_child_types<rng::GrammarType> {
    using types = std::tuple<rng::DefineType, rng::StartType>;
};

#endif // RELAXNG_H

