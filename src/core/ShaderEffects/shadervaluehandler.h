// enve - 2D animations software
// Copyright (C) 2016-2019 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SHADERVALUEHANDLER_H
#define SHADERVALUEHANDLER_H
#include <QJSEngine>

#include "glhelpers.h"
#include "smartPointers/ememory.h"

typedef std::function<void(QGL33 * const, QJSEngine&)> UniformSpecifier;

enum class GLValueType {
    Float, Vec2, Vec3, Vec4,
    Int, iVec2, iVec3, iVec4
};

class ShaderValueHandler : public StdSelfRef {
public:
    ShaderValueHandler(const QString& name,
                       const bool glValue,
                       const GLValueType type,
                       const QString& script);

    UniformSpecifier create(const GLint loc) const;
    void evaluate(QJSEngine& engine) const;

    const QString fName;
    const bool fGLValue;
private:
    const GLValueType mType;
    const QString mScript;
};

#endif // SHADERVALUEHANDLER_H
