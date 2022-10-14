#include "Chat.h"
#include "ScriptMgr.h"
#include "Player.h"

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

	static bool applyTemplate(ChatHandler* handler)
	{
		return true;
	}
};

void AddSC_ptr_template_commandscript()
{
	new ptr_template_commandscript();
}