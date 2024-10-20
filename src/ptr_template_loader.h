#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"
#include "TaskScheduler.h"

#define module_string "ptr-template"

TaskScheduler scheduler;

void SendTalentReset(Player* player)
{
    WorldPacket data(SMSG_TALENTS_INVOLUNTARILY_RESET, 1);
    data << uint8(0);
    player->SendMessageToSet(&data, true);
}

void Add_ptr_template();
void AddSC_ptr_template_commandscript();

void Addmod_ptr_templateScripts()
{
    Add_ptr_template();
    AddSC_ptr_template_commandscript();
}
