// My implementation of Chapter 7 from http://www.arcsynthesis.org/gltut/Positioning/Tutorial%2007.html

#include <glload/gl_3_3.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Framework.h"
#include "../framework/Mesh.h"
#include <glutil/glutil.h>
#include <vector>
#include <memory>

class ProgramData
{
public:
	ProgramData(GLuint programID, int globalMatricesBindingIndex)
		: mProgramID(programID)
		, mModelToWorldMatrixUniform(glGetUniformLocation(programID, "modelToWorldMatrix"))
		, mBaseColorUniform(glGetUniformLocation(programID, "baseColor"))
		, mGlobalUniformBlockIndex(glGetUniformBlockIndex(programID, "GlobalMatrices"))
	{
		glUniformBlockBinding(programID, mGlobalUniformBlockIndex, globalMatricesBindingIndex);
	}

	GLuint GetProgramID() const
	{
		return mProgramID;
	}

	GLint GetModelToWorldMatrixUniform() const
	{
		return mModelToWorldMatrixUniform;
	}

	GLint GetBaseColorUniform() const
	{
		return mBaseColorUniform;
	}

	GLint GetGlobalUniformBlockIndex() const
	{
		return mGlobalUniformBlockIndex;
	}

private:
	const GLuint mProgramID;
	const GLint mModelToWorldMatrixUniform;
	const GLint mBaseColorUniform;
	const GLint mGlobalUniformBlockIndex;
};

std::unique_ptr<const ProgramData> gUniformColorProgram;
std::unique_ptr<const ProgramData> gUniformColorTintProgram;
std::unique_ptr<const ProgramData> gObjectColorProgram;
std::unique_ptr<const Framework::Mesh> gPlaneMesh;
std::unique_ptr<const Framework::Mesh> gCubeColorMesh;
std::unique_ptr<const Framework::Mesh> gCubeTintMesh;
std::unique_ptr<const Framework::Mesh> gCylinderTintMesh;
std::unique_ptr<const Framework::Mesh> gConeTintMesh;

GLuint gGlobalMatricesUBO;
const int gGlobalMatricesBindingIndex = 0;

static bool gDrawLookAtPoint = false;

const float PARTHENON_POS_X = 20.0f;
const float PARTHENON_POS_Z = -10.0f;

const float PARTHENON_WIDTH = 20.0f;
const float PARTHENON_LENGTH = 30.0f;
const float PARTHENON_BASE_HEIGHT = 1.0f;
const float PARTHENON_COLUMN_HEIGHT = 5.0f;
const float PARTHENON_TOP_HEIGHT = 2.0f;

const float PARTHENON_INTERIOR_WIDTH = 8.0f;
const float PARTHENON_INTERIOR_LENGTH = 14.0f;

const float PARTHENON_COLUMN_BASE_WIDTH = 1.0f;
const float PARTHENON_COLUMN_BASE_LENGTH = 1.0f;
const float PARTHENON_COLUMN_BASE_HEIGHT = 0.25f;

const float PARTHENON_COLUMN_SPACING = 2 * PARTHENON_COLUMN_BASE_LENGTH;
const float PARTHENON_COLUMN_MARGIN_OFFSET = 1.0f;

std::unique_ptr<ProgramData> LoadProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	std::vector<GLuint> shaders;
	shaders.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShaderPath));
	shaders.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShaderPath));

	return std::unique_ptr<ProgramData>(new ProgramData(Framework::CreateProgram(shaders), gGlobalMatricesBindingIndex));
}

void InitializePrograms()
{
	gUniformColorProgram = LoadProgram("PosOnlyWorldTransform.vert", "ColorUniform.frag");
	gUniformColorTintProgram = LoadProgram("PosColorWorldTransform.vert", "ColorMultUniform.frag");
	gObjectColorProgram = LoadProgram("PosColorWorldTransform.vert", "ColorPassthrough.frag");
}

void LoadMeshes()
{
	try
	{
		gPlaneMesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh("UnitPlane.xml"));
		gCubeColorMesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh("UnitCubeColor.xml"));
		gCubeTintMesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh("UnitCubeTint.xml"));
		gCylinderTintMesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh("UnitCylinderTint.xml"));
		gConeTintMesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh("UnitConeTint.xml"));
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}
}

void InitBuffers()
{
	glGenBuffers(1, &gGlobalMatricesUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, gGlobalMatricesUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);

	glBindBufferRange(GL_UNIFORM_BUFFER, gGlobalMatricesBindingIndex, gGlobalMatricesUBO, 0, sizeof(glm::mat4) * 2);
}

void init()
{
	InitializePrograms();
	LoadMeshes();
	InitBuffers();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glEnable(GL_DEPTH_CLAMP);
}

float GetRadiansFromDegrees(const float angleInDegrees)
{
	return 3.14159f * angleInDegrees / 180.0f;
}

static glm::vec3 gCameraTarget(0.0f, 0.4f, 0.0f);
static glm::vec3 gCameraPositionInRelativeSphere(60.0f, -40.0f, 60.0f);
glm::vec3 ResolveCameraPosition()
{
	const float phi = GetRadiansFromDegrees(gCameraPositionInRelativeSphere.x);
	const float theta = GetRadiansFromDegrees(gCameraPositionInRelativeSphere.y + 90.0f);

	const float sinPhi = sinf(phi);
	const float cosPhi = cosf(phi);
	const float sinTheta = sinf(theta);
	const float cosTheta = cosf(theta);

	glm::vec3 directionToCamera(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);
	return (directionToCamera * gCameraPositionInRelativeSphere.z) + gCameraTarget;
}

glm::mat4 CalculateLookAtMatrix(const glm::vec3& cameraPosition, const glm::vec3& lookAtPoint, const glm::vec3& upPosition)
{
	const glm::vec3 lookDirection = glm::normalize(lookAtPoint - cameraPosition);
	const glm::vec3 upDirection = glm::normalize(upPosition);

	const glm::vec3 rightDirection = glm::normalize(glm::cross(lookDirection, upDirection));
	const glm::vec3 perpendicularUpDirection = glm::cross(rightDirection, lookDirection);

	glm::mat4 rotationMatrix(1.0f);
	rotationMatrix[0] = glm::vec4(rightDirection, 0.0f);
	rotationMatrix[1] = glm::vec4(perpendicularUpDirection, 0.0f);
	rotationMatrix[2] = glm::vec4(-lookDirection, 0.0f);

	rotationMatrix = glm::transpose(rotationMatrix);

	glm::mat4 translationMatrix(1.0f);
	translationMatrix[3] = glm::vec4(-cameraPosition, 1.0f);

	return rotationMatrix * translationMatrix;
}

void SetWorldToCameraMatrix(const glm::mat4& worldToCameraTransform)
{
	glBindBuffer(GL_UNIFORM_BUFFER, gGlobalMatricesUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(worldToCameraTransform));
	glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
}

void SetCameraToClipMatrix(const glm::mat4& cameraToClipTransform)
{
	glBindBuffer(GL_UNIFORM_BUFFER, gGlobalMatricesUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cameraToClipTransform));
	glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
}

void RenderPlane(const glm::mat4& modelToWorldMatrix)
{
	glUseProgram(gUniformColorProgram->GetProgramID());
	glUniformMatrix4fv(gUniformColorProgram->GetModelToWorldMatrixUniform(), 1, GL_FALSE, glm::value_ptr(modelToWorldMatrix));
	glUniform4f(gUniformColorProgram->GetBaseColorUniform(), 0.302f, 0.416f, 0.0589f, 1.0f);
	gPlaneMesh->Render();
	glUseProgram(GL_NONE);
}

void RenderCubeTintMesh(const glm::mat4& modelToWorldMatrix)
{
	glUseProgram(gUniformColorTintProgram->GetProgramID());
	glUniformMatrix4fv(gUniformColorTintProgram->GetModelToWorldMatrixUniform(), 1, GL_FALSE, glm::value_ptr(modelToWorldMatrix));
	glUniform4f(gUniformColorTintProgram->GetBaseColorUniform(), 0.9f, 0.9f, 0.9f, 0.9f);
	gCubeTintMesh->Render();
	glUseProgram(GL_NONE);
}

void RenderCylinderTintMesh(const glm::mat4& modelToWorldMatrix, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
{
	glUseProgram(gUniformColorTintProgram->GetProgramID());
	glUniformMatrix4fv(gUniformColorTintProgram->GetModelToWorldMatrixUniform(), 1, GL_FALSE, glm::value_ptr(modelToWorldMatrix));
	glUniform4f(gUniformColorTintProgram->GetBaseColorUniform(), r, g, b, a);
	gCylinderTintMesh->Render();
	glUseProgram(GL_NONE);
}

void RenderConeTintMesh(const glm::mat4& modelToWorldMatrix, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
{
	glUseProgram(gUniformColorTintProgram->GetProgramID());
	glUniformMatrix4fv(gUniformColorTintProgram->GetModelToWorldMatrixUniform(), 1, GL_FALSE, glm::value_ptr(modelToWorldMatrix));
	glUniform4f(gUniformColorTintProgram->GetBaseColorUniform(), r, g, b, a);
	gConeTintMesh->Render();
	glUseProgram(GL_NONE);
}

void DrawGroundPlane(glutil::MatrixStack& modelMatrixStack)
{
	glutil::PushStack push(modelMatrixStack);
	modelMatrixStack.Scale(glm::vec3(100.0f, 1.0f, 100.0f));
	RenderPlane(modelMatrixStack.Top());
}

void DrawParthenonBase(glutil::MatrixStack& modelMatrixStack)
{
	glutil::PushStack push(modelMatrixStack);
	modelMatrixStack.Scale(glm::vec3(PARTHENON_WIDTH, 1.0f, PARTHENON_LENGTH));
	modelMatrixStack.Translate(glm::vec3(0.0f, 0.5f, 0.0f));
	RenderCubeTintMesh(modelMatrixStack.Top());
}

void RenderCube(const glm::mat4& modelToWorldMatrix)
{
	glUseProgram(gObjectColorProgram->GetProgramID());
	glUniformMatrix4fv(gObjectColorProgram->GetModelToWorldMatrixUniform(), 1, GL_FALSE, glm::value_ptr(modelToWorldMatrix));
	gCubeColorMesh->Render();
	glUseProgram(GL_NONE);
}

void DrawParthenonInterior(glutil::MatrixStack& modelMatrixStack)
{
	glutil::PushStack push(modelMatrixStack);
	modelMatrixStack.Translate(glm::vec3(0.0f, PARTHENON_BASE_HEIGHT, 0.0f));
	modelMatrixStack.Scale(glm::vec3(PARTHENON_INTERIOR_WIDTH, PARTHENON_COLUMN_HEIGHT, PARTHENON_INTERIOR_LENGTH));
	modelMatrixStack.Translate(glm::vec3(0.0f, 0.5f, 0.0f));
	RenderCube(modelMatrixStack.Top());
}

void DrawColumn(glutil::MatrixStack& modelMatrixStack)
{
	// draw column base
	{
		glutil::PushStack push(modelMatrixStack);
		modelMatrixStack.Translate(0.0f, 0.0f, 0.0f);
		modelMatrixStack.Scale(glm::vec3(PARTHENON_COLUMN_BASE_WIDTH, PARTHENON_COLUMN_BASE_HEIGHT, PARTHENON_COLUMN_BASE_LENGTH));
		modelMatrixStack.Translate(0.0f, 0.5f, 0.0f);
		RenderCubeTintMesh(modelMatrixStack.Top());
	}
	
	// draw column
	{
		glutil::PushStack push(modelMatrixStack);
		modelMatrixStack.Translate(0.0f, PARTHENON_COLUMN_BASE_HEIGHT, 0.0f);
		modelMatrixStack.Scale(glm::vec3(0.8f, PARTHENON_COLUMN_HEIGHT - 2 * PARTHENON_COLUMN_BASE_HEIGHT, 0.8f));
		modelMatrixStack.Translate(0.0f, 0.5f, 0.0f);
		RenderCylinderTintMesh(modelMatrixStack.Top(), 0.9f, 0.9f, 0.9f, 0.9f);
	}

	// draw column top
	{
		glutil::PushStack push(modelMatrixStack);
		modelMatrixStack.Translate(0.0f, PARTHENON_COLUMN_HEIGHT - PARTHENON_COLUMN_BASE_HEIGHT, 0.0f);
		modelMatrixStack.Scale(glm::vec3(PARTHENON_COLUMN_BASE_WIDTH, PARTHENON_COLUMN_BASE_HEIGHT, PARTHENON_COLUMN_BASE_LENGTH));
		modelMatrixStack.Translate(0.0f, 0.5f, 0.0f);
		RenderCubeTintMesh(modelMatrixStack.Top());
	}
}

void DrawLeftAndRightSidesColumns(glutil::MatrixStack& modelMatrixStack)
{
	const float COLUMN_POS_X = (PARTHENON_WIDTH / 2.0f) - PARTHENON_COLUMN_MARGIN_OFFSET;
	const int COLUMNS_COUNT = static_cast<int>(PARTHENON_LENGTH / 2.0f);
	float columnPosZ = (PARTHENON_LENGTH / 2.0f) - PARTHENON_COLUMN_MARGIN_OFFSET;
	for (int i = 0; i < COLUMNS_COUNT; ++i)
	{
		{
			glutil::PushStack push(modelMatrixStack);
			modelMatrixStack.Translate(glm::vec3(COLUMN_POS_X, PARTHENON_BASE_HEIGHT, columnPosZ));
			DrawColumn(modelMatrixStack);
		}
		{
			glutil::PushStack push(modelMatrixStack);
			modelMatrixStack.Translate(glm::vec3(-COLUMN_POS_X, PARTHENON_BASE_HEIGHT, columnPosZ));
			DrawColumn(modelMatrixStack);
		}
		columnPosZ -= PARTHENON_COLUMN_SPACING;
	}
}

void DrawFrontAndBackSidesColumns(glutil::MatrixStack& modelMatrixStack)
{
	const float COLUMN_POS_Z = (PARTHENON_LENGTH / 2.0f) - PARTHENON_COLUMN_MARGIN_OFFSET;
	int COLUMNS_COUNT = static_cast<int>(PARTHENON_WIDTH / 2.0f);
	COLUMNS_COUNT -= 2; // don't draw the margin columns
	float columnPosX = (PARTHENON_WIDTH / 2.0f) - PARTHENON_COLUMN_MARGIN_OFFSET;
	columnPosX -= PARTHENON_COLUMN_SPACING + PARTHENON_COLUMN_BASE_WIDTH - PARTHENON_COLUMN_MARGIN_OFFSET;
	for (int i = 0; i < COLUMNS_COUNT; ++i)
	{
		{
			glutil::PushStack push(modelMatrixStack);
			modelMatrixStack.Translate(glm::vec3(columnPosX, PARTHENON_BASE_HEIGHT, COLUMN_POS_Z));
			DrawColumn(modelMatrixStack);
		}
		{
			glutil::PushStack push(modelMatrixStack);
			modelMatrixStack.Translate(glm::vec3(columnPosX, PARTHENON_BASE_HEIGHT, -COLUMN_POS_Z));
			DrawColumn(modelMatrixStack);
		}
		columnPosX -= PARTHENON_COLUMN_SPACING;
	}
}

void DrawColumns(glutil::MatrixStack& modelMatrixStack)
{
	DrawLeftAndRightSidesColumns(modelMatrixStack);
	DrawFrontAndBackSidesColumns(modelMatrixStack);
}

void RenderParthenonTop(const glm::mat4& modelToWorldTransform)
{
	glUseProgram(gUniformColorTintProgram->GetProgramID());
	glUniformMatrix4fv(gUniformColorTintProgram->GetModelToWorldMatrixUniform(), 1, GL_FALSE, glm::value_ptr(modelToWorldTransform));
	glUniform4f(gUniformColorTintProgram->GetBaseColorUniform(), 0.9f, 0.9f, 0.9f, 0.9f);
	gCubeTintMesh->Render();
	glUseProgram(GL_NONE);
}

void DrawParthenonTop(glutil::MatrixStack& modelMatrixStack)
{
	glutil::PushStack push(modelMatrixStack);
	modelMatrixStack.Translate(glm::vec3(0.0f, PARTHENON_BASE_HEIGHT + PARTHENON_COLUMN_HEIGHT, 0.0f));
	modelMatrixStack.Scale(glm::vec3(PARTHENON_WIDTH, 1.0f, PARTHENON_LENGTH));
	modelMatrixStack.Translate(glm::vec3(0.0f, 0.5f, 0.0f));
	RenderParthenonTop(modelMatrixStack.Top());
}

void DrawParthenon(glutil::MatrixStack& modelMatrixStack)
{
	glutil::PushStack push(modelMatrixStack);
	modelMatrixStack.Translate(glm::vec3(PARTHENON_POS_X, 0.0f, PARTHENON_POS_Z));
	DrawParthenonBase(modelMatrixStack);
	DrawParthenonInterior(modelMatrixStack);
	DrawColumns(modelMatrixStack);
	DrawParthenonTop(modelMatrixStack);
}

void DrawTree(glutil::MatrixStack& modelMatrixStack)
{
	const float TREE_WIDTH = 1.25f;
	const float TREE_HEIGHT = 2.0f;
	// draw the tree trunk
	{
		glutil::PushStack push(modelMatrixStack);
		modelMatrixStack.Scale(glm::vec3(TREE_WIDTH, TREE_HEIGHT, TREE_WIDTH));
		modelMatrixStack.Translate(glm::vec3(0.0f, 0.5f, 0.0f));
		RenderCylinderTintMesh(modelMatrixStack.Top(), 0.694f, 0.4f, 0.106f, 1.0f);
	}

	// draw the tree top
	{
		const float TREE_TOP_WIDTH = 3.15f;
		const float TREE_TOP_HEIGHT = 3.25f;
		glutil::PushStack push(modelMatrixStack);
		modelMatrixStack.Translate(glm::vec3(0.0f, TREE_HEIGHT, 0.0f));
		modelMatrixStack.Scale(glm::vec3(TREE_TOP_WIDTH, TREE_TOP_HEIGHT, TREE_TOP_WIDTH));
		RenderConeTintMesh(modelMatrixStack.Top(), 0.0f, 1.0f, 0.0f, 1.0f);
	}
}

void DrawEntranceForest(glutil::MatrixStack& modelMatrixStack)
{
	const float ENTRANCE_FOREST_POS_LEFT_X = (PARTHENON_POS_X - PARTHENON_INTERIOR_WIDTH / 2.0f);
	const float ENTRANCE_FOREST_POS_RIGHT_X = (PARTHENON_POS_X + PARTHENON_INTERIOR_WIDTH / 2.0f);

	const float TREE_SPACING = 4.0f;
	const float ENTRANCE_FOREST_POS_START_Z = (PARTHENON_POS_Z + PARTHENON_LENGTH / 2.0f) + TREE_SPACING;

	const int TREE_COUNT = 10;
	float treePosZ = ENTRANCE_FOREST_POS_START_Z;
	for (int i = 0; i < TREE_COUNT; ++i)
	{
		{
			glutil::PushStack push(modelMatrixStack);
			modelMatrixStack.Translate(glm::vec3(ENTRANCE_FOREST_POS_LEFT_X, 0.0f, treePosZ));
			DrawTree(modelMatrixStack);
		}
		{
			glutil::PushStack push(modelMatrixStack);
			modelMatrixStack.Translate(glm::vec3(ENTRANCE_FOREST_POS_RIGHT_X, 0.0f, treePosZ));
			DrawTree(modelMatrixStack);
		}
		treePosZ += TREE_SPACING;
	}
}

void RenderLookAtPoint(const glm::mat4& modelToWorldMatrix)
{
	glUseProgram(gObjectColorProgram->GetProgramID());
	glUniformMatrix4fv(gObjectColorProgram->GetModelToWorldMatrixUniform(), 1, GL_FALSE, glm::value_ptr(modelToWorldMatrix));
	gCubeColorMesh->Render();
	glUseProgram(GL_NONE);
}

void DrawLookAtPoint(glutil::MatrixStack& modelMatrixStack, const glm::vec3& cameraPosition)
{
	if (gDrawLookAtPoint)
	{
		glDisable(GL_DEPTH_TEST);

		glutil::PushStack push(modelMatrixStack);

		const glm::vec3 cameraAimVector = gCameraTarget - cameraPosition;
		modelMatrixStack.Translate(gCameraTarget);
		modelMatrixStack.Scale(1.0f, 1.0f, 1.0f);

		RenderLookAtPoint(modelMatrixStack.Top());

		glEnable(GL_DEPTH_TEST);
	}
}

void DrawWorld()
{
	const glm::vec3& cameraPosition = ResolveCameraPosition();

	glutil::MatrixStack cameraMatrixStack;
	cameraMatrixStack.SetMatrix(CalculateLookAtMatrix(cameraPosition, gCameraTarget, glm::vec3(0.0f, 1.0f, 0.0f)));

	SetWorldToCameraMatrix(cameraMatrixStack.Top());

	glutil::MatrixStack modelMatrixStack;
	DrawGroundPlane(modelMatrixStack);
	DrawParthenon(modelMatrixStack);
	DrawEntranceForest(modelMatrixStack);
	DrawLookAtPoint(modelMatrixStack, cameraPosition);
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawWorld();

	glutSwapBuffers();
}

void OnViewportResized(const float aspectRatio)
{
	glutil::MatrixStack perspectiveMatrix;
	perspectiveMatrix.Perspective(45.0f, aspectRatio, 1.0f, 1000.0f);
	
	SetCameraToClipMatrix(perspectiveMatrix.Top());
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	
	OnViewportResized(float(w) / float(h));

	glutPostRedisplay();
}

const float CAMERA_TARGET_MOVE_INC = 4.0f;
const float CAMERA_POS_X_INC = 10.0f;
const float CAMERA_POS_Y_INC = 10.0f;
const float CAMERA_POS_Z_INC = 5.0f;

const float SHIFT_MODIFIER = 1 / 20.0f;

void ClampCameraPosition()
{
	gCameraPositionInRelativeSphere.x = (gCameraPositionInRelativeSphere.x <= 360.0f ? gCameraPositionInRelativeSphere.x :
		gCameraPositionInRelativeSphere.x - static_cast<int>(gCameraPositionInRelativeSphere.x / 360.0f) * 360.0f);
	gCameraPositionInRelativeSphere.y = glm::clamp(gCameraPositionInRelativeSphere.y, -85.0f, 0.0f);
	gCameraPositionInRelativeSphere.z = (gCameraPositionInRelativeSphere.z > 5.0f ? gCameraPositionInRelativeSphere.z : 5.0f);
}

glm::vec4 gCameraTargetDelta(0.0f, 0.0f, 0.0f, 0.0f);
void UpdateCameraTarget()
{
	if (gCameraTargetDelta.length() != 0.0f)
	{
		static glutil::MatrixStack matrixStack;

		glutil::PushStack push(matrixStack);
		matrixStack.RotateY(-gCameraPositionInRelativeSphere.x);
		gCameraTargetDelta = matrixStack.Top() * gCameraTargetDelta;

		gCameraTarget.x += gCameraTargetDelta.x;
		gCameraTarget.y += gCameraTargetDelta.y;
		gCameraTarget.z += gCameraTargetDelta.z;
		gCameraTargetDelta.x = 0.0f;
		gCameraTargetDelta.y = 0.0f;
		gCameraTargetDelta.z = 0.0f;
	}
}

void keyboard(unsigned char key, int x, int y)
{
	bool redraw = true;
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		redraw = false;
		break;

	case ' ': gDrawLookAtPoint = !gDrawLookAtPoint; break;

	case 'w': gCameraTargetDelta.x = -CAMERA_TARGET_MOVE_INC; break;
	case 's': gCameraTargetDelta.x =  CAMERA_TARGET_MOVE_INC; break;
	case 'd': gCameraTargetDelta.z = -CAMERA_TARGET_MOVE_INC; break;
	case 'a': gCameraTargetDelta.z =  CAMERA_TARGET_MOVE_INC; break;
	case 'e': gCameraTargetDelta.y =  CAMERA_TARGET_MOVE_INC; break;
	case 'q': gCameraTargetDelta.y = -CAMERA_TARGET_MOVE_INC; break;

	case 'W': gCameraTargetDelta.x = -CAMERA_TARGET_MOVE_INC * SHIFT_MODIFIER; break;
	case 'S': gCameraTargetDelta.x =  CAMERA_TARGET_MOVE_INC * SHIFT_MODIFIER; break;
	case 'D': gCameraTargetDelta.z = -CAMERA_TARGET_MOVE_INC * SHIFT_MODIFIER; break;
	case 'A': gCameraTargetDelta.z =  CAMERA_TARGET_MOVE_INC * SHIFT_MODIFIER; break;
	case 'E': gCameraTargetDelta.y =  CAMERA_TARGET_MOVE_INC * SHIFT_MODIFIER; break;
	case 'Q': gCameraTargetDelta.y = -CAMERA_TARGET_MOVE_INC * SHIFT_MODIFIER; break;

	case 'i': gCameraPositionInRelativeSphere.y -= CAMERA_POS_Y_INC; break;
	case 'k': gCameraPositionInRelativeSphere.y += CAMERA_POS_Y_INC; break;
	case 'l': gCameraPositionInRelativeSphere.x -= CAMERA_POS_X_INC; break;
	case 'j': gCameraPositionInRelativeSphere.x += CAMERA_POS_X_INC; break;
	case 'o': gCameraPositionInRelativeSphere.z -= CAMERA_POS_Z_INC; break;
	case 'u': gCameraPositionInRelativeSphere.z += CAMERA_POS_Z_INC; break;

	case 'I': gCameraPositionInRelativeSphere.y -= CAMERA_POS_Y_INC * SHIFT_MODIFIER; break;
	case 'K': gCameraPositionInRelativeSphere.y += CAMERA_POS_Y_INC * SHIFT_MODIFIER; break;
	case 'L': gCameraPositionInRelativeSphere.x -= CAMERA_POS_X_INC * SHIFT_MODIFIER; break;
	case 'J': gCameraPositionInRelativeSphere.x += CAMERA_POS_X_INC * SHIFT_MODIFIER; break;
	case 'O': gCameraPositionInRelativeSphere.z -= CAMERA_POS_Z_INC * SHIFT_MODIFIER; break;
	case 'U': gCameraPositionInRelativeSphere.z += CAMERA_POS_Z_INC * SHIFT_MODIFIER; break;

	default:
		redraw = false;
		break;
	}

	if (redraw)
	{
		ClampCameraPosition();
		UpdateCameraTarget();

		glutPostRedisplay();
	}
}

unsigned int defaults(unsigned int displayMode, int &width, int &height)
{
	width = 800;
	return displayMode;
}

