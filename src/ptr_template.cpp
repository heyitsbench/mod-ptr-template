#include "Chat.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "ReputationMgr.h"
#include "Object.h"


class createPTR : public WorldScript {

public:
    createPTR() : WorldScript("createPTR") { }

    void OnBeforeWorldInitialized() override
    {
        uint32 oldMSTime = getMSTime();
        uint32 count = 0;

        LOG_INFO("sql.sql", "Loading index entries for the PTR template module...");
        QueryResult result = WorldDatabase.Query("SELECT ID FROM mod_ptrtemplate_index;");

        if (!result)
        {
            LOG_WARN("sql.sql", "No index entries are available for the PTR template module to use.");
            return;
        }
        do
        {
            ++count;
        } while (result->NextRow());
        LOG_INFO("module", ">> Loaded {} templates in {} ms.", count, GetMSTimeDiffToNow(oldMSTime));
        return;
    }
};

class createTemplate : public PlayerScript {

public:
    createTemplate() : PlayerScript("createTemplate") { }

    static void AddTemplateLevel(Player* player, uint32 index)
    { //                                                  0
        QueryResult check = WorldDatabase.Query("SELECT Level FROM mod_ptrtemplate_index WHERE ID={}", index);
        uint8 levelEntry = (*check)[0].Get<uint8>();
        if (player->getLevel() == (player->getClass() != CLASS_DEATH_KNIGHT
            ? sWorld->getIntConfig(CONFIG_START_PLAYER_LEVEL)
            : sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)))
        {
            player->SetLevel(levelEntry);
        }
    }

    static void AddTemplatePosition(Player* player, uint32 index)
    { //                                                     0           1          2          3          4         5        6       7       8       9
        QueryResult posEntry = WorldDatabase.Query("SELECT MapAlliance, XAlliance, YAlliance, ZAlliance, OAlliance, MapHorde, XHorde, YHorde, ZHorde, OHorde FROM mod_ptrtemplate_index WHERE ID={}", index);
        uint16 mapAllianceEntry = (*posEntry)[0].Get<uint16>();
        float XAllianceEntry = (*posEntry)[1].Get<float>();
        float YAllianceEntry = (*posEntry)[2].Get<float>();
        float ZAllianceEntry = (*posEntry)[3].Get<float>();
        float OAllianceEntry = (*posEntry)[4].Get<float>();
        uint16 mapHordeEntry = (*posEntry)[5].Get<uint16>();
        float XHordeEntry = (*posEntry)[6].Get<float>();
        float YHordeEntry = (*posEntry)[7].Get<float>();
        float ZHordeEntry = (*posEntry)[8].Get<float>();
        float OHordeEntry = (*posEntry)[9].Get<float>();
        if (player->GetTeamId() == TEAM_HORDE)
        {
            player->TeleportTo(mapHordeEntry, XHordeEntry, YHordeEntry, ZHordeEntry, OHordeEntry);
        }
        else
        {
            player->TeleportTo(mapAllianceEntry, XAllianceEntry, YAllianceEntry, ZAllianceEntry, OAllianceEntry);
        }
    }

    static void AddTemplateReputation(Player* player, uint32 index)
    { //                                                     0          1          2         3
        QueryResult repInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, FactionID, Standing FROM mod_ptrtemplate_reputations WHERE ID={}", index);
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
        } while (repInfo->NextRow());
    }

    static void AddTemplateHotbar(Player* player, uint32 index)
    { //                                                     0          1        2       3      4
        QueryResult barInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, Button, Action, Type FROM mod_ptrtemplate_action WHERE ID={}", index);
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
        } while (barInfo->NextRow());
    }

    static void AddTemplateWornGear(Player* player, uint32 index)
    { //                                                      0          1        2      3       4
        QueryResult gearInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, BagID, SlotID, ItemID FROM mod_ptrtemplate_inventory WHERE ID={}", index);
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
        } while (gearInfo->NextRow());
    }

    static void AddTemplateBagGear(Player* player, uint32 index)
    { //                                                     0          1        2      3       4        5
        QueryResult bagInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, BagID, SlotID, ItemID, Quantity FROM mod_ptrtemplate_inventory WHERE ID={}", index);
        do
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
            ItemPosCountVec dest;
            if (bagEntry == 0 && slotEntry == 0) // This won't get triggered for included templates, but if someone's lazy/testing, this works
            {
                uint8 validCheck = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemEntry, quantityEntry);
                if (validCheck == EQUIP_ERR_OK)
                    player->StoreNewItem(dest, itemEntry, true);
            }
            else
            {
                uint8 validCheck = player->CanStoreNewItem(bagEntry, slotEntry, dest, itemEntry, quantityEntry);
                if (validCheck == EQUIP_ERR_OK)
                    player->StoreNewItem(dest, itemEntry, true);
            }
        } while (bagInfo->NextRow());
    }

    static void AddTemplateSkills(Player* player, uint32 index)
    { //                                                       0          1         2       3     4
        QueryResult skillInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, SkillID, Value, Max FROM mod_ptrtemplate_skills WHERE ID={}", index);
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

        } while (skillInfo->NextRow());
        player->UpdateSkillsForLevel();
    }

    static void AddTemplateSpells(Player* player, uint32 index)
    { //                                                       0          1         2
        QueryResult spellInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, SpellID FROM mod_ptrtemplate_spells WHERE ID={}", index);
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
        } while (spellInfo->NextRow());
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
