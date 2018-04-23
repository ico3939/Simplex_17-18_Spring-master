#ifndef __OCTREE_H_
#define __OCTREE_H_

#include "MyEntityManager.h"

namespace Simplex
{
	class Octree {

		// Variables used throughtout 
		// --------------------------
		static uint m_uiNumOctant;
		static uint m_uiNumLeaves;
		static uint m_uiMaxLevel;
		static uint m_EntityCount;

		uint m_uiOctID = 0; // Will store the current ID for this octant
		uint m_uiCurrLevel = 0; // Will store the current level of the octant
		uint m_uiNumChildren = 0;// Number of children on the octant (either 0 or 8)

		MeshManager* m_pMeshMngr = nullptr; //Mesh Manager singleton
		MyEntityManager* m_pEntityMngr = nullptr; //Entity Manager Singleton

		vector3 m_v3OctantSize = ZERO_V3; // size of the octant
		vector3 m_v3OctantCenter = ZERO_V3; // Will store the center point of the octant
		vector3 m_v3OctantMin = ZERO_V3; // Will store the minimum vector of the octant
		vector3 m_v3OctantMax = ZERO_V3; // Will store the maximum vector of the octant

		Octree* m_pOctParent = nullptr; // Will store the parent of current octant
		Octree* m_aOctChildren[8]; // Will store the children of the current octant

		std::vector<uint> m_vEntityList; // List of Entities under this octant (Index in Entity Manager)
		uint m_uiCurrEntityCount = 0;

		Octree* m_pOctRoot = nullptr; // Root octant
		std::vector<Octree*> m_v3RootChildren; // List of nodes that contain objects (this will be applied to root only)
		// -----------------------------------

	public:
		bool wireFrameSwitch = true;

		// Constructor, will create an octant containing all MagnaEntities Instances in the Mesh
		Octree(uint p_MaxLevel = 2, uint p_EntityCount = 5);

		// Constructor a_v3Center->Center of the octant in global space a_fSize->size of each side of the octant volume
		Octree(vector3 p_Center, vector3 p_Size);

		// Copy constructor
		Octree(Octree const& other);

		// Deconstructor
		~Octree(void);

		// Getter methods
		// --------------
		vector3 GetSize(void);

		vector3 GetCenterGlobal(void);

		vector3 GetMinGlobal(void);

		vector3 GetMaxGlobal(void);
		// ------------------------

		// Asks if there is a collision with the Entity specified by index from the Bounding Object Manager
		bool IsColliding(uint a_uRBIndex);

		// Displays the Octree
		void Display(uint a_uIndex, vector3 a_v3Color = C_GREEN);

		// Displays the Octree
		void DisplayCurrent(vector3 a_v3Color = C_GREEN);

		// Displays the whole Octree
		void DisplayAll(vector3 a_v3Color = C_GREEN);

		// Clears the Entity list for each node
		void ClearEntityList(void);

		// Allocates 8 smaller octants in the child pointers
		void Subdivide(void);

		// Returns the child specified in the index
		Octree* GetChild(uint a_nChild);

		// Returns the parent of the Octree
		Octree* GetParent(void);

		// Asks the Octree if it does not contain any children
		bool IsLeaf(void);

		// Asks the Octree if it contains more than this many Bounding Objects
		bool ContainsMoreThan(uint a_nEntities);

		// Deletes all children and the children of their children
		void KillBranches(void);

		// Gets the total number of octants in the world
		uint GetOctantCount(void);

		// Gets the total number Leaves in the world
		uint GetLeafCount(void);

		// Assigns the appropriate to the entities in the dimension
		void ConfigureDimensions(void);

	private:

		// Deallocates member fields
		void Release(void);

		// Allocates member fields
		void Init(void);
	};
}
#endif