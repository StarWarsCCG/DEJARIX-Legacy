#ifndef TEXTUREMANAGER_HPP
#define TEXTUREMANAGER_HPP

#include <QGLWidget>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class TextureManager
{
public:
    TextureManager(QGLWidget& widget);
    ~TextureManager();

private:
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    QGLWidget* _widget;
    std::unordered_set<GLuint> _textures;
};

#endif
