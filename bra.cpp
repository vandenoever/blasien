#include <QString>

namespace {

template <typename Base = int, typename Tag = int>
class Writer {
public:
};

struct QName {
    const QString ns;
    const QString name;
    QName(const QString& ns_, const QString& name_) :ns(ns_), name(name_) {}
};

QString xhtmlns = "http://www.w3.org/1999/xhtml";

struct HtmlQName : public QName {
    HtmlQName() :QName(xhtmlns, "html") {}
};
struct HeadQName : public QName {
    HeadQName() :QName(xhtmlns, "head") {}
};
struct TitleQName : public QName {
    TitleQName() :QName(xhtmlns, "title") {}
};
struct BodyQName : public QName {
    BodyQName() :QName(xhtmlns, "body") {}
};
struct PQName : public QName {
    PQName() :QName(xhtmlns, "p") {}
};
struct SpanQName : public QName {
    SpanQName() :QName(xhtmlns, "span") {}
};
struct BrQName : public QName {
    BrQName() :QName(xhtmlns, "br") {}
};

static const HtmlQName html;
static const HeadQName head;
static const TitleQName title;
static const BodyQName body;
static const PQName p;
static const SpanQName span;
static const BrQName br;

template <typename Base, typename Tag, typename ChildTag>
Writer<Writer<Base,Tag>, ChildTag> operator<(Writer<Base,Tag>, const ChildTag&) {
    return Writer<Writer<Base,Tag>, ChildTag>();
}

template <typename Base, typename Tag>
Base operator>(Writer<Base,Tag>, const Tag&) {
    return Base();
}

template <typename Base, typename Tag>
Writer<Base,Tag> operator<(Writer<Base,Tag>, const char*) {
    return Writer<Base,Tag>();
}

template <typename Base, typename Tag>
Writer<Base,Tag>
makeParagraph(Writer<Base,Tag> w) {
    return w
    <p
      <"hello "
      <span
        <"world"
      >span
    >p;
}

class Functor {
public:
    template <typename Base, typename Tag>
    Writer<Base,Tag> operator()(Writer<Base,Tag> w) {return w;}
};

template<typename F, typename Base, typename Tag>
auto operator<(Writer<Base,Tag> w, F f) -> decltype(f(w)) {
    return f(w);
}

template<typename Base, typename Tag>
Writer<Base,Tag> operator<(Writer<Base,Tag> w, Writer<Base,Tag> (*f)(Writer<Base,Tag>)) {
    return f(w);
}

void test() {
    Functor f;
    Writer<> a;
    a <html
        <head
          <title
            <"hello world"
          >title
        >head
        <body
          <(makeParagraph)
          <br>br
          //<f
          <br>br
        >body
      >html;
}

}
