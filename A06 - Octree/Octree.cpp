#include "Octree.h"
using namespace Simplex;

// declaring static variables
// --------------------------
uint Octree::m_uiNumOctant;
uint Octree::m_uiNumLeaves;
uint Octree::m_uiMaxLevel;
uint Octree::m_EntityCount;
// --------------------------

// Constructor
Octree::Octree(uint p_MaxLevel, uint p_EntityCount) {

	// creates the root
	Init();
	m_pOctRoot = this;
	m_uiMaxLevel = p_MaxLevel;
	m_EntityCount = p_EntityCount;

	// sets the max/min to center
	m_v3OctantMax = m_v3OctantMin = m_pEntityMngr->GetRigidBody()->GetCenterGlobal();
	m_uiCurrEntityCount = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < m_uiCurrEntityCount; i++) {

		m_vEntityList.push_back(i);

		// get the min/max
		vector3 rb_min = m_pEntityMngr->GetRigidBody(i)->GetMinGlobal();
		vector3 rb_max = m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal();

		// setting the min and max for x
		if (rb_min.x < m_v3OctantMin.x) {

			m_v3OctantMin.x = rb_min.x;
		}
		if (rb_max.x > m_v3OctantMax.x) {

			m_v3OctantMax.x = rb_max.x;
		}

		// setting the min and max for y
		if (rb_min.y < m_v3OctantMin.y) {

			m_v3OctantMin.y = rb_min.y;
		}
		if (rb_max.y > m_v3OctantMax.x) {

			m_v3OctantMax.y = rb_max.y;
		}

		// setting the min and max for z
		if (rb_min.z < m_v3OctantMin.z) {

			m_v3OctantMin.z = rb_min.z;
		}
		if (rb_max.z > m_v3OctantMax.z) {

			m_v3OctantMax.z = rb_max.z;
		}
	}

	// Calculate the center/size
	m_v3OctantCenter = (m_v3OctantMin + m_v3OctantMax) / 2.0f;
	m_v3OctantSize = m_v3OctantMax - m_v3OctantMin;

	// create appropriate number of children
	Subdivide();

	// add leaf dimensions
	ConfigureDimensions();
}

// Constructor for branch and leaf
Octree::Octree(vector3 p_Center, vector3 p_Size) {
	
	Init();
	m_v3OctantCenter = p_Center;
	m_v3OctantSize = p_Size;

	m_v3OctantMax = p_Center + m_v3OctantSize / 2.0f;
	m_v3OctantMin = p_Center - m_v3OctantSize / 2.0f;

}

Octree::Octree(Octree const & other) {

	Init();

	// copying data over
	m_uiCurrLevel = other.m_uiCurrLevel;
	m_v3OctantSize = other.m_v3OctantSize;
	m_v3OctantCenter = other.m_v3OctantCenter;
	m_v3OctantMin = other.m_v3OctantMin;
	m_v3OctantMax = other.m_v3OctantMax;
	m_pOctParent = other.m_pOctParent;
	Release();

	// loops through and recursively copies/creates more nodes
	m_uiNumChildren = other.m_uiNumChildren;
	for (uint i = 0; i < m_uiNumChildren; i++) {
		m_aOctChildren[i] = new Octree(*other.m_aOctChildren[i]);
	}

	// create a new entity list
	m_uiCurrEntityCount = other.m_uiCurrEntityCount;
	for (uint i = 0; i < m_uiCurrEntityCount; i++) {
		m_vEntityList.push_back(other.m_vEntityList[i]);
	}

	// if it's a root node, then copy m_lChildren over
	m_pOctRoot = other.m_pOctRoot;
	if (this == m_pOctRoot) {
		float childCount = other.m_v3RootChildren.size();

		for (uint i = 0; i < childCount; i++) {
			m_v3RootChildren.push_back(other.m_v3RootChildren[i]);
		}
	}
}

//destructor
Octree::~Octree(void) {

	Release();
}

//Getters
vector3 Octree::GetSize(void) {

	return m_v3OctantSize;
}

// Getter Methods
// ----------------------------------
vector3 Octree::GetCenterGlobal(void) {

	return m_v3OctantCenter;
}

vector3 Octree::GetMinGlobal(void) {

	return m_v3OctantMin;
}

vector3 Octree::GetMaxGlobal(void) {

	return m_v3OctantMax;
}

uint Octree::GetOctantCount(void) {

	return m_uiNumOctant;
}

uint Simplex::Octree::GetLeafCount(void) {

	return m_uiNumLeaves;
}

Octree * Octree::GetParent(void) {

	return m_pOctParent;
}

Octree * Octree::GetChild(uint a_nChild) {

	if (m_uiNumChildren == 0) {

		return nullptr;
	}
	else {
		return m_aOctChildren[a_nChild];
	}
}
// ---------------------------------------


bool Octree::IsLeaf(void) {

	return m_uiNumChildren == 0;
}

bool Octree::ContainsMoreThan(uint a_nEntities) {

	return m_uiCurrEntityCount > a_nEntities;
}

bool Octree::IsColliding(uint a_uRBIndex) {

	MyRigidBody* rb = m_pEntityMngr->GetRigidBody(a_uRBIndex);
	vector3 rb_max = rb->GetMaxGlobal();
	vector3 rb_min = rb->GetMinGlobal();

	if (rb_max.x > m_v3OctantMin.x &&
		rb_max.y > m_v3OctantMin.y &&
		rb_max.z > m_v3OctantMin.z &&
		rb_min.x < m_v3OctantMax.x &&
		rb_min.y < m_v3OctantMax.y &&
		rb_min.z < m_v3OctantMax.z)
	{
		return true;
	}
	else {

		return false;
	}
}

//function to display selected octant
void Simplex::Octree::Display(uint a_uIndex, vector3 a_v3Color) {

	if (a_uIndex >= m_uiNumOctant && wireFrameSwitch == true) {

		DisplayAll();
		return;
	}
	m_v3RootChildren[a_uIndex]->DisplayCurrent(a_v3Color);
}

// helper function to only display an octant
void Octree::DisplayCurrent(vector3 a_v3Color) {

	if (wireFrameSwitch) {
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3OctantCenter) * glm::scale(IDENTITY_M4, m_v3OctantSize), a_v3Color);
	}
}

// helper function to display all octants
void Simplex::Octree::DisplayAll(vector3 a_v3Color) {

	if (wireFrameSwitch) {
		if (IsLeaf()) {
			DisplayCurrent(a_v3Color);
		}
		else {

			for (uint i = 0; i < m_uiNumChildren; i++) {
				m_aOctChildren[i]->DisplayAll(a_v3Color);
			}
		}
	}
	
}

//recursively clears all child nodes
void Octree::ClearEntityList(void) {
	
	for (uint i = 0; i < m_uiNumChildren; i++) {

		m_aOctChildren[i]->ClearEntityList();
	}
	m_vEntityList.clear();
}

//recursively subdivides
void Octree::Subdivide(void) {

	// stops subdividing if at desired level if there are 5 or less entities in the octant
	if (m_uiCurrLevel >= m_uiMaxLevel || !ContainsMoreThan(m_EntityCount)) {

		m_pOctRoot->m_v3RootChildren.push_back(this);
		m_uiNumLeaves++;
		return;
	}

	// prints out if someone is trying to subdivide an octant that's not a leaf
	if (m_uiNumChildren == 8) {

		std::cout << "Octree cannot go over Max subdivisions" << std::endl;
		return;
	}

	// creating each octant at the right position
	m_aOctChildren[0] = new Octree(m_v3OctantCenter + vector3(-m_v3OctantSize.x / 4, m_v3OctantSize.y / 4, -m_v3OctantSize.z / 4), m_v3OctantSize / 2.0f);
	m_aOctChildren[1] = new Octree(m_v3OctantCenter + vector3(-m_v3OctantSize.x / 4, m_v3OctantSize.y / 4, m_v3OctantSize.z / 4), m_v3OctantSize / 2.0f);
	m_aOctChildren[2] = new Octree(m_v3OctantCenter + vector3(-m_v3OctantSize.x / 4, -m_v3OctantSize.y / 4, -m_v3OctantSize.z / 4), m_v3OctantSize / 2.0f);
	m_aOctChildren[3] = new Octree(m_v3OctantCenter + vector3(-m_v3OctantSize.x / 4, -m_v3OctantSize.y / 4, m_v3OctantSize.z / 4), m_v3OctantSize / 2.0f);
	m_aOctChildren[4] = new Octree(m_v3OctantCenter + vector3(m_v3OctantSize.x / 4, -m_v3OctantSize.y / 4, -m_v3OctantSize.z / 4), m_v3OctantSize / 2.0f);
	m_aOctChildren[5] = new Octree(m_v3OctantCenter + vector3(m_v3OctantSize.x / 4, -m_v3OctantSize.y / 4, m_v3OctantSize.z / 4), m_v3OctantSize / 2.0f);
	m_aOctChildren[6] = new Octree(m_v3OctantCenter + vector3(m_v3OctantSize.x / 4, m_v3OctantSize.y / 4, -m_v3OctantSize.z / 4), m_v3OctantSize / 2.0f);
	m_aOctChildren[7] = new Octree(m_v3OctantCenter + vector3(m_v3OctantSize.x / 4, m_v3OctantSize.y / 4, m_v3OctantSize.z / 4), m_v3OctantSize / 2.0f);
	m_uiNumChildren = 8;

	// loop through and initialize the children
	for (uint i = 0; i < m_uiNumChildren; i++) {

		m_aOctChildren[i]->m_pOctParent = this;
		m_aOctChildren[i]->m_uiCurrLevel = m_uiCurrLevel + 1;
		m_aOctChildren[i]->m_pOctRoot = m_pOctRoot;

		// loops through and adds colliding rigidbodies
		for (uint j = 0; j < m_uiCurrEntityCount; j++) {
			
			if (m_aOctChildren[i]->IsColliding(m_vEntityList[j])) {

				m_aOctChildren[i]->m_vEntityList.push_back(m_vEntityList[j]);
			}
		}

		// update entity count
		m_aOctChildren[i]->m_uiCurrEntityCount = m_aOctChildren[i]->m_vEntityList.size();

		// recursive call
		m_aOctChildren[i]->Subdivide();
	}

}

//recursively kills all nodes except root
void Octree::KillBranches(void) {

	if (IsLeaf()) {

		return;
	}
	else {

		for (uint i = 0; i < m_uiNumChildren; i++) {

			m_aOctChildren[i]->KillBranches();
			SafeDelete(m_aOctChildren[i]);
		}
	}
}

//recursive call to configure dimensions for all leaves
void Simplex::Octree::ConfigureDimensions() {

	if (IsLeaf()) {

		for (uint i = 0; i < m_uiCurrEntityCount; i++) {

			m_pEntityMngr->AddDimension(m_vEntityList[i], m_uiOctID);
		}
	}
	else {

		for (uint i = 0; i < m_uiNumChildren; i++) {

			m_aOctChildren[i]->ConfigureDimensions();
		}
	}
}

//release
void Octree::Release(void) {

	if (this == m_pOctRoot) {

		KillBranches();
	}
}

//init
void Octree::Init(void) {

	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();

	m_uiOctID = m_uiNumOctant;
	m_uiNumOctant++;
}