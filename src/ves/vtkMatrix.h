///*=========================================================================
//
// Program:   Visualization Toolkit
// Module:    vtkMatrix.h
//
// Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// All rights reserved.
// See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
//
// This software is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE.  See the above copyright notice for more information.
//
// =========================================================================*/
//
//// .NAME vtkVector - templated base type for storage of vectors.
////
//// .SECTION Description
//// This class is a templated data type for storing and manipulating fixed size
//// vectors, which can be used to represent two and three dimensional points. The
//// memory layout is a contiguous array of the specified type, such that a
//// float[2] can be cast to a vtkVector2f and manipulated. Also a float[6] could
//// be cast and used as a vtkVector2f[3].
//
//#ifndef __vtkVector_h
//#define __vtkVector_h
//
//#include <cmath>
//
//template<typename T, int Size>
//class vtkVector
//{
//public:
//	vtkVector()
//	{
//		for (int i = 0; i < Size; ++i)
//		{
//			Data[i] = 0;
//		}
//	}
//
//	vtkVector(const T* init)
//	{
//		for (int i = 0; i < Size; ++i)
//		{
//			Data[i] = init[i];
//		}
//	}
//
//	// Description:
//	// Get the size of the vtkVector.
//	int GetSize() const { return Size; }
//
//	// Description:
//	// Get a pointer to the underlying data of the vtkVector.
//	T* GetData() { return this->Data; }
//	const T* GetData() const { return this->Data; }
//
//	// Description:
//	// Get a reference to the underlying data element of the vtkVector. Can be
//	// used in much the same way as vector[i] is used.
//	T& operator[](int i) { return this->Data[i]; }
//	const T& operator[](int i) const { return this->Data[i]; }
//
//	// Description:
//	// Get the value of the vector at the index speciifed. Does bounds checking,
//	// used in much the same way as vector.at(i) is used.
//	T& operator()(int i) { return this->Data[i]; }
//	const T& operator()(int i) const { return this->Data[i]; }
//
//public:
//	// Description:
//	// The only thing stored in memory!
//	T Data[Size];
//};
//
//// .NAME vtkVector2 - templated base type for storage of 2D vectors.
////
//template<typename T>
//class vtkVector2 : public vtkVector<T, 2>
//{
//public:
//	vtkVector2(const T& x = 0.0, const T& y = 0.0)
//	{
//		this->Data[0] = x;
//		this->Data[1] = y;
//	}
//
//	vtkVector2(const T* init) : vtkVector<T, 2>(init)
//	{
//	}
//
//	// Description:
//	// Set the x and y components of the vector.
//	void Set(const T& x, const T& y)
//	{
//		this->Data[0] = x;
//		this->Data[1] = y;
//	}
//
//	// Description:
//	// Set the x component of the vector, i.e. element 0.
//	void SetX(const T& x) { this->Data[0] = x; }
//
//	// Description:
//	// Get the x component of the vector, i.e. element 0.
//	const T& GetX() const { return this->Data[0]; }
//	const T& X() const { return this->Data[0]; }
//
//	// Description:
//	// Set the y component of the vector, i.e. element 1.
//	void SetY(const T& y) { this->Data[1] = y; }
//
//	// Description:
//	// Get the y component of the vector, i.e. element 1.
//	const T& GetY() const { return this->Data[1]; }
//	const T& Y() const { return this->Data[1]; }
//};
//
//// .NAME vtkVector3 - templated base type for storage of 3D vectors.
////
//template<typename T>
//class vtkVector3 : public vtkVector<T, 3>
//{
//public:
//	vtkVector3(const T& x = 0.0, const T& y = 0.0, const T& z = 0.0)
//	{
//		this->Data[0] = x;
//		this->Data[1] = y;
//		this->Data[2] = z;
//	}
//
//	// Description:
//	// Set the x, y and z components of the vector.
//	void Set(const T& x, const T& y, const T& z)
//	{
//		this->Data[0] = x;
//		this->Data[1] = y;
//		this->Data[2] = z;
//	}
//
//	// Description:
//	// Set the x component of the vector, i.e. element 0.
//	void SetX(const T& x) { this->Data[0] = x; }
//
//	// Description:
//	// Get the x component of the vector, i.e. element 0.
//	const T& GetX() const { return this->Data[0]; }
//	const T& X() const { return this->Data[0]; }
//
//	// Description:
//	// Set the y component of the vector, i.e. element 1.
//	void SetY(const T& y) { this->Data[1] = y; }
//
//	// Description:
//	// Get the y component of the vector, i.e. element 1.
//	const T& GetY() const { return this->Data[1]; }
//	const T& Y() const { return this->Data[1]; }
//
//	// Description:
//	// Set the z component of the vector, i.e. element 2.
//	void SetZ(const T& z) { this->Data[2] = z; }
//
//	// Description:
//	// Get the z component of the vector, i.e. element 2.
//	const T& GetZ() const { return this->Data[2]; }
//	const T& Z() const { return this->Data[2]; }
//
//};
//
//// .NAME vtkVector3 - templated base type for storage of 3D vectors.
////
//template<typename T>
//class vtkVector4 : public vtkVector<T, 4>
//{
//public:
//  vtkVector4(const T& x = 0.0, const T& y = 0.0, const T& z = 0.0, const T& w = 0.0)
//  {
//    this->Data[0] = x;
//    this->Data[1] = y;
//    this->Data[2] = z;
//    this->Data[3] = w;
//  }
//
//	// Description:
//	// Set the x, y and z components of the vector.
//	void Set(const T& x, const T& y, const T& z, const T& w)
//	{
//		this->Data[0] = x;
//		this->Data[1] = y;
//		this->Data[2] = z;
//        this->Data[3] = w;
//	}
//
//	// Description:
//	// Set the x component of the vector, i.e. element 0.
//	void SetX(const T& x) { this->Data[0] = x; }
//
//	// Description:
//	// Get the x component of the vector, i.e. element 0.
//	const T& GetX() const { return this->Data[0]; }
//	const T& X() const { return this->Data[0]; }
//
//	// Description:
//	// Set the y component of the vector, i.e. element 1.
//	void SetY(const T& y) { this->Data[1] = y; }
//
//	// Description:
//	// Get the y component of the vector, i.e. element 1.
//	const T& GetY() const { return this->Data[1]; }
//	const T& Y() const { return this->Data[1]; }
//
//	// Description:
//	// Set the z component of the vector, i.e. element 2.
//	void SetZ(const T& z) { this->Data[2] = z; }
//
//	// Description:
//	// Get the z component of the vector, i.e. element 2.
//	const T& GetZ() const { return this->Data[2]; }
//	const T& Z() const { return this->Data[2]; }
//
//	// Description:
//	// Set the w component of the vector, i.e. element 3.
//	void SetW(const T& w) { this->Data[3] = w; }
//
//	// Description:
//	// Get the w component of the vector, i.e. element 3.
//	const T& GetW() const { return this->Data[3]; }
//	const T& W() const { return this->Data[3]; }
//
//  // vtkVector4 operator+ (const vtkVector4 lhs, const vtkVector4 rhs){
//  //   vtkVector4 result;
//  //   result.SetX(lsh.X()+rhs.X());
//  //   result.SetY(lsh.X()+rhs.X());
//  //   result.SetZ(lsh.Z()+rhs.Z());
//  //   result.SetW(lsh.X()+rhs.W());
//  //   return result;
//  // }
//};
//
//// .NAME vtkRect - templated base type for storage of 2D rectangles.
////
//template<typename T>
//class vtkRect : public vtkVector<T, 4>
//{
//public:
//	vtkRect(const T& x = 0.0, const T& y = 0.0, const T width = 0.0,
//			const T& height = 0.0 )
//	{
//		this->Data[0] = x;
//		this->Data[1] = y;
//		this->Data[2] = width;
//		this->Data[3] = height;
//	}
//
//	// Description:
//	// Set the x, y components of the rectangle, and the width/height.
//	void Set(const T& x, const T& y, const T& width, const T& height)
//	{
//		this->Data[0] = x;
//		this->Data[1] = y;
//		this->Data[2] = width;
//		this->Data[3] = height;
//	}
//
//	// Description:
//	// Set the x component of the rectangle bottom corner, i.e. element 0.
//	void SetX(const T& x) { this->Data[0] = x; }
//
//	// Description:
//	// Get the x component of the rectangle bottom corner, i.e. element 0.
//	const T& GetX() const { return this->Data[0]; }
//	const T& X() const { return this->Data[0]; }
//
//	// Description:
//	// Set the y component of the rectangle bottom corner, i.e. element 1.
//	void SetY(const T& y) { this->Data[1] = y; }
//
//	// Description:
//	// Get the y component of the rectangle bottom corner, i.e. element 1.
//	const T& GetY() const { return this->Data[1]; }
//	const T& Y() const { return this->Data[1]; }
//
//	// Description:
//	// Set the width of the rectanle, i.e. element 2.
//	void SetWidth(const T& width) { this->Data[2] = width; }
//
//	// Description:
//	// Get the width of the rectangle, i.e. element 2.
//	const T& GetWidth() const { return this->Data[2]; }
//	const T& Width() const { return this->Data[2]; }
//
//	// Description:
//	// Set the height of the rectangle, i.e. element 3.
//	void SetHeight(const T& height) { this->Data[3] = height; }
//
//	// Description:
//	// Get the height of the rectangle, i.e. element 3.
//	const T& GetHeight() const { return this->Data[3]; }
//	const T& Height() const { return this->Data[3]; }
//
//};
//
//// Description:
//// Some derived classes for the different vectors commonly used.
//class vtkVector2i : public vtkVector2<int>
//{
//public:
//	vtkVector2i(int x = 0, int y = 0) : vtkVector2<int>(x, y) {}
//};
//
//class vtkVector2f : public vtkVector2<float>
//{
//public:
//	vtkVector2f(float x = 0.0, float y = 0.0) : vtkVector2<float>(x, y) {}
//	vtkVector2f(const float* i) : vtkVector2<float>(i) {}
//};
//
//class vtkVector2d : public vtkVector2<double>
//{
//public:
//	vtkVector2d(double x = 0.0, double y = 0.0) : vtkVector2<double>(x, y) {}
//};
//
//class vtkVector3us : public vtkVector3<unsigned short>
//{
//public:
//  vtkVector3us(unsigned short x = 0, unsigned short y = 0, unsigned short z = 0)
//    : vtkVector3<unsigned short>(x, y, z) {}
//};
//
//class vtkVector3i : public vtkVector3<int>
//{
//public:
//	vtkVector3i(int x = 0, int y = 0, int z = 0) : vtkVector3<int>(x, y, z) {}
//};
//
////class vtkVector3f : public vtkVector3<float>
////{
////public:
////	vtkVector3f(float x = 0.0, float y = 0.0, float z = 0.0)
////    : vtkVector3<float>(x, y, z) {}
////};
//
//class vtkVector3d : public vtkVector3<double>
//{
//public:
//	vtkVector3d(double x = 0.0, double y = 0.0, double z = 0.0)
//    : vtkVector3<double>(x, y, z) {}
//};
//
////class vtkVector4f : public vtkVector4<float>
////{
////public:
////	vtkVector4f(float x = 0.0, float y = 0.0, float z = 0.0, float w = 0.0)
////    : vtkVector4<float>(x, y, z, w) {}
////};
//
//class vtkRecti : public vtkRect<int>
//{
//public:
//	vtkRecti(int x = 0, int y = 0, int width = 0, int height = 0)
//    : vtkRect<int>(x, y, width, height) {}
//};
//
//class vtkRectf : public vtkRect<float>
//{
//public:
//	vtkRectf(float x = 0.0, float y = 0.0, float width = 0.0, float height = 0.0)
//    : vtkRect<float>(x, y, width, height) {}
//};
//
//class vtkRectd : public vtkRect<double>
//{
//public:
//	vtkRectd(double x = 0.0, double y = 0.0, double width = 0.0,
//			 double height = 0.0)
//    : vtkRect<double>(x, y, width, height) {}
//};
//
//class vtkMatrix4f;
//
//class vtkMatrix3f
//{
//public:
//	vtkMatrix3f()
//	{
//		for (int i = 0; i < 3; ++i) {
//			for (int j = 0; j < 3; ++j) {
//				this->Data[i * 3 + j] = i == j ? 1.0 : 0.0;
//			}
//		}
//	}
//
//	vtkMatrix3f(const vtkMatrix4f& m);
//    // Create a 3x3 matrix, removing the specified row and column.
//    vtkMatrix3f(const vtkMatrix4f& m, int removeRow, int removeCol);
//	float& operator() (int row, int col)
//	{
//		return Data[col * 3 + row];
//	}
//
//	const float& operator() (int row, int col) const
//	{
//		return Data[col * 3 + row];
//	}
//
//  float determinant();
//
//  float Data[9];
//};
//
//class vtkMatrix4f
//{
//public:
//	void init(float value[16])
//	{
//		for(int i =0; i< 16; ++i){
//			this->Data[i]=value[i];
//		}
//	}
//
//	vtkMatrix4f()
//	{
//		for (int i = 0; i < 4; ++i) {
//			for (int j = 0; j < 4; ++j) {
//				this->Data[i * 4 + j] = i == j ? 1.0 : 0.0;
//			}
//		}
//	}
//
//	float& operator() (int row, int col)
//	{
//		return Data[col * 4 + row];
//	}
//
//	float operator() (int row, int col) const
//	{
//		return Data[col * 4 + row];
//	}
//
//	vtkMatrix4f operator * (const vtkMatrix4f& b) const
//    {
//        vtkMatrix4f m;
//		m(0, 0) = (*this)(0, 0) * b(0, 0) + (*this)(0, 1) * b(1, 0) + (*this)(0, 2) * b(2, 0) + (*this)(0, 3) * b(3, 0);
//		m(0, 1) = (*this)(0, 0) * b(0, 1) + (*this)(0, 1) * b(1, 1) + (*this)(0, 2) * b(2, 1) + (*this)(0, 3) * b(3, 1);
//		m(0, 2) = (*this)(0, 0) * b(0, 2) + (*this)(0, 1) * b(1, 2) + (*this)(0, 2) * b(2, 2) + (*this)(0, 3) * b(3, 2);
//		m(0, 3) = (*this)(0, 0) * b(0, 3) + (*this)(0, 1) * b(1, 3) + (*this)(0, 2) * b(2, 3) + (*this)(0, 3) * b(3, 3);
//		m(1, 0) = (*this)(1, 0) * b(0, 0) + (*this)(1, 1) * b(1, 0) + (*this)(1, 2) * b(2, 0) + (*this)(1, 3) * b(3, 0);
//		m(1, 1) = (*this)(1, 0) * b(0, 1) + (*this)(1, 1) * b(1, 1) + (*this)(1, 2) * b(2, 1) + (*this)(1, 3) * b(3, 1);
//		m(1, 2) = (*this)(1, 0) * b(0, 2) + (*this)(1, 1) * b(1, 2) + (*this)(1, 2) * b(2, 2) + (*this)(1, 3) * b(3, 2);
//		m(1, 3) = (*this)(1, 0) * b(0, 3) + (*this)(1, 1) * b(1, 3) + (*this)(1, 2) * b(2, 3) + (*this)(1, 3) * b(3, 3);
//		m(2, 0) = (*this)(2, 0) * b(0, 0) + (*this)(2, 1) * b(1, 0) + (*this)(2, 2) * b(2, 0) + (*this)(2, 3) * b(3, 0);
//		m(2, 1) = (*this)(2, 0) * b(0, 1) + (*this)(2, 1) * b(1, 1) + (*this)(2, 2) * b(2, 1) + (*this)(2, 3) * b(3, 1);
//		m(2, 2) = (*this)(2, 0) * b(0, 2) + (*this)(2, 2) * b(1, 2) + (*this)(2, 2) * b(2, 2) + (*this)(2, 3) * b(3, 2);
//		m(2, 3) = (*this)(2, 0) * b(0, 3) + (*this)(2, 1) * b(1, 3) + (*this)(2, 2) * b(2, 3) + (*this)(2, 3) * b(3, 3);
//		m(3, 0) = (*this)(3, 0) * b(0, 0) + (*this)(3, 1) * b(1, 0) + (*this)(3, 2) * b(2, 0) + (*this)(3, 3) * b(3, 0);
//		m(3, 1) = (*this)(3, 0) * b(0, 1) + (*this)(3, 1) * b(1, 1) + (*this)(3, 2) * b(2, 1) + (*this)(3, 3) * b(3, 1);
//		m(3, 2) = (*this)(3, 0) * b(0, 2) + (*this)(3, 1) * b(1, 2) + (*this)(3, 2) * b(2, 2) + (*this)(3, 3) * b(3, 2);
//		m(3, 3) = (*this)(3, 0) * b(0, 3) + (*this)(3, 1) * b(1, 3) + (*this)(3, 3) * b(2, 3) + (*this)(3, 3) * b(3, 3);
//        return m;
//    }
//
////    vtkVector4f operator * (const vtkVector4f& u) const
////    {
////        vtkVector4f v;
////        v(0) = (*this)(0, 0) * u(0) + (*this)(0, 1) * u(1) + (*this)(0, 2) * u(2) + (*this)(0, 3) * u(3);
////        v(1) = (*this)(1, 0) * u(0) + (*this)(1, 1) * u(1) + (*this)(1, 2) * u(2) + (*this)(1, 3) * u(3);
////        v(2) = (*this)(2, 0) * u(0) + (*this)(2, 1) * u(1) + (*this)(2, 2) * u(2) + (*this)(2, 3) * u(3);
////        v(3) = (*this)(3, 0) * u(0) + (*this)(3, 1) * u(1) + (*this)(3, 2) * u(2) + (*this)(3, 3) * u(3);
////        return v;
////    }
//
//	void frustum(float left, float right, float bottom, float top, float near, float far)
//	{
//		float a = 2 * near / (right - left);
//		float b = 2 * near / (top - bottom);
//		float c = (right + left) / (right - left);
//		float d = (top + bottom) / (top - bottom);
//		float e = - (far + near) / (far - near);
//		float f = -2 * far * near / (far - near);
//		(*this)(0, 0) = a; (*this)(0, 1) = 0; (*this)(0, 2) = 0; (*this)(0, 3) = 0;
//		(*this)(1, 0) = 0; (*this)(1, 1) = b; (*this)(1, 2) = 0; (*this)(1, 3) = 0;
//		(*this)(2, 0) = c; (*this)(2, 1) = d; (*this)(2, 2) = e; (*this)(2, 3) = -1;
//		(*this)(3, 0) = 0; (*this)(3, 1) = 0; (*this)(3, 2) = f; (*this)(3, 3) = 1;
//	}
//
//	void ortho(float left, float right, float bottom, float top, float near, float far)
//    {
//        float a = 2.0f / (right - left);
//        float b = 2.0f / (top - bottom);
//        float c = -2.0f / (far - near);
//        float tx = (right + left) / (right - left);
//        float ty = (top + bottom) / (top - bottom);
//        float tz = (far + near) / (far - near);
//        (*this)(0,0) = a; (*this)(0,1) = 0; (*this)(0,2) = 0; (*this)(0,3) = tx;
//        (*this)(1,0) = 0; (*this)(1,1) = b; (*this)(1,2) = 0; (*this)(1,3) = ty;
//        (*this)(2,0) = 0; (*this)(2,1) = 0; (*this)(2,2) = c; (*this)(2,3) = tz;
//        (*this)(3,0) = 0; (*this)(3,1) = 0; (*this)(3,2) = 0; (*this)(3,3) = 1;
//    }
//
//	vtkMatrix4f scale(float s)
//	{
//		vtkMatrix4f m;
//		m(0, 0) = s;
//		m(1, 1) = s;
//		m(2, 2) = s;
//		return m;
//	}
//
//	vtkMatrix4f translate(float x, float y = 0.0, float z = 0.0)
//	{
//		vtkMatrix4f m;
//		m(0, 3) = x;
//		m(1, 3) = y;
//		m(2, 3) = z;
//		return m;
//	}
//
//    void normalized()
//    {
//      float length = sqrt((*this)(0,0)*(*this)(0,0) + (*this)(0,1)*(*this)(0,1)
//                          + (*this)(0,2)*(*this)(0,2));
//      (*this)(0,0) = (*this)(0,0) / length;
//      (*this)(0,1) = (*this)(0,1) / length;
//      (*this)(0,2) = (*this)(0,2) / length;
//
//      length = sqrt((*this)(1,0)*(*this)(1,0) + (*this)(1,1)*(*this)(1,1)
//                    + (*this)(1,2)*(*this)(1,2));
//      (*this)(1,0) = (*this)(1,0) / length;
//      (*this)(1,1) = (*this)(1,1) / length;
//      (*this)(1,2) = (*this)(1,2) / length;
//
//      length = sqrt((*this)(2,0)*(*this)(2,0) + (*this)(2,1)*(*this)(2,1)
//                    + (*this)(2,2)*(*this)(2,2));
//      (*this)(2,0) = (*this)(2,0) / length;
//      (*this)(2,1) = (*this)(2,1) / length;
//      (*this)(2,2) = (*this)(2,2) / length;
//    }
//
//
////	vtkMatrix4f rotate(float degrees, const vtkVector3f &axis)
////	{
////		vtkMatrix4f m;
////		float radians = degrees * 3.14159f / 180.0f;
////		float s = std::sin(radians);
////		float c = std::cos(radians);
////
////		m(0, 0) = c + (1 - c) * axis.X() * axis.X();
////		m(0, 1) = (1 - c) * axis.X() * axis.Y() - axis.Z() * s;
////		m(0, 2) = (1 - c) * axis.X() * axis.Z() + axis.Y() * s;
////		m(1, 0) = (1 - c) * axis.X() * axis.Y() + axis.Z() * s;
////		m(1, 1) = c + (1 - c) * axis.Y() * axis.Y();
////		m(1, 2) = (1 - c) * axis.Y() * axis.Z() - axis.X() * s;
////		m(2, 0) = (1 - c) * axis.X() * axis.Z() - axis.Y() * s;
////		m(2, 1) = (1 - c) * axis.Y() * axis.Z() + axis.X() * s;
////		m(2, 2) = c + (1 - c) * axis.Z() * axis.Z();
////		m.normalized();
////        return m;
////	}
//
//	vtkMatrix4f transpose()
//	{
//		vtkMatrix4f m;
//		m(0, 0) = (*this)(0, 0); m(0, 1) = (*this)(1, 0); m(0, 2) = (*this)(2, 0); m(0, 3) = (*this)(3, 0);
//		m(1, 0) = (*this)(0, 1); m(1, 1) = (*this)(1, 1); m(1, 2) = (*this)(2, 1); m(1, 3) = (*this)(3, 1);
//		m(2, 0) = (*this)(0, 2); m(2, 1) = (*this)(1, 2); m(2, 2) = (*this)(2, 2); m(2, 3) = (*this)(3, 2);
//		m(3, 0) = (*this)(0, 3); m(3, 1) = (*this)(1, 3); m(3, 2) = (*this)(2, 3); m(3, 3) = (*this)(3, 3);
//		return m;
//	}
//
//  float determinant()
//  {
//    vtkMatrix3f m1(*this, 0, 0);
//    vtkMatrix3f m2(*this, 1, 0);
//    vtkMatrix3f m3(*this, 2, 0);
//    vtkMatrix3f m4(*this, 3, 0);
//    return (*this)(0, 0) * m1.determinant() - (*this)(0, 1) * m2.determinant() +
//      (*this)(0, 2) * m3.determinant() - (*this)(0, 3) * m4.determinant();
//  }
//
//  vtkMatrix4f adjoint()
//  {
//    vtkMatrix4f m;
//    for (int i = 0; i < 4; ++i) {
//      for (int j = 0; j < 4; ++j) {
//        m(i, j) = vtkMatrix3f(*this, j, i).determinant();
//      }
//    }
//    return m;
//  }
//
//  vtkMatrix4f inverse()
//  {
//    vtkMatrix4f m;
//    float det = determinant();
//    if (det == 0.0)
//      return m;
//
//    m = adjoint();
//
//    for (int i = 0; i < 4; ++i) {
//      for (int j = 0; j < 4; ++j) {
//        m(i, j) = m(i, j) / det;
//      }
//    }
//    return m;
//  }
//
//  float Data[16];
//};
//
//inline vtkMatrix3f::vtkMatrix3f(const vtkMatrix4f& m)
//{
//	for (int i = 0; i < 3; ++i) {
//		for (int j = 0; j < 3; ++j) {
//			(*this)(i, j) = m(i, j);
//		}
//	}
//}
//
//inline vtkMatrix3f::vtkMatrix3f(const vtkMatrix4f& m, int removeRow, int removeCol)
//{
//  if (removeRow < 4 && removeRow >=0 && removeCol < 4 && removeCol >= 0) {
//    int effRow = 0;
//    int effCol = 0;
//    for (int i = 0; i < 4; ++i) {
//      if (i != removeRow) {
//        effCol = 0;
//        for (int j = 0; j < 4; ++j) {
//          if (j != removeCol) {
//            (*this)(effRow, effCol++) = m(i, j);
//          }
//        }
//        ++effRow;
//      }
//    }
//  }
//}
//
//inline float vtkMatrix3f::determinant()
//{
//  return (*this)(0, 0) * (*this)(1, 1) * (*this)(2, 2) -
//    (*this)(0, 0) * (*this)(1, 2) * (*this)(2, 1) -
//    (*this)(0, 1) * (*this)(1, 0) * (*this)(2, 2) +
//    (*this)(0, 1) * (*this)(1, 2) * (*this)(2, 0) +
//    (*this)(0, 2) * (*this)(1, 0) * (*this)(2, 1) -
//    (*this)(0, 2) * (*this)(1, 1) * (*this)(2, 0);
//}
//
//#endif // __vtkVector_h
