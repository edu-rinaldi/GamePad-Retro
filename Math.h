#ifndef MATH_H
#define MATH_H

/*
    VERY tiny math library for basic vector operations.
    Following choices are made for optimizing memory:
        - Points and vectors are same thing (basically 2d coordinates)
        - Only 2d vectors have been implemented
        - Since the screen space is mapped by only positive low integers we use uint8_t as main type
*/

#include "Arduino.h"
#include "Util.h"

// Positive modulo operation (e.g. -1 modulo 3 should give 2 not -1 like '%' operator does)
static inline int posmod(int i, int n) { return (i % n + n) % n; }

// 2D Vector where x and y can be float
struct vec2f
{
    // Coordinates
    float x, y;
    
    inline vec2f operator+(const vec2f &other) const { return {x + other.x, y + other.y}; }
    inline vec2f &operator+=(const vec2f &other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    inline vec2f operator-(const vec2f &other) const { return {x - other.x, y - other.y}; }
    inline vec2f operator-() const { return {-x, -y}; }
    inline vec2f operator*(const vec2f &other) const { return {x * other.x, y * other.y}; }
    inline vec2f operator/(const vec2f &other) const { return {x / other.x, y / other.y}; }

    inline bool operator==(const vec2f &other) const { return x == other.x && y == other.y; }
    inline bool operator!=(const vec2f &other) const { return !(*this == other); }
};

// 2D Vector where x and y can be integers (more specifically 1 byte integers)
struct vec2i
{
    // Coordinates
    uint8_t x, y;

    inline vec2i operator+(const vec2i &other) const { return {x + other.x, y + other.y}; }
    inline vec2i &operator+=(const vec2i &other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    inline vec2i operator-(const vec2i &other) const { return {x - other.x, y - other.y}; }
    inline vec2i operator-() const { return {-x, -y}; }
    inline vec2i operator*(const vec2i &other) const { return {x * other.x, y * other.y}; }
    inline vec2i operator*(int other) const { return {x * other, y * other}; }
    inline vec2i operator*(float other) const { return {x * other, y * other}; }
    inline vec2i operator/(const vec2i &other) const { return {x / other.x, y / other.y}; }

    inline bool operator==(const vec2i &other) const { return x == other.x && y == other.y; }
    inline bool operator!=(const vec2i &other) const { return !(*this == other); }
};

// Euclidean distance between two points
static inline float distance(const vec2i &a, const vec2i &b)
{
    return sqrt(pow(a.x - b.x, 2.f) + pow(a.y - b.y, 2.f));
}

// Check if a point (p) is between the line composed by a and b points
static inline bool pointBetween(const vec2i &p, const vec2i &a, const vec2i &b)
{
    return distance(a, p) + distance(b, p) == distance(a, b);
}

/* 
Check if a point (p) is inside a rectangle
Rectangle defined as:
  - position (pBBox)
  - width (widthBBox) and height (heightBBox)
*/
static inline bool inside(const vec2i &p, const vec2i &pBBox, int widthBBox, int heightBBox)
{
    return p.x >= pBBox.x && p.x < pBBox.x + widthBBox && p.y >= pBBox.y && p.y < pBBox.y + heightBBox;
}

#endif
