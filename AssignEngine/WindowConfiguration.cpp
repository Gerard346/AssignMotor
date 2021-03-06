#include "Application.h"
#include "WindowConfiguration.h"
#include "glew/include/glew.h"

WindowConfiguration::WindowConfiguration() :ModuleUIManager()
{
	isActive.push_back(Activate());
}

WindowConfiguration::~WindowConfiguration()
{
	fps_log.clear();
	ms_log.clear();
	isActive.clear();
}

bool WindowConfiguration::Start()
{
	name_engine = "Assign Engine";
	name_organization = "CITM - UPC";
	fps_log.resize(100, 0);
	ms_log.resize(100, 0);
	return true;
}

update_status WindowConfiguration::Update(float dt)
{
	if (isActive[0].isActivate) {
		bool* p = &isActive[0].isActivate;
		Draw("Config Window", p);

	}

	return UPDATE_CONTINUE;
}

bool WindowConfiguration::CleanUp()
{
	fps_log.clear();
	ms_log.clear();

	return true;
}

void WindowConfiguration::Draw(const char* title, bool *p_open)
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);

	ImGui::Begin("Configuration Window", p_open);

	if (ImGui::CollapsingHeader("Application")) {
		ImGui::Text("Name Of The Engine: %s", name_engine);
		ImGui::Text("Organization: %s", name_organization);
		
		static int capped_fps = App->GetFPS();
		if (ImGui::SliderInt("Max FPS", &capped_fps, 0, 120)) {
			App->SetFPS(capped_fps);
		}

		char title[25];
		//Fps
		fps_log.erase(fps_log.begin());
		fps_log.push_back(App->GetFPS());
		sprintf_s(title, 25, "Framerate %1.f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

		//Ms
		ms_log.erase(ms_log.begin());
		ms_log.push_back(App->GetLastDt() * 1000);
		sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}
	if (ImGui::CollapsingHeader("Window")) {
		float brightness = App->window->GetBrightness();
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f)) {
			App->window->SetBrightness(brightness);
		}
		int widht, height;
		if (ImGui::SliderInt("Width", &App->window->width, 1440, 1900)) {
			App->window->wantToresize();
		}
		if (ImGui::SliderInt("Height", &App->window->height,500, 1440)) {
			App->window->wantToresize();
		}
		if (ImGui::Checkbox("Fullscreen", &App->window->fullscreen)) {
			App->window->SetFullScreen();
		}
		if (ImGui::Checkbox("Resizable", &App->window->resizable)) {
			App->window->SetResizable();
		}
		if (ImGui::Checkbox("Borderless", &App->window->borderless)) {
			App->window->SetBorderless();
		}
		if (ImGui::Checkbox("Full Desktop", &App->window->fulldesktop)) {
			App->window->setFullDesktop();
		}
	}
	if (ImGui::CollapsingHeader("FileSystem")) {

	}
	if (ImGui::CollapsingHeader("Input")) {
		ImGui::Text("Mouse position: x:%d y:%d", App->input->GetMouseX(), App->input->GetMouseY());
		ImGui::Text("Mouse motion: x:%d y:%d", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());
	}
	if (ImGui::CollapsingHeader("Hardware")) {
		ImGui::Text("CPU Cache line: %d", SDL_GetCPUCacheLineSize());
		ImGui::Text("CPU Cache line: %d", SDL_GetCPUCount());
		ImGui::Text("CPU Cache line: %d", SDL_GetSystemRAM());
		ImGui::Text("GPU: %s", (const char*)glGetString(GL_RENDERER));
		ImGui::Text("GPU Brand: %s", (const char*)glGetString(GL_VENDOR));
	}

	ImGui::End();

}

