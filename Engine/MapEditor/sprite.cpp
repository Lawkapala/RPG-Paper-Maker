/*
    RPG Paper Maker Copyright (C) 2017 Marie Laporte

    This file is part of RPG Paper Maker.

    RPG Paper Maker is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RPG Paper Maker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sprite.h"
#include "map.h"

// -------------------------------------------------------
//
//
//  ---------- SPRITE
//
//
// -------------------------------------------------------

QVector3D Sprite::verticesQuad[4]{
                                  QVector3D(0.0f, 1.0f, 0.0f),
                                  QVector3D(1.0f, 1.0f, 0.0f),
                                  QVector3D(1.0f, 0.0f, 0.0f),
                                  QVector3D(0.0f, 0.0f, 0.0f)
                                };

QVector2D Sprite::modelQuad[4]{
    QVector2D(-0.5f, 0.5f),
    QVector2D(0.5f, 0.5f),
    QVector2D(0.5f, -0.5f),
    QVector2D(-0.5f, -0.5f)
};

GLuint Sprite::indexesQuad[6]{0, 1, 2, 0, 2, 3};

int Sprite::nbVerticesQuad(4);

int Sprite::nbIndexesQuad(6);

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

Sprite::Sprite()
{

}

Sprite::~Sprite()
{

}

// -------------------------------------------------------
//
//
//  ---------- SPRITEDATAS
//
//
// -------------------------------------------------------

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

SpriteDatas::SpriteDatas() :
    SpriteDatas(MapEditorSubSelectionKind::SpritesFace, 50, 0,
                new QRect(0, 0, 2, 2))
{

}

SpriteDatas::SpriteDatas(MapEditorSubSelectionKind kind,
                         int widthPosition, int angle, QRect *textureRect) :
    m_kind(kind),
    m_widthPosition(widthPosition),
    m_angle(angle),
    m_textureRect(textureRect)
{

}

SpriteDatas::~SpriteDatas()
{
    delete m_textureRect;
}

MapEditorSelectionKind SpriteDatas::getKind() const {
    return MapEditorSelectionKind::Sprites;
}

MapEditorSubSelectionKind SpriteDatas::getSubKind() const { return m_kind; }

int SpriteDatas::widthPosition() const { return m_widthPosition; }

int SpriteDatas::angle() const { return m_angle; }

QRect* SpriteDatas::textureRect() const { return m_textureRect; }

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

void SpriteDatas::initializeVertices(int squareSize,
                                     int width, int height,
                                     QVector<Vertex>& verticesStatic,
                                     QVector<GLuint>& indexesStatic,
                                     QVector<VertexBillboard>& verticesFace,
                                     QVector<GLuint>& indexesFace,
                                     Position3D& position, int& countStatic,
                                     int& countFace, int &spritesOffset)
{
    float x, y, w, h;
    int offset;
    x = (float)(m_textureRect->x() * squareSize) / width;
    y = (float)(m_textureRect->y() * squareSize) / height;
    w = (float)(m_textureRect->width() * squareSize) / width;
    h = (float)(m_textureRect->height() * squareSize) / height;
    float coefX = 0.1 / width;
    float coefY = 0.1 / height;
    x += coefX;
    y += coefY;
    w -= (coefX * 2);
    h -= (coefY * 2);

    QVector3D pos((float) position.x() * squareSize -
                  ((textureRect()->width() - 1) * squareSize / 2) +
                  spritesOffset,
                  (float) position.getY(squareSize),
                  (float) position.z() * squareSize +
                  (widthPosition() * squareSize / 100) + spritesOffset);
    spritesOffset += Sprite::SPRITES_OFFSET_COEF;
    QVector3D size((float) textureRect()->width() * squareSize,
                   (float) textureRect()->height() * squareSize,
                   0.0f);
    QVector3D center = Sprite::verticesQuad[0] * size + pos +
            QVector3D(size.x() / 2, - size.y() / 2, 0);
    QVector2D texA(x, y), texB(x + w, y), texC(x + w, y + h), texD(x, y + h);

    // Adding to buffers according to the kind of sprite
    switch (m_kind) {
    case MapEditorSubSelectionKind::SpritesFix:
    case MapEditorSubSelectionKind::SpritesDouble:
    case MapEditorSubSelectionKind::SpritesQuadra:
    {
        QVector3D vecA = Sprite::verticesQuad[0] * size + pos,
                  vecB = Sprite::verticesQuad[1] * size + pos,
                  vecC = Sprite::verticesQuad[2] * size + pos,
                  vecD = Sprite::verticesQuad[3] * size + pos;

        addStaticSpriteToBuffer(verticesStatic, indexesStatic, countStatic,
                                vecA, vecB, vecC, vecD, texA, texB, texC, texD);

        // If double sprite, add one sprite more
        if (m_kind == MapEditorSubSelectionKind::SpritesDouble ||
            m_kind == MapEditorSubSelectionKind::SpritesQuadra) {
            QVector3D vecDoubleA(vecA), vecDoubleB(vecB),
                      vecDoubleC(vecC), vecDoubleD(vecD);

            rotateSprite(vecDoubleA, vecDoubleB, vecDoubleC, vecDoubleD, center,
                         90);
            addStaticSpriteToBuffer(verticesStatic, indexesStatic, countStatic,
                                    vecDoubleA, vecDoubleB, vecDoubleC,
                                    vecDoubleD, texA, texB, texC, texD);

            if (m_kind == MapEditorSubSelectionKind::SpritesQuadra) {
                QVector3D vecQuadra1A(vecA), vecQuadra1B(vecB),
                          vecQuadra1C(vecC), vecQuadra1D(vecD),
                          vecQuadra2A(vecA), vecQuadra2B(vecB),
                          vecQuadra2C(vecC), vecQuadra2D(vecD);

                rotateSprite(vecQuadra1A, vecQuadra1B, vecQuadra1C, vecQuadra1D,
                             center, 45);
                rotateSprite(vecQuadra2A, vecQuadra2B, vecQuadra2C, vecQuadra2D,
                             center, -45);
                addStaticSpriteToBuffer(verticesStatic, indexesStatic,
                                        countStatic, vecQuadra1A, vecQuadra1B,
                                        vecQuadra1C, vecQuadra1D, texA, texB,
                                        texC, texD);
                addStaticSpriteToBuffer(verticesStatic, indexesStatic,
                                        countStatic, vecQuadra2A, vecQuadra2B,
                                        vecQuadra2C, vecQuadra2D, texA, texB,
                                        texC, texD);
            }
        }

        break;
    }
    case MapEditorSubSelectionKind::SpritesFace:
    {
        QVector2D s(size.x(), size.y());

        // Vertices
        verticesFace.append(
              VertexBillboard(center, texA, s, Sprite::modelQuad[0]));
        verticesFace.append(
              VertexBillboard(center, texB, s, Sprite::modelQuad[1]));
        verticesFace.append(
              VertexBillboard(center, texC, s, Sprite::modelQuad[2]));
        verticesFace.append(
              VertexBillboard(center, texD, s, Sprite::modelQuad[3]));

        // indexes
        offset = countFace * Sprite::nbVerticesQuad;
        for (int i = 0; i < Sprite::nbIndexesQuad; i++)
            indexesFace.append(Sprite::indexesQuad[i] + offset);
        countFace++;
        break;
    }
        /*
    case MapEditorSubSelectionKind::SpritesWall:
    {
        QVector3D vecA = Sprite::verticesQuad[0] * size + pos,
                  vecB = Sprite::verticesQuad[1] * size + pos,
                  vecC = Sprite::verticesQuad[2] * size + pos,
                  vecD = Sprite::verticesQuad[3] * size + pos;

        addStaticSpriteToBuffer(verticesStatic, indexesStatic, countStatic,
                                vecA, vecB, vecC, vecD, texA, texB, texC, texD);
    }*/
    default:
        break;
    }
}

// -------------------------------------------------------

void SpriteDatas::rotateVertex(QVector3D& vec, QVector3D& center, int angle) {
    QMatrix4x4 m;
    QVector3D v(vec);

    v -= center;
    m.rotate(angle, 0.0, 1.0, 0.0);
    v = v * m + center;

    vec.setX(v.x());
    vec.setY(v.y());
    vec.setZ(v.z());
}

// -------------------------------------------------------

void SpriteDatas::rotateSprite(QVector3D& vecA, QVector3D& vecB,
                               QVector3D& vecC, QVector3D& vecD,
                               QVector3D& center, int angle)
{
    rotateVertex(vecA, center, angle);
    rotateVertex(vecB, center, angle);
    rotateVertex(vecC, center, angle);
    rotateVertex(vecD, center, angle);
}

// -------------------------------------------------------

void SpriteDatas::addStaticSpriteToBuffer(QVector<Vertex>& verticesStatic,
                                          QVector<GLuint>& indexesStatic,
                                          int& count,
                                          QVector3D& vecA, QVector3D& vecB,
                                          QVector3D& vecC, QVector3D& vecD,
                                          QVector2D& texA, QVector2D& texB,
                                          QVector2D& texC, QVector2D& texD)
{
    // Vertices
    verticesStatic.append(Vertex(vecA, texA));
    verticesStatic.append(Vertex(vecB, texB));
    verticesStatic.append(Vertex(vecC, texC));
    verticesStatic.append(Vertex(vecD, texD));

    // indexes
    int offset = count * Sprite::nbVerticesQuad;
    for (int i = 0; i < Sprite::nbIndexesQuad; i++)
        indexesStatic.append(Sprite::indexesQuad[i] + offset);
    count++;
}

// -------------------------------------------------------
//
//  READ / WRITE
//
// -------------------------------------------------------

void SpriteDatas::read(const QJsonObject & json){
    m_kind = static_cast<MapEditorSubSelectionKind>(json["k"].toInt());
    m_widthPosition = json["p"].toInt();
    m_angle = json["a"].toInt();

    QJsonArray tab = json["t"].toArray();
    m_textureRect->setLeft(tab[0].toInt());
    m_textureRect->setTop(tab[1].toInt());
    m_textureRect->setWidth(tab[2].toInt());
    m_textureRect->setHeight(tab[3].toInt());
}

// -------------------------------------------------------

void SpriteDatas::write(QJsonObject & json) const{
    QJsonArray tab;

    json["k"] = (int) m_kind;
    json["p"] = m_widthPosition;
    json["a"] = m_angle;

    // Texture
    tab.append(m_textureRect->left());
    tab.append(m_textureRect->top());
    tab.append(m_textureRect->width());
    tab.append(m_textureRect->height());
    json["t"] = tab;
}

// -------------------------------------------------------
//
//
//  ---------- SPRITEOBJECT
//
//
// -------------------------------------------------------

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

SpriteObject::SpriteObject(SpriteDatas &datas, QOpenGLTexture* texture) :
    m_datas(datas),
    m_texture(texture),
    m_vertexBuffer(QOpenGLBuffer::VertexBuffer),
    m_indexBuffer(QOpenGLBuffer::IndexBuffer),
    m_programStatic(nullptr),
    m_programFace(nullptr)
{

}

SpriteObject::~SpriteObject()
{

}

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

void SpriteObject::initializeVertices(int squareSize, Position3D& position,
                                      int& spritesOffset)
{
    m_verticesStatic.clear();
    m_verticesFace.clear();
    m_indexes.clear();
    int count = 0;
    m_datas.initializeVertices(squareSize,
                               m_texture->width(),
                               m_texture->height(),
                               m_verticesStatic, m_indexes, m_verticesFace,
                               m_indexes, position, count, count,
                               spritesOffset);
}

// -------------------------------------------------------
//
//  GL
//
// -------------------------------------------------------

void SpriteObject::initializeStaticGL(QOpenGLShaderProgram* programStatic){
    if (m_programStatic == nullptr){
        initializeOpenGLFunctions();
        m_programStatic = programStatic;
    }
}

// -------------------------------------------------------

void SpriteObject::initializeFaceGL(QOpenGLShaderProgram *programFace){
    if (m_programFace == nullptr){
        initializeOpenGLFunctions();
        m_programFace = programFace;
    }
}

// -------------------------------------------------------

void SpriteObject::updateStaticGL(){
    Map::updateGLStatic(m_vertexBuffer, m_indexBuffer, m_verticesStatic,
                        m_indexes, m_vao, m_programStatic);
}

// -------------------------------------------------------

void SpriteObject::updateFaceGL(){
    Map::updateGLFace(m_vertexBuffer, m_indexBuffer, m_verticesFace,
                      m_indexes, m_vao, m_programFace);
}

// -------------------------------------------------------

void SpriteObject::paintGL(){
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, m_indexes.size(), GL_UNSIGNED_INT, 0);
    m_vao.bind();
}

// -------------------------------------------------------
//
//
//  ---------- SPRITEWALLDATAS
//
//
// -------------------------------------------------------

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

SpriteWallDatas::SpriteWallDatas() :
    SpriteWallDatas(1)
{

}

SpriteWallDatas::SpriteWallDatas(int wallID) :
    m_wallID(wallID)
{

}

MapEditorSelectionKind SpriteWallDatas::getKind() const {
    return MapEditorSelectionKind::Sprites;
}

MapEditorSubSelectionKind SpriteWallDatas::getSubKind() const {
    return MapEditorSubSelectionKind::SpritesWall;
}

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

void SpriteWallDatas::initializeVertices(int squareSize, int width, int height,
                                    QVector<Vertex>& vertices,
                                    QVector<GLuint>& indexes,
                                    GridPosition& position, int& count)
{
    float x, y, w, h;
    x = (float)(0 * squareSize) / width;
    y = (float)(0 * squareSize) / height;
    w = (float)(1 * squareSize) / width;
    h = (float)(1 * squareSize) / height;
    float coefX = 0.1 / width;
    float coefY = 0.1 / height;
    x += coefX;
    y += coefY;
    w -= (coefX * 2);
    h -= (coefY * 2);

    QVector3D pos((float) position.x1() * squareSize,
                  (float) position.y() * squareSize,
                  (float) position.z1() * squareSize);
    QVector3D size(squareSize,
                   squareSize,
                   0.0f);
    QVector2D texA(x, y), texB(x + w, y), texC(x + w, y + h), texD(x, y + h);

    QVector3D vecA = Sprite::verticesQuad[0] * size + pos,
              vecB = Sprite::verticesQuad[1] * size + pos,
              vecC = Sprite::verticesQuad[2] * size + pos,
              vecD = Sprite::verticesQuad[3] * size + pos;

    if (!position.isHorizontal())
        SpriteDatas::rotateSprite(vecA, vecB, vecC, vecD, vecD, 90);
    SpriteDatas::addStaticSpriteToBuffer(vertices, indexes, count, vecA, vecB,
                                         vecC, vecD, texA, texB, texC, texD);
}