// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>

// YYTK is in this now
#include "MyPlugin.h"

// Plugin functionality
#include <fstream>
#include <iterator>
#include <format>
#include "LHCore.h"
#include "LHRooms.h"
#include "LHSprites.h"
#include "LHObjects.h"

#define _CRT_SECURE_NO_WARNINGS

// BUTTONS
constexpr uint32_t HEVT_BUTTON_DRAW = Hash("o_menu_button_Draw_0");
constexpr uint32_t HEVT_BUTTON_BASE_CREATE = Hash("gml_Object_o_base_button_Create_0");
constexpr uint32_t HEVT_BUTTON_PRESS = Hash("gml_Object_o_base_button_Mouse_4");
constexpr uint32_t HEVT_BUTTON_ENTER = Hash("gml_Object_o_base_button_Mouse_10");
constexpr uint32_t HEVT_BUTTON_LEAVE = Hash("gml_Object_o_base_button_Mouse_11");
constexpr uint32_t HEVT_CONTINUE_BUTTON_STEP = Hash("gml_Object_o_continue_button_Step_2");
//ROOMS
constexpr uint32_t HEVT_ROOM_RM_TITLES = Hash("gml_Room_rm_titles_Create");
constexpr uint32_t HEVT_ROOM_RM_CREATER_DIALOG = Hash("gml_Room_rm_creater_dialog_Create");
constexpr uint32_t HEVT_ROOM_RM_PIRAMIDKA = Hash("gml_Room_rm_piramidka_Create");
constexpr uint32_t HEVT_ROOM_RM_LOAD = Hash("gml_Room_rm_load_Create");
constexpr uint32_t HEVT_ROOM_RM_INTRO = Hash("gml_Room_rm_intro_Create");
constexpr uint32_t HEVT_ROOM_RM_CAMP = Hash("gml_Room_rm_camp_Create");
constexpr uint32_t HEVT_ROOM_RM_TUTOR = Hash("gml_Room_rm_tutor_Create");
constexpr uint32_t HEVT_ROOM_RM_GAME = Hash("gml_Room_rm_game_Create");
constexpr uint32_t HEVT_ROOM_RM_CUTSCENES = Hash("gml_Room_rm_cutscenes_Create");
constexpr uint32_t HEVT_ROOM_RM_MUSIC_MAKER = Hash("gml_Room_rm_music_maker_Create");
// Globals
bool buttonCreated = false;
YYRValue buttonRef = -4.;
bool hoveringButton = true;
int gCurrentRoom = LHRooms::rm_intro; // Current room, updated on event


// Unload
YYTKStatus PluginUnload()
{
    LHCore::pUnregisterModule(gPluginName);
    return YYTK_OK;
}

void Printvars(double inst)
{
    YYRValue names = Binds::CallBuiltinA("variable_instance_get_names", { inst });
    YYRValue count = Binds::CallBuiltinA("array_length", { names });

    for (int i = 0; i < (int)count; i++)
    {
        YYRValue varname = Binds::CallBuiltinA("array_get", { names, (double)i });

        YYRValue value = Binds::CallBuiltinA(
            "variable_instance_get",
            { inst, varname }
        );

        YYRValue type;
        CallBuiltin(type, "typeof", nullptr, nullptr, { value });
        std::string typestr = std::string(static_cast<const char*>(type));

        
        Misc::Print(type);
        Misc::Print(varname);

        if (type == "array")
        {
            continue;
        }
        if (Misc::StringHasSubstr(typestr, "bool"))
        {
            Misc::Print("SIBOOL");
            Misc::Print(std::to_string(int((bool)value)));
        }
        if (Misc::StringHasSubstr(typestr, "number"))
        {
            Misc::Print(std::to_string(int(value)));
        }

        Misc::Print(value);
        Misc::Print("------");
    }
}

void UpdateRoom(uint32_t hevt, int& current_room)
{
    switch (hevt)
    {
    case HEVT_ROOM_RM_TITLES:
        current_room = LHRooms::LHRoom::rm_titles;
        break;

    case HEVT_ROOM_RM_CREATER_DIALOG:
        current_room = LHRooms::LHRoom::rm_creater_dialog;
        break;

    case HEVT_ROOM_RM_PIRAMIDKA:
        current_room = LHRooms::LHRoom::rm_piramidka;
        break;

    case HEVT_ROOM_RM_LOAD:
        current_room = LHRooms::LHRoom::rm_load;
        break;

    case HEVT_ROOM_RM_INTRO:
        current_room = LHRooms::LHRoom::rm_intro;
        break;

    case HEVT_ROOM_RM_CAMP:
        current_room = LHRooms::LHRoom::rm_camp;
        break;

    case HEVT_ROOM_RM_TUTOR:
        current_room = LHRooms::LHRoom::rm_tutor;
        break;

    case HEVT_ROOM_RM_GAME:
        current_room = LHRooms::LHRoom::rm_game;
        break;

    case HEVT_ROOM_RM_CUTSCENES:
        current_room = LHRooms::LHRoom::rm_cutscenes;
        break;

    case HEVT_ROOM_RM_MUSIC_MAKER:
        current_room = LHRooms::LHRoom::rm_music_maker;
        break;

    default:
        break;
    }
}

bool CustomButtonExists()
{
    if (!buttonCreated) { 
        return false;
    }
    YYRValue exists = Binds::CallBuiltinA("instance_exists", {buttonRef});
    return (bool)exists;
}

void ResetMusicButtonText()
{

    if (!CustomButtonExists()) { 
        return; 
    }

    std::string label =
        (gCurrentRoom == LHRooms::rm_music_maker)
        ? "Exit & Restart"
        : "Music Maker";

    Binds::CallBuiltinA("variable_instance_set", { buttonRef, "text", label });
    Binds::CallBuiltinA("variable_instance_set", { buttonRef, "fa_ltext", label });

    
}

int CodePostPatch(YYTKCodeEvent* codeEvent, void*)
{
    CCode* codeObj = std::get<CCode*>(codeEvent->Arguments());
    CInstance* selfInst = std::get<0>(codeEvent->Arguments());
    CInstance* otherInst = std::get<1>(codeEvent->Arguments());

    // If we have invalid data???
    if (!codeObj)
        return YYTK_INVALIDARG;

    if (!codeObj->i_pName)
        return YYTK_INVALIDARG;
    
    // hash event
    const char* eventName = codeObj->i_pName;
    uint32_t evt = Hash(eventName);

    // Track rooms
    UpdateRoom(evt, gCurrentRoom);

    // Events
    switch (evt)
    {
    case HEVT_BUTTON_PRESS: 
        if (selfInst->i_spriteindex == buttonRef.As<double>()) // Pressed music maker button
        {
            if (gCurrentRoom == LHRooms::rm_intro)
            {
                Binds::CallBuiltinA("room_goto", { static_cast<double>(LHRooms::rm_music_maker) });
            }
            else if (gCurrentRoom == LHRooms::rm_music_maker)
            {
                buttonCreated = false;
                //Binds::CallBuiltinA("room_goto", { static_cast<double>(LHRooms::rm_load) });
                Binds::CallBuiltinA("game_restart", {});
            }
        }
        break;

    case HEVT_BUTTON_BASE_CREATE:
        if (!buttonCreated && gCurrentRoom == LHRooms::rm_intro) // doesnt exist, spawn button
        {
            buttonCreated = true;
            buttonRef = Binds::CallBuiltinA("instance_create_depth", { 480., 320., -200., (double)LHObjectEnum::o_continue_button });
            
            ResetMusicButtonText();
            Binds::CallBuiltinA("variable_instance_set", { buttonRef, "click_event", -1. });

        }

        break;

    case HEVT_CONTINUE_BUTTON_STEP:
        if(selfInst->i_spriteindex == buttonRef.As<double>())
        {
            ResetMusicButtonText();
        }
        break;

    case HEVT_ROOM_RM_CAMP:
    case HEVT_ROOM_RM_GAME:

        if (CustomButtonExists())
        {
            //Binds::CallBuiltinA("variable_instance_set", { buttonRef, "disabled", 1.0});
            Binds::CallBuiltinA("instance_destroy", { buttonRef });
        }
        break;

    }
    /*if (Misc::StringHasSubstr(codeObj->i_pName, "Mouse_10"))
    {
        Misc::Print(codeObj->i_pName);
        Printvars(selfInst->i_spriteindex);
    }*/


    
    return YYTK_OK;  
}

void InstallPatches()
{
    if (LHCore::pInstallPostPatch != nullptr)
    {
        LHCore::pInstallPostPatch(CodePostPatch);
        Misc::Print("Installed patch method(s)", CLR_GREEN);
    }
}


// Entry
DllExport YYTKStatus PluginEntry(
    YYTKPlugin* PluginObject // A pointer to the dedicated plugin object
)
{
    LHCore::CoreReadyPack* pack = new LHCore::CoreReadyPack(PluginObject, InstallPatches);
    PluginObject->PluginUnload = PluginUnload;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LHCore::ResolveCore, (LPVOID)pack, 0, NULL); // Wait for LHCC
    return YYTK_OK; // Successful PluginEntry.
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
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

