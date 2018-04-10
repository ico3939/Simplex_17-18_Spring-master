#pragma once
#include "MyEntity.h"
namespace Simplex {

class MyEntityManager
{

	std::vector<MyEntity*> m_allEntities; // A vector containing all entities
	static MyEntityManager* m_pInstance; // a singleton pointer instance of the manager
	uint m_EntityCount = 0;
public:

	// WHAT IT NEEDS TO HAVE
	//----------------------

	// 1. A vector containing all entities (check)
	// 1a. The ability to initialize entities by setting their model/world matrices
	// 2. Methods to add and subtract entities from the vector
	// 3. A method to check collisions for each entitiy within a certain range
	// 3a. Potentially collision check optimizations such as using an octree
	// 4. Methods for handling transformations (translation, rotation, scaling)
	// 5. Handling the rendering of each of the entities


	void AddEntity(MyEntity* a_entity);
	void RemoveEntity(uint a_uIndex);
	void RemoveEntity(String a_sID);
	String GetUniqueID(uint a_uIndex);
	int GetEntityIndex(String a_sUniqueID);
	void RenderAllEntities(void);
	void CheckCollisions(void);
	void PopulateEntities(std::vector<MyEntity*> a_entities);
	void ClearEntities(void);
	matrix4 GetModelMatrix(uint a_uIndex = -1);
	void SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID);
	void SetModelMatrices();
	static MyEntityManager* GetInstance();
	static void ReleaseInstance(void);

private:
	MyEntityManager(void);
	MyEntityManager(MyEntityManager const& other);
	MyEntityManager& operator=(MyEntityManager const& other);
	~MyEntityManager(void);
	void Release(void);
	void Init(void);
};
}


