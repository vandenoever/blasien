#include <rngtest.h>
#include <QDebug>

using namespace rng::types;
using namespace rng::functions;

namespace {

const QName<emptyString,emptyString> emptyQName;
const std::tuple<> emptyAttributeNodes;
const ElementStart<QName<emptyString,emptyString>,std::tuple<>> emptyElementStart{emptyAttributeNodes};
const TextNode emptyTextNode("");


constexpr char empty[] = "";
constexpr char xhtmlns[] = "http://www.w3.org/1999/xhtml";
constexpr char pLocalName[] = "p";
constexpr char brLocalName[] = "br";
constexpr char imgLocalName[] = "img";
constexpr char altLocalName[] = "alt";
constexpr char srcLocalName[] = "src";
using BrQName = QName<xhtmlns,brLocalName>;
using PQName = QName<xhtmlns,pLocalName>;
using SrcQName = QName<empty,srcLocalName>;
using AltQName = QName<empty,altLocalName>;
using ImgQName = QName<xhtmlns,imgLocalName>;
using SrcAttribute = Attribute<SrcQName,Text>;
using AltAttribute = Attribute<AltQName,Text>;
using BrElement = Element<BrQName,Empty>;
using PElement = Element<PQName,BrElement>;
using ImgElement = Element<ImgQName,Interleave<SrcAttribute,AltAttribute>>;

void testBrElement() {
    // test a pattern for one element with an instance of that element
    ElementStart<BrQName,std::tuple<>> elementStart{emptyAttributeNodes};
    using p1 = typename startTagDeriv<BrElement,decltype(elementStart)>::type;
    static_assert(std::is_same<p1, After<Empty,Empty>>(),"wrong type");
    using p2 = typename endTagDeriv<p1>::type;
    static_assert(std::is_same<p2, Empty>(),"not empty");
}
void testPElement() {
    ElementStart<PQName,std::tuple<>> pStart{emptyAttributeNodes};
    ElementStart<BrQName,std::tuple<>> brStart{emptyAttributeNodes};
    using p1 = typename startTagDeriv<PElement,decltype(pStart)>::type;
    using p2 = typename startTagDeriv<p1,decltype(brStart)>::type;
    using p3 = typename endTagDeriv<p2>::type;
    using p4 = typename endTagDeriv<p3>::type;
    static_assert(std::is_same<p4, Empty>(),"not empty");
}
void testImgElementMissingSrc() {
    using ImgInstance = ElementStart<ImgQName,std::tuple<>>;
    using p1 = typename startTagDeriv<ImgElement,ImgInstance>::type;
    static_assert(std::is_same<p1, NotAllowed>(),"should be NotAllowed");
}
void testImgElement() {
    using SrcAtt = AttributeNode<SrcQName,String>;
    using AltAtt = AttributeNode<AltQName,String>;
    using ImgStart = ElementStart<ImgQName,std::tuple<AltAtt,SrcAtt>>;
    using p1 = typename startTagDeriv<ImgElement,ImgStart>::type;
    static_assert(!std::is_same<p1, NotAllowed>(),"not allowed");
    using p2 = typename endTagDeriv<p1>::type;
    static_assert(std::is_same<p2, Empty>(),"not empty");
}
void testImgElement2() {
    using AttDef = Attribute<SrcQName,Value<srcLocalName>>;
    using ImgDef = Element<ImgQName,AttDef>;
    using SrcAtt = StringAttribute<SrcQName,srcLocalName>;
    using ImgStart = ElementStart<ImgQName,std::tuple<SrcAtt>>;
    using p1 = typename startTagDeriv<ImgDef,ImgStart>::type;
    static_assert(!std::is_same<p1, NotAllowed>(),"not allowed");
    using p2 = typename endTagDeriv<p1>::type;
    static_assert(std::is_same<p2, Empty>(),"not empty");
}
}

int main2(int , char *[]) {
    return 0;
}
