#include <cstdio>
#include "discord-rpc/win64-static/include/discord_rpc.h"
#include "src/Utils.h"

#pragma comment(lib, "ws2_32.lib")

static void DiscordRpc()
{
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    Discord_Initialize("1083458744186327192", &handlers, 1, NULL);

    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(presence));
    presence.state = "Growtopia Bot Application ZeroByte#1337";
    presence.details = opt::details_rpc;

    presence.largeImageKey = "";
    presence.largeImageText = "";
    presence.smallImageKey = "";
    presence.smallImageText = "";





    Discord_UpdatePresence(&presence);

    /*while (true)
    {

           Discord_RunCallbacks();
           Sleep(2000);

    }
    Discord_Shutdown();*/
}