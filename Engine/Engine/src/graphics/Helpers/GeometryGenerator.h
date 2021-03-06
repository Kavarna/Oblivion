#pragma once

#include "../../common/common.h"
#include "../Model.h"

#include <OblivionObjects.h>

class GeometryGenerator
{
public:

	struct MeshData
	{
		std::vector<Oblivion::SVertex> Vertices;
		std::vector<UINT> Indices;
	};

	///<summary>
	/// Creates a box centered at the origin with the given dimensions.
	///</summary>
	void CreateBox(float width, float height, float depth, MeshData& meshData);

	///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
	void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData);

	///<summary>
	/// Creates a geosphere centered at the origin with the given radius.  The
	/// depth controls the level of tessellation.
	///</summary>
	void CreateGeosphere(float radius, UINT numSubdivisions, MeshData& meshData);

	///<summary>
	/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	/// The bottom and top radius can vary to form various cone shapes rather than true
	// cylinders.  The slices and stacks parameters control the degree of tessellation.
	///</summary>
	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);

	///<summary>
	/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	/// at the origin with the specified width and depth.
	///</summary>
	void CreateGrid(float width, float depth, UINT m, UINT n, MeshData& meshData);

	///<summary>
	/// Creates a quad covering the screen in NDC coordinates.  This is useful for
	/// postprocessing effects.
	///</summary>
	void CreateFullscreenQuad(MeshData& meshData);

	///<summary>
	/// Creates a quad. This is useful for simple 2D rendering.
	///</summary>
	void CreateQuad(MeshData& meshData, float width, float height);

private:
	void Subdivide(MeshData& meshData);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
};