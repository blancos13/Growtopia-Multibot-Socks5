
#include "Functions.h"
#include "../Utils.h"
#include "../ENetClient.h"

void CheatData::OnUpdate() {
	ENetClient* bot = (ENetClient*)client;
	if (!bot->InWorld())
		return;

	uint64_t time = Utils::GetTime();

	if (auto_c && auto_c_last < time) {
		auto_c_last = time + auto_c_d;
		bot->Collect(auto_c_r, auto_c_f);
	}

	if (auto_f && auto_f_last < time && auto_f_id > -1 && (auto_f_p || auto_f_b)) {
		switch (auto_f_m) {
		case 0:
		case 1:
		case 2:
		case 3: {
			int offset = auto_f_m == 2 ? 2 : auto_f_m - 2;
			if (auto_f_p) {
				if (auto_f_bhit > 0) {
					if (auto_f_step > auto_f_bhit) {
						auto_f_step = 1;
						auto_f_last = time + auto_f_pd;
						bot->Hit(0, offset, auto_f_id);
					}
				}
				else {
					if (auto_f_step > 2) {
						auto_f_step = 1;
						auto_f_last = time + auto_f_pd - 10;
						bot->Hit(0, offset, auto_f_id);
					}
				}
			}
			if (auto_f_b && auto_f_last < time) {
				auto_f_step++;
				auto_f_last = time + auto_f_bd;
				bot->Hit(0, offset, 18);
			}
		} break;
		}
	}

	if (auto_s && auto_s_last < time && auto_s_t.size() > 0) {
		auto_s_last = time + auto_s_d + rand() % 100;
		if (bot->local.netid != 0 && bot->local.worldname != "") {
			bot->SendPacket(2, "action|input\n|text|" + auto_s_t[auto_s_step % auto_s_t.size()]);
			auto_s_step++;
			if (auto_s_step > auto_s_t.size())
				auto_s_step = 1;
		}
	}
}

//logging options
namespace opt {
	bool enable_blacklist = false;
	std::vector<std::string> accnames;
	std::vector<std::string> banlogs;

	char details_rpc[200];
}