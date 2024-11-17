#pragma once
class Quaternion {
public:
    float real_part;
    glm::vec3 vector_part = glm::vec3(0);
    Quaternion();
    explicit Quaternion(glm::vec3 vector);
    explicit Quaternion(glm::vec4 vector);
    Quaternion(float real_part, glm::vec3 vector);
    Quaternion operator *(Quaternion rhs) const;

};
namespace Rotations {
    //Rotates a vector around given axis axis.w degrees (4th component of the axis vector)
    glm::vec3 Rotate(glm::vec4 axis, glm::vec3 vector);
}

Quaternion::Quaternion(glm::vec3 vector) {
    real_part = 0;
    vector_part = vector;
}
Quaternion::Quaternion(float angle, glm::vec3 vector) {
    real_part = cos(angle);
    vector_part = sin(angle) * vector;
}
Quaternion::Quaternion(glm::vec4 vector) {
    real_part = cos(vector.w);
    vector_part = sin(vector.w) * glm::vec3(vector.x, vector.y, vector.z);
}
Quaternion::Quaternion() {
    real_part = 0;
    vector_part = glm::vec3(0, 0, 0);
}

Quaternion Quaternion::operator*(Quaternion rhs) const {
    Quaternion output;
    output.real_part = real_part * rhs.real_part - dot(vector_part, rhs.vector_part);
    output.vector_part = real_part * rhs.vector_part + rhs.real_part * vector_part + cross(vector_part, rhs.vector_part);
    return output;
}

glm::vec3 Rotations::Rotate(glm::vec4 axis, glm::vec3 vector) {
    return ((Quaternion(axis) * Quaternion(vector)) * Quaternion(axis.w, glm::vec3(-axis.x, -axis.y, -axis.z))).vector_part;
}
class Basis {
public:
    glm::mat4 basis = glm::mat4(1);
    explicit Basis();
    //Basis(mat4 custom_basis);
    explicit Basis(glm::vec3 x, glm::vec3 y, glm::vec3 z);
};
namespace Transform {
    void Rotate(Basis& origin, glm::vec4& axis);
};

Basis::Basis() = default;
Basis::Basis(glm::vec3 x, glm::vec3 y, glm::vec3 z) {
    basis[0] = glm::vec4(x, 0);
    basis[1] = glm::vec4(y, 0);
    basis[2] = glm::vec4(z, 0);
}

void Transform::Rotate(Basis& origin, glm::vec4& axis) {
    origin.basis[0] = glm::vec4(Rotations::Rotate(axis, origin.basis[0]), 0);
    origin.basis[1] = glm::vec4(Rotations::Rotate(axis, origin.basis[1]), 0);
    origin.basis[2] = glm::vec4(Rotations::Rotate(axis, origin.basis[2]), 0);
}