#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	ui = new ModuleUI(this);
	console = new WindowConsole(this);
	mesh = new ModuleMesh3D(this);
	texture = new ModuleTexture3D(this);
	file_system = new ModuleFileSystem(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	// Scenes
	AddModule(scene);
	AddModule(console);

	AddModule(ui);

	AddModule(mesh);
	AddModule(texture);

	AddModule(file_system);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	std::list<Module*>::iterator item = list_modules.begin();

	while(item != list_modules.end())
	{
		delete item._Ptr->_Myval;
		item++;
	}
}

bool Application::Init()
{
	bool ret = true;
	//max_fps = 60;

	ms_timer.Start();

	config_value = json_parse_file("config.json");

	if (config_value != NULL) {
		config_object = json_value_get_object(config_value);
		LOG("Config file loaded successfully");
	}

	else {
		LOG("Config file not found. Creating one instead.");
		CreatingConfigJSON();
	}

	LoadingData();

	// Call Init() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = item._Ptr->_Myval->Init();
		item++;
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = item._Ptr->_Myval->Start();
		item++;
	}
	
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.ReadSec();
	fps = 1.0f / dt;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	Uint32 last_frame_ms = ms_timer.Read();

	if (last_frame_ms < fps_capped) {
		SDL_Delay(fps_capped - last_frame_ms);
	}
}

void Application::CreatingConfigJSON()
{
	JSON_Value* root_value = json_value_init_object();
	JSON_Object* root_object = json_value_get_object(root_value);
	char* serialized_string = NULL;
	json_object_set_string(root_object, "name", "John Smith");
	json_object_set_number(root_object, "age", 25);
	json_object_dotset_string(root_object, "address.city", "Cupertino");
	json_object_dotset_value(root_object, "contact.emails", json_parse_string("[\"email@example.com\",\"email2@example.com\"]"));
	serialized_string = json_serialize_to_string_pretty(root_value);
	puts(serialized_string);
	json_serialize_to_file(root_value, "config.json");
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);
}

void Application::LoadingData()
{
	JSON_Value* schema = json_parse_string("{\"name\":\"\"}");
	JSON_Value* user_data = json_parse_file("user_data.json");
	char buf[10] = "Gerard";
	const char* name = NULL;
	if (user_data == NULL || json_validate(schema, user_data) != JSONSuccess || json_object_get_string(json_object(user_data), "name") != buf) {
		user_data = json_value_init_object();
		json_object_set_string(json_object(user_data), "name", buf);
		json_serialize_to_file(user_data, "user_data.json");
	}
	name = json_object_get_string(json_object(user_data), "name");
	printf("Hello, %s.", name);
	json_value_free(schema);
	json_value_free(user_data);
	return;
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::iterator item = list_modules.begin();
	
	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = item._Ptr->_Myval->PreUpdate(dt);
		item++;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = item._Ptr->_Myval->Update(dt);
		item++;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = item._Ptr->_Myval->PostUpdate(dt);
		item++;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	std::list<Module*>::iterator item = list_modules.end();

	while(item != list_modules.end() && ret == true)
	{
		ret = item._Ptr->_Myval->CleanUp();
		item++;
	}
	return ret;
}

float Application::GetFPS()
{
	return fps;
}

float Application::GetLastDt()
{
	return dt;
}

void Application::SetFPS(int fps_cap)
{
	fps_capped = 1000 / fps_cap;
}

int Application::GetFPSCapped()
{
	return 1000/fps_capped;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}