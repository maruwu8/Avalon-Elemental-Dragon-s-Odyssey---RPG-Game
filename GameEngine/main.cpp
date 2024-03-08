#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw.h"
#include "imgui\imgui_impl_opengl3.h"

#include "Graphics\window.h"
#include "Camera\camera.h"
#include "Shaders\shader.h"
#include "Model Loading\mesh.h"
#include "Model Loading\texture.h"
#include "Model Loading\meshLoaderObj.h"
#include <stdio.h>
#include <Windows.h>


void processKeyboardInput();

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

Window window("Game Engine", 1000, 1000);
Camera camera;

glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPos = glm::vec3(-200.0f, 200.0f, -200.0f);
glm::vec3 cameraPosition;

std::vector<glm::vec3> bullets;


bool mainQuest = false;
bool fireQuest = false;
bool waterQuest = false;
bool airQuest = false;
bool finalQuest = false;
bool prisonQuest = false;
bool burnDoor = false;
bool talkFairy1 = false;
bool talkFairy2 = false;
bool torch1Completed = false;
bool torch2Completed = false;
bool torch3Completed = false;
bool torch4Completed = false;
bool fireElement = false;
bool waterElement = false;
bool airElement = false;
bool natureElement = false;
bool tornadoHit = false;
bool wizardCollision = false;

struct FireBreath {
	glm::vec3 position;
	float lifetime;  // Lifetime of the fire breath in seconds
};
const float breathLifetime = 1.0f;
std::vector<FireBreath> breaths;  // Vector to store the fire breaths


void RenderQuestUI()
{

	// Inside the game loop, after starting ImGui frame
	ImGui::Begin("Quests");

	// Main Quest
	ImGui::TextColored(ImVec4(0.5f, 0.0f, 0.5f, 1.0f), "Main Quest: Help the fairy and defeat the Evil Wizard!");
	ImGui::TextColored(ImVec4(0.5f, 0.0f, 0.5f, 1.0f), "Status: %s", mainQuest ? "Completed" : "Not Completed");
	if (ImGui::IsItemHovered())
	{
		if(!airQuest)
			ImGui::SetTooltip("Explore the elemental realms and find the Wizard!");
		else
			ImGui::SetTooltip("Restore the balance, talk with the fairy again!");
	}
		
	// Sub-quests
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Sub-quests:");

	// Prison Quest
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "	Prison Quest: %s", prisonQuest ? "Completed" : "Not Completed");
	if (ImGui::IsItemHovered())
	{
		if (burnDoor)
		{
			if(!talkFairy1)
				ImGui::SetTooltip("Maybe the fairy can tell us something!");
			else
			{
				if(!fireElement)
					ImGui::SetTooltip("Grab the fire magic and go through the portal!");
				else
					ImGui::SetTooltip("All done here Avalon!");
			}
				
		}
			
		else
				ImGui::SetTooltip("Escape the prison, try to attack the door!");
		}
			
	
	// Fire Element Quest
	if (prisonQuest)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "	Fire Quest: %s", fireQuest ? "Completed" : "Not Completed");
		if (ImGui::IsItemHovered())
		{
			if(!waterElement)
				ImGui::SetTooltip("Light the torches and gather new powers!");
			else
				ImGui::SetTooltip("All done here Avalon!");
		}
			
	}
	
	// Water Element Quest
	if (fireQuest)
	{
		ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "	Water Quest: %s", waterQuest? "Completed" : "Not Completed");
		if (ImGui::IsItemHovered())
		{
			if (!airElement)
				ImGui::SetTooltip("Place the Aqua Egg on the pedestal!");
			else
				ImGui::SetTooltip("All done here Avalon!");
		}
	}
	
	// Air Element Quest
	if (waterQuest)
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "	Air Quest: %s", airQuest ? "Completed" : "Not Completed");
		if (ImGui::IsItemHovered())
		{
			if (!natureElement)
				ImGui::SetTooltip("Fend off the Tornado that turned the city to ruins!");
			else
				ImGui::SetTooltip("All done here Avalon!");
		}
	}
	if (wizardCollision)
	{
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Avalon fell into the trap of the fairies!");

	}
		


	
	//make text bigger
	ImGui::SetWindowFontScale(1.2f);
	ImGui::SetWindowSize(ImVec2(1000, 400));

	if (ImGui::Button("Quit Game (Weakling)"))
	{
		glfwSetWindowShouldClose(window.getWindow(), true);
	}

	// End ImGui window
	ImGui::End();
}


int main()
{
	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	const char* fontPath = "imgui/da.ttf";
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(fontPath, 24);
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");


	glClearColor(0.2f, 0.8f, 1.0f, 1.0f);

	//building and compiling shader program
	Shader shader("Shaders/vertex_shader.glsl", "Shaders/fragment_shader.glsl");
	Shader sunShader("Shaders/sun_vertex_shader.glsl", "Shaders/sun_fragment_shader.glsl");

	//Textures
	GLuint tex = loadBMP("Resources/Textures/wood.bmp");
	GLuint tex2 = loadBMP("Resources/Textures/rock.bmp");

	GLuint tex3 = loadBMP("Resources/Textures/stone.bmp");
	GLuint tex4 = loadBMP("Resources/Textures/terrain.bmp");
	GLuint tex5 = loadBMP("Resources/Textures/tornado.bmp");
	GLuint tex6 = loadBMP("Resources/Textures/waterIsland2.bmp");

	GLuint skyboxTexture = loadBMP("Resources/Textures/outerspace.bmp");
	GLuint fairyTexture = loadBMP("Resources/Textures/fairyother_hr-light.bmp");
	GLuint dungeonSkyboxTexture = loadBMP("Resources/Textures/dungeonsky.bmp");
	//GLuint fireTorchTexture = loadBMP("Resources/Textures/fireTorch.bmp");
	GLuint metalTexture = loadBMP("Resources/Textures/metal.bmp");
	GLuint magmaTexture = loadBMP("Resources/Textures/magma.bmp");
	GLuint waterPondTexture = loadBMP("Resources/Textures/waterPond.bmp");
	GLuint dragonEggTexture = loadBMP("Resources/Textures/dragonegg.bmp");
	GLuint tornadoTexture = loadBMP("Resources/Textures/storm-clouds.bmp");
	GLuint natureTexture = loadBMP("Resources/Textures/nature.bmp");
	GLuint dragonHornsTexture = loadBMP("Resources/Textures/dragonegg.bmp");
	GLuint wizardTexture = loadBMP("Resources/Textures/wizard.bmp");
	GLuint lastPlane = loadBMP("Resources/Textures/void.bmp");

	glEnable(GL_DEPTH_TEST);

	

	std::vector<int> ind = { 0, 1, 3,
		1, 2, 3 };

	std::vector<Texture> textures;
	textures.push_back(Texture());
	textures[0].id = tex;
	textures[0].type = "texture_diffuse";

	std::vector<Texture> textures1;
	textures1.push_back(Texture());
	textures1[0].id = skyboxTexture;
	textures1[0].type = "texture_diffuse";

	std::vector<Texture> textures2;
	textures2.push_back(Texture());
	textures2[0].id = tex2;
	textures2[0].type = "texture_diffuse";

	std::vector<Texture> textures3;
	textures3.push_back(Texture());
	textures3[0].id = tex3;
	textures3[0].type = "texture_diffuse";

	std::vector<Texture> textures4;
	textures4.push_back(Texture());
	textures4[0].id = tex4;
	textures4[0].type = "texture_diffuse";

	std::vector<Texture> textures5;
	textures5.push_back(Texture());
	textures5[0].id = tex5;
	textures5[0].type = "texture_diffuse";

	std::vector<Texture> textures6;
	textures6.push_back(Texture());
	textures6[0].id = tex6;
	textures6[0].type = "texture_diffuse";

	std::vector<Texture> textures7;
	textures7.push_back(Texture());
	textures7[0].id = fairyTexture;
	textures7[0].type = "texture_diffuse";

	std::vector<Texture> textures8;
	textures8.push_back(Texture());
	textures8[0].id = dungeonSkyboxTexture;
	textures8[0].type = "texture_diffuse";

	std::vector<Texture> textures9;
	textures9.push_back(Texture());
	textures9[0].id = magmaTexture;
	textures9[0].type = "texture_diffuse";

	std::vector<Texture> textures10;
	textures10.push_back(Texture());
	textures10[0].id = metalTexture;
	textures10[0].type = "texture_diffuse";

	std::vector<Texture> textures11;
	textures11.push_back(Texture());
	textures11[0].id = waterPondTexture;
	textures11[0].type = "texture_diffuse";

	std::vector<Texture> textures12;
	textures12.push_back(Texture());
	textures12[0].id = dragonEggTexture;
	textures12[0].type = "texture_diffuse";

	std::vector<Texture> textures14;
	textures14.push_back(Texture());
	textures14[0].id = tornadoTexture;
	textures14[0].type = "texture_diffuse";

	std::vector<Texture> textures15;
	textures15.push_back(Texture());
	textures15[0].id = natureTexture;
	textures15[0].type = "texture_diffuse";

	std::vector<Texture> textures16;
	textures16.push_back(Texture());
	textures16[0].id = lastPlane;
	textures16[0].type = "texture_diffuse";

	std::vector<Texture> textures17;
	textures17.push_back(Texture());
	textures17[0].id = wizardTexture;
	textures17[0].type = "texture_diffuse";
	

	MeshLoaderObj loader;
	Mesh sun = loader.loadObj("Resources/Models/sphere.obj");
	Mesh box = loader.loadObj("Resources/Models/cube.obj", textures);

	Mesh waterSphere = loader.loadObj("Resources/Models/sphere.obj", textures6);
	Mesh fireSphere = loader.loadObj("Resources/Models/sphere.obj", textures4);
	Mesh airSphere = loader.loadObj("Resources/Models/sphere.obj", textures5);

	Mesh planePrison = loader.loadObj("Resources/Models/ISLAND.obj", textures3);
	Mesh planeFire = loader.loadObj("Resources/Models/ISLAND.obj", textures4);
	Mesh planeAir = loader.loadObj("Resources/Models/ISLAND.obj", textures5);
	Mesh planeWater = loader.loadObj("Resources/Models/ISLAND.obj", textures6);
	Mesh natureSphere = loader.loadObj("Resources/Models/sphere.obj", textures15);
	Mesh skyboxSphere = loader.loadObj("Resources/Models/sphere.obj", textures1);

	Mesh castleMesh = loader.loadObj("Resources/Models/castle_dungeon.obj", textures3);
	Mesh fairy = loader.loadObj("Resources/Models/fairyother_hr-light.obj", textures7);
	Mesh dungeonTop = loader.loadObj("Resources/Models/plane.obj", textures3);
	Mesh castleMeshWihoutDoor = loader.loadObj("Resources/Models/castle_dungeon_without_door.obj", textures3);
	Mesh portal = loader.loadObj("Resources/Models/portal.obj", textures3);
	Mesh portalFire = loader.loadObj("Resources/Models/portal.obj", textures4);
	Mesh torch = loader.loadObj("Resources/Models/torch.obj", textures10);
	Mesh magmaBall = loader.loadObj("Resources/Models/magmaBall.obj", textures9);
	Mesh portalWater = loader.loadObj("Resources/Models/portal.obj", textures6);
	Mesh waterPond = loader.loadObj("Resources/Models/waterPond.obj", textures11);
	Mesh bridge = loader.loadObj("Resources/Models/bridge.obj", textures);
	Mesh dragonEgg = loader.loadObj("Resources/Models/dragonegg.obj", textures12);
	Mesh lever = loader.loadObj("Resources/Models/lever.obj", textures10);
	Mesh lever2= loader.loadObj("Resources/Models/lever2.obj", textures10);
	Mesh pedestal = loader.loadObj("Resources/Models/pedestal.obj", textures3);
	Mesh portalAir= loader.loadObj("Resources/Models/portal.obj", textures5);
	Mesh tornado = loader.loadObj("Resources/Models/tornado.obj", textures14);
	Mesh ruined_building = loader.loadObj("Resources/Models/ruined_building.obj", textures3);
	Mesh portalNature = loader.loadObj("Resources/Models/portal.obj", textures15);
	Mesh wizard = loader.loadObj("Resources/Models/wizard.obj", textures17);
	Mesh lastPlaneMesh = loader.loadObj("Resources/Models/ISLAND.obj", textures16);
	Mesh lastPortal = loader.loadObj("Resources/Models/portal.obj", textures16);

	PlaySound(TEXT("Resources/Sounds/pokemon.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);

	bool openedDoor = false;
	bool openedPrisonPortal = false;  
	bool torch1 = false;
	bool torch2 = false;
	bool torch3 = false;
	bool torch4 = false;
	bool leverPulled = false;
	bool eggPicked = false;
	bool onPedestal = false;
	


	//check if we close the window or press the escape button
	while (!window.isPressed(GLFW_KEY_ESCAPE) &&
		glfwWindowShouldClose(window.getWindow()) == 0)
	{

		window.clear();
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		cameraPosition = camera.getCameraPosition();

		processKeyboardInput();

		//test mouse input
		if (window.isMousePressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			std::cout << "Pressing mouse button" << std::endl;
		}
		//// Code for the light ////

		sunShader.use();

		glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
		glm::mat4 ViewMatrix = glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());
		GLuint MatrixID = glGetUniformLocation(sunShader.getId(), "MVP");

		//Test for one Obj loading = light source

		glm::mat4 ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, lightPos);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		sun.draw(sunShader);

		//// End code for the light ////


		shader.use();

		///// Test Obj files for box ////

		GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


		///// Test plane Obj file //////

		float planeSize = 5.0f; 
		float scaleValue = 100.0f; 
		float castleScale = 40.0f;

		// Render the first plane
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-planeSize, -50.0f, -planeSize));
		//ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaleValue));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		planePrison.draw(shader);

		

		if (window.isPressed(GLFW_KEY_E)) 
		{
			// Get camera position
			cameraPosition = camera.getCameraPosition();

			// Check if camera is around the specified positions
			if ((cameraPosition.x >= 1280.0f && cameraPosition.x <= 1305.0f) &&
				(cameraPosition.z >= -160.0f && cameraPosition.z <= -95.0f)) 
			{
				openedDoor = true;
				burnDoor = true;

			}
		}

		if (openedDoor) 
		{
			// Render the castle without the door
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1282.9f, 0.0f, -59.3588f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(castleScale));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			castleMeshWihoutDoor.draw(shader);
		}
		else 
		{
			// Render the castle with the door
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1282.9f, 0.0f, -59.3588f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(castleScale));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			castleMesh.draw(shader);

			if (!((cameraPosition.x >= 1204.0f && cameraPosition.x <= 1305.0f) &&
				(cameraPosition.z >= -170.0f && cameraPosition.z <= 50.0f)))
			{
				camera.setCameraPosition(glm::vec3(1282.9f, 2.0f, -59.3588f));
			}

		}



		// Position and render the dungeonTop on top of the castleMesh
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1282.9f, 57.3f, -59.3588f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		dungeonTop.draw(shader);

		// render fairy
		if (!airQuest)
		{
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1368.42f, -2.0f, -22.0093f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.22f));
			//rotate 90 degrees
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			fairy.draw(shader);

			// Check if camera is close to the fairy
			//if we press E while we are close to the fairy
			if (window.isPressed(GLFW_KEY_E) && talkFairy1==false)
			{
				if ((cameraPosition.x >= 1360.0f && cameraPosition.x <= 1400.0f) &&
					(cameraPosition.z >= -13.0f && cameraPosition.z <= 12.0f)) 
				{
					talkFairy1 = true;
					openedPrisonPortal = true;
				}
			}
		}
		else
		{
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(944.42f, -2.0f, -142.03f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.22f));
			//rotate 90 degrees
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			fairy.draw(shader);

			if (window.isPressed(GLFW_KEY_E) && talkFairy2 == false)
			{
				if ((cameraPosition.x >= 920.0f && cameraPosition.x <= 960.0f) &&
					(cameraPosition.z >= -153.0f && cameraPosition.z <= -120.0f))
					talkFairy2 = true;
			}
		}
		

		if (talkFairy1 == true)
		{
				float rotationSpeedOx = glm::radians(500.0f);
				float rotationSpeedOy = glm::radians(500.0f);
				static float totalRotationOx = 0.0f;
				static float totalRotationOy = 0.0f;
				totalRotationOx += rotationSpeedOx * deltaTime;
				totalRotationOy += rotationSpeedOy * deltaTime;
				ModelMatrix = glm::mat4(1.0f);
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1395.0, 8.0f, -60.0f));  
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3f));
				ModelMatrix = glm::rotate(ModelMatrix, totalRotationOx, glm::vec3(1.0f, 0.0f, 0.0f));// Rotate around the Ox axis
				ModelMatrix = glm::rotate(ModelMatrix, totalRotationOy, glm::vec3(0.0f, 1.0f, 0.0f));// Rotate around the Oy axis
				MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f));
				MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
				glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
				if (fireElement == false)
					fireSphere.draw(shader);

				if (window.isPressed(GLFW_KEY_E) && fireElement==false)
				{
					if ((cameraPosition.x >= 1365.0f && cameraPosition.x <= 1425.0f) &&
						(cameraPosition.z >= -90.0f && cameraPosition.z <= -30.0f)) 
					{
						fireElement = true;
					}
				}
		}

		if (openedPrisonPortal)
		{
			glm::vec3 castlePosition = glm::vec3(1282.9f, 0.0f, -59.3588f);
			glm::vec3 portalOffset = glm::vec3(145.0f, -13.0f, 0.0f);  
			glm::vec3 portalPosition = castlePosition + portalOffset;
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, portalPosition);
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f));  
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			portalFire.draw(shader);

		}
		if (openedPrisonPortal && fireElement==true)
		{
			if (cameraPosition.x <= 1430.59f && cameraPosition.x >= 1422.59f && cameraPosition.z >= -76.5539f && cameraPosition.z <= -45.5539f)
			{
				camera.setCameraPosition(glm::vec3(1286.46f, -52.0f, 1058.3f));
			}
			prisonQuest = true;
		}

		//render fire portal for arrival
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1286.46f, -65.0f, 1065.3f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f)); 
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		if (fireQuest==false)
		portalFire.draw(shader);


		// Render the fire plane
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(429.523f, -100.0f, 1157.24f));  
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		planeFire.draw(shader);

		//render torches on fire plane
		 // Render the first torch
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1567.12f, -80.0f, 1239.21f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(100.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		torch.draw(shader);

		if (window.isPressed(GLFW_KEY_E) && !torch1) 
		{
			if ((cameraPosition.x >= 1567.12f - 20.0f && cameraPosition.x <= 1567.12f + 20.0f) &&
				(cameraPosition.z >= 1239.21f - 20.0f && cameraPosition.z <= 1239.21f + 20.0f))
			{
				torch1 = true;
				torch1Completed = true;

			}
		}

		if (torch1)
		{
			// Render the first magmaBall on top of the first torchs
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1566.0f, -17.0f, 1248.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			magmaBall.draw(shader);
		}



		// Render torch object at the second position
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1395.38f, -80.0f, 1161.92f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(100.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		torch.draw(shader);

		if (window.isPressed(GLFW_KEY_E) && !torch2) 
		{
			if ((cameraPosition.x >= 1395.38f - 20.0f && cameraPosition.x <= 1395.38f + 20.0f) &&
				(cameraPosition.z >= 1161.92f - 20.0f && cameraPosition.z <= 1161.92f + 20.0f))
			{
				torch2 = true;
				torch2Completed = true;

			}
		}
		if (torch2)
		{
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1393.5f, -17.0f, 1172.1f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			magmaBall.draw(shader);
		}


		// Render torch object at the third position
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1516.34f, -80.0f, 995.564f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(100.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		torch.draw(shader);

		if (window.isPressed(GLFW_KEY_E) && !torch3) 
		{
			if ((cameraPosition.x >= 1516.34f - 20.0f && cameraPosition.x <= 1516.34f + 20.0f) &&
				(cameraPosition.z >= 995.564f - 20.0f && cameraPosition.z <= 995.564f + 20.0f))
			{
				torch3 = true;
				torch3Completed = true;

			}
		}
		if (torch3)
		{
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1515.0f, -17.0f, 1005.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			magmaBall.draw(shader);
		}





		// Render torch object at the fourth position
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1731.07f, -80.0f, 1031.19f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(100.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		torch.draw(shader);

		if (window.isPressed(GLFW_KEY_E) && !torch4) 
		{
			if ((cameraPosition.x >= 1731.07f - 20.0f && cameraPosition.x <= 1731.07f + 20.0f) &&
				(cameraPosition.z >= 1031.19f - 20.0f && cameraPosition.z <= 1031.19f + 20.0f))
			{
				torch4 = true;
				torch4Completed = true;
			}
		}
		if (torch4)
		{
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1729.3f, -17.0f, 1040.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			magmaBall.draw(shader);
		}

		if (torch1 && torch2 && torch3 && torch4)
		{
			fireQuest = true;
			// Render the portal to the Water Realm
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1286.46f, -65.0f, 1065.3f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f));  // Adjust the scale accordingly
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			portalWater.draw(shader);

			cameraPosition = camera.getCameraPosition();

			if ((cameraPosition.x >= 1304.18f - 20.0f && cameraPosition.x <= 1304.18f + 20.0f) &&
				(cameraPosition.z >= 1075.98f - 20.0f && cameraPosition.z <= 1075.98f + 20.0f) && waterElement==true)
			{
				camera.setCameraPosition(glm::vec3(573.35f, 70.0f, 445.89f));
			}

		}

		//get next element
		if (fireQuest==true)
		{
			float rotationSpeedOx = glm::radians(500.0f);
			float rotationSpeedOy = glm::radians(500.0f);

			// Accumulate rotation angles over time
			static float totalRotationOx = 0.0f;
			static float totalRotationOy = 0.0f;

			totalRotationOx += rotationSpeedOx * deltaTime;
			totalRotationOy += rotationSpeedOy * deltaTime;

			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1359.0, -45.0f, 1074.95));  // Adjust the position as needed
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3f));
			ModelMatrix = glm::rotate(ModelMatrix, totalRotationOx, glm::vec3(1.0f, 0.0f, 0.0f));// Rotate around the Ox axis
			ModelMatrix = glm::rotate(ModelMatrix, totalRotationOy, glm::vec3(0.0f, 1.0f, 0.0f));// Rotate around the Oy axis
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			if (!waterElement)
				waterSphere.draw(shader);

			if (window.isPressed(GLFW_KEY_E) && !waterElement)
			{
				 cameraPosition = camera.getCameraPosition();
				if ((cameraPosition.x >= 1345.0f && cameraPosition.x <= 1405.0f) &&
					(cameraPosition.z >= 1050.0f && cameraPosition.z <= 1100.0f)) {
					waterElement = true;
				}
			}
		}

		//Water Level

		//render water arrival portal
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(578.878f, 38.0f, 432.395f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f)); 
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		if(waterQuest==false)
		portalWater.draw(shader);

		// Render the water plane
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-858.136f, 0.0f, 522.456f)); 
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		planeWater.draw(shader);

		//render water pond
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(420.247f, 33.5319f, 500.962f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(21.0f,30.0f,21.0f));  
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		waterPond.draw(shader);

		cameraPosition = camera.getCameraPosition();
		if ((cameraPosition.x >= 255.0f && cameraPosition.x <= 530.0f) &&
			(cameraPosition.z >= 375.480f && cameraPosition.z <= 540.0f) && leverPulled==false)
		{
			camera.setCameraPosition(glm::vec3(573.35f, 70.0f, 445.89f));
		}

		//load bridge
		if (leverPulled)
		{
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(380.247f, 68.5319f, 440.962f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(32.0f));  
			//ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			bridge.draw(shader);
		}


		//load egg
		if (!onPedestal)
		{
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(557.539f, 40.6704f, 480.358f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3000.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			if (eggPicked == false)
				dragonEgg.draw(shader);
		}
		else
		{
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(208.0f, 90.6704f, 451.358f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3000.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			dragonEgg.draw(shader);
		}
		

		//egg pick
		if (window.isPressed(GLFW_KEY_E) && !eggPicked)
		{
			if ((cameraPosition.x >= 540.0f && cameraPosition.x <= 567.0f) &&
				(cameraPosition.z >= 455.0f && cameraPosition.z <= 505.0f) && eggPicked == false)
				eggPicked = true;
		}


		//load lever
		if (leverPulled)
		{
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(555.539f, 39.300f, 395.358f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(12.0f));  
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			lever.draw(shader);
		}
		else
		{
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(555.539f, 39.300f, 395.358f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(12.0f)); 
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			lever2.draw(shader);
		}
		if (window.isPressed(GLFW_KEY_E) && !leverPulled)
		{
			if ((cameraPosition.x >= 540.0f && cameraPosition.x <= 567.0f) &&
				(cameraPosition.z >= 375.480f && cameraPosition.z <= 415.0f) && leverPulled == false)
				leverPulled = true;
		}
		

		//load pedestal
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(208.601f, 28.169f, 455.456f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(500.0f));  // Adjust the scale accordingly
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		pedestal.draw(shader);

		if (window.isPressed(GLFW_KEY_E) && !onPedestal)
		{
			cameraPosition = camera.getCameraPosition();
			if ((cameraPosition.x >= 180.0f && cameraPosition.x <= 280.0f) &&
				(cameraPosition.z >=415.0f && cameraPosition.z <= 460.0f) && waterQuest==false) 
			{
				onPedestal = true;
				waterQuest = true;
			}
		}

		//get next element
		if (waterQuest == true)
		{
			float rotationSpeedOx = glm::radians(500.0f);
			float rotationSpeedOy = glm::radians(500.0f);

			// Accumulate rotation angles over time
			static float totalRotationOx = 0.0f;
			static float totalRotationOy = 0.0f;

			totalRotationOx += rotationSpeedOx * deltaTime;
			totalRotationOy += rotationSpeedOy * deltaTime;

			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(544.0, 67.0f, 444.95));  // Adjust the position as needed
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3f));
			ModelMatrix = glm::rotate(ModelMatrix, totalRotationOx, glm::vec3(1.0f, 0.0f, 0.0f));// Rotate around the Ox axis
			ModelMatrix = glm::rotate(ModelMatrix, totalRotationOy, glm::vec3(0.0f, 1.0f, 0.0f));// Rotate around the Oy axis
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			if (airElement == false)
				airSphere.draw(shader);

			if (window.isPressed(GLFW_KEY_E) && airElement==false)
			{
				cameraPosition = camera.getCameraPosition();
				if ((cameraPosition.x >= 514.0f && cameraPosition.x <= 574.0f) &&
					(cameraPosition.z >= 414.0f && cameraPosition.z <= 474.0f)) 
				{
					airElement = true;
				}
			}
		}

		if (waterQuest)
		{
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(578.878f, 38.0f, 432.395f));  
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f)); 
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
				portalAir.draw(shader);

				if ((cameraPosition.x >= 578.0f - 20.0f && cameraPosition.x <= 578.0f + 20.0f) &&
					(cameraPosition.z >=431.98f - 20.0f && cameraPosition.z <= 431.98f + 20.0f) && airElement == true)
				{
					// Teleport the player to the new coordinates
					camera.setCameraPosition(glm::vec3(-156.35f, 155.0f, -790.89f));
				}
		}


		//Air Level
		
		// Render the air plane
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-1087.11, 100.0f, -700.95)); 
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		planeAir.draw(shader);

		//render air portal for arrival on air realm
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-160.11, 133.0f, -790.95)); 
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f));  
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		if (!airQuest)
			portalAir.draw(shader);
		else
			portalNature.draw(shader);
	
		if (!tornadoHit) 
		{
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(110.11, 140.0f, -790.95));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(20.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			ruined_building.draw(shader);

			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(100.11, 150.0f, -790.95));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(90.0f));
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
			ModelMatrix = glm::rotate(ModelMatrix, (float)glfwGetTime() * glm::radians(90.0f) * 5, glm::vec3(0.0f, 1.0f, 0.0));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			tornado.draw(shader);

			if ((cameraPosition.x >= 9.0f && cameraPosition.x <= 230.0f) &&
				(cameraPosition.z >= -927.0f && cameraPosition.z <= -620.0f))
			{
				camera.setCameraPosition(glm::vec3(-106.35f, 155.0f, -780.89f));
			}

		}
		else
		{
				ModelMatrix = glm::mat4(1.0);
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(110.11, 140.0f, -790.95));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(20.0f));
				MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
				glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
				ruined_building.draw(shader);

				float rotationSpeedOx = glm::radians(500.0f);
				float rotationSpeedOy = glm::radians(500.0f);
				static float totalRotationOx = 0.0f;
				static float totalRotationOy = 0.0f;
				totalRotationOx += rotationSpeedOx * deltaTime;
				totalRotationOy += rotationSpeedOy * deltaTime;
				ModelMatrix = glm::mat4(1.0f);
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(130.11, 160.0f, -710.95)); 
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3f));
				ModelMatrix = glm::rotate(ModelMatrix, totalRotationOx, glm::vec3(1.0f, 0.0f, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, totalRotationOy, glm::vec3(0.0f, 1.0f, 0.0f));
				MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f));
				MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
				glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
				if(!natureElement)
				natureSphere.draw(shader);

				if (window.isPressed(GLFW_KEY_E) && !natureElement)
				{
					glm::vec3 cameraPosition = camera.getCameraPosition();
					if ((cameraPosition.x >= 90.0f && cameraPosition.x <= 170.0f) &&
						(cameraPosition.z >= -750.0f && cameraPosition.z <= -660.0f)) 
					{
						natureElement = true;
						airQuest = true;
					}
				}
		}

		if (airQuest)
		{
			if ((cameraPosition.x >= -180.0f && cameraPosition.x <= -130.0f) &&
				(cameraPosition.z >= -820.0f && cameraPosition.z <= -750.0f) && natureElement == true)
			{
				camera.setCameraPosition(glm::vec3(1154.0f, 2.0f, -69.0f));
			}
		}



		float breathSpeed = 50.0f; 
		float maxBreathDistance = 20.0f; 

		if (window.isPressed(GLFW_KEY_Q)) 
		{
			FireBreath newBreath;
			newBreath.position = camera.getCameraPosition();
			breaths.push_back(newBreath);
		}

		// Update and render the fire breaths
		for (int i = 0; i < breaths.size(); i++) 
		{
			// Move the breath forward
			breaths[i].position += camera.getCameraViewDirection() * deltaTime * breathSpeed;

			// Calculate the distance traveled by the breath
			float distanceTraveled = glm::length(camera.getCameraPosition() - breaths[i].position);

			// Remove breaths that have traveled beyond the maximum distance
			if (distanceTraveled > maxBreathDistance) 
			{
				breaths.erase(breaths.begin() + i);
				i--;
				continue;
			}

			// Check if the breath has hit the tornado
			float tornadoHitRange = 150.0f; 
			float wizardHitRangeX = 30.0f;
			float wizardHitRangeZ = 80.0f;


			GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
			GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");
			glm::mat4 ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, breaths[i].position);
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f));  // Adjust the scale to make it smaller
			glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
			glm::mat4 ViewMatrix = glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			magmaBall.draw(shader);
			if (
				breaths[i].position.x >= 100.11 - tornadoHitRange && breaths[i].position.x <= 100.11 + tornadoHitRange &&
				breaths[i].position.z >= -790.95 - tornadoHitRange && breaths[i].position.z <= -790.95 + tornadoHitRange && !tornadoHit)
			{
				tornadoHit = true;
			}
			else
			{
				if(
					breaths[i].position.x >= 2004.35f - wizardHitRangeX && breaths[i].position.x <= 2004.35f + wizardHitRangeX &&
					breaths[i].position.z >= -1300.89f - wizardHitRangeZ && breaths[i].position.z <= -1300.89f + wizardHitRangeZ && airQuest && talkFairy2)
					{
						wizardCollision = true;
						mainQuest = true;
					}
			}
		}


		if (natureElement)
		{
			//render portal to the prison realm
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1163.35f, -13.5f, -61.89f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			if (!talkFairy2)
				portalNature.draw(shader);
			else
				lastPortal.draw(shader);
		}

		if (talkFairy2 && airQuest)
		{
			if ((cameraPosition.x >= 1138.0f && cameraPosition.x <= 1173.0f) &&
				(cameraPosition.z >= -100.0f && cameraPosition.z <= -25.0f))
			{
				camera.setCameraPosition(glm::vec3(1784.0f, -40.0f, -1230.0f));
			}
		}

		//final part


		// Render the last plane
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(906.11, -85.0f, -1150.95));  // Adjust the position as needed
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		lastPlaneMesh.draw(shader);

		//render last portal
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1780.11, -49.0f, -1230.95));  // Adjust the position as needed
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f));  // Adjust the scale accordingly
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		lastPortal.draw(shader);



		if (!wizardCollision)
		{
			//render wizard
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(2004.35f, -55.0f, -1300.89f));
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));  // Rotate 90 degrees
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(8.0f));  // Adjust the scale accordingly
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			wizard.draw(shader);
		}				


		glm::vec3 cameraPosition = camera.getCameraPosition();
		glm::vec3 cameraDirection = camera.getCameraViewDirection();
		glm::vec3 cameraUp = camera.getCameraUp();

		//render skybox object
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(150.0f, 150.0f, 150.0f));
		ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
		ViewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		skyboxSphere.draw(shader);

		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(camera.getCameraPosition().x + 0.2f, camera.getCameraPosition().y + 0.1, camera.getCameraPosition().z));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(300.0f - 180.0f * 180.0f / 3.14f / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		



		//gui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// Inside the game loop, after starting ImGui frame
		RenderQuestUI();
		// Rendering ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		window.update();

		
	}




	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void processKeyboardInput()
{
	float cameraSpeed = 100 * deltaTime;
	float originalCameraSpeed = 50 * deltaTime;

	// Sprinting
	if (window.isPressed(GLFW_KEY_LEFT_SHIFT))
		cameraSpeed = camera.sprint(true, cameraSpeed);
	else
		cameraSpeed = originalCameraSpeed;

	//translation
	if (window.isPressed(GLFW_KEY_W))
		camera.keyboardMoveFront(cameraSpeed);
	if (window.isPressed(GLFW_KEY_S))
		camera.keyboardMoveBack(cameraSpeed);
	if (window.isPressed(GLFW_KEY_A))
		camera.keyboardMoveLeft(cameraSpeed);
	if (window.isPressed(GLFW_KEY_D))
		camera.keyboardMoveRight(cameraSpeed);
	if (window.isPressed(GLFW_KEY_R))
		camera.keyboardMoveUp(cameraSpeed);
	if (window.isPressed(GLFW_KEY_F))
		camera.keyboardMoveDown(cameraSpeed);

	//rotation
	if (window.isPressed(GLFW_KEY_LEFT))
		camera.rotateOy(cameraSpeed);
	if (window.isPressed(GLFW_KEY_RIGHT))
		camera.rotateOy(-cameraSpeed);
	if (window.isPressed(GLFW_KEY_UP))
		camera.rotateOx(cameraSpeed);
	if (window.isPressed(GLFW_KEY_DOWN))
		camera.rotateOx(-cameraSpeed);
	camera.printPosition();

}