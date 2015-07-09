#ifndef RNGTEST_H
#define RNGTEST_H

#include <utility>
#include <vector>
#include <tuple>
#include <cstring>

/**
 * This relaxng validation implementation was ported from Haskell.
 * The Haskell implementation can be found here:
 *   http://www.thaiopensource.com/relaxng/derivative.html
 * The Haskell implementation is extensively documented. This implemention
 * follows function names and function order closely so the Haskell
 * documentation applies mostly to this implementation.
 * Exceptions are noted.
 */

namespace rng {

namespace types {

using String = const char*;
constexpr char emptyString[] = "";
using Uri = String;
using LocalName = String;
struct ParamList {
    struct Item {
        const LocalName& localName;
        const String& string;
    };
    const std::vector<Item>& paramList;
};
template <Uri U, LocalName L>
struct Datatype;
/** NameClasses */
struct AnyName {};
template <typename NameClass>
struct AnyNameExcept  {
    const NameClass& nameClass;
    explicit constexpr AnyNameExcept(const NameClass& n)
        :nameClass(n) {}
};
template <String U>
struct NsName {
    static constexpr String Uri = U;
};
template <String U,typename NC>
struct NsNameExcept {
    static constexpr String Uri = U;
    using NameClass = NC;
};
template <typename NC1, typename NC2>
struct NameClassChoice {
    using NameClass1 = NC1;
    using NameClass2 = NC2;
};

/** Patterns */
struct Empty {};
struct NotAllowed {};
struct Text {};
template <typename Pattern1, typename Pattern2>
struct Choice;
template <typename Pattern1, typename Pattern2>
struct Interleave;
template <typename Pattern1, typename Pattern2>
struct Group;
template <typename Pattern>
struct OneOrMore;
template <typename Pattern>
struct List;
template <typename Pattern>
struct Data;
template <typename Pattern>
struct DataExcept;
template <String V>
struct Value;
template <typename NameClass, typename Pattern>
struct Attribute;
template <typename NC, typename P>
struct Element;
template <typename Pattern1, typename Pattern2>
struct After;
template <String U, String L>
struct QName {
    static constexpr String Uri = U;
    static constexpr String LocalName = L;
};
template <typename QName, typename ValueType>
struct AttributeNode {};
template <typename QName, String Value>
struct StringAttribute {};
template <typename QName, typename Atts>
struct ElementStart;

template <String U, String L, typename... Atts>
struct ElementStart<QName<U,L>,std::tuple<Atts...>> {
    using QName = rng::types::QName<U,L>;
    using Attributes = std::tuple<Atts...>;
    const Attributes attributes;
    explicit constexpr ElementStart(const Attributes& a)
        :attributes(a) {}
    ElementStart(const ElementStart&) = delete;
};
struct TextNode {
    const String& text;
    explicit constexpr TextNode(const String& t) :text(t) {}
};
}

namespace functions {
using namespace rng::types;

/** declaratations */
template <typename NameClass, typename QName>
struct contains_struct;
template <typename NameClass, typename QName>
constexpr bool contains() {
    return contains_struct<NameClass, QName>::value;
}
template <typename Pattern>
struct nullable_struct;
template <typename Pattern>
constexpr bool
nullable() {
    return nullable_struct<Pattern>::value;
}
/* This code uses startTagDeriv instead of childDeriv.
 * Since not all children are know at compile time, childrenDeriv is left out
 * and endTagDeriv and textDeriv must be called explicitly.
 */
template <typename Pattern, typename ElementStart>
struct startTagDeriv;
template <typename Pattern, String V>
struct textDeriv;
// TODO: listDeriv

template <typename Pattern1, typename Pattern2>
struct choice;
template <typename Pattern1, typename Pattern2>
struct group;
template <typename Pattern1, typename Pattern2>
struct interleave;
template <typename Pattern1, typename Pattern2>
struct after;
// TODO: datatypeAllows
// TODO: datatypeEqual
// TODO: normalizeWhitespace
template <template<typename,typename> class F, typename P, typename Pattern>
struct applyAfter;
// instead of implementing 'flip', a flipped variant of applyAfter is used
template <template<typename,typename> class F, typename P, typename Pattern>
struct applyAfterFlip;
template <typename Pattern, typename QName>
struct startTagOpenDeriv;
template <typename Pattern, typename Att>
struct attsDeriv;
template <typename Pattern, typename Att>
struct attDeriv;
template <typename Pattern, String V>
struct valueMatch;
template <typename Pattern>
struct startTagCloseDeriv;
template <typename Pattern>
struct oneOrMore;
// childrenDeriv, stripChildrenDeriv,strip, and whitespace are not implemented
// in this code
template <String v>
struct whitespace;
template <typename Pattern>
struct endTagDeriv;

/** specializations */
template <String U2, String L2>
struct contains_struct<AnyName,QName<U2,L2>> {
    static constexpr bool value = true;
};
template <typename NameClass, String U2, String L2>
struct contains_struct<AnyNameExcept<NameClass>,QName<U2,L2>> {
    static constexpr bool value = !contains<NameClass,QName<U2,L2>>();
};
template <String U1, String U2, String L2>
struct contains_struct<NsName<U1>,QName<U2,L2>> {
    static constexpr bool value = strcmp(U1, U2) == 0;
};
template <String U1, typename NameClass, String U2, String L2>
struct contains_struct<NsNameExcept<U1,NameClass>,QName<U2,L2>> {
    static constexpr bool value = strcmp(U1, U2) == 0 && !contains<NameClass,QName<U2,L2>>();
};
template <String U1, String L1, String U2, String L2>
struct contains_struct<QName<U1,L1>,QName<U2,L2>> {
    static constexpr bool value = strcmp(U1, U2) == 0 && strcmp(L1, L2) == 0;
};
template <typename NameClass1, typename NameClass2,String U2, String L2>
struct contains_struct<NameClassChoice<NameClass1,NameClass2>,QName<U2,L2>> {
    static constexpr bool value = contains<NameClass1,QName<U2,L2>>()
        || contains<NameClass2,QName<U2,L2>>();
};

template <typename Pattern1, typename Pattern2>
struct nullable_struct<Group<Pattern1,Pattern2>> {
    static constexpr bool value = nullable<Pattern1>() && nullable<Pattern2>();
};
template <typename Pattern1, typename Pattern2>
struct nullable_struct<Interleave<Pattern1,Pattern2>> {
    static constexpr bool value = nullable<Pattern1>() && nullable<Pattern2>();
};
template <typename Pattern1, typename Pattern2>
struct nullable_struct<Choice<Pattern1,Pattern2>> {
    static constexpr bool value = nullable<Pattern1>() || nullable<Pattern2>();
};
template <typename Pattern>
struct nullable_struct<OneOrMore<Pattern>> {
    static constexpr bool value = nullable<Pattern>();
};
template <typename NameClass, typename Pattern>
struct nullable_struct<Element<NameClass, Pattern>> {
    static constexpr bool value = false;
};
template <typename NameClass, typename Pattern>
struct nullable_struct<Attribute<NameClass, Pattern>> {
    static constexpr bool value = false;
};
template <typename Pattern>
struct nullable_struct<List<Pattern>> {
    static constexpr bool value = false;
};
template <String V>
struct nullable_struct<Value<V>> {
    static constexpr bool value = false;
};
template <typename Pattern>
struct nullable_struct<Data<Pattern>> {
    static constexpr bool value = false;
};
template <typename Pattern>
struct nullable_struct<DataExcept<Pattern>> {
    static constexpr bool value = false;
};
template <>
struct nullable_struct<NotAllowed> {
    static constexpr bool value = false;
};
template <>
struct nullable_struct<Empty> {
    static constexpr bool value = true;
};
template <>
struct nullable_struct<Text> {
    static constexpr bool value = true;
};
template <typename Pattern1, typename Pattern2>
struct nullable_struct<After<Pattern1,Pattern2>> {
    static constexpr bool value = false;
};

template <typename Pattern, String U, String L, typename... Args>
struct startTagDeriv<Pattern, ElementStart<QName<U,L>,std::tuple<Args...>>> {
    using p1 = typename startTagOpenDeriv<Pattern, QName<U,L>>::type;
    using p2 = typename attsDeriv<p1, std::tuple<Args...>>::type;
    using type = typename startTagCloseDeriv<p2>::type;
};


template <typename Pattern1, typename Pattern2, String V>
struct textDeriv<Choice<Pattern1,Pattern2>,V> {
    using p1 = typename textDeriv<Pattern1,V>::type;
    using p2 = typename textDeriv<Pattern1,V>::type;
    using type = typename choice<p1,p2>::type;
};
template <typename Pattern1, typename Pattern2, String V>
struct textDeriv<Interleave<Pattern1,Pattern2>,V> {
    using p1 = typename textDeriv<Pattern1,V>::type;
    using p2 = typename textDeriv<Pattern1,V>::type;
    using i1 = typename interleave<p1,Pattern2>::type;
    using i2 = typename interleave<Pattern1,p2>::type;
    using type = typename choice<i1,i2>::type;
};
template <typename Pattern1, typename Pattern2, String V>
struct textDeriv<Group<Pattern1,Pattern2>,V> {
    using p1 = typename textDeriv<Pattern1,V>::type;
    using p2 = typename textDeriv<Pattern1,V>::type;
    using p = typename group<p1,Pattern2>::type;
    using c = typename choice<p,p2>::type;
    using type = typename std::conditional<nullable<Pattern1>(),c,p>::type;
};
template <typename Pattern1, typename Pattern2, String V>
struct textDeriv<After<Pattern1,Pattern2>,V> {
    using p1 = typename textDeriv<Pattern1,V>::type;
    using type = typename after<p1,Pattern2>::type;
};
template <typename Pattern, String V>
struct textDeriv<OneOrMore<Pattern>,V> {
    using p = typename textDeriv<Pattern,V>::type;
    using c = typename choice<OneOrMore<Pattern>,Empty>::type;
    using type = typename group<p,c>::type;
};
template <String V>
struct textDeriv<Text,V> {
    using type = Text;
};
// TODO: implement for more types than String
template <String V>
struct textDeriv<Value<V>,V> {
    using type = Empty;
};
template <String V, String W>
struct textDeriv<Value<V>,W> {
    using type = NotAllowed;
};
// TODO: implement for Data<>
// TODO: implement for DataExcept<>
// TODO: implement for List<>
template <typename Pattern, String V>
struct textDeriv {
    using type = NotAllowed;
};

// TODO: listDeriv

template <typename Pattern>
struct choice<Pattern,NotAllowed> {
    using type = Pattern;
};
template <typename Pattern>
struct choice<NotAllowed,Pattern> {
    using type = Pattern;
};
template <typename Pattern1, typename Pattern2>
struct choice {
    using type = Choice<Pattern1,Pattern2>;
};

template <typename Pattern>
struct group<Pattern,NotAllowed> {
    using type = NotAllowed;
};
template <typename Pattern>
struct group<NotAllowed,Pattern> {
    using type = NotAllowed;
};
template <typename Pattern>
struct group<Pattern,Empty> {
    using type = Pattern;
};
template <typename Pattern>
struct group<Empty,Pattern> {
    using type = Pattern;
};
template <typename Pattern1, typename Pattern2>
struct group {
    using type = Group<Pattern1,Pattern2>;
};

template <>
struct interleave<NotAllowed,NotAllowed> {
    using type = NotAllowed;
};
template <typename Pattern>
struct interleave<Pattern,NotAllowed> {
    using type = NotAllowed;
};
template <typename Pattern>
struct interleave<NotAllowed,Pattern> {
    using type = NotAllowed;
};
template <typename Pattern>
struct interleave<Pattern,Empty> {
    using type = Pattern;
};
template <typename Pattern>
struct interleave<Empty,Pattern> {
    using type = Pattern;
};
template <typename Pattern1, typename Pattern2>
struct interleave {
    using type = Interleave<Pattern1,Pattern2>;
};

template <typename Pattern>
struct after<Pattern,NotAllowed> {
    using type = NotAllowed;
};
template <typename Pattern>
struct after<NotAllowed,Pattern> {
    using type = NotAllowed;
};
template <typename Pattern1, typename Pattern2>
struct after {
    using type = After<Pattern1,Pattern2>;
};

// TODO: datatypeAllows
// TODO: datatypeEqual
// TODO: normalizeWhitespace

template <template<typename,typename> class F, typename P, typename Pattern>
struct applyAfter {
    using type = NotAllowed;
};
template <template<typename,typename> class F, typename P, typename Pattern>
struct applyAfterFlip {
    using type = NotAllowed;
};
template <template<typename,typename> class F, typename P, typename Pattern1, typename Pattern2>
struct applyAfter<F, P, After<Pattern1,Pattern2>> {
    using p1 = typename F<P,Pattern2>::type;
    using type = typename after<Pattern1, p1>::type;
};
template <template<typename,typename> class F, typename P, typename Pattern1, typename Pattern2>
struct applyAfterFlip<F, P, After<Pattern1,Pattern2>> {
    using p1 = typename F<Pattern2,P>::type;
    using type = typename after<Pattern1, p1>::type;
};
template <template<typename,typename> class F, typename P, typename Pattern1, typename Pattern2>
struct applyAfter<F, P, Choice<Pattern1,Pattern2>> {
    using p1 = typename applyAfter<F,P,Pattern1>::type;
    using p2 = typename applyAfter<F,P,Pattern2>::type;
    using type = typename choice<p1, p2>::type;
};
template <template<typename,typename> class F, typename P, typename Pattern1, typename Pattern2>
struct applyAfterFlip<F, P, Choice<Pattern1,Pattern2>> {
    using p1 = typename applyAfterFlip<F,P,Pattern1>::type;
    using p2 = typename applyAfterFlip<F,P,Pattern2>::type;
    using type = typename choice<p1, p2>::type;
};

template <typename Pattern1, typename Pattern2, typename QName>
struct startTagOpenDeriv<Choice<Pattern1,Pattern2>,QName> {
    using c1 = typename startTagOpenDeriv<Pattern1,QName>::type;
    using c2 = typename startTagOpenDeriv<Pattern2,QName>::type;
    using type = typename choice<c1,c2>::type;
};
template <typename NameClass, typename Pattern, typename QName>
struct startTagOpenDeriv<Element<NameClass,Pattern>,QName> {
    static constexpr bool contains = contains<NameClass,QName>();
    using after = typename after<Pattern, Empty>::type;
    using type = typename std::conditional<contains,after,NotAllowed>::type;
};
template <typename Pattern1, typename Pattern2, typename QName>
struct startTagOpenDeriv<Interleave<Pattern1,Pattern2>,QName> {
    using p1 = typename startTagOpenDeriv<Pattern1,QName>::type;
    using p2 = typename startTagOpenDeriv<Pattern2,QName>::type;
    using c1 = typename applyAfterFlip<interleave,Pattern2,p1>::type;
    using c2 = typename applyAfter<interleave,Pattern1,p2>::type;
    using type = typename choice<c1,c2>::type;
};
template <typename Pattern, typename QName>
struct startTagOpenDeriv<OneOrMore<Pattern>,QName> {
    using p = typename startTagOpenDeriv<Pattern,QName>::type;
    using c = typename choice<OneOrMore<Pattern>,Empty>::type;
    using type = typename applyAfterFlip<group,c,p>::type;
};
template <typename Pattern1, typename Pattern2, typename QName>
struct startTagOpenDeriv<Group<Pattern1,Pattern2>,QName> {
    using p1 = typename startTagOpenDeriv<Pattern1,QName>::type;
    using p2 = typename startTagOpenDeriv<Pattern2,QName>::type;
    using x = typename applyAfterFlip<group,Pattern2,p1>::type;
    using c = typename choice<x,p2>::type;
    using type = typename std::conditional<nullable<Pattern1>(),c,x>::type;
};
template <typename Pattern1, typename Pattern2, typename QName>
struct startTagOpenDeriv<After<Pattern1,Pattern2>,QName> {
    using p1 = typename startTagOpenDeriv<Pattern1,QName>::type;
    using type = typename applyAfterFlip<after, Pattern2, p1>::type;
};
template <typename Pattern, typename QName>
struct startTagOpenDeriv {
    using type = NotAllowed;
};

template <typename Pattern, typename Att, typename... Atts>
struct attsDeriv<Pattern, std::tuple<Att, Atts...>> {
    using p = typename attDeriv<Pattern, Att>::type;
    using type = typename attsDeriv<p, std::tuple<Atts...>>::type;
};
template <typename Pattern>
struct attsDeriv<Pattern, std::tuple<>> {
    using type = Pattern;
};

template <typename Pattern1, typename Pattern2, typename Att>
struct attDeriv<After<Pattern1,Pattern2>,Att> {
    using p1 = typename attDeriv<Pattern1,Att>::type;
    using type = typename after<p1,Pattern2>::type;
};
template <typename Pattern1, typename Pattern2, typename Att>
struct attDeriv<Choice<Pattern1,Pattern2>,Att> {
    using p1 = typename attDeriv<Pattern1,Att>::type;
    using p2 = typename attDeriv<Pattern2,Att>::type;
    using type = typename choice<p1,p2>::type;
};
template <typename Pattern1, typename Pattern2, typename Att>
struct attDeriv<Group<Pattern1,Pattern2>,Att> {
    using p1 = typename attDeriv<Pattern1,Att>::type;
    using p2 = typename attDeriv<Pattern2,Att>::type;
    using g1 = typename group<p1,Pattern2>::type;
    using g2 = typename group<Pattern1,p2>::type;
    using type = typename choice<g1,g2>::type;
};
template <typename Pattern1, typename Pattern2, typename Att>
struct attDeriv<Interleave<Pattern1,Pattern2>,Att> {
    using p1 = typename attDeriv<Pattern1,Att>::type;
    using p2 = typename attDeriv<Pattern2,Att>::type;
    using i1 = typename interleave<p1,Pattern2>::type;
    using i2 = typename interleave<Pattern1,p2>::type;
    using type = typename choice<i1,i2>::type;
};
template <typename Pattern, typename Att>
struct attDeriv<OneOrMore<Pattern>,Att> {
    using p = typename attDeriv<Pattern,Att>::type;
    using c = typename choice<OneOrMore<Pattern>,Empty>::type;
    using type = typename group<p,c>::type;
};
template <typename NameClass, typename Pattern, typename QName, typename T>
struct attDeriv<Attribute<NameClass,Pattern>,AttributeNode<QName,T>> {
    static constexpr bool c = contains<NameClass,QName>();
    // there is not enough information to do value matching
    // TODO: check data type
    using type = typename std::conditional<c,Empty,NotAllowed>::type;
};
template <typename NameClass, typename Pattern, typename QName, String V>
struct attDeriv<Attribute<NameClass,Pattern>,StringAttribute<QName,V>> {
    static constexpr bool c = contains<NameClass,QName>();
    static constexpr bool vm = valueMatch<Pattern,V>::value;
    using type = typename std::conditional<c && vm,Empty,NotAllowed>::type;
};
template <typename Pattern, typename Att>
struct attDeriv {
    using type = NotAllowed;
};

template <typename Pattern, String V>
struct valueMatch {
    using p = typename textDeriv<Pattern, V>::type;
    static constexpr bool value = (nullable<Pattern>() && whitespace<V>::value)
            || nullable<p>();
};

template <typename Pattern1, typename Pattern2>
struct startTagCloseDeriv<After<Pattern1,Pattern2>> {
    using p1 =  typename startTagCloseDeriv<Pattern1>::type;
    using type = typename after<p1, Pattern2>::type;
};
template <typename Pattern1, typename Pattern2>
struct startTagCloseDeriv<Choice<Pattern1,Pattern2>> {
    using p1 =  typename startTagCloseDeriv<Pattern1>::type;
    using p2 =  typename startTagCloseDeriv<Pattern2>::type;
    using type = typename choice<p1, p2>::type;
};
template <typename Pattern1, typename Pattern2>
struct startTagCloseDeriv<Group<Pattern1,Pattern2>> {
    using p1 =  typename startTagCloseDeriv<Pattern1>::type;
    using p2 =  typename startTagCloseDeriv<Pattern2>::type;
    using type = typename group<p1, p2>::type;
};
template <typename Pattern1, typename Pattern2>
struct startTagCloseDeriv<Interleave<Pattern1,Pattern2>> {
    using p1 =  typename startTagCloseDeriv<Pattern1>::type;
    using p2 =  typename startTagCloseDeriv<Pattern2>::type;
    using type = typename interleave<p1, p2>::type;
};
template <typename Pattern>
struct startTagCloseDeriv<OneOrMore<Pattern>> {
    using p =  typename startTagCloseDeriv<Pattern>::type;
    using type = typename oneOrMore<p>::type;
};
template <typename NameClass, typename Pattern>
struct startTagCloseDeriv<Attribute<Pattern, NameClass>> {
    using type = NotAllowed;
};
template <typename Pattern>
struct startTagCloseDeriv {
    using type = Pattern;
};

template <typename Pattern>
struct oneOrMore {
    using type = Pattern;
};
template <>
struct oneOrMore<NotAllowed> {
    using type = NotAllowed;
};

// childrenDeriv, stripChildrenDeriv, and strip are ommitted

template <String V>
struct whitespace {
    static constexpr bool value = strspn(V, " \t\n\r") == strlen(V);
};

template <typename Pattern1, typename Pattern2>
struct endTagDeriv<Choice<Pattern1,Pattern2>> {
    using p1 = typename endTagDeriv<Pattern1>::type;
    using p2 = typename endTagDeriv<Pattern2>::type;
    using type = typename choice<p1, p2>::type;
};
template <typename Pattern1, typename Pattern2>
struct endTagDeriv<After<Pattern1,Pattern2>> {
    const bool c = nullable<Pattern1>();
    using type = typename std::conditional<nullable<Pattern1>(),Pattern2,NotAllowed>::type;
};
template <typename Pattern>
struct endTagDeriv {
    using type = NotAllowed;
};
}
} // rng

#endif // RNGTEST_H

