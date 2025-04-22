// UI-обёртка вокруг мониторинга аукциона STALCRAFT
// Зависимости: ImGui, ImPlot, GLFW, OpenGL3, cpr, nlohmann/json

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <chrono>
#include <thread>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct LotData {
    std::string itemId;
    int64_t buyoutPrice;
    int32_t amount;
    std::string startTime;
};

std::vector<LotData> lots;
std::set<std::string> knownStartTimes;

// Загрузка конфигурации
json loadConfig(const std::string& filename) {
    std::ifstream configFile(filename);
    if (!configFile.is_open()) {
        throw std::runtime_error("Не удалось открыть config.json");
    }
    json config;
    configFile >> config;
    return config;
}

void fetchLots(const std::string& region, const std::string& itemId, const std::string& apiKey) {
    int offset = 0;
    const int limit = 100;
    lots.clear();

    while (true) {
        std::string url = "https://eapi.stalcraft.net/" + region + "/auction/" + itemId + "/lots?offset=" +
                          std::to_string(offset) + "&limit=" + std::to_string(limit);
        cpr::Response r = cpr::Get(cpr::Url{url}, cpr::Header{{"Authorization", "Bearer " + apiKey}});
        if (r.status_code != 200) break;

        json result = json::parse(r.text);
        if (!result.contains("lots") || !result["lots"].is_array()) break;

        size_t count = result["lots"].size();
        for (const auto& lot : result["lots"]) {
            std::string startTime = lot["startTime"];
            if (knownStartTimes.count(startTime)) continue;

            lots.push_back({
                lot["itemId"],
                lot["buyoutPrice"].get<int64_t>(),
                lot["amount"].get<int32_t>(),
                startTime
            });
            knownStartTimes.insert(startTime);
        }

        if (count < limit) break;
        offset += limit;
    }
}

void drawUI(const std::string& region, const std::string& itemId, const std::string& apiKey) {
    static bool autoRefresh = false;
    static float lastFetchTime = 0.0f;
    
    if (autoRefresh && ImGui::GetTime() - lastFetchTime > 10.0f) {
        fetchLots(region, itemId, apiKey);
        lastFetchTime = ImGui::GetTime();
    }

    ImGui::Begin("STALCRAFT Auction Monitor");
    if (ImGui::Button("Обновить")) {
        fetchLots(region, itemId, apiKey);
    }
    ImGui::SameLine();
    ImGui::Checkbox("Автообновление", &autoRefresh);

    ImGui::Text("Лотов загружено: %zu", lots.size());
    if (ImGui::BeginTable("Лоты", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Цена");
        ImGui::TableSetupColumn("Кол-во");
        ImGui::TableSetupColumn("Цена/шт");
        ImGui::TableSetupColumn("StartTime");
        ImGui::TableHeadersRow();

        for (const auto& lot : lots) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%lld", lot.buyoutPrice);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%d", lot.amount);
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%lld", lot.buyoutPrice / std::max(1, lot.amount));
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%s", lot.startTime.c_str());
        }

        ImGui::EndTable();
    }
    ImGui::End();
}

int main() {
    json config = loadConfig("config.json");
    std::string apiKey = config["api_key"];
    std::string region = config["region"];
    std::string itemId = config["item_id"];

    // Setup window
    if (!glfwInit()) return 1;
    const char* glsl_version = "#version 130";
    GLFWwindow* window = glfwCreateWindow(1280, 720, "STALCRAFT Auction", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        drawUI(region, itemId, apiKey);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
