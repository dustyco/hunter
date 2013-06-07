#pragma once


#include "hmath/linear_algebra.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

template <class R> vec<2,R>       convert (const sf::Vector2<R>& v) { return vec<2,R>(v.x, v.y); }
template <class R> sf::Vector2<R> convert (const vec<2,R>& v)       { return sf::Vector2<R>(v.x, v.y); }

template <class R> vec<3,R>       convert (const sf::Vector3<R>& v) { return vec<3,R>(v.x, v.y, v.z); }
template <class R> sf::Vector3<R> convert (const vec<3,R>& v)       { return sf::Vector3<R>(v.x, v.y, v.z); }
