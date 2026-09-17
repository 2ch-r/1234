LuaEngineUI -> LuaEngine 1.3.2 compatibility patch

WHY:
The old UI directly calls getLuaFiles()/getLuas() and copies LuaScriptData from
LuaEngine.dll. LuaEngine 1.3.2 still exports those decorated names, but relying
on an old C++ struct layout is fragile. 1.3.2 now exports a stable C entry point:
LuaCoreAddStateProcessor.

THIS PATCH:
- removes getLuaFiles/getLuas/LuaScriptData use from LuaEngineUI
- registers sol_ImGui::Init for Lua states through LuaCoreAddStateProcessor
- keeps LuaCore::run("on_imgui"), whose exact decorated export still exists in 1.3.2
- keeps the original DX12 renderer/hook from LuaEngineUI

TEST MODE:
1. Game DX12 ON (this UI source is DX12).
2. Keep LuaEngine 1.3.2.
3. Compile x64 Release and put LuaEngineUI.dll in nativePC/plugins.
4. First test only a minimal on_imgui script; then enable dataview.lua.

NOTE:
This source needs Windows/MSVC to build. A Linux build here cannot produce a
reliable MHW/Stracker-compatible Windows plugin DLL.
