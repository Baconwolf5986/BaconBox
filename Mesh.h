#pragma once
#include "stdafx.h"
#include "D3DUtil.h"

// Mesh struct representing collections of vertex and index data
// =================================================================
struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<UINT16> indices;
};

// Class used to generate geometry
// =================================================================
class GeometryGenerator
{
public:
	GeometryGenerator();

	Mesh GenerateGrid(float width, float depth, UINT m, UINT n);
	Mesh GenerateBox(float width, float height, float depth);
	Mesh GenerateGeosphere(float radius, UINT numSubdivisions);

private:
	void Subdivide(Mesh& mesh);
	Vertex MidPoint(const Vertex& v0, const Vertex& v1);
};

// Class used to generate models with given vertex and index data from file
// =================================================================
class ModelGenerator
{
public:
	ModelGenerator();
	~ModelGenerator();

	// Load a model from a .txt file with normal and position data
	Mesh LoadModelPosNorm(const std::string& filename);
};