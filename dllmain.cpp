// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>

// YYTK is in this now
#include "MyHelper.h"
#include "dllmain.h"
#include "LoopHeroEnums.h"

// Plugin functionality
#include <fstream>
#include <iterator>
#define _CRT_SECURE_NO_WARNINGS

// Unload
YYTKStatus PluginUnload()
{
    PmRemoveCallback(callbackAttr);

    return YYTK_OK;
}

YYTKStatus ExecuteCodeCallback(YYTKCodeEvent* codeEvent, void*)
{
    CCode* codeObj = std::get<CCode*>(codeEvent->Arguments());
    CInstance* selfInst = std::get<0>(codeEvent->Arguments());
    CInstance* otherInst = std::get<1>(codeEvent->Arguments());

    // If we have invalid data???
    if (!codeObj)
        return YYTK_INVALIDARG;

    if (!codeObj->i_pName)
        return YYTK_INVALIDARG;

    if (Misc::StringHasSubstr(codeObj->i_pName, "gml_Room_"))
    {
        if (Misc::StringHasSubstr(codeObj->i_pName, "gml_Room_rm_music_maker_Create"))
        {
            gInMusicmaker = true;
        }
        else 
        {
            gInMusicmaker = false;
        }
    }
   
    return YYTK_OK;
}


// Entry
DllExport YYTKStatus PluginEntry(
    YYTKPlugin* PluginObject // A pointer to the dedicated plugin object
)
{
    Misc::Print("Music Maker mod loaded!");
    gThisPlugin = PluginObject;
    gThisPlugin->PluginUnload = PluginUnload;

    PluginAttributes_t* pluginAttributes = nullptr;
    if (PmGetPluginAttributes(gThisPlugin, pluginAttributes) == YYTK_OK)
    {
        PmCreateCallback(pluginAttributes, callbackAttr, reinterpret_cast<FNEventHandler>(ExecuteCodeCallback), EVT_CODE_EXECUTE, nullptr);
    }


    // Initialize the plugin, set callbacks inside the PluginObject.
    // Set-up buffers.
    return YYTK_OK; // Successful PluginEntry.
}

DWORD WINAPI Menu(HINSTANCE hModule)
{
    while (true)
    {
        Sleep(500);
        if (GetAsyncKeyState(VK_F1))
        {
            Misc::Print("Value of gisIn: " + std::to_string((gInMusicmaker)));
            YYRValue r;
            switch (gInMusicmaker)
            {
            case false:              
                Misc::Print("Going to musicmaker");
                CallBuiltin(r, "room_goto", nullptr, nullptr, { static_cast<double>(LHRooms::rm_music_maker) });
                break;

            case true:
                Misc::Print("Going to main menu");
                CallBuiltin(r, "game_restart", nullptr, nullptr, {});
                break;
            }            
        }
    }
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DllHandle = hModule;
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Menu, NULL, 0, NULL); // For the input
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

