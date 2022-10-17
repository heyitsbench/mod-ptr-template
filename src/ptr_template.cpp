#include "ptr_template_loader.h"
#include "Chat.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "ReputationMgr.cpp"
#include "Object.h"


class createPTR : public WorldScript {

public:
    createPTR() : WorldScript("createPTR") { }

    void OnBeforeWorldInitialized() override
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

class createTemplate : public PlayerScript {

public:
    createTemplate() : PlayerScript("createTemplate") { }

    void AddTemplateLevel(Player* player, uint32 index)
    { //                                                  0      1
        QueryResult check = WorldDatabase.Query("SELECT Level, Enable FROM mod_ptrtemplate_index WHERE ID={}", index);
        uint8 enable = (*check)[1].Get<uint8>();
        uint8 levelEntry = (*check)[0].Get<uint8>();
        if (enable == 1 && (player->getLevel() == (player->getClass() != CLASS_DEATH_KNIGHT
            ? sWorld->getIntConfig(CONFIG_START_PLAYER_LEVEL)
            : sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL))))
        {
            player->SetLevel(levelEntry);
        }
    }

    void AddTemplatePosition(Player* player, uint32 index)
    { //                                                     0           1          2          3          4         5        6       7       8       9
        QueryResult check = WorldDatabase.Query("SELECT MapAlliance, XAlliance, YAlliance, ZAlliance, OAlliance, MapHorde, XHorde, YHorde, ZHorde, OHorde, Enable FROM mod_ptrtemplate_index WHERE ID={}", index);
        uint8 enable = (*check)[9].Get<uint8>();
        uint16 mapAllianceEntry = (*check)[0].Get<uint16>();
        float XAllianceEntry = (*check)[1].Get<float>();
        float YAllianceEntry = (*check)[2].Get<float>();
        float ZAllianceEntry = (*check)[3].Get<float>();
        float OAllianceEntry = (*check)[4].Get<float>();
        uint16 mapHordeEntry = (*check)[5].Get<uint16>();
        float XHordeEntry = (*check)[6].Get<float>();
        float YHordeEntry = (*check)[7].Get<float>();
        float ZHordeEntry = (*check)[8].Get<float>();
        float OHordeEntry = (*check)[9].Get<float>();
        if (player->GetTeamId() == TEAM_HORDE)
        {
            player->TeleportTo(mapHordeEntry, XHordeEntry, YHordeEntry, ZHordeEntry, OHordeEntry);
        }
        else
        {
            player->TeleportTo(mapAllianceEntry, XAllianceEntry, YAllianceEntry, ZAllianceEntry, OAllianceEntry);
        }
    }

    void AddTemplateReputation(Player* player, uint32 index)
    { //                                                  0
        QueryResult check = WorldDatabase.Query("SELECT Enable FROM mod_ptrtemplate_index WHERE ID={}", index);
        uint8 enable = (*check)[0].Get<uint8>(); //          0          1          2          3
        QueryResult repInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, FactionID, Standing FROM mod_ptrtemplate_reputations WHERE ID={}", index);
        if (enable == 1)
        {
            do
            {
                uint16 raceMaskEntry = (*repInfo)[0].Get<uint16>();
                uint16 classMaskEntry = (*repInfo)[1].Get<uint16>();
                uint16 factionEntry = (*repInfo)[2].Get<uint16>();
                int32 standingEntry = (*repInfo)[3].Get<int32>();
                if (!(raceMaskEntry & player->getRaceMask()))
                    break;
                if (!(classMaskEntry & player->getClassMask()))
                    break;
                player->SetReputation(factionEntry, standingEntry);
            } while (check->NextRow());
        }
    }

    void AddTemplateHotbar(Player* player, uint32 index)
    { //                                                  0
        QueryResult check = WorldDatabase.Query("SELECT Enable FROM mod_ptrtemplate_index WHERE ID={}", index);
        uint8 enable = (*check)[0].Get<uint8>(); //          0          1        2       3      4
        QueryResult barInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, Button, Action, Type FROM mod_ptrtemplate_action WHERE ID={}", index);
        if (enable == 1)
        {
            do
            {
                uint16 raceMaskEntry = (*barInfo)[0].Get<uint16>();
                uint16 classMaskEntry = (*barInfo)[1].Get<uint16>();
                uint8 buttonEntry = (*barInfo)[2].Get<uint8>();
                uint32 actionEntry = (*barInfo)[3].Get<uint32>();
                uint8 typeEntry = (*barInfo)[4].Get<uint8>();
                if (!(raceMaskEntry & player->getRaceMask()))
                    break;
                if (!(classMaskEntry & player->getClassMask()))
                    break;
                for (uint8 j = 0; j < 255; j++)
                {
                    player->removeActionButton(j); // Remove any existing action buttons
                }
                player->addActionButton(buttonEntry, actionEntry, typeEntry);
            } while (check->NextRow());
        }
    }

    void AddTemplateWornGear(Player* player, uint32 index)
    { //                                                  0
        QueryResult check = WorldDatabase.Query("SELECT Enable FROM mod_ptrtemplate_index WHERE ID={}", index);
        uint8 enable = (*check)[0].Get<uint8>(); //           0          1        2      3       4
        QueryResult gearInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, BagID, SlotID, ItemID FROM mod_ptrtemplate_inventory WHERE ID={}", index);
        if (enable == 1)
        {
            do
            {
                uint16 raceMaskEntry = (*gearInfo)[0].Get<uint16>();
                uint16 classMaskEntry = (*gearInfo)[1].Get<uint16>();
                uint32 bagEntry = (*gearInfo)[2].Get<uint32>();
                uint8 slotEntry = (*gearInfo)[3].Get<uint8>();
                uint32 itemEntry = (*gearInfo)[4].Get<uint32>();
                if (!(raceMaskEntry & player->getRaceMask()))
                    break;
                if (!(classMaskEntry & player->getClassMask()))
                    break;
                if (!(bagEntry == 7)) // Arbitrary bag ID that isn't actually a bag
                    break;
                player->EquipNewItem(slotEntry, itemEntry, true);
            } while (check->NextRow());
        }
    }

    void AddTemplateBagGear(Player* player, uint32 index)
    { //                                                  0
        QueryResult check = WorldDatabase.Query("SELECT Enable FROM mod_ptrtemplate_index WHERE ID={}", index);
        uint8 enable = (*check)[0].Get<uint8>(); //          0          1        2      3       4        5
        QueryResult bagInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, BagID, SlotID, ItemID, Quantity FROM mod_ptrtemplate_inventory WHERE ID={}", index);
        if (enable == 1)
        {
            do // TODO: Finish this function
            {
                uint16 raceMaskEntry = (*bagInfo)[0].Get<uint16>();
                uint16 classMaskEntry = (*bagInfo)[1].Get<uint16>();
                uint32 bagEntry = (*bagInfo)[2].Get<uint32>();
                uint8 slotEntry = (*bagInfo)[3].Get<uint8>();
                uint32 itemEntry = (*bagInfo)[4].Get<uint32>();
                uint32 quantityEntry = (*bagInfo)[5].Get<uint32>();
                if (!(raceMaskEntry & player->getRaceMask()))
                    break;
                if (!(classMaskEntry & player->getClassMask()))
                    break;
                ItemPosCountVec dest; // This is an enigma
                player->StoreNewItem(dest, itemEntry, true, 0);
            } while (check->NextRow());
        }
    }

    void AddTemplateSkills(Player* player, uint32 index)
    { //                                                  0
        QueryResult check = WorldDatabase.Query("SELECT Enable FROM mod_ptrtemplate_index WHERE ID={}", index);
        uint8 enable = (*check)[0].Get<uint8>(); //            0          1         2       3     4
        QueryResult skillInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, SkillID, Value, Max FROM mod_ptrtemplate_skills WHERE ID={}", index);
        if (enable == 1)
        {
            do
            {
                uint16 raceMaskEntry = (*skillInfo)[0].Get<uint16>();
                uint16 classMaskEntry = (*skillInfo)[1].Get<uint16>();
                uint16 skillEntry = (*skillInfo)[2].Get<uint16>();
                uint16 valueEntry = (*skillInfo)[3].Get<uint16>();
                uint16 maxEntry = (*skillInfo)[4].Get<uint16>();
                if (!(raceMaskEntry & player->getRaceMask()))
                    break;
                if (!(classMaskEntry & player->getClassMask()))
                    break;
                player->SetSkill(skillEntry, 0, valueEntry, maxEntry);
                
            } while (check->NextRow());
            player->UpdateSkillsForLevel();
        }
    }

    void AddTemplateSpells(Player* player, uint32 index)
    { //                                                  0
        QueryResult check = WorldDatabase.Query("SELECT Enable FROM mod_ptrtemplate_index WHERE ID={}", index);
        uint8 enable = (*check)[0].Get<uint8>(); //            0          1         2
        QueryResult spellInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, SpellID FROM mod_ptrtemplate_spells WHERE ID={}", index);
        if (enable == 1)
        {
            do
            {
                uint16 raceMaskEntry = (*spellInfo)[0].Get<uint16>();
                uint16 classMaskEntry = (*spellInfo)[1].Get<uint16>();
                uint64 spellEntry = (*spellInfo)[2].Get<uint64>();
                if (!(raceMaskEntry & player->getRaceMask()))
                    break;
                if (!(classMaskEntry & player->getClassMask()))
                    break;
                player->learnSpellHighRank(spellEntry);
            } while (check->NextRow());
        }
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
    new createTemplate();
    new announce();
}
