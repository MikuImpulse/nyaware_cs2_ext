#include "config.hpp"

#include <format>
#include <fstream>
#include <filesystem>

json c_config_manager::save_weapon_cfg(const weapon_config_t& weapon_cfg) {
	json new_array{};

	new_array["aimbot"] = weapon_cfg.aimbot;
	new_array["aim_bind"] = weapon_cfg.aim_bind;
	new_array["smooth"] = weapon_cfg.smooth;
	new_array["rcs_strength"] = weapon_cfg.rcs_strength;

	new_array["fov"]["draw"] = weapon_cfg.fov.draw;
	new_array["fov"]["value"] = weapon_cfg.fov.value;

	new_array["triggerbot"]["enable"] = weapon_cfg.triggerbot.enable;
	new_array["triggerbot"]["delay"] = weapon_cfg.triggerbot.delay;

	new_array["bone"] = weapon_cfg.bone;
	
	return new_array;
}

weapon_config_t c_config_manager::load_weapon_cfg(const json& object) {
	if (object.is_object()) {
		weapon_config_t cfg{};

		cfg.aimbot = object.value("aimbot", false);
		cfg.aim_bind = object.value("aim_bind", 0);
		cfg.smooth = object.value("smooth", 1.5f);
		cfg.rcs_strength = object.value("rcs_strength", 14);

		if (object.contains("fov") && object["fov"].is_object()) {
			cfg.fov.draw = object["fov"].value("draw", false);
			cfg.fov.value = object["fov"].value("value", 40.f);
		}

		if (object.contains("triggerbot") && object["triggerbot"].is_object()) {
			cfg.triggerbot.enable = object["triggerbot"].value("enable", false);
			cfg.triggerbot.delay = object["triggerbot"].value("delay", 0.f);
		}

		cfg.bone = object.value("bone", 0);

		return cfg;
	}

	return {};
}

json c_config_manager::save_color(const ImColor& color) {
	return json::array({ std::lround(color.Value.x * 255), std::lround(color.Value.y * 255), std::lround(color.Value.z * 255), std::lround(color.Value.w * 255) });
}

ImColor c_config_manager::load_color(const json& array) {
	if (array.is_array() && array.size() >= 4)
		return ImColor(array[0].get<int>(), array[1].get<int>(), array[2].get<int>(), array[3].get<int>());

	return {};
}

std::vector<std::string> c_config_manager::get_configList() {
	std::vector<std::string> result{};

	if (std::filesystem::exists(m_config_path)) {
		for (const auto& entry : std::filesystem::directory_iterator(m_config_path)) {
			if (entry.path().extension() == ".json")
				result.push_back(entry.path().stem().string());
		}

		std::sort(result.begin(), result.end());
	}

	return result;
}

bool c_config_manager::save(const std::string& cfg_name) {
	json config{};

	config["ui"]["accent_color"] = save_color(cfg.ui.accent_color);
	config["ui"]["menu_keybind"] = cfg.ui.menu_keybind;

	config["ui"]["watermark"]["draw"] = cfg.ui.watermark.draw;
	config["ui"]["watermark"]["position_num"] = cfg.ui.watermark.position_num;
	config["ui"]["watermark"]["elements"] = save_array(cfg.ui.watermark.elements);

	config["visuals"]["font_num"] = cfg.visuals.font_num;

	config["visuals"]["esp"]["player"]["tracer"]["draw"] = cfg.visuals.esp.player.tracer.draw;
	config["visuals"]["esp"]["player"]["tracer"]["color"] = save_color(cfg.visuals.esp.player.tracer.color);

	config["visuals"]["esp"]["player"]["rect"]["draw"] = cfg.visuals.esp.player.rect.draw;
	config["visuals"]["esp"]["player"]["rect"]["mode"] = cfg.visuals.esp.player.rect.mode;
	config["visuals"]["esp"]["player"]["rect"]["color"] = save_color(cfg.visuals.esp.player.rect.color);

	config["visuals"]["esp"]["player"]["health"]["draw"] = cfg.visuals.esp.player.health.draw;
	config["visuals"]["esp"]["player"]["health"]["bar_color"] = save_color(cfg.visuals.esp.player.health.bar_color);
	config["visuals"]["esp"]["player"]["health"]["text_color"] = save_color(cfg.visuals.esp.player.health.text_color);

	config["visuals"]["esp"]["player"]["nickName"]["draw"] = cfg.visuals.esp.player.nickName.draw;
	config["visuals"]["esp"]["player"]["nickName"]["color"] = save_color(cfg.visuals.esp.player.nickName.color);

	config["visuals"]["esp"]["player"]["skeleton"]["draw"] = cfg.visuals.esp.player.skeleton.draw;
	config["visuals"]["esp"]["player"]["skeleton"]["visible_check"] = cfg.visuals.esp.player.skeleton.visible_check;
	config["visuals"]["esp"]["player"]["skeleton"]["color"] = save_color(cfg.visuals.esp.player.skeleton.color);
	config["visuals"]["esp"]["player"]["skeleton"]["visible_color"] = save_color(cfg.visuals.esp.player.skeleton.visible_color);
	config["visuals"]["esp"]["player"]["skeleton"]["invisible_color"] = save_color(cfg.visuals.esp.player.skeleton.invisible_color);

	config["visuals"]["esp"]["player"]["weapon"]["draw"] = cfg.visuals.esp.player.weapon.draw;
	config["visuals"]["esp"]["player"]["weapon"]["modes"] = save_array(cfg.visuals.esp.player.weapon.modes);
	config["visuals"]["esp"]["player"]["weapon"]["colors"] = save_array(cfg.visuals.esp.player.weapon.colors);

	config["visuals"]["esp"]["player"]["flags"]["draw"] = cfg.visuals.esp.player.flags.draw;
	config["visuals"]["esp"]["player"]["flags"]["modes"] = save_array(cfg.visuals.esp.player.flags.modes);
	config["visuals"]["esp"]["player"]["flags"]["colors"] = save_array(cfg.visuals.esp.player.flags.colors);

	config["visuals"]["esp"]["world"]["bomb"]["draw"] = cfg.visuals.esp.world.bomb.draw;

	config["visuals"]["esp"]["outline"]["draw"] = cfg.visuals.esp.outline.draw;
	config["visuals"]["esp"]["outline"]["color"] = save_color(cfg.visuals.esp.outline.color);

	config["visuals"]["anti_flash"] = cfg.visuals.anti_flash;
	config["visuals"]["force_crosshair"] = cfg.visuals.force_crosshair;

	config["visuals"]["fov"]["enable"] = cfg.visuals.fov.enable;
	config["visuals"]["fov"]["value"] = cfg.visuals.fov.value;

	config["legitbot"]["enable"] = cfg.legitbot.enable;
	config["legitbot"]["early_shot"] = cfg.legitbot.early_shot;
	config["legitbot"]["scope_check"] = cfg.legitbot.scope_check;

	config["legitbot"]["configs"]["pistol"] = save_weapon_cfg(cfg.legitbot.configs.pistol);
	config["legitbot"]["configs"]["submachine_gun"] = save_weapon_cfg(cfg.legitbot.configs.submachine_gun);
	config["legitbot"]["configs"]["rifle"] = save_weapon_cfg(cfg.legitbot.configs.rifle);
	config["legitbot"]["configs"]["shotgun"] = save_weapon_cfg(cfg.legitbot.configs.shotgun);
	config["legitbot"]["configs"]["sniper_rifle"] = save_weapon_cfg(cfg.legitbot.configs.sniper_rifle);
	config["legitbot"]["configs"]["machine_gun"] = save_weapon_cfg(cfg.legitbot.configs.machine_gun);

	config["misc"]["bunny_hop"]["enable"] = cfg.misc.bunny_hop.enable;
	config["misc"]["bunny_hop"]["bind"] = cfg.misc.bunny_hop.bind;

	if (!std::filesystem::exists(m_config_path)) {
		if (!std::filesystem::create_directories(m_config_path))
			return false;
	}

	std::string file_path = std::format("{}\\{}.json", m_config_path, cfg_name);

	std::ofstream file(file_path);
	if (file.is_open()) {
		file << config.dump(4);
		file.close();

		return true;
	}
	
	return false;
}

bool c_config_manager::load(const std::string& cfg_name) {
	std::string file_path = std::format("{}\\{}.json", m_config_path, cfg_name);

	if (std::filesystem::exists(file_path)) {
		std::ifstream file(file_path);
		if (file.is_open()) {
			json config{};
			file >> config;

			if (config.contains("ui") && config["ui"].is_object()) {
				auto& ui = config["ui"];

				if (ui.contains("accent_color") && ui["accent_color"].is_array() && ui["accent_color"].size() == 4)
					cfg.ui.accent_color = load_color(ui["accent_color"]);

				cfg.ui.menu_keybind = ui.value("menu_keybind", cfg.ui.menu_keybind);

				if (ui.contains("watermark") && ui["watermark"].is_object()) {
					auto& watermark = ui["watermark"];

					cfg.ui.watermark.draw = watermark.value("draw", cfg.ui.watermark.draw);
					cfg.ui.watermark.position_num = watermark.value("position_num", cfg.ui.watermark.position_num);

					if (watermark.contains("elements") && watermark["elements"].is_array())
						cfg.ui.watermark.elements = load_array<bool>(watermark["elements"]);
				}
			}

			if (config.contains("visuals") && config["visuals"].is_object()) {
				auto& visuals = config["visuals"];

				cfg.visuals.font_num = visuals.value("font_num", cfg.visuals.font_num);

				if (visuals.contains("esp") && visuals["esp"].is_object()) {
					auto& esp = visuals["esp"];

					if (esp.contains("player") && esp["player"].is_object()) {
						auto& player = esp["player"];

						if (player.contains("tracer") && player["tracer"].is_object()) {
							auto& tracer = player["tracer"];

							cfg.visuals.esp.player.tracer.draw = tracer.value("draw", cfg.visuals.esp.player.tracer.draw);

							if (tracer.contains("color") && tracer["color"].is_array() && tracer["color"].size() == 4)
								cfg.visuals.esp.player.tracer.color = load_color(tracer["color"]);
						}

						if (player.contains("rect") && player["rect"].is_object()) {
							auto& rect = player["rect"];

							cfg.visuals.esp.player.rect.draw = rect.value("draw", cfg.visuals.esp.player.rect.draw);
							cfg.visuals.esp.player.rect.mode = rect.value("mode", cfg.visuals.esp.player.rect.mode);

							if (rect.contains("color") && rect["color"].is_array() && rect["color"].size() == 4)
								cfg.visuals.esp.player.rect.color = load_color(rect["color"]);
						}

						if (player.contains("health") && player["health"].is_object()) {
							auto& health = player["health"];

							cfg.visuals.esp.player.health.draw = health.value("draw", cfg.visuals.esp.player.health.draw);

							if (health.contains("bar_color") && health["bar_color"].is_array() && health["bar_color"].size() == 4)
								cfg.visuals.esp.player.health.bar_color = load_color(health["bar_color"]);

							if (health.contains("text_color") && health["text_color"].is_array() && health["text_color"].size() == 4)
								cfg.visuals.esp.player.health.text_color = load_color(health["text_color"]);
						}

						if (player.contains("nickName") && player["nickName"].is_object()) {
							auto& nickName = player["nickName"];

							cfg.visuals.esp.player.nickName.draw = nickName.value("draw", cfg.visuals.esp.player.nickName.draw);

							if (nickName.contains("color") && nickName["color"].is_array() && nickName["color"].size() == 4)
								cfg.visuals.esp.player.nickName.color = load_color(nickName["color"]);
						}

						if (player.contains("skeleton") && player["skeleton"].is_object()) {
							auto& skeleton = player["skeleton"];

							cfg.visuals.esp.player.skeleton.draw = skeleton.value("draw", cfg.visuals.esp.player.skeleton.draw);
							cfg.visuals.esp.player.skeleton.visible_check = skeleton.value("visible_check", cfg.visuals.esp.player.skeleton.visible_check);

							if (skeleton.contains("color") && skeleton["color"].is_array() && skeleton["color"].size() == 4)
								cfg.visuals.esp.player.skeleton.color = load_color(skeleton["color"]);

							if (skeleton.contains("visible_color") && skeleton["visible_color"].is_array() && skeleton["visible_color"].size() == 4)
								cfg.visuals.esp.player.skeleton.visible_color = load_color(skeleton["visible_color"]);

							if (skeleton.contains("invisible_color") && skeleton["invisible_color"].is_array() && skeleton["invisible_color"].size() == 4)
								cfg.visuals.esp.player.skeleton.invisible_color = load_color(skeleton["invisible_color"]);
						}

						if (player.contains("weapon") && player["weapon"].is_object()) {
							auto& weapon = player["weapon"];

							cfg.visuals.esp.player.weapon.draw = weapon.value("draw", cfg.visuals.esp.player.weapon.draw);

							if (weapon.contains("modes") && weapon["modes"].is_array())
								cfg.visuals.esp.player.weapon.modes = load_array<bool>(weapon["modes"]);

							if (weapon.contains("colors") && weapon["colors"].is_array())
								cfg.visuals.esp.player.weapon.colors = load_array<ImColor>(weapon["colors"]);
						}

						if (player.contains("flags") && player["flags"].is_object()) {
							auto& flags = player["flags"];

							cfg.visuals.esp.player.flags.draw = flags.value("draw", cfg.visuals.esp.player.flags.draw);

							if (flags.contains("modes") && flags["modes"].is_array())
								cfg.visuals.esp.player.flags.modes = load_array<bool>(flags["modes"]);

							if (flags.contains("colors") && flags["colors"].is_array())
								cfg.visuals.esp.player.flags.colors = load_array<ImColor>(flags["colors"]);
						}
					}

					if (esp.contains("world") && esp["world"].is_object()) {
						auto& world = esp["world"];

						if (world.contains("bomb") && world["bomb"].is_object()) {
							auto& bomb = world["bomb"];

							cfg.visuals.esp.world.bomb.draw = bomb.value("draw", cfg.visuals.esp.world.bomb.draw);
						}
					}

					if (esp.contains("outline") && esp["outline"].is_object()) {
						auto& outline = esp["outline"];

						cfg.visuals.esp.outline.draw = outline.value("draw", cfg.visuals.esp.outline.draw);

						if (outline.contains("color") && outline["color"].is_array() && outline["color"].size() == 4)
							cfg.visuals.esp.outline.color = load_color(outline["color"]);
					}
				}

				cfg.visuals.anti_flash = visuals.value("anti_flash", cfg.visuals.anti_flash);
				cfg.visuals.force_crosshair = visuals.value("force_crosshair", cfg.visuals.force_crosshair);

				if (visuals.contains("fov") && visuals["fov"].is_object()) {
					auto& fov = visuals["fov"];

					cfg.visuals.fov.enable = fov.value("enable", cfg.visuals.fov.enable);
					cfg.visuals.fov.value = fov.value("value", cfg.visuals.fov.value);
				}
			}

			if (config.contains("legitbot") && config["legitbot"].is_object()) {
				auto& legitbot = config["legitbot"];

				cfg.legitbot.enable = legitbot.value("enable", cfg.legitbot.enable);
				cfg.legitbot.early_shot = legitbot.value("early_shot", cfg.legitbot.early_shot);
				cfg.legitbot.scope_check = legitbot.value("scope_check", cfg.legitbot.scope_check);

				if (legitbot.contains("configs") && legitbot["configs"].is_object()) {
					auto& configs = legitbot["configs"];

					if (configs.contains("pistol") && configs["pistol"].is_object())
						cfg.legitbot.configs.pistol = load_weapon_cfg(configs["pistol"]);

					if (configs.contains("submachine_gun") && configs["submachine_gun"].is_object())
						cfg.legitbot.configs.submachine_gun = load_weapon_cfg(configs["submachine_gun"]);

					if (configs.contains("rifle") && configs["rifle"].is_object())
						cfg.legitbot.configs.rifle = load_weapon_cfg(configs["rifle"]);

					if (configs.contains("shotgun") && configs["shotgun"].is_object())
						cfg.legitbot.configs.shotgun = load_weapon_cfg(configs["shotgun"]);

					if (configs.contains("sniper_rifle") && configs["sniper_rifle"].is_object())
						cfg.legitbot.configs.sniper_rifle = load_weapon_cfg(configs["sniper_rifle"]);

					if (configs.contains("machine_gun") && configs["machine_gun"].is_object())
						cfg.legitbot.configs.machine_gun = load_weapon_cfg(configs["machine_gun"]);
				}
			}

			if (config.contains("misc") && config["misc"].is_object()) {
				auto& misc = config["misc"];

				if (misc.contains("bunny_hop") && misc["bunny_hop"].is_object()) {
					auto& bunny_hop = misc["bunny_hop"];

					cfg.misc.bunny_hop.enable = bunny_hop.value("enable", cfg.misc.bunny_hop.enable);
					cfg.misc.bunny_hop.bind = bunny_hop.value("bind", cfg.misc.bunny_hop.bind);
				}
			}

			return true;
		}
	}

	return false;
}

bool c_config_manager::create(const std::string& cfg_name) {
	std::string file_path = std::format("{}\\{}.json", m_config_path, cfg_name);

	if (!std::filesystem::exists(m_config_path)) {
		if (!std::filesystem::create_directories(m_config_path))
			return false;
	}

	if (!std::filesystem::exists(file_path)) {
		std::ofstream(file_path) << "";
		return true;
	}

	return false;
}

bool c_config_manager::remove(const std::string& cfg_name) {
	std::string file_path = std::format("{}\\{}.json", m_config_path, cfg_name);

	if (!std::filesystem::exists(m_config_path) || !std::filesystem::exists(file_path))
		return false;

	std::filesystem::remove(file_path);
	return true;
}