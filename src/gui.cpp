#include "PortCE_gui.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>

#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

static ImGuiIO* io_IMGUI = nullptr;
static SDL_Window* imgui_window = nullptr;
static SDL_Renderer* imgui_renderer = nullptr;
static bool imgui_visible = true;
static bool imgui_initialized = false;

void PortCE_set_imgui_visible(bool visible) {
	imgui_visible = visible;
	if (imgui_window == nullptr) {
		return;
	}
	if (imgui_visible) {
		SDL_ShowWindow(imgui_window);
		SDL_RaiseWindow(imgui_window);
	} else {
		SDL_HideWindow(imgui_window);
	}
}

bool PortCE_is_imgui_visible(void) {
	return imgui_visible;
}

bool PortCE_is_imgui_window_id(uint32_t window_id) {
	if (imgui_window == nullptr) {
		return false;
	}
	return SDL_GetWindowID(imgui_window) == static_cast<Uint32>(window_id);
}

void PortCE_process_imgui_event(const SDL_Event* event) {
	if (event == nullptr || imgui_window == nullptr || !imgui_initialized) {
		return;
	}
	ImGui_ImplSDL2_ProcessEvent(event);
	if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_CLOSE) {
		const Uint32 imgui_window_id = SDL_GetWindowID(imgui_window);
		if (event->window.windowID == imgui_window_id) {
			PortCE_set_imgui_visible(false);
		}
	}
}

void begin_PortCE_IMGUI(void) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io_IMGUI = &ImGui::GetIO();
	io_IMGUI->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io_IMGUI->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io_IMGUI->IniFilename = nullptr;
	ImGui::StyleColorsDark();

	const int imgui_width = 640;
	const int imgui_height = 480;
	imgui_window = SDL_CreateWindow(
		"PortCE ImGui",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		imgui_width, imgui_height,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN
	);
	if (imgui_window == nullptr) {
		fprintf(stderr, "Error: failed to create ImGui window\n");
		ImGui::DestroyContext();
		io_IMGUI = nullptr;
		return;
	}

	imgui_renderer = SDL_CreateRenderer(imgui_window, -1, SDL_RENDERER_ACCELERATED);
	if (imgui_renderer == nullptr) {
		fprintf(stderr, "Error: failed to create ImGui renderer\n");
		SDL_DestroyWindow(imgui_window);
		imgui_window = nullptr;
		ImGui::DestroyContext();
		io_IMGUI = nullptr;
		return;
	}

	ImGui_ImplSDL2_InitForSDLRenderer(imgui_window, imgui_renderer);
	ImGui_ImplSDLRenderer2_Init(imgui_renderer);
	imgui_initialized = true;

	PortCE_set_imgui_visible(imgui_visible);
}

void finish_PortCE_IMGUI(void) {
	if (imgui_initialized) {
		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		imgui_initialized = false;
	}

	if (imgui_renderer != nullptr) {
		SDL_DestroyRenderer(imgui_renderer);
		imgui_renderer = nullptr;
	}
	if (imgui_window != nullptr) {
		SDL_DestroyWindow(imgui_window);
		imgui_window = nullptr;
	}
	io_IMGUI = nullptr;
}

void render_IMGUI(void) {
	if (imgui_window == nullptr || imgui_renderer == nullptr || !imgui_visible || !imgui_initialized) {
		return;
	}

	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	bool open = imgui_visible;
	ImGui::Begin("PortCE Controls", &open, ImGuiWindowFlags_NoCollapse);
	ImGui::Text("PortCE ImGui window");
	if (ImGui::Button("Hide Window")) {
		open = false;
	}
	ImGui::End();

	if (!open) {
		PortCE_set_imgui_visible(false);
	}

	ImGui::Render();
	SDL_SetRenderDrawColor(imgui_renderer, 0, 0, 0, 255);
	SDL_RenderClear(imgui_renderer);
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), imgui_renderer);
	SDL_RenderPresent(imgui_renderer);
}
