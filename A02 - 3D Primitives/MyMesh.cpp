#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code

	vector3 vecArray[360];
	float degreesBetween = (2 * PI) / a_nSubdivisions;

	vector3 bottomCenter(0.0f, 0.0f, 0.0f); // the point in the center of the bottom of the cone
	vector3 top(0.0f, a_fHeight, 0.0f); // the top center of the cone
	vector3 firstPoint(a_fRadius, 0.0f, 0.0f); // the initial starting point

	vecArray[0] = firstPoint;
	
	// this loop goes around the cone, sets up the outer points, 
	// then adds them to the array
	for (int i = 1; i <= a_nSubdivisions; i++) {
		float currentDegree = degreesBetween * i;

		vector3 point(cos(currentDegree) * a_fRadius, 0.0f, sin(currentDegree) * a_fRadius);
		vecArray[i] = point;

		// builds the triangles with the points
		if (i < a_nSubdivisions) {
			AddTri(vecArray[i - 1], top, vecArray[i]);
			AddTri(vecArray[i - 1], vecArray[i], bottomCenter);
		}
		else {
			AddTri(vecArray[i - 1], top, vecArray[0]);
			AddTri(vecArray[i - 1], vecArray[0], bottomCenter);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 topVecArray[360]; // an array holding all of the top ring's points
	vector3 bottomVecArray[360];
	float degreesBetween = (2 * PI) / a_nSubdivisions;

	vector3 bottomCenter(0.0f, 0.0f, 0.0f); // the point in the center of the bottom of the cylinder
	vector3 topCenter(0.0f, a_fHeight, 0.0f); // the point in the center of the top of the cylinder
	vector3 firstTopPoint(a_fRadius, a_fHeight, 0.0f); // the initial starting point on the top ring
	vector3 firstBottomPoint(a_fRadius, 0.0f, 0.0f); // the initial starting point on the bottom ring

	topVecArray[0] = firstTopPoint;
	bottomVecArray[0] = firstBottomPoint;

	// this loop goes around the cylinder, sets up the outer points, 
	// then adds them to the array
	for (int i = 1; i <= a_nSubdivisions; i++) {
		float currentDegree = degreesBetween * i;

		vector3 topPoint(cos(currentDegree) * a_fRadius, a_fHeight, sin(currentDegree) * a_fRadius);
		topVecArray[i] = topPoint;

		vector3 bottomPoint(cos(currentDegree) * a_fRadius, 0.0f, sin(currentDegree) * a_fRadius);
		bottomVecArray[i] = bottomPoint;

		// builds the triangles and quads with the points
		if (i < a_nSubdivisions) {
			// builds top ring
			AddTri(topVecArray[i - 1], topCenter, topVecArray[i]);

			// builds bottom ring
			AddTri(bottomVecArray[i - 1], bottomVecArray[i], bottomCenter);

			// builds quad side face
			AddQuad(bottomVecArray[i], bottomVecArray[i - 1], topVecArray[i], topVecArray[i - 1]);
		}
		else {
			// builds top ring
			AddTri(topVecArray[i - 1], topCenter, topVecArray[0]);

			// builds bottom ring
			AddTri(bottomVecArray[i - 1], bottomVecArray[0], bottomCenter);

			// builds quad side face
			AddQuad(bottomVecArray[0], bottomVecArray[i - 1], topVecArray[0], topVecArray[i - 1]);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 topInnerVecArray[360]; // an array holding all of the inner top ring's points
	vector3 topOuterVecArray[360]; // an array holding all of the outer top ring's points
	vector3 bottomInnerVecArray[360]; // holds all of the points for the inner ring on the bottom
	vector3 bottomOuterVecArray[360]; // holds all of the points for the outer ring on the bottom
	float degreesBetween = (2 * PI) / a_nSubdivisions;
	
	vector3 firstTopInnerPoint(a_fInnerRadius, a_fHeight, 0.0f); // the initial starting point on the inside of the top ring
	vector3 firstTopOuterPoint(a_fInnerRadius + a_fOuterRadius, a_fHeight, 0.0f); // the initial starting point on the outside of the top ring
	vector3 firstBottomInnerPoint(a_fInnerRadius, 0.0f, 0.0f); // the initial starting point on the inside of the bottom ring
	vector3 firstBottomOuterPoint(a_fInnerRadius + a_fOuterRadius, 0.0f, 0.0f); // the initial starting point on the outside of the bottom ring

	topInnerVecArray[0] = firstTopInnerPoint;
	topOuterVecArray[0] = firstTopOuterPoint;
	bottomInnerVecArray[0] = firstBottomInnerPoint;
	bottomOuterVecArray[0] = firstBottomOuterPoint;

	// this loop goes around the tube, sets up the outer points, 
	// then adds them to the array
	for (int i = 1; i <= a_nSubdivisions; i++) {
		float currentDegree = degreesBetween * i;

		vector3 topInnerPoint(cos(currentDegree) * a_fInnerRadius, a_fHeight, sin(currentDegree) * a_fInnerRadius);
		topInnerVecArray[i] = topInnerPoint;

		vector3 topOuterPoint(cos(currentDegree) * (a_fInnerRadius + a_fOuterRadius), a_fHeight, sin(currentDegree) * (a_fInnerRadius + a_fOuterRadius));
		topOuterVecArray[i] = topOuterPoint;

		vector3 bottomInnerPoint(cos(currentDegree) * a_fInnerRadius, 0.0f, sin(currentDegree) * a_fInnerRadius);
		bottomInnerVecArray[i] = bottomInnerPoint;

		vector3 bottomOuterPoint(cos(currentDegree) * (a_fInnerRadius + a_fOuterRadius), 0.0f, sin(currentDegree) * (a_fInnerRadius + a_fOuterRadius));
		bottomOuterVecArray[i] = bottomOuterPoint;


		// builds the triangles and quads with the points
		if (i < a_nSubdivisions) {
			// builds top ring
			AddQuad(topOuterVecArray[i], topOuterVecArray[i - 1], topInnerVecArray[i], topInnerVecArray[i - 1]);

			// builds bottom ring
			AddQuad(bottomOuterVecArray[i - 1], bottomOuterVecArray[i], bottomInnerVecArray[i - 1], bottomInnerVecArray[i]);

			// builds inner quad side face
			AddQuad(bottomInnerVecArray[i - 1], bottomInnerVecArray[i], topInnerVecArray[i - 1], topInnerVecArray[i]);

			// builds outer quad side face
			AddQuad(bottomOuterVecArray[i], bottomOuterVecArray[i - 1], topOuterVecArray[i], topOuterVecArray[i - 1]);
		}
		else {
			// builds top ring
			AddQuad(topOuterVecArray[0], topOuterVecArray[i - 1], topInnerVecArray[0], topInnerVecArray[i - 1]);

			// builds bottom ring
			AddQuad(bottomOuterVecArray[i - 1], bottomOuterVecArray[0], bottomInnerVecArray[i - 1], bottomInnerVecArray[0]);

			// builds inner quad side face
			AddQuad(bottomInnerVecArray[i - 1], bottomInnerVecArray[0], topInnerVecArray[i - 1], topInnerVecArray[0]);

			// builds outer quad side face
			AddQuad(bottomOuterVecArray[0], bottomOuterVecArray[i - 1], topOuterVecArray[0], topOuterVecArray[i - 1]);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code

	// create a vector of vector3 arrays to store the points in the rings of the sphere
	vector3 vec3ArrayVector[10][10];

	float degreesBetween = (2 * PI) / a_nSubdivisions;

	vector3 topCenter(0.0f, a_fRadius, 0.0f); // the point in the center of the top of the sphere
	vector3 bottomCenter(0.0f, -(a_fRadius), 0.0f); // the point in the center of the bottom of the sphere

	// set up initial points
	for (int i = 1; i <= a_nSubdivisions; i++) {
		float currentDegree = degreesBetween * i;
		vector3 firstRingPoint(sin(currentDegree) * a_fRadius, cos(currentDegree) * a_fRadius, 0.0f); // the initial starting point on the current ring
		vec3ArrayVector[i - 1][0] = firstRingPoint;
	}

	// this loop goes around the cylinder, sets up the outer points, 
	// then adds them to the array
	for (int i = 1; i <= a_nSubdivisions; i++) {
		for (int j = 1; j <= a_nSubdivisions; j++) {
			float currentDegree = degreesBetween * j;

			vector3 currentRingPoint(cos(currentDegree) * a_fRadius, a_fRadius - (2 * (a_fRadius)/a_nSubdivisions) * i, sin(currentDegree) * a_fRadius);
			vec3ArrayVector[i][j] = currentRingPoint;


			// if it's the first or last rings, create triangles connecting those points to the top and bottom points respectively
			if (i == 1) {
				if (j == 1) {
					AddTri(vec3ArrayVector[i][j - 1], topCenter, vec3ArrayVector[i][j]);
				}
				else {
					AddTri(vec3ArrayVector[i][j - 1], topCenter, vec3ArrayVector[i][0]);
				}
			}
			else if (i == a_nSubdivisions) {
				if (j == 1) {
					AddTri(vec3ArrayVector[i][j - 1], vec3ArrayVector[i][j], bottomCenter);
				}
				else {
					AddTri(vec3ArrayVector[i][j - 1], vec3ArrayVector[i][0], bottomCenter);
				}
			}


			// builds the quad faces with the points
			if (j < a_nSubdivisions) {
				// builds quad side face
				AddQuad(vec3ArrayVector[i][j], vec3ArrayVector[i - 1][j], vec3ArrayVector[i][j - 1], vec3ArrayVector[i - 1][j - 1]);
			}
			else {
				// builds quad side face
				AddQuad(vec3ArrayVector[i][0], vec3ArrayVector[i - 1][0], vec3ArrayVector[i][j-1], vec3ArrayVector[i - 1][j - 1]);
			}
		}
	}

		
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}