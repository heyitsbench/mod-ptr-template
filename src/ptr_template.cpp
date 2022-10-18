#include "Chat.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "ReputationMgr.h"
#include "Object.h"
#include "ptr_template_loader.h"

using namespace Acore::ChatCommands;

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
        if (check)
        {
            uint8 levelEntry = (*check)[0].Get<uint8>();
            if (player->getLevel() == (player->getClass() != CLASS_DEATH_KNIGHT
                ? sWorld->getIntConfig(CONFIG_START_PLAYER_LEVEL)
                : sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)))
            {
                player->GiveLevel(levelEntry);
            }
        }
    }

    static void AddTemplatePosition(Player* player, uint32 index)
    { //                                                        0           1          2          3          4         5        6       7       8       9
        QueryResult posEntry = WorldDatabase.Query("SELECT MapAlliance, XAlliance, YAlliance, ZAlliance, OAlliance, MapHorde, XHorde, YHorde, ZHorde, OHorde FROM mod_ptrtemplate_index WHERE ID={}", index);
        if (posEntry) {
            uint16 mapAllianceEntry = (*posEntry)[0].Get<uint16>();
            float XAllianceEntry = (*posEntry)[1].Get<float>();
            float YAllianceEntry = (*posEntry)[2].Get<float>();
            float ZAllianceEntry = (*posEntry)[3].Get<float>();
            float OAllianceEntry = (*posEntry)[4].Get<float>();
            int16 mapHordeEntry = (*posEntry)[5].Get<int16>();
            float XHordeEntry = (*posEntry)[6].Get<float>();
            float YHordeEntry = (*posEntry)[7].Get<float>();
            float ZHordeEntry = (*posEntry)[8].Get<float>();
            float OHordeEntry = (*posEntry)[9].Get<float>();
            if (mapHordeEntry == -1 || (player->GetTeamId() == TEAM_ALLIANCE))
            {
                player->TeleportTo(mapAllianceEntry, XAllianceEntry, YAllianceEntry, ZAllianceEntry, OAllianceEntry);
            }
            else
            {
                player->TeleportTo(mapHordeEntry, XHordeEntry, YHordeEntry, ZHordeEntry, OHordeEntry);
            }
        }
    }

    static void AddTemplateReputation(Player* player, uint32 index)
    { //                                                     0          1          2         3
        QueryResult repInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, FactionID, Standing FROM mod_ptrtemplate_reputations WHERE ID={}", index);
        if (repInfo)
        {
            do
            {
                Field* fields = repInfo->Fetch();
                uint16 raceMaskEntry = fields[0].Get<uint16>();
                uint16 classMaskEntry = fields[1].Get<uint16>();
                uint16 factionEntry = fields[2].Get<uint16>();
                int32 standingEntry = fields[3].Get<int32>();
                if (!(raceMaskEntry & player->getRaceMask()))
                {
                    continue;
                }
                else
                if (!(classMaskEntry & player->getClassMask()))
                {
                    continue;
                }
                else
                {
                    FactionEntry const* factionId = sFactionStore.LookupEntry(factionEntry);
                    player->GetReputationMgr().SetOneFactionReputation(factionId, float(standingEntry), false);
                    player->GetReputationMgr().SendState(player->GetReputationMgr().GetState(factionId));
                }
            } while (repInfo->NextRow());
        }
    }

    static void AddTemplateHotbar(Player* player, uint32 index)
    { //                                                     0          1        2       3      4
        QueryResult barInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, Button, Action, Type FROM mod_ptrtemplate_action WHERE ID={}", index);
        if (barInfo)
        {
            do
            {
                uint16 raceMaskEntry = (*barInfo)[0].Get<uint16>();
                uint16 classMaskEntry = (*barInfo)[1].Get<uint16>();
                uint8 buttonEntry = (*barInfo)[2].Get<uint8>();
                uint32 actionEntry = (*barInfo)[3].Get<uint32>();
                uint8 typeEntry = (*barInfo)[4].Get<uint8>();
                if (!(raceMaskEntry & player->getRaceMask()))
                    continue;
                if (!(classMaskEntry & player->getClassMask()))
                    continue;
                for (uint8 j = 0; j < 255; j++)
                {
                    player->removeActionButton(j); // Remove any existing action buttons
                }
                player->addActionButton(buttonEntry, actionEntry, typeEntry); // Requires re-log
            } while (barInfo->NextRow());
        }
    }

    static void AddTemplateWornGear(Player* player, uint32 index)
    { //                                                      0          1        2      3       4
        QueryResult gearInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, BagID, SlotID, ItemID FROM mod_ptrtemplate_inventory WHERE ID={}", index);
        if (gearInfo)
        {
            do
            {
                uint16 raceMaskEntry = (*gearInfo)[0].Get<uint16>();
                uint16 classMaskEntry = (*gearInfo)[1].Get<uint16>();
                uint32 bagEntry = (*gearInfo)[2].Get<uint32>();
                uint8 slotEntry = (*gearInfo)[3].Get<uint8>(); //   00-18 = equipped gear
                uint32 itemEntry = (*gearInfo)[4].Get<uint32>(); // 19-22 = container slots
                if (!(raceMaskEntry & player->getRaceMask()))
                    continue;
                if (!(classMaskEntry & player->getClassMask()))
                    continue;
                if (slotEntry > 22 || bagEntry != 0)
                    continue;
                player->EquipNewItem(slotEntry, itemEntry, true);
            } while (gearInfo->NextRow());
        }
    }

    static void AddTemplateBagGear(Player* player, uint32 index)
    { //                                                     0          1        2      3       4        5
        QueryResult bagInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, BagID, SlotID, ItemID, Quantity FROM mod_ptrtemplate_inventory WHERE ID={}", index);
        if (bagInfo)
        {
            do
            {
                uint16 raceMaskEntry = (*bagInfo)[0].Get<uint16>();
                uint16 classMaskEntry = (*bagInfo)[1].Get<uint16>();
                uint32 bagEntry = (*bagInfo)[2].Get<uint32>();
                uint8 slotEntry = (*bagInfo)[3].Get<uint8>(); // 23-38 = backpack slots
                uint32 itemEntry = (*bagInfo)[4].Get<uint32>();
                uint32 quantityEntry = (*bagInfo)[5].Get<uint32>();
                if (!(raceMaskEntry & player->getRaceMask()))
                    continue;
                if (!(classMaskEntry & player->getClassMask()))
                    continue;
                if (itemEntry == 8) // Arbitrary non-existent itemID value
                {
                    player->SetMoney(quantityEntry);
                    continue;
                }
                if (slotEntry < 23 && bagEntry == 0)
                    continue;
                ItemPosCountVec dest;
                if (bagEntry == 6) // This won't get triggered for included templates, but if someone's lazy/testing, this works
                {
                    uint8 validCheck = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemEntry, quantityEntry);
                    if (validCheck == EQUIP_ERR_OK)
                        player->StoreNewItem(dest, itemEntry, true);
                }
                else
                {
                    uint8 validCheck = player->CanStoreNewItem(bagEntry, slotEntry, dest, itemEntry, quantityEntry); // TODO: Add support for more bag slots. Otherwise, all features basically done :partying_face:
                    if (validCheck == EQUIP_ERR_OK)
                        player->StoreNewItem(dest, itemEntry, true);
                }
            } while (bagInfo->NextRow());
        }
    }

    static void AddTemplateSkills(Player* player, uint32 index)
    { //                                                       0          1         2       3     4
        QueryResult skillInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, SkillID, Value, Max FROM mod_ptrtemplate_skills WHERE ID={}", index);
        if (skillInfo)
        {
            do
            {
                uint16 raceMaskEntry = (*skillInfo)[0].Get<uint16>();
                uint16 classMaskEntry = (*skillInfo)[1].Get<uint16>();
                uint16 skillEntry = (*skillInfo)[2].Get<uint16>();
                uint16 valueEntry = (*skillInfo)[3].Get<uint16>();
                uint16 maxEntry = (*skillInfo)[4].Get<uint16>();
                if (!(raceMaskEntry & player->getRaceMask()))
                    continue;
                if (!(classMaskEntry & player->getClassMask()))
                    continue;
                player->SetSkill(skillEntry, 0, valueEntry, maxEntry);

            } while (skillInfo->NextRow());
            player->UpdateSkillsForLevel();
        }
    }

    static void AddTemplateSpells(Player* player, uint32 index)
    { //                                                       0          1         2
        QueryResult spellInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, SpellID FROM mod_ptrtemplate_spells WHERE ID={}", index);
        if (spellInfo)
        {
            do
            {
                uint16 raceMaskEntry = (*spellInfo)[0].Get<uint16>();
                uint16 classMaskEntry = (*spellInfo)[1].Get<uint16>();
                uint64 spellEntry = (*spellInfo)[2].Get<uint64>();
                if (!(raceMaskEntry & player->getRaceMask()))
                    continue;
                if (!(classMaskEntry & player->getClassMask()))
                    continue;
                player->learnSpellHighRank(spellEntry);
            } while (spellInfo->NextRow());
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
            { "apply", applyTemplate, SEC_PLAYER, Console::No },
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
        if (result)
        {
            std::string comment = (*result)[0].Get<std::string>();
            handler->PSendSysMessage("Set index %u enable flag(%s) to enabled.", index, comment);
            return true;
        }
        else
        {
            return false;
        }
    }

    static bool disableTemplate(ChatHandler* handler, uint32 index)
    {
        QueryResult result = WorldDatabase.Query("SELECT Comment FROM mod_ptrtemplate_index WHERE ID={}", index);
        WorldDatabase.Execute("UPDATE mod_ptrtemplate_index SET Enable=0 WHERE ID={}", index);
        if (result)
        {
            std::string comment = (*result)[0].Get<std::string>();
            handler->PSendSysMessage("Set index %u enable flag(%s) to disabled.", index, comment);
            return true;
        }
        else
        {
            return false;
        }
    }

    static bool applyTemplate(ChatHandler* handler, Optional<PlayerIdentifier> player, uint32 index)
    {
        QueryResult check = WorldDatabase.Query("SELECT Enable FROM mod_ptrtemplate_index WHERE ID={}", index); // TODO: Check keywords column for template...keywords
        if(check)
        {
            uint8 enable = (*check)[0].Get<uint8>();
            if (enable == 1)
            {
                if (!player)
                {
                    player = PlayerIdentifier::FromTargetOrSelf(handler);
                }
                Player* target = player->GetConnectedPlayer();
                createTemplate::AddTemplateLevel(target, index);
                createTemplate::AddTemplatePosition(target, index);
                createTemplate::AddTemplateReputation(target, index);
                createTemplate::AddTemplateSkills(target, index);
                createTemplate::AddTemplateWornGear(target, index);
                createTemplate::AddTemplateBagGear(target, index);
                createTemplate::AddTemplateSpells(target, index);
                createTemplate::AddTemplateHotbar(target, index);
            }
            return true;
        }
        else
        {
            return false;
        }
    }
};

void Add_ptr_template()
{
    new createPTR();
    new createTemplate();
    new announce();
}

void AddSC_ptr_template_commandscript()
{
    new ptr_template_commandscript();
}
