#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(vector3(0.0f, 3.0f, 13.0f), //Position
		vector3(0.0f, 3.0f, 12.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	m_pEntityManager = MyEntityManager::GetInstance();

	std::vector<MyEntity*> initialEntities = {};
	//creeper
	m_pCreeper = new MyEntity("Minecraft\\Creeper.obj", "Creeper");
	initialEntities.push_back(m_pCreeper);
	
	//steve
	m_pSteve = new MyEntity("Minecraft\\Steve.obj", "Steve");
	initialEntities.push_back(m_pSteve);

	m_pCow = new MyEntity("Minecraft\\Cow.obj", "Cow");
	m_pEntityManager->SetModelMatrix(glm::translate(vector3(2.0f, -1.5f, -1.0f)), m_pCow->GetUniqueID());
	initialEntities.push_back(m_pCow);
	m_pZombie = new MyEntity("Minecraft\\Zombie.obj", "Zombie");
	m_pEntityManager->SetModelMatrix(glm::translate(vector3(0.0f, -2.5f, 0.0f)), m_pZombie->GetUniqueID());
	initialEntities.push_back(m_pZombie);
	m_pPig = new MyEntity("Minecraft\\Pig.obj", "Pig");
	initialEntities.push_back(m_pPig);
	m_pEntityManager->SetModelMatrix(glm::translate(vector3(-2.0f, -1.0f, -1.0f)), m_pPig->GetUniqueID());

	m_pEntityManager->PopulateEntities(initialEntities);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();


	//Set model matrix to the creeper
	matrix4 mCreeper = glm::translate(m_v3Creeper) * ToMatrix4(m_qCreeper) * ToMatrix4(m_qArcBall);
	m_pEntityManager->SetModelMatrix(mCreeper, "Creeper");



	//Set model matrix to Steve 
	matrix4 mSteve = glm::translate(vector3(2.25f, 0.0f, 0.0f)) * glm::rotate(IDENTITY_M4, -55.0f, AXIS_Z);
	m_pEntityManager->SetModelMatrix(mSteve, "Steve");

	//Move the last entity added slowly to the right
	matrix4 lastMatrix = m_pEntityManager->GetModelMatrix();// get the model matrix of the last added
	lastMatrix *= glm::translate(IDENTITY_M4, vector3(0.01f, 0.0f, 0.0f)); //translate it
	m_pEntityManager->SetModelMatrix(lastMatrix); //return it to its owner


	//Check collision
	m_pEntityManager->CheckCollisions();



	m_pEntityManager->RenderAllEntities();

}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}

void Application::Release(void)
{
	//release the entity manager
	m_pEntityManager->ReleaseInstance();

	//release GUI
	ShutdownGUI();
}
