#include <QString>

namespace {

template <typename T = int, typename P = int>
class Writer {
public:
    using Base = T;
    using Tag = P;
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

template <typename T, typename P>
Writer<Writer<T,P>, QName> operator<(Writer<T,P>, const QName&) {
    return Writer<Writer<T,P>, QName>();
}

template <typename T, typename P>
T operator>(Writer<T,P>, const QName&) {
    return T();
}

template <typename T, typename P>
Writer<T,P> operator<(Writer<T,P>, const char*) {
    return Writer<T,P>();
}

template <typename T, typename P>
Writer<T,P>
makeParagraph(Writer<T,P> w) {
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
    template <typename T, typename P>
    Writer<T,P> operator()(Writer<T,P> w) {return w;}
};

template<typename F, typename T, typename P>
auto operator<(Writer<T,P> w, F f) -> decltype(f(w)) {
    return f(w);
}

template<typename T, typename P>
Writer<T,P> operator<(Writer<T,P> w, Writer<T,P> (*f)(Writer<T,P>)) {
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
          <f
          <br>br
        >body
      >html;
}

}
