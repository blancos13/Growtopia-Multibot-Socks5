

#include "wrapper/HTTPRequest.hpp"
#include "ENetClient.h"
#include "Utils.h"
#include "Struct.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include "../httplib.h"
#include <windows.h>
#include "proton/rtparam.hpp"
using namespace std;


class PuzzleSolver {
public:
	bool Solved = false;
	std::string LatestAnswer = "";
	PuzzleSolver(std::string g) : SecretKey(g) {}
	void AddParams(std::string Key, std::string Value)
	{
		Params.append(Key + "=" + Value + "&");
	}

	std::string GetAnswer(std::string CaptchaUID)
	{
		AddParams("Action", "Solve");
		AddParams("Puzzle", CaptchaUID);
		AddParams("Secret", SecretKey);
		AddParams("Format", "txt");
		std::string FullUrl = API + Path + Params;
		http::Request request{ std::string{FullUrl}.c_str() };
		const auto response = request.send("GET");
		std::string captchaAnswer = std::string{ response.body.begin(), response.body.end() };
		Solved = captchaAnswer.find("Failed") == std::string::npos && captchaAnswer.length() > 6 && response.status == 202;

		return LatestAnswer = (captchaAnswer.length() > 6) ? captchaAnswer.erase(0, 7) : "Failed";

	}
private:
	std::string SecretKey;
	std::string API = "http://api.surferwallet.net/";
	std::string Path = "Captcha?";
	std::string Params = "";
};


// PersonalSetting

int PersonalSetting::GetCbits() {
	int m_ret = 0;
	
	m_ret |= disable_public_broadcast << 5;
	m_ret |= disable_adding_friends << 1;
	m_ret |= disable_guild_invites << 6;
	m_ret |= disable_guild_flag << 7;
	m_ret |= disable_player_text << 2;
	m_ret |= disable_billboard << 8;
	m_ret |= use_classic_store << 9;
	m_ret |= disable_in_app << 3;
	m_ret |= disable_tapjoy << 4;
	
	return m_ret;
}

// ClientData
int protocol = 188;
float game_version = 4.22;
int player_age = 25;

ClientData::ClientData() {
	rid = Utils::Random(32);
	mac = Utils::GenerateMac();
}

bool ClientData::Flush() {
	try {
		using namespace httplib;
		Headers Header;
		Header.insert(std::make_pair("User-Agent", "UbiServices_SDK_2019.Release.27_PC64_unicode_static"));
		Header.insert(std::make_pair("Host", "www.growtopia1.com"));
		Client cli("https://104.125.3.135");
		cli.set_default_headers(Header);
		cli.enable_server_certificate_verification(false);
		cli.set_connection_timeout(2, 0);
		auto res = cli.Post("/growtopia/server_data.php");

		for (auto& nln : Utils::Split(res->body, "\n")) {
			auto split = Utils::Split(nln, "|");
			if (split.size() < 2)
				continue;
			if (split[0] == "server")
				ip = split[1];
			else if (split[0] == "port")
				port = std::stoi(split[1]);
			else if (split[0] == "meta")
				meta = split[1];
		}
		
#ifdef _CONSOLE
		std::cout << "flushed " << ip << ":" << port << std::endl;
#endif
		return true;
	} catch (const std::exception&) {
		return false;
	}
}



void ClientData::Set(variantlist_t& var) {
	port = var[1].get_int32();

	if (var[2].get_int32() != -1)
		token = var[2].get_int32();

	user = var[3].get_int32();

	auto split = Utils::Split(var[4].get_string(), "|");
	if (split.size() == 3) {
		ip = split[0];
		doorid = split[1];
		if (split[2] != "-1")
			uuid = split[2];
	}

	lmode = var[5].get_int32();
#ifdef _CONSOLE
	std::cout << "redirected " << ip << ":" << port << std::endl;
#endif
}

void ClientData::Reset() {
	ip = "";
	port = 0;

	lmode = 0;

	meta = "";

	user = 0;
	token = 0;

	uuid = "";
	doorid = "";
}



std::string ClientData::Create() {
	std::stringstream m_ret;

	if (name != "" && pass != "") {
		m_ret << "tankIDName|" << name << "\n";
		m_ret << "tankIDPass|" << pass << "\n";
	}

	//m_ret << "requestedName|" << "555" << "\n";
	//m_ret << "f|" << personal.filter_innapropriate << "\n";
	//m_ret << "protocol|" << protocol << "\n";
	//m_ret << "game_version|" << game_version << "\n";
	//m_ret << "lmode|" << lmode << "\n";
	//m_ret << "cbits|" << personal.GetCbits() << "\n";
	//m_ret << "player_age|" << player_age << "\n";
	//m_ret << "GDPR|" << (int)1 << "\n";
	//m_ret << "category|" << "_-5100gid" << "\n";
	//m_ret << "tr|" << (int)4322 << "\n";
	//m_ret << "meta|" << meta << "\n";
	//m_ret << "fhash|" << (int)-716928004 << "\n";
	//m_ret << "rid|" << rid << "\n";
	//m_ret << "platformID|" << (int)4 << "\n";
	//m_ret << "deviceVersion|" << (int)0 << "\n";
	//m_ret << "country|" << "ru" << "\n";
	//m_ret << "hash|" << (int)1225092726 << "\n";
	//m_ret << "mac|" << mac << "\n";

	if (user != 0 && token != 0) {
		m_ret << "user|" << user << "\n";
		m_ret << "token|" << token << "\n";
	}

	if (uuid != "")
		m_ret << "UUIDToken|" << uuid << "\n";

	if (doorid != "")
		m_ret << "doorID|" << doorid << "\n";

	m_ret << "wk|NONE0" << "\n";

	return m_ret.str();
}



// ENetClient

ENetClient::ENetClient(std::string alpha, std::string beta) {
	cheat.client = this;
	data.name = alpha;
	data.pass = beta;
	pathfinder = new Pathfinder(&local.tiles);
	Init();
}



bool ENetClient::InWorld() {
	return (local.worldname != "" && local.netid != 0);
}


bool ENetClient::FindPath(int x, int y) {
	if (!InWorld())
		return false;

	finding_path = true;
	int fx = floor((local.pos.m_x + 10) / 32);
	int fy = floor((local.pos.m_y + 15) / 32);

	pathfinder->Update(local.width, local.height);
	std::vector<Node> result = pathfinder->GetBestPath(fx, fy, x, y);

	if (result.size() > 0) {
		int counter = 0;
		TankPacketStruct tank;
		tank.type = 0;
		for (auto& node : result) {
			if (callback)
				break;
			if (!InWorld())
				break;
			if ((counter + 1) % 4 != 0 && rand() % 10 != counter) { // lol
				counter++;
				continue;
			}
			counter = 0;
			tank.x = (node.x) * 32 + 6;
			tank.y = (node.y) * 32 + 2;
			local.pos.m_x = tank.x;
			local.pos.m_y = tank.y;
			SendPacketRaw(4, (uint8_t*)&tank, 0);
		}

		if (!callback && InWorld() && counter != 0) {
			tank.x = (result[result.size() - 1].x) * 32 + 6;
			tank.y = (result[result.size() - 1].y) * 32 + 2;
			local.pos.m_x = tank.x;
			local.pos.m_y = tank.y;
			SendPacketRaw(4, (uint8_t*)&tank, 0);
		}
	}

	bool ret = (!callback && result.size() > 0) || (fx == x && fy == y);
	callback = false;
	finding_path = false;
	return ret;
}



void ENetClient::Collect(int radius, bool force) {
	if (!InWorld())
		return;
	for (auto& obj : local.objects) {
		if (floor(local.pos.distance(obj.pos) / 32) <= radius) {
			TankPacketStruct col;
			col.type = 11;
			col.x = obj.pos.m_x;
			col.y = obj.pos.m_y;
			col.value = obj.oid;
			SendPacketRaw(4, (uint8_t*)&col, 0);
			if (!force)
				break;
		}
	}
}

void ENetClient::Door() {
	if (!InWorld())
		return;
	TankPacketStruct door;
	door.type = 7;
	door.tilex = (int)floor((local.pos.m_x + 10) / 32);
	door.tiley = (int)floor((local.pos.m_y + 15) / 32);
	SendPacketRaw(4, (uint8_t*)&door, 0);
}

void ENetClient::Hit(int radx, int rady, int id) {
	if (!InWorld())
		return;
	TankPacketStruct tank;
	tank.type = 3;
	tank.tilex = (int)floor((local.pos.m_x + 10) / 32) + radx;
	tank.tiley = (int)floor((local.pos.m_y + 10) / 32) + rady;
	tank.x = local.pos.m_x;
	tank.y = local.pos.m_y;
	tank.value = id;
	if (abs((tank.x + 10) / 32 - tank.tilex) < 3 && abs((tank.y + 10) / 32 - tank.tiley) < 3)
		SendPacketRaw(4, (uint8_t*)&tank);
}

void ENetClient::Move(int radx, int rady) {
	if (!InWorld())
		return;
	TankPacketStruct tank;
	tank.x = 32 * (floor((local.pos.m_x + 10) / 32) + radx) + 6;
	tank.y = 32 * (floor((local.pos.m_y + 15) / 32) + rady) + 2;
	local.pos.m_x = tank.x;
	local.pos.m_y = tank.y;
	SendPacketRaw(4, (uint8_t*)&tank, 0);
}



int ENetClient::GetState() {
	if (peer)
		return peer->state;
	return 0;
}

int ENetClient::GetPing() {
	if (peer)
		return peer->roundTripTime;
	return 500;
}



void ENetClient::SendPacket(int type, std::string const& text) {
	if (!peer)
		return;
	if (peer->state != ENET_PEER_STATE_CONNECTED)
		return;

	int len = 5 + text.length();

	char* packet = new char[len];

	memset(packet, 0, len);

	memcpy(packet, &type, 4);
	memcpy(packet + 4, text.c_str(), text.length());

	ENetPacket* nPacket = enet_packet_create(packet, len, 1);
	enet_peer_send(peer, 0, nPacket);

	delete[] packet;
}

void ENetClient::SendPacketRaw(int type, uint8_t* ptr, int flags) {
	if (!peer)
		return;
	if (peer->state != ENET_PEER_STATE_CONNECTED)
		return;

	int len = 4 + 56;

	char* packet = new char[len];

	memset(packet, 0, len);

	memcpy(packet, &type, 4);
	memcpy(packet + 4, ptr, 56);

	ENetPacket* nPacket = enet_packet_create(packet, len, flags);
	enet_peer_send(peer, 0, nPacket);

	delete[] packet;
}



void ENetClient::Init() {
	host = enet_host_create(0, 1, 2, 0, 0);
	if (!host) {
#ifdef _CONSOLE
		std::cout << "failed create host" << std::endl;
#endif
		return;
	}
	host->checksum = enet_crc32;
	host->usingNewPacket = true;
	enet_host_compress_with_range_coder(host);
}

void ENetClient::Destroy() {


}



void ENetClient::Connect(bool flush) {
	if (peer)
		if (peer->state != ENET_PEER_STATE_DISCONNECTED)
			return;
	if (flush)
		if (!data.Flush())
			return;
	if (data.ip == "" || data.port == 0)
		return;

	data.captcha = false;

	ENetAddress address;

	enet_address_set_host(&address, data.ip.c_str());
	address.port = data.port;

	peer = enet_host_connect(host, &address, 2, 0);

	if (!peer) {
#ifdef _CONSOLE
		std::cout << "failed connect client" << std::endl;
#endif
		return;
	}

	enet_host_flush(host);
}

void ENetClient::Disconnect(bool reset) {
	if (peer) {
		if (reset)
			SendPacket(3, "action|quit");
		enet_peer_disconnect(peer, 0);
	}
	if (reset) {
		data.Reset();
		local.Reset();
	}
}



void ENetClient::Poll() {
	if (!host)
		return;
	ENetEvent event;
	while (enet_host_service(host, &event, 3) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_NONE:
			break;
		case ENET_EVENT_TYPE_CONNECT: {
			//OnConnect();
		} break;
		case ENET_EVENT_TYPE_DISCONNECT: {
			OnDisconnect();
		} break;
		case ENET_EVENT_TYPE_RECEIVE: {
			OnReceive(event.packet);
			enet_packet_destroy(event.packet);
		} break;
		}
	}
	if (!peer || data.captcha)
		return;
	cheat.OnUpdate();
}


void ENetClient::OnDisconnect() {
	Connect();
}

void ENetClient::OnReceive(ENetPacket* packet) {
	if (packet->dataLength < 4 || !packet->data)
		return;
	int type = *(int*)packet->data;
	switch (type) {
	case 1: {
		SendPacket(2, data.Create());
	} break;
	case 3:
	case 5:
	case 6: {
		packet->data[packet->dataLength - 1] = '\x00';
		OnTextPacket(type, (char*)(packet->data + 4));
	} break;
	case 4: {
		OnTankPacket(type, packet->data + 4, packet->dataLength - 4);
	} break;
	default: {
	} break;
	}
}



void ENetClient::OnTextPacket(int type, std::string text) {
	logger.Text(type, text);

	if (text.find("eventName|100_MOBILE.START") != std::string::npos) {
		for (auto row : Utils::Split(text, "\n")) {
			auto nrow = Utils::Split(row, "|");
			if (nrow.size() < 2)
				continue;
			if (nrow[0] == "Gems_balance")
				local.gems = std::stoi(nrow[1]);
			else if (nrow[0] == "Level")
				local.level = std::stoi(nrow[1]);
		}
	}

	else if (text.find("action|log\nmsg|") != std::string::npos) {
		Utils::Replace(text, "action|log\n|msg|", "");
		if (text.find("`4Unable to create new account for guest") != std::string::npos)
			data.status = "unable to create account guest (IP limited)";
		else if (text.find("`4Unable to log on") != std::string::npos)
			data.status = "unable to logon (GrowID or password wrong)";
		else if (text.find("`4OOPS:`` Too many people logging in at once.") != std::string::npos)
			data.status = "too many people logging in at once";
		else if (text.find("`5The game is currently undergoing maintenance.") != std::string::npos)
			data.status = "game is currently undergoing maintenance";
		else if (text.find("`4Sorry, this account ") != std::string::npos && text.find("has been suspended.") != std::string::npos) {
			data.status = "account suspended";
			Disconnect(true);
		}
		else if (text.find("`4Sorry, this account is currently banned.") != std::string::npos) {
			data.status = "account banned";
			try {
				text = text.substr(text.find("You will have to wait `w") + 24);
				text = text.substr(0, text.find("`` for this ban to expire"));
				data.status += " (" + text + ")";
			}
			catch (std::exception) {}
			Disconnect(true);
		}
		else if (text.find("`4Sorry, this account, device or location") != std::string::npos) {
			data.status = "account, device or location temporarily suspended";
			Disconnect(true);
		}
	}

	else if (text.find("action|logon_fail") != std::string::npos)
		Disconnect(true);
}

void SolveCaptcha(ENetClient* cl, std::string link, std::string captchatitle, int captchaid) {
#ifdef _CONSOLE
	std::cout << "(" << cl->data.name << ") solving captcha: " << link << std::endl;
#endif
	try {
		http::Request request{ link };
		http::Response response = request.send("GET");
		std::ofstream output(captchatitle, std::ios::out | std::ios::binary);
		output.write((char*)&response.body[0], response.body.size());
		output.close();
		response.body.clear();

		RTTEX Image(captchatitle.c_str());

		std::stringstream cstream;
		cstream << Utils::SolveCaptcha(Image);
		std::string result = cstream.str();

		if (result == "-1") {
			cl->data.status = "failed captcha";
		}
		else {
#ifdef _CONSOLE
			std::cout << "(" << cl->data.name << ") captcha solved: " << result << std::endl;
#endif
			cl->SendPacket(2, "action|dialog_return\ndialog_name|puzzle_captcha_submit\ncaptcha_answer|" + result + "|CaptchaID|" + std::to_string(captchaid));
			cl->data.status = "Catpcha Solved : " + result;
			cl->data.captcha = false;
		}

		std::remove(captchatitle.c_str());
	}
	catch (const std::exception&) {
		cl->data.status = "failed captcha";
	}
}

void ENetClient::OnTankPacket(int type, uint8_t* ptr, int size) {
	TankPacketStruct* packet = (TankPacketStruct*)ptr;
	switch (packet->type) {
	case 0: {
		// state
		local.PlayerMove(packet);
	} break;
	case 1: {
		// call
		variantlist_t var{};
		var.serialize_from_mem(ptr + 56);
		std::string func = var[0].get_string();

		if (l && hook_var) {
			if (lua_getglobal(l, "CallHook") == LUA_TFUNCTION) {
				Api::PUSHVAR(l, &var);
				if (lua_pcall(l, 1, 0, 0))
					logger.Text(7, "hook err: " + std::string(luaL_checkstring(l, -1)), 50);
			}
		}

		if (func.find("OnSuperMain") != std::string::npos) {
			SendPacket(2, "action|enter_game\n");
			TankPacketStruct tank;
			tank.type = 24;
			tank.netid = -1;
			tank.value = 1125432991;
			SendPacketRaw(type, (uint8_t*)&tank);
		}

		else if (func == "OnSendToServer") {
			data.Set(var);
			TankPacketStruct tank;
			tank.type = 26;
			tank.netid = -1;
			SendPacketRaw(type, (uint8_t*)&tank);
			Disconnect();
		}

		else if (func == "OnConsoleMessage") {
			std::string console = var[1].get_string();
			logger.Text(1, console);
			if (console.find("wants to add you to") != std::string::npos && console.find("Wrench yourself to accept.") != std::string::npos) {
				if (cheat.auto_aa)
					SendPacket(2, "action|wrench\n|netid|" + std::to_string(local.netid) + "\n");
			}
		}
		else if (func == "OnSpawn") {
			std::string spawn = var[1].get_string();
			local.PlayerSpawn(spawn);
			if (spawn.find("type|local") != std::string::npos)
				local.InitOnSpawn(spawn);
		}
		else if (func == "OnRemove") {
			std::string remove = var[1].get_string();
			local.PlayerRemove(remove);
		}

		else if (func == "OnSetPos") {
			if (finding_path)
				callback = true;
			local.pos = var[1].get_vector2();
		}

		else if (func == "onShowCaptcha") {
			auto split = Utils::Split(var[1].get_string(), "|");
			data.status = "solving captcha";
			std::string captchaid = split[1];
			Utils::Replace(captchaid, "0098/captcha/generated/", "");
			Utils::Replace(captchaid, "PuzzleWithMissingPiece.rttex", "");
			captchaid = captchaid.substr(0, captchaid.size() - 1);
			std::string SecretCode = "340133be-ceb5-4894-b099-004e4047d98ef9f860dc252e491"; //secret code, you can buy at  https://surferwallet.net/SurferShop
			auto x = PuzzleSolver(SecretCode);
			x.GetAnswer(captchaid);
			if (x.Solved) {
				SendPacket(2, "action|dialog_return\ndialog_name|puzzle_captcha_submit\ncaptcha_answer|" + x.LatestAnswer + "|CaptchaID|" + split[4]);
			}
		}
		else if (func == "OnRequestWorldSelectMenu")
			local.ExitWorld();

		else if (func == "SetHasGrowID") {
			data.name = var[2].get_string();
			data.pass = var[3].get_string();
		}

		else if (func == "OnDialogRequest") {
			std::string dialog = var[1].get_string();

			if (dialog.find("end_dialog|popup||Continue|") != std::string::npos) {
				if (dialog.find("add_button|acceptlock|") != std::string::npos)
					SendPacket(2, "action|dialog_return\ndialog_name|popup\nnetID|" + std::to_string(local.netid) + "|\nbuttonClicked|acceptlock");
			}

			else if (dialog.find("end_dialog|acceptaccess|No|Yes|") != std::string::npos)
				SendPacket(2, "action|dialog_return\ndialog_name|acceptaccess");

			else if (dialog.find("end_dialog|unaccess|No|Yes|") != std::string::npos)
				SendPacket(2, "action|dialog_return\ndialog_name|unaccess");

			else if (dialog.find("end_dialog|drop_item|Cancel|OK|") != std::string::npos) {
				if (dt_amount > 0 && dialog.find("embed_data|itemID|") != std::string::npos) {
					std::string itemid = dialog.substr(dialog.find("embed_data|itemID|") + 18, dialog.length() - dialog.find("embed_data|itemID") - 1);
					SendPacket(2, "action|dialog_return\ndialog_name|drop_item\nitemID|" + itemid + "|\ncount|" + std::to_string(dt_amount));
					dt_amount = 0;
				}
			}
			else if (dialog.find("end_dialog|trash_item|Cancel|OK|") != std::string::npos) {
				if (dt_amount > 0 && dialog.find("embed_data|itemID|") != std::string::npos) {
					std::string itemid = dialog.substr(dialog.find("embed_data|itemID|") + 18, dialog.length() - dialog.find("embed_data|itemID") - 1);
					SendPacket(2, "action|dialog_return\ndialog_name|trash_item\nitemID|" + itemid + "|\ncount|" + std::to_string(dt_amount));
					dt_amount = 0;
				}
			}
		}

		else if (func == "OnTextOverlay") {
			std::string toast = var[1].get_string();
			if (toast.find("You'd be sorry if you lost that!") != std::string::npos)
				dt_amount = 0;
			else if (toast.find("You can't drop that.") != std::string::npos)
				dt_amount = 0;
			else if (toast.find("You can't drop that here.") != std::string::npos)
				dt_amount = 0;
		}
	} break;
	case 3: {
		local.Update3(packet);
	} break;
	case 4: {
		local.Serialize4(ptr);
	} break;
	case 9: {
		local.Serialize9(ptr);
	} break;
	case 12: {
		Tile* tile = local.GetTile(packet->tilex, packet->tiley);
		if (tile)
			tile->fg = 0; // hold on
	} break;
	case 13: {
		local.Update13(packet);
	} break;
	case 14: {
		local.Update14(packet);
	} break;
	case 22: {
		TankPacketStruct tank;
		tank.type = 21;
		tank.x = 64;
		tank.y = 64;
		tank.value = packet->value + 5000;
		SendPacketRaw(type, (uint8_t*)&tank);
	} break;
	default: {

	} break;
	}
}


