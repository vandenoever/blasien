#include <QString>

namespace {

class Writer {

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

struct EndElement {
};

Writer operator<(Writer a, QName) {
    return a;
}

Writer operator>(Writer a, QName) {
    return a;
}

Writer operator<=(Writer a, QName) {
    return a;
}

Writer operator<(Writer a, const char*) {
    return a;
}


Writer operator<(Writer a, EndElement) {
    return a;
}

Writer operator>(Writer a, EndElement) {
    return a;
}

EndElement operator!(EndElement) {
    return EndElement();
}

EndElement end() {
    return EndElement();
}
void crazy() {
    Writer a;
    a <html
        <head
          <title
            <"hello world"
          <end()
        <end()
        <body
          <p
            <"hello "
            <span
              <"world"
            <end()
          <end()
          <br<end()
        <end()
      <end();
}

static const EndElement _;

struct D {};
void operator+=(D, std::initializer_list<int>) {}

void crazy2() {
    Writer a;
    a <html
        <head
          <title
            <"hello world"
          <_
        <_
        <body
          <p
            <"hello "
            <span
              <"world"
            <_
          <_
          <br<_
        <0
      <_;
}

void crazy3() {
    Writer a;
    a <html
        <head
          <title
            <"hello world"
        <body
          <p
            <"hello "
            <span
              <"world"
          <br;
}

void crazy4() {
    Writer a;
    a <html
        <head
          <=title
            <"hello world"
        <=body
          <p
            <"hello "
            <=span
              <"world"
          <=br;
}

void crazy5() {
    Writer a;
    a <html
        <head
          <title
            <"hello world"
          >title
        >head
        <body
          <p
            <"hello "
            <span
              <"world"
            >span
          >p
          <br>br
        >body
      >html;
}

Writer
makeParagraph(Writer w) {
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
    Writer operator()(Writer w) {return w;}
};

template<typename F>
auto operator<(Writer w, F f) -> decltype(f(w)) {
    return f(w);
}

void crazy6() {
    Functor f;
    Writer a;
    a <html
        <head
          <title
            <"hello world"
          >title
        >head
        <body
          <makeParagraph
          <f
          <br>br
        >body
      >html;
}

}
