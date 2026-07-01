#include "core.hpp"

#include "data/globals.hpp"
#include "data/config.hpp"
#include "data/patterns.hpp"

#include "utils/log.hpp"
#include "utils/memory.hpp"

#include <thread>
#include <chrono>
#include <lmcons.h>
#include <format>

#define fsleep(sec) std::this_thread::sleep_for(std::chrono::seconds(sec))

void c_cheat_core::init() {
	SetConsoleTitleA("nyaware cs2 [external]");

	char username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserNameA(username, &username_len);

	LOG("+ | welcome back, %s!", username);

	LOG("");

	int pid{};

	LOGI("waiting the game process..");
	do {
		pid = mem.get_processID("cs2.exe");
		fsleep(1);
	} while (pid <= 0);

	LOGI("found! pid: %i | wait for initialization..", pid);
	if (!mem.init(pid)) {
		LOG("");

		LOGE("error while initializing memory manager. please restart program");
		std::exit(1);
	}

	do {
		g.modules.client = dll_t(mem.findDLL(pid, "client.dll"));
		g.modules.schemasystem = dll_t(mem.findDLL(pid, "schemasystem.dll"));

		fsleep(1);
	} while (!g.modules.isValid());

	this->processID = pid;

	g.uinterface.init(&g.screen);
	g.schema_dumper.dump(mem.resolve_pattern(g.modules.schemasystem.base, g.modules.schemasystem.size, signatures::dwSchemaSystemInterface));

	LOGI("initialization success. enjoy!");
	LOGD("client.dll: 0x%lx", g.modules.client.base);
	LOGD("schemasystem.dll: 0x%lx", g.modules.schemasystem.base);

	this->update();

	g.uinterface.shutdown();
}

void c_cheat_core::update() {
	while (true) {
		if (g.panic) {
			LOGI("panic");

			static bool skipped_frame = false;
			
			if (skipped_frame)
				break;

			if (!skipped_frame)
				skipped_frame = true;
		}

		if (!g.modules.isValid()) {
			LOGI("game closed. bye!");
			break;
		}

		g.uinterface.newFrame();
		g.uinterface.render();
		g.uinterface.endFrame();
	}
}