#pragma once
#include <windows.h>
#include <sdkddkver.h>
#include <sstream>
#include <vector>
#include <string>
#include "imgui.h"
#include "sol_ImGui.h"

// LuaEngine 1.3.2 compatibility layer.
// 1.3.2 exports LuaCoreAddStateProcessor, so the UI no longer copies
// LuaCore::LuaScriptData/getLuas() across DLL boundaries. This avoids ABI/layout
// dependence on the old 1.1.x LuaScriptData structure.
struct lua_State;

namespace LuaCore {
    using StateProcessor = void(*)(lua_State*);
    using AddStateProcessorFn = void(*)(StateProcessor);

    using RunFn = void(*)(std::string, lua_State*);

    inline bool initUI = false;
    inline bool processorInstalled = false;
    inline HMODULE engineModule = nullptr;
    inline AddStateProcessorFn addStateProcessor = nullptr;
    inline RunFn runFn = nullptr;

    inline void BindOneState(lua_State* L) {
        if (!L) return;
        sol::state_view lua(L);
        sol_ImGui::Init(lua);

        // LuaEngine 1.3.2 compatibility: register the status function directly
        // into every Lua state handled by LuaCoreAddStateProcessor.
        lua.set_function("CheckImguiStatus", []() -> int {
            return 0;
        });

        // LoadTexture is installed separately by D3D12Hook after renderer init.
    }

    inline bool Resolve132API() {
        if (!engineModule) engineModule = GetModuleHandleW(L"LuaEngine.dll");
        if (!engineModule) return false;
        if (!addStateProcessor)
            addStateProcessor = reinterpret_cast<AddStateProcessorFn>(GetProcAddress(engineModule, "LuaCoreAddStateProcessor"));
        // run is C++ decorated; keep static import in D3D12Hook via compatibility declaration below.
        return addStateProcessor != nullptr;
    }

    inline void Imgui_Bindings() {
        if (processorInstalled) { initUI = true; return; }
        if (!Resolve132API()) return;
        addStateProcessor(&BindOneState);
        processorInstalled = true;
        initUI = true;
    }

    // Exact exported ABI still present in LuaEngine 1.3.2 (verified from DLL exports).
    __declspec(dllimport) extern void run(std::string func, lua_State* runL = nullptr);
}
