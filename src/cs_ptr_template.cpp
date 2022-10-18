#include "Chat.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "ptr_template_loader.h"

using namespace Acore::ChatCommands;

class ptr_template_commandscript : public CommandScript
{
public:
	ptr_template_commandscript() : CommandScript("ptr_template_commandscript") { }
	
	ChatCommandTable GetCommands() const override
	{
		static ChatCommandTable templateTable =
		{
			{ "enable", enableTemplate, SEC_MODERATOR, Console::Yes },
			{ "disable", disableTemplate, SEC_MODERATOR, Console::Yes },
			{ "", applyTemplate, SEC_PLAYER, Console::No },
		};
		
		static ChatCommandTable commandTable =
		{
			{ "template", templateTable },
		};
		return commandTable;
	}

	static bool enableTemplate(ChatHandler* handler, uint32 index)
	{
		QueryResult result = WorldDatabase.Query("SELECT Comment FROM mod_ptrtemplate_index WHERE ID={}", index);
		WorldDatabase.Execute("UPDATE mod_ptrtemplate_index SET Enable=1 WHERE ID={}", index);
		Field* fields = result->Fetch();
		std::string comment = fields[0].Get<std::string>();
		handler->PSendSysMessage("Set index %u enable flag(%s) to enabled.", index, comment);
		return true;
	}

	static bool disableTemplate(ChatHandler* handler, uint32 index)
	{
		QueryResult result = WorldDatabase.Query("SELECT Comment FROM mod_ptrtemplate_index WHERE ID={}", index);
		WorldDatabase.Execute("UPDATE mod_ptrtemplate_index SET Enable=1 WHERE ID={}", index);
		Field* fields = result->Fetch();
		std::string comment = fields[0].Get<std::string>();
		handler->PSendSysMessage("Set index %u enable flag(%s) to disabled.", index, comment);
		return true;
	}

	static bool applyTemplate(ChatHandler* handler, Optional<PlayerIdentifier> player, uint32 index)
	{
        QueryResult check = WorldDatabase.Query("SELECT Enable FROM mod_ptrtemplate_index WHERE ID={}", index);
        uint8 enable = (*check)[0].Get<uint8>();
        if (enable == 1)
        {
            if (!player)
            {
                player = PlayerIdentifier::FromTargetOrSelf(handler);
            }
            Player* target = player->GetConnectedPlayer();
            // createTemplate::AddTemplateLevel(target, index);
            // createTemplate::AddTemplatePosition(target, index);
            // createTemplate::AddTemplateReputation(target, index);
            // createTemplate::AddTemplateHotbar(target, index); I CAN'T FIGURE THIS OUT GOD HELP ME
            // createTemplate::AddTemplateWornGear(target, index);
            // createTemplate::AddTemplateBagGear(target, index);
            // createTemplate::AddTemplateSkills(target, index);
            // createTemplate::AddTemplateSpells(target, index);
            handler->PSendSysMessage("I don't know what the heck you expect at this point lol");
        }
		return true;
	}
};

void AddSC_ptr_template_commandscript()
{
	new ptr_template_commandscript();
}
