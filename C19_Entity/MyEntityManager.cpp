#include "MyEntityManager.h"
using namespace Simplex;
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
	m_EntityCount = 0;
	m_allEntities.clear();
}
void MyEntityManager::Release(void)
{
	for (uint uEntity = 0; uEntity < m_EntityCount; ++uEntity)
	{
		MyEntity* pEntity = m_allEntities[uEntity];
		SafeDelete(pEntity);
	}
	m_EntityCount = 0;
	m_allEntities.clear();
}

void MyEntityManager::CheckCollisions() {
	for (uint i = 0; i < m_EntityCount - 1; i++)
	{
		for (uint j = i + 1; j < m_EntityCount; j++)
		{
			m_allEntities[i]->IsColliding(m_allEntities[j]);
		}
	}
}

void MyEntityManager::RenderAllEntities() {
	for (uint i = 0; i < m_EntityCount - 1; i++) {
		m_allEntities[i]->AddToRenderList();
	}
}

int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	//look one by one for the specified unique id
	for (uint uIndex = 0; uIndex < m_EntityCount; ++uIndex)
	{
		if (a_sUniqueID == m_allEntities[uIndex]->GetUniqueID())
			return uIndex;
	}
	//if not found return -1
	return -1;
}

void Simplex::MyEntityManager::AddEntity(MyEntity* a_entity)
{
	MyEntity* pTemp = a_entity;
	if (pTemp->IsInitialized())
	{
		m_allEntities.push_back(pTemp);
		m_EntityCount = m_allEntities.size();
	}
}

void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	int nIndex = GetEntityIndex(a_sUniqueID);
	RemoveEntity((uint)nIndex);
}

void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	//if the list is empty return
	if (m_allEntities.size() == 0)
		return;

	// if out of bounds choose the last one
	if (a_uIndex >= m_EntityCount)
		a_uIndex = m_EntityCount - 1;

	// if the entity is not the very last we swap it for the last one
	if (a_uIndex != m_EntityCount - 1)
	{
		std::swap(m_allEntities[a_uIndex], m_allEntities[m_EntityCount - 1]);
	}

	//and then pop the last one
	MyEntity* pTemp = m_allEntities[m_EntityCount - 1];
	SafeDelete(pTemp);
	m_allEntities.pop_back();
	--m_EntityCount;
	return;
}

void MyEntityManager::PopulateEntities(std::vector<MyEntity*> a_entities) {
	for (uint i = 0; i < a_entities.size() - 1; i++) {
		m_pInstance->AddEntity(a_entities[i]);
	}
}

void MyEntityManager::ClearEntities() {
	for (uint i = 0; i < m_EntityCount - 1; i++) {
		m_pInstance->RemoveEntity(i);
	}
	m_EntityCount = 0;
	m_allEntities.clear();
}

matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	//if the list is empty return blank
	if (m_allEntities.size() == 0)
		return IDENTITY_M4;

	// if out of bounds
	if (a_uIndex >= m_EntityCount)
		a_uIndex = m_EntityCount - 1;

	return m_allEntities[a_uIndex]->GetModelMatrix();
}

void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->SetModelMatrix(a_m4ToWorld);
	}
}

void MyEntityManager::SetModelMatrices() {
	for (uint i = 0; i < m_EntityCount - 1; i++) {
		SetModelMatrix(m_allEntities[i]->GetModelMatrix(), m_allEntities[i]->GetUniqueID());
	}
}

MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}

void MyEntityManager::ReleaseInstance()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}


MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
