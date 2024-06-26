#include "rect.h"
#include "../util/color.h"

Rect::Rect(Shader & shader, vec2 pos, vec2 size, struct color color)
        : Shape(shader, pos, size, color) {
    initVectors();
    initVAO();
    initVBO();
    initEBO();
}

Rect::Rect(Shader & shader, vec2 pos, vec2 size, struct color color, vec2 velocity)
        : Shape(shader, pos, size, color) {
    initVectors();
    initVAO();
    initVBO();
    initEBO();
}

Rect::Rect(Rect const& other) : Shape(other) {
    initVectors();
    initVAO();
    initVBO();
    initEBO();
}

Rect::~Rect() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Rect::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Rect::initVectors() {
    this->vertices.insert(vertices.end(), {
            // TO DO: Add other three corners here
            0.5f, -0.5f,  // x, y of bottom right corner
            -0.5f, -0.5f, // bottom left corner
            -0.5f, 0.5f, // top left corner
            0.5f, 0.5f // top right corner
    });

    this->indices.insert(indices.end(), {
            0, 1, 2, // First triangle
            2, 3, 0  // Second triangle
    });
}
// Overridden Getters from Shape
float Rect::getLeft() const        { return pos.x - (size.x / 2); }
float Rect::getRight() const       { return pos.x + (size.x / 2); }
float Rect::getTop() const         { return pos.y + (size.y / 2); }
float Rect::getBottom() const      { return pos.y - (size.y / 2); }

/*bool Rect::isOverlapping(const Rect &r1, const Rect &r2) {
    // TO DO: Implement this method
    // There are only two cases when rectangles are *not* overlapping:
    //    1. when one is to the left of the other
    if(r1.getRight() < r2.getLeft() || r1.getLeft() > r2.getRight() && r1.getBottom() < r2.getTop() || r1.getTop() < r2.getBottom()){
        //if(r1.getBottom() > r2.getTop() || r1.getTop() < r2.getBottom()) {
        return false;
    }
    else{
        return true;
    }
}*/
bool Rect::isOverlapping(const Rect &r1, const Rect &r2) {
    // TO DO: Implement this method
    // There are only two cases when rectangles are *not* overlapping:
    //    1. when one is to the left of the other
    if(r1.getRight() < r2.getLeft() || r1.getLeft() > r2.getRight() && r1.getBottom() < r2.getTop() || r1.getTop() < r2.getBottom()){
        //if(r1.getBottom() > r2.getTop() || r1.getTop() < r2.getBottom()) {
        return false;
        //  }
    }
    else{
        return true;
    }
}

bool Rect::isOverlapping(const Rect &other) const {
    return isOverlapping(*this, other);
}

bool Rect::isOverlapping(const Shape &other) const {
    // Dynamic cast to check if the other shape is a Rect
    const Rect* otherRect = dynamic_cast<const Rect*>(&other);
    if (otherRect) {
        return isOverlapping(*this, *otherRect);
    }
    return false;
}