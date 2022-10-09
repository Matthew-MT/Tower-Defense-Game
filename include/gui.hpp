#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl.h"
#include "../imgui/imgui_impl_sdlrenderer.h"
#include "game.hpp"
//#include <stdio.h>
//#include <SDL2/SDL.h>

namespace game
{
    class GUI {
    protected:
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    SDL_Window* window;
    SDL_Renderer* renderer;
    public:
        GUI(SDL_Window* initWindow, SDL_Renderer* initRenderer): window{initWindow}, renderer{initRenderer}
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io=ImGui::GetIO(); (void)io;
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            //bool ImGui_ImplSDL2_Init{};
            ImGui::StyleColorsDark();

            ImGui_ImplSDL2_InitForSDLRenderer(this->window, this->renderer);
            ImGui_ImplSDLRenderer_Init(this->renderer);
            //ImGui_ImplSDLRenderer_NewFrame();
            //ImGui_ImplSDL2_NewFrame();
            //ImGui::NewFrame();
            //io.Fonts->AddFontDefault();
        }
        void render()
        {
            ImGui_ImplSDLRenderer_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            if(show_demo_window)
                ImGui::ShowDemoWindow(&show_demo_window);

            ImGui::Render();
            SDL_SetRenderDrawColor(this->renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
            //SDL_RenderClear(this->renderer);
            ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
            //SDL_RenderPresent(this->renderer);

        }
        ~GUI()
        {
            ImGui_ImplSDLRenderer_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();
        }
        


    };
};


