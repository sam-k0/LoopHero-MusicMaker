#pragma once
#include <unordered_map>

namespace LHRooms {

	enum LHRoom {
		rm_titles = 0,
		rm_creater_dialog = 1,
		rm_piramidka = 2,
		rm_load = 3,
		rm_intro = 4,
		rm_camp = 5,
		rm_tutor = 6,
		rm_game = 7,
		rm_cutscenes = 8,
		rm_music_maker = 9
	};

	static const std::unordered_map<std::string, LHRoom> LHRoomMap = {
	{"rm_titles", rm_titles},
	{"rm_creater_dialog", rm_creater_dialog},
	{"rm_piramidka", rm_piramidka},
	{"rm_load", rm_load},
	{"rm_intro", rm_intro},
	{"rm_camp", rm_camp},
	{"rm_tutor", rm_tutor},
	{"rm_game", rm_game},
	{"rm_cutscenes", rm_cutscenes},
	{"rm_music_maker", rm_music_maker}
	};

}
