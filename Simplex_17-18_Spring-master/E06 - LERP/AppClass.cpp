#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Ian Oliver - ico3939@g.rit.edu";

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(vector3(5.0f,3.0f,15.0f), ZERO_V3, AXIS_Y);

	m_pModel = new Simplex::Model();
	m_pModel->Load("Sorted\\WallEye.bto");
	
	m_stopsList.push_back(vector3(-4.0f, -2.0f, 5.0f));
	m_stopsList.push_back(vector3(1.0f, -2.0f, 5.0f));

	m_stopsList.push_back(vector3(-3.0f, -1.0f, 3.0f));
	m_stopsList.push_back(vector3(2.0f, -1.0f, 3.0f));

	m_stopsList.push_back(vector3(-2.0f, 0.0f, 0.0f));
	m_stopsList.push_back(vector3(3.0f, 0.0f, 0.0f));

	m_stopsList.push_back(vector3(-1.0f, 1.0f, -3.0f));
	m_stopsList.push_back(vector3(4.0f, 1.0f, -3.0f));

	m_stopsList.push_back(vector3(0.0f, 2.0f, -5.0f));
	m_stopsList.push_back(vector3(5.0f, 2.0f, -5.0f));

	m_stopsList.push_back(vector3(1.0f, 3.0f, -5.0f));
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// Draw the model
	m_pModel->PlaySequence();

	//Get a timer
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	//calculate the current position
	vector3 v3CurrentPos;
	

	//your code goes here

	vector3 v3Start; //start point
	vector3 v3End; //end point
	static uint route = 0; //current route
	v3Start = m_stopsList[route]; //start at the current route
	v3End = m_stopsList[(route + 1) % m_stopsList.size()]; //end at route +1 (if overboard will restart from 0)

	//get the percentage
	float fTimeBetweenStops = 2.0;//in seconds
	//map the value to be between 0.0 and 1.0
	float fPercentage = MapValue(fTimer, 0.0f, fTimeBetweenStops, 0.0f, 1.0f);

	v3CurrentPos = glm::lerp(v3Start, v3End, fPercentage);

	//if we are done with this route
	if (fPercentage >= 1.0f)
	{
		route++; //go to the next route
		fTimer = m_pSystem->GetDeltaTime(uClock);//restart the clock
		route %= m_stopsList.size();//make sure we are within boundries
	}
	//-------------------
	


	
	matrix4 m4Model = glm::translate(v3CurrentPos);
	m_pModel->SetModelMatrix(m4Model);

	m_pMeshMngr->Print("\nTimer: ");//Add a line on top
	m_pMeshMngr->PrintLine(std::to_string(fTimer), C_YELLOW);

	// Draw stops
	for (uint i = 0; i < m_stopsList.size(); ++i)
	{
		m_pMeshMngr->AddSphereToRenderList(glm::translate(m_stopsList[i]) * glm::scale(vector3(0.05f)), C_GREEN, RENDER_WIRE);
	}
	
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
	SafeDelete(m_pModel);
	//release GUI
	ShutdownGUI();
}