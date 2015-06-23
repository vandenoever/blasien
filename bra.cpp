#include <QString>

namespace {

template <int depth_ = 0>
class Writer {
public:
    //using int d = depth_;
    static const int depth = depth_;
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

template <int depth>
Writer<depth+1> operator<(Writer<depth>, const QName&) {
    return Writer<depth+1>();
}

template <int depth>
Writer<depth-1> operator>(Writer<depth>, const QName&) {
    return Writer<depth-1>();
}

template <int depth>
Writer<depth> operator<(Writer<depth>, const char*) {
    return Writer<depth>();
}

template <int depth>
Writer<depth>
makeParagraph(Writer<depth> w) {
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
    template<int depth>
    Writer<depth> operator()(Writer<depth> w) {return w;}
};

template<typename F, int depth>
auto operator<(Writer<depth> w, F f) -> decltype(f(w)) {
    return f(w);
}

template<int depth>
Writer<depth> operator<(Writer<depth> w, Writer<depth> (*f)(Writer<depth>)) {
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
