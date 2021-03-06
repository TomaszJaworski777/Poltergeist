﻿#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "ImGUIContent.hpp"
#include <PoltergeistEngine/Macros.hpp>
#ifdef WIN32
#include <Windows.h>
#endif

extern "C"
{
	POLTERGEIST_PUBLIC uint32_t NvOptimusEnablement = 1;
	POLTERGEIST_PUBLIC int32_t AmdPowerXpressRequestHighPerformance = 1;
}

void OnWindowSizeUpdate(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
#ifdef WIN32
	SetConsoleOutputCP(CP_UTF8);
	setvbuf(stdout, nullptr, _IOFBF, 1024);
#endif

	std::cout << "Hello editor!" << std::endl;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

	{
		std::unique_ptr<GLFWwindow, decltype(glfwDestroyWindow)*> window {
				glfwCreateWindow(800, 600, "Poltergeist Editor", nullptr, nullptr),
				&glfwDestroyWindow
		};

		if (window.get() == nullptr)
		{
			glfwTerminate();
			return -1;
		}

		glfwMakeContextCurrent(window.get());
		glfwSetWindowSizeCallback(window.get(), OnWindowSizeUpdate);

		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			glfwTerminate();
			return -2;
		}

		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
		ImGui_ImplOpenGL3_Init();

		while (!glfwWindowShouldClose(window.get()))
		{
			glfwPollEvents();
			
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			GUIContent();
			ImGui::Render();

			ImVec4 backgroundColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
			glClear(GL_COLOR_BUFFER_BIT);

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window.get());
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	return 0;
}
