#include "Mesh.h"

// =================================================================
// Geometry Generator Class
// =================================================================
GeometryGenerator::GeometryGenerator()
{

}

Mesh GeometryGenerator::GenerateGrid(float width, float depth, UINT m, UINT n)
{
	Mesh meshData;

	uint32_t vertexCount = m * n;
	uint32_t faceCount = (m - 1) * (n - 1) * 2;

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

    // Create the vertices
	meshData.vertices.resize(vertexCount);
    for (UINT i = 0; i < m; ++i)
    {
        float z = halfDepth - i * dz;
        for (UINT j = 0; j < n; ++j)
        {
            float x = -halfWidth + j * dx;

            // Calculate position, normal, tangent, and texture coordinates
            XMFLOAT3 position = XMFLOAT3(x, 0.0f, z);
            XMFLOAT3 normal = XMFLOAT3(0.0f, 1.0f, 0.0f); // Upward normal
            XMFLOAT3 tangent = XMFLOAT3(1.0f, 0.0f, 0.0f); // Tangent along the X-axis
            XMFLOAT2 texCoord = XMFLOAT2(j * du, i * dv);

            // Assign the calculated values to the vertex
            meshData.vertices[i * n + j] = Vertex{ position, normal, tangent, texCoord };
        }
    }

    // Create the indices
    meshData.indices.resize(faceCount * 3); // 3 indices per face
    uint32_t k = 0;
    for (uint32_t i = 0; i < m - 1; ++i)
	{
		for (uint32_t j = 0; j < n - 1; ++j)
		{
			meshData.indices[k] = i * n + j;
			meshData.indices[k + 1] = i * n + j + 1;
			meshData.indices[k + 2] = (i + 1) * n + j;

			meshData.indices[k + 3] = (i + 1) * n + j;
			meshData.indices[k + 4] = i * n + j + 1;
			meshData.indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; // Next quad
		}
	}

	return meshData;
}

Mesh GeometryGenerator::GenerateBox(float width, float height, float depth)
{
	Mesh meshData;

	uint32_t faceCount = 6;

	// Points on edge of the box
	float halfW = 0.5f * width;
	float halfH = 0.5f * height;
	float halfD = 0.5f * depth;

	// Create the vertices
	meshData.vertices.resize(24);

	// Front Face Vertices
	meshData.vertices[0] = Vertex(-halfW, -halfH, -halfD, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	meshData.vertices[1] = Vertex(-halfW, +halfH, -halfD, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	meshData.vertices[2] = Vertex(+halfW, +halfH, -halfD, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	meshData.vertices[3] = Vertex(+halfW, -halfH, -halfD, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Back Face Vertices
	meshData.vertices[4] = Vertex(-halfW, -halfH, +halfD, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	meshData.vertices[5] = Vertex(+halfW, -halfH, +halfD, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	meshData.vertices[6] = Vertex(-halfW, +halfH, +halfD, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	meshData.vertices[7] = Vertex(+halfW, +halfH, +halfD, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);


	// Left Face Vertices
	meshData.vertices[8] = Vertex(-halfW, -halfH, +halfD, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	meshData.vertices[9] = Vertex(-halfW, +halfH, +halfD, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	meshData.vertices[10] = Vertex(-halfW, +halfH, -halfD, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	meshData.vertices[11] = Vertex(-halfW, -halfH, -halfD, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Right Face Vertices
	meshData.vertices[12] = Vertex(+halfW, -halfH, -halfD, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	meshData.vertices[13] = Vertex(+halfW, +halfH, -halfD, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	meshData.vertices[14] = Vertex(+halfW, +halfH, +halfD, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	meshData.vertices[15] = Vertex(+halfW, -halfH, +halfD, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Top Face Vertices
	meshData.vertices[16] = Vertex(-halfW, +halfH, -halfD, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	meshData.vertices[17] = Vertex(-halfW, +halfH, +halfD, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	meshData.vertices[18] = Vertex(+halfW, +halfH, +halfD, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	meshData.vertices[19] = Vertex(+halfW, +halfH, -halfD, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Bottom Face Vertices
	meshData.vertices[20] = Vertex(-halfW, -halfH, -halfD, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	meshData.vertices[21] = Vertex(+halfW, -halfH, -halfD, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	meshData.vertices[22] = Vertex(+halfW, -halfH, +halfD, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	meshData.vertices[23] = Vertex(-halfW, -halfH, +halfD, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Create the indices
	meshData.indices = {
		// Front face
		0, 1, 2,
		0, 2, 3,

		// Back face
		4, 6, 5,
		4, 7, 6,

		// Left face
		8, 9, 10,
		8, 10, 11,

		// Right face
		12, 14, 13,
		12, 15, 14,

		// Top face
		16, 17, 18,
		16, 18, 19,

		// Bottom face
		20, 22, 21,
		20, 23, 22
	};

	return meshData;
}

Mesh GeometryGenerator::GenerateGeosphere(float radius, UINT numSubdivisions)
{
	Mesh meshData;

	const float X = 0.525731f;
	const float Z = 0.850651f;
	const float N = 0.0f;

	// Create the initial icosahedron 
	XMFLOAT3 icosahedronVertices[12] = {
		{-X, N, Z}, {X, N, Z}, {-X, N, -Z}, {X, N, -Z},
		{N, Z, X}, {N, Z, -X}, {N, -Z, X}, {N, -Z, -X},
		{Z, X, N}, {-Z, X, N}, {Z, -X, N}, {-Z, -X, N}
	};

	UINT16 icosahedronIndices[60] = {
		0, 4, 1, 0, 9, 4, 9, 5, 4, 4, 5, 8, 4, 8, 1,
		8, 10, 1, 8, 3, 10, 5, 3, 8, 5, 2, 3, 2, 7, 3,
		7, 10, 3, 7, 6, 10, 7, 11, 6, 11, 0, 6, 0, 1, 6,
		6, 1, 10, 9, 0, 11, 9, 11, 2, 9, 2, 5, 7, 2, 11
	};

	// Create the initial icosahedron vertex list
		// Convert icosahedron vertices to the initial vertex list
	for (const auto& vertex : icosahedronVertices)
	{
		XMVECTOR pos = XMLoadFloat3(&vertex);
		pos = XMVector3Normalize(pos);  // Normalize the position for the initial vertices

		XMFLOAT3 normalizedPos;
		XMStoreFloat3(&normalizedPos, pos);

		XMFLOAT3 normal = normalizedPos; // Normal is the normalized position on the unit sphere

		// Derive texture coordinates from spherical coordinates
		float theta = atan2f(normalizedPos.z, normalizedPos.x);
		if (theta < 0.0f)
			theta += XM_2PI;

		float phi = acosf(normalizedPos.y);

		XMFLOAT2 texCoord;
		texCoord.x = theta / XM_2PI;
		texCoord.y = phi / XM_PI;

		// Tangent - this is an approximation assuming the sphere is mostly uniform.
		XMFLOAT3 tangent;
		tangent.x = -normal.z;
		tangent.y = 0.0f;
		tangent.z = normal.x;

		meshData.vertices.push_back(Vertex{ normalizedPos, normal, tangent, texCoord });
	}
	
	// Put the initial indices to the initial index list
	meshData.indices.assign(std::begin(icosahedronIndices), std::end(icosahedronIndices));

	// Subdivide the triangles of the icosahedron
		// Subdivide triangles
	for (UINT i = 0; i < numSubdivisions; ++i)
	{
		std::vector<UINT16> newIndices;

		for (size_t j = 0; j < meshData.indices.size(); j += 3)
		{
			UINT16 a = meshData.indices[j];
			UINT16 b = meshData.indices[j + 1];
			UINT16 c = meshData.indices[j + 2];

			XMFLOAT3 v1 = meshData.vertices[a].Position;
			XMFLOAT3 v2 = meshData.vertices[b].Position;
			XMFLOAT3 v3 = meshData.vertices[c].Position;

			// Calculate midpoints
			XMFLOAT3 m1 = {
				(v1.x + v2.x) / 2.0f,
				(v1.y + v2.y) / 2.0f,
				(v1.z + v2.z) / 2.0f
			};

			XMFLOAT3 m2 = {
				(v2.x + v3.x) / 2.0f,
				(v2.y + v3.y) / 2.0f,
				(v2.z + v3.z) / 2.0f
			};

			XMFLOAT3 m3 = {
				(v3.x + v1.x) / 2.0f,
				(v3.y + v1.y) / 2.0f,
				(v3.z + v1.z) / 2.0f
			};

			// Normalize midpoints to lie on the sphere surface
			auto normalize = [](XMFLOAT3& v) {
				XMVECTOR vec = XMLoadFloat3(&v);
				vec = XMVector3Normalize(vec);
				XMStoreFloat3(&v, vec);
				};

			normalize(m1);
			normalize(m2);
			normalize(m3);

			auto createVertex = [&](const XMFLOAT3& pos) -> Vertex {
				XMFLOAT3 normal = pos;

				float theta = atan2f(pos.z, pos.x);
				if (theta < 0.0f)
					theta += XM_2PI;

				float phi = acosf(pos.y);

				XMFLOAT2 texCoord;
				texCoord.x = theta / XM_2PI;
				texCoord.y = phi / XM_PI;

				XMFLOAT3 tangent;
				tangent.x = -normal.z;
				tangent.y = 0.0f;
				tangent.z = normal.x;

				return Vertex{ pos, normal, tangent, texCoord };
				};

			UINT16 i1 = static_cast<UINT16>(meshData.vertices.size());
			meshData.vertices.push_back(createVertex(m1));

			UINT16 i2 = static_cast<UINT16>(meshData.vertices.size());
			meshData.vertices.push_back(createVertex(m2));

			UINT16 i3 = static_cast<UINT16>(meshData.vertices.size());
			meshData.vertices.push_back(createVertex(m3));

			// Add new triangles
			newIndices.push_back(a); newIndices.push_back(i1); newIndices.push_back(i3);
			newIndices.push_back(b); newIndices.push_back(i2); newIndices.push_back(i1);
			newIndices.push_back(c); newIndices.push_back(i3); newIndices.push_back(i2);
			newIndices.push_back(i1); newIndices.push_back(i2); newIndices.push_back(i3);
		}

		meshData.indices = newIndices;
	}

	// Scale the sphere to the given radius
	for (auto& vertex : meshData.vertices)
	{
		vertex.Position.x *= radius;
		vertex.Position.y *= radius;
		vertex.Position.z *= radius;
	}

	return meshData;
}

// Helper functions
// -----------------------------------------------------------------
void GeometryGenerator::Subdivide(Mesh& mesh)
{
	std::vector<Vertex> newVertices;
	std::vector<UINT16> newIndices;

	// Map to store the midpoint of an edge and its corresponding index in the new vertex list
	std::map<std::pair<UINT16, UINT16>, UINT16> midpointMap;

	auto getMidpointVertex = [&](const Vertex& v0, const Vertex& v1) -> Vertex
		{
			Vertex midpointVertex;

			// Interpolate positions
			XMStoreFloat3(&midpointVertex.Position, 0.5f * (XMLoadFloat3(&v0.Position) + XMLoadFloat3(&v1.Position)));

			// Interpolate normals
			XMStoreFloat3(&midpointVertex.Normal, XMVector3Normalize(0.5f * (XMLoadFloat3(&v0.Normal) + XMLoadFloat3(&v1.Normal))));

			// Interpolate tangents
			XMStoreFloat3(&midpointVertex.Tangent, XMVector3Normalize(0.5f * (XMLoadFloat3(&v0.Tangent) + XMLoadFloat3(&v1.Tangent))));

			// Interpolate texture coordinates
			midpointVertex.TexCoord.x = 0.5f * (v0.TexCoord.x + v1.TexCoord.x);
			midpointVertex.TexCoord.y = 0.5f * (v0.TexCoord.y + v1.TexCoord.y);

			return midpointVertex;
		};

	auto getMidpointIndex = [&](UINT16 v0, UINT16 v1) -> UINT16
		{
			if (v0 > v1) std::swap(v0, v1);

			auto key = std::make_pair(v0, v1);

			if (midpointMap.find(key) != midpointMap.end())
				return midpointMap[key];

			// Create the new vertex at the midpoint
			Vertex midpointVertex = getMidpointVertex(mesh.vertices[v0], mesh.vertices[v1]);

			newVertices.push_back(midpointVertex);
			UINT16 midIndex = static_cast<UINT16>(newVertices.size() - 1);

			midpointMap[key] = midIndex;

			return midIndex;
		};

	for (size_t i = 0; i < mesh.indices.size(); i += 3)
	{
		UINT16 v0 = mesh.indices[i + 0];
		UINT16 v1 = mesh.indices[i + 1];
		UINT16 v2 = mesh.indices[i + 2];

		UINT16 m0 = getMidpointIndex(v0, v1);
		UINT16 m1 = getMidpointIndex(v1, v2);
		UINT16 m2 = getMidpointIndex(v2, v0);

		newIndices.push_back(v0);
		newIndices.push_back(m0);
		newIndices.push_back(m2);

		newIndices.push_back(v1);
		newIndices.push_back(m1);
		newIndices.push_back(m0);

		newIndices.push_back(v2);
		newIndices.push_back(m2);
		newIndices.push_back(m1);

		newIndices.push_back(m0);
		newIndices.push_back(m1);
		newIndices.push_back(m2);
	}

	// Update model data
	mesh.vertices = std::move(newVertices);
	mesh.indices = std::move(newIndices);
}

Vertex GeometryGenerator::MidPoint(const Vertex& v0, const Vertex& v1)
{
	Vertex midpoint;

	XMVECTOR p0 = XMLoadFloat3(&v0.Position);
	XMVECTOR p1 = XMLoadFloat3(&v1.Position);

	XMVECTOR n0 = XMLoadFloat3(&v0.Normal);
	XMVECTOR n1 = XMLoadFloat3(&v1.Normal);

	XMVECTOR tan0 = XMLoadFloat3(&v0.Tangent);
	XMVECTOR tan1 = XMLoadFloat3(&v1.Tangent);

	XMVECTOR tex0 = XMLoadFloat2(&v0.TexCoord);
	XMVECTOR tex1 = XMLoadFloat2(&v1.TexCoord);

	XMVECTOR midPos = 0.5f * (p0 + p1);
	XMVECTOR midNorm = XMVector3Normalize(0.5f * (n0 + n1));
	XMVECTOR midTan = XMVector3Normalize(0.5f * (tan0 + tan1));
	XMVECTOR midTex = 0.5f * (tex0 + tex1);

	XMStoreFloat3(&midpoint.Position, midPos);
	XMStoreFloat3(&midpoint.Normal, midNorm);
	XMStoreFloat3(&midpoint.Tangent, midTan);
	XMStoreFloat2(&midpoint.TexCoord, midTex);

	return midpoint;
}


// =================================================================
// Model Generator Class
// =================================================================