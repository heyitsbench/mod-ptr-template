#include "ptr_template_loader.h"
#include "Chat.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"


class createPTR : public WorldScript {

public:
    createPTR() : WorldScript("createPTRScript") { }

    void OnStartup() override
    {
        uint32 oldMSTime = getMSTime();
        uint32 count = 0;

        LOG_ERROR("sql.sql", "Loading index entries for the PTR template module...");
        QueryResult result = WorldDatabase.Query("SELECT ID FROM mod_ptrtemplate_index;");

        if (!result)
        {
            LOG_ERROR("sql.sql", "No index entries are available for the PTR template module to use.");
            return;
        }
        do
        {
            Field* fields = result->Fetch();

            ++count;
        } while (result->NextRow());
        LOG_ERROR("module", ">> Loaded {} templates in {} ms.", count, GetMSTimeDiffToNow(oldMSTime));
        return;
    }
};

class announce : public PlayerScript {

public:
    announce() : PlayerScript("announce") { }

    void OnLogin(Player* player) override
    {
        if (sConfigMgr->GetOption<bool>("Announce.enable", true))
        {
            ChatHandler(player->GetSession()).SendSysMessage("This server is running the PTR Template module.");
        }
    }
};

void Add_ptr_template()
{
    new createPTR();
    new announce();
}
