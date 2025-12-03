/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "Vec2.hpp"

#include <cmath>
#include <initializer_list>

namespace Math {
    class TransformationMatrix {
    public:
        double At(int r, int c) const { return matrix[r][c]; }
        TransformationMatrix();

        const double* operator[](int index) const { return matrix[index]; }
        TransformationMatrix operator * (TransformationMatrix m) const;
        TransformationMatrix& operator *= (TransformationMatrix m);
        vec2 operator* (vec2 v) const;
        void Reset();
    protected:
        double matrix[3][3];
    };

    class TranslationMatrix : public TransformationMatrix {
    public:
        TranslationMatrix(ivec2 translate);
        TranslationMatrix(vec2 translate);
    };

    class ScaleMatrix : public TransformationMatrix {
    public:
        ScaleMatrix(std::initializer_list<double> il)
        : ScaleMatrix(Math::vec2{(il.size() > 0 ? *(il.begin()) : 1.0), (il.size()) > 1 ? *(il.begin() + 1) : (il.size() > 0 ? *(il.begin()) : 1.0)}) {}
        ScaleMatrix(double scale);
        ScaleMatrix(vec2 scale);

        ScaleMatrix(double sx, double sy);
    };

    class RotationMatrix : public TransformationMatrix {
    public:
        RotationMatrix(double theta);
    };
}
