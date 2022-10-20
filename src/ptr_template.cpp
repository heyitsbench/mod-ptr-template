#include "Chat.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "ReputationMgr.h"
#include "Object.h"
#include "ptr_template_loader.h"
#include <stdlib.h>

using namespace Acore::ChatCommands;

class createPTR : public WorldScript {

public:
    createPTR() : WorldScript("createPTR") { }

    void OnBeforeWorldInitialized() override
    {
        uint32 oldMSTime = getMSTime();
        uint32 count = 0;

        LOG_INFO("module", "Loading index entries for the PTR template module...");
        QueryResult result = WorldDatabase.Query("SELECT ID FROM mod_ptrtemplate_index;");

        if (!result)
        {
            LOG_WARN("module", "No index entries are available for the PTR template module to use.");
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
            //if(player->getLevel() == (player->getClass() != CLASS_DEATH_KNIGHT    I want this whole if to happen for the entire command. I put it here cause I'm dumb.
            //    ? sWorld->getIntConfig(CONFIG_START_PLAYER_LEVEL)                 I'm keeping it here because it does work as a function,
            //    : sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)))        but it doesn't work how I want inside of one of the functions. To be re-implemented elsewhere basically.
            //{
                player->GiveLevel(levelEntry);
            //}
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
            int16 mapHordeEntry = (*posEntry)[5].Get<int16>(); // Has to be signed to allow for condition below.
            float XHordeEntry = (*posEntry)[6].Get<float>();
            float YHordeEntry = (*posEntry)[7].Get<float>();
            float ZHordeEntry = (*posEntry)[8].Get<float>();
            float OHordeEntry = (*posEntry)[9].Get<float>();
            if (mapHordeEntry == -1 || (player->GetTeamId() == TEAM_ALLIANCE)) // -1 is used if Alliance/Horde pos is the same.
                player->TeleportTo(mapAllianceEntry, XAllianceEntry, YAllianceEntry, ZAllianceEntry, OAllianceEntry);
            else
                player->TeleportTo(mapHordeEntry, XHordeEntry, YHordeEntry, ZHordeEntry, OHordeEntry);
        }
    }

    static void AddTemplateHomebind(Player* player, uint32 index)
    { //                                                         0              1            2           3           4           5           6          7          8        9       10       11
        QueryResult homeEntry = WorldDatabase.Query("SELECT HMapAlliance, HZoneAlliance, HXAlliance, HYAlliance, HZAlliance, HOAlliance, HMapHorde, HZoneHorde, HXHorde, HYHorde, HZHorde, HOHorde FROM mod_ptrtemplate_index WHERE ID={}", index);
        if (homeEntry) {
            uint16 hMapAllianceEntry = (*homeEntry)[0].Get<uint16>();
            uint16 hZoneAllianceEntry = (*homeEntry)[1].Get<uint16>();
            float HXAllianceEntry = (*homeEntry)[2].Get<float>();
            float HYAllianceEntry = (*homeEntry)[3].Get<float>();
            float HZAllianceEntry = (*homeEntry)[4].Get<float>();
            float HOAllianceEntry = (*homeEntry)[5].Get<float>();
            int16 hMapHordeEntry = (*homeEntry)[6].Get<int16>(); // Has to be signed to allow for condition below.
            uint16 hZoneHordeEntry = (*homeEntry)[7].Get<uint16>();
            float HXHordeEntry = (*homeEntry)[8].Get<float>();
            float HYHordeEntry = (*homeEntry)[9].Get<float>();
            float HZHordeEntry = (*homeEntry)[10].Get<float>();
            float HOHordeEntry = (*homeEntry)[11].Get<float>();
            WorldLocation homebinding;
            if (hMapHordeEntry == -1 || (player->GetTeamId() == TEAM_ALLIANCE)) // -1 is used if Alliance/Horde binding is the same.
            {
                homebinding = WorldLocation(hMapAllianceEntry, HXAllianceEntry, HYAllianceEntry, HZAllianceEntry, HOAllianceEntry);
                player->SetHomebind(homebinding, hZoneAllianceEntry);
            }
            else
            {
                homebinding = WorldLocation(hMapHordeEntry, HXHordeEntry, HYHordeEntry, HZHordeEntry, HOHordeEntry);
                player->SetHomebind(homebinding, hZoneHordeEntry);
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
                    continue;
                if (!(classMaskEntry & player->getClassMask()))
                    continue;
                FactionEntry const* factionId = sFactionStore.LookupEntry(factionEntry);
                player->GetReputationMgr().SetOneFactionReputation(factionId, float(standingEntry), false); // This was ripped from the `.modify reputation` command from base AC.
                player->GetReputationMgr().SendState(player->GetReputationMgr().GetState(factionId));
            } while (repInfo->NextRow());
        }
    }

    static void AddTemplateHotbar(Player* player, uint32 index)
    { //                                                     0          1        2       3      4
        QueryResult barInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, Button, Action, Type FROM mod_ptrtemplate_action WHERE ID={}", index);
        if (barInfo)
        {
            for (uint8 j = 0; j < 255; j++)
            {
                player->removeActionButton(j);
            }
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
                // player->removeActionButton(buttonEntry); // Remove any existing action buttons (This doesn't work for anything added by this function, need to fix that)
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
                if (slotEntry > 22 || bagEntry != 0) // If item is not either an equipped armorpiece, weapon, or container.
                    continue;
                player->EquipNewItem(slotEntry, itemEntry, true);
            } while (gearInfo->NextRow());
        }
        player->SaveToDB(false, false);
    }

    static void AddTemplateBagGear(Player* player, uint32 index)
    { //                                                     0          1        2      3       4        5
        QueryResult bagInfo = WorldDatabase.Query("SELECT RaceMask, ClassMask, BagID, SlotID, ItemID, Quantity FROM mod_ptrtemplate_inventory WHERE ID={}", index);
        if (bagInfo)
        {
            do
            {
                //                                                           0
                QueryResult containerInfo = CharacterDatabase.Query("SELECT slot FROM character_inventory WHERE (bag = 0 AND guid={})", (player->GetGUID().GetCounter()));
                Field* bagFields = bagInfo->Fetch();
                Field* containerFields = containerInfo->Fetch();
                uint16 raceMaskEntry = bagFields[0].Get<uint16>();
                uint16 classMaskEntry = bagFields[1].Get<uint16>();
                uint32 bagEntry = bagFields[2].Get<uint32>();
                uint8 slotEntry = bagFields[3].Get<uint8>(); // 23-38 = backpack slots
                uint32 itemEntry = bagFields[4].Get<uint32>();
                uint32 quantityEntry = bagFields[5].Get<uint32>();
                if (!(raceMaskEntry & player->getRaceMask()))
                    continue;
                if (!(classMaskEntry & player->getClassMask()))
                    continue;
                if (itemEntry == 8) // Arbitrary non-existent itemID value (Used for gold)
                {
                    player->SetMoney(quantityEntry);
                    continue;
                }
                if (slotEntry < 23 && bagEntry == 0) // If item is either an equipped armorpiece, weapon, or container.
                    continue;
                ItemPosCountVec dest;
                if (bagEntry > 0 && bagEntry < 5) // If bag is an equipped container.
                {
                    do
                    {
                        if (!containerFields) // Apparently this can happen sometimes.
                            continue;
                        uint8 slotDBInfo = containerFields[0].Get<uint8>();
                        if (bagEntry != (slotDBInfo - 18)) // Check to see if equipped bag matches specified bag for module.
                            continue;
                        if (slotDBInfo < 19 || slotDBInfo > 22)
                            continue; // Ignore any non-container slots (i.e. backpack gear, equipped gear)
                        uint8 validCheck = player->CanStoreNewItem(slotDBInfo, slotEntry, dest, itemEntry, quantityEntry);
                        if (validCheck == EQUIP_ERR_OK)
                        {
                            player->StoreNewItem(dest, itemEntry, true);
                        }
                    } while (containerInfo->NextRow());
                }
                else if (bagEntry == 0) // If bag is backpack
                {
                    if (!containerFields) // Apparently this can happen sometimes.
                        continue;
                    if (slotEntry < 23)
                        continue; // Ignore any equipped pieces.
                    player->DestroyItem(INVENTORY_SLOT_BAG_0, slotEntry, true);
                    uint8 validCheck = player->CanStoreNewItem(INVENTORY_SLOT_BAG_0, slotEntry, dest, itemEntry, quantityEntry);
                    if (validCheck == EQUIP_ERR_OK)
                    {
                        player->DestroyItem(INVENTORY_SLOT_BAG_0, slotEntry, true);
                        player->StoreNewItem(dest, itemEntry, true);
                    }
                }
                else if (bagEntry >= 5) // Basically just used for currencies, random sorting, and gold.
                {
                    uint8 validCheck = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemEntry, quantityEntry);
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
                player->SetSkill(skillEntry, 0, valueEntry, maxEntry); // Don't know what step parameter is used for, being zeroed here.
            } while (skillInfo->NextRow());
            player->UpdateSkillsForLevel(); // Might not be necessary anymore, leftover from use of old level function.
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
                player->learnSpell(spellEntry);
            } while (spellInfo->NextRow());
        }
    }

    static void AddTemplateDeathKnight(Player* player) // Pretty much all of this is copied from acidmanifesto's lovely work on the skip-dk-starting-area module.
    {
        if (player->getClass() == CLASS_DEATH_KNIGHT)
        {
            int STARTER_QUESTS[33] = { 12593, 12619, 12842, 12848, 12636, 12641, 12657, 12678, 12679, 12680, 12687, 12698, 12701, 12706, 12716, 12719, 12720, 12722, 12724, 12725, 12727, 12733, -1, 12751, 12754, 12755, 12756, 12757, 12779, 12801, 13165, 13166 };

            int specialSurpriseQuestId = -1;
            switch (player->getRace())
            {
            case RACE_TAUREN:
                specialSurpriseQuestId = 12739;
                break;
            case RACE_HUMAN:
                specialSurpriseQuestId = 12742;
                break;
            case RACE_NIGHTELF:
                specialSurpriseQuestId = 12743;
                break;
            case RACE_DWARF:
                specialSurpriseQuestId = 12744;
                break;
            case RACE_GNOME:
                specialSurpriseQuestId = 12745;
                break;
            case RACE_DRAENEI:
                specialSurpriseQuestId = 12746;
                break;
            case RACE_BLOODELF:
                specialSurpriseQuestId = 12747;
                break;
            case RACE_ORC:
                specialSurpriseQuestId = 12748;
                break;
            case RACE_TROLL:
                specialSurpriseQuestId = 12749;
                break;
            case RACE_UNDEAD_PLAYER:
                specialSurpriseQuestId = 12750;
                break;
            }

            STARTER_QUESTS[22] = specialSurpriseQuestId;
            STARTER_QUESTS[32] = player->GetTeamId() == TEAM_ALLIANCE ? 13188 : 13189;

            for (int questId : STARTER_QUESTS)
            {
                if (player->GetQuestStatus(questId) == QUEST_STATUS_NONE)
                {
                    player->AddQuest(sObjectMgr->GetQuestTemplate(questId), nullptr);
                    player->RewardQuest(sObjectMgr->GetQuestTemplate(questId), 0, player, false);
                }
            }
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
            handler->PSendSysMessage("Set index %u enable flag (%s) to enabled.", index, comment);
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
            handler->PSendSysMessage("Set index %u enable flag (%s) to disabled.", index, comment);
            return true;
        }
        else
        {
            return false;
        }
    }

    static bool applyTemplate(ChatHandler* handler, Optional<PlayerIdentifier> player, uint32 index)
    {
        QueryResult check = WorldDatabase.Query("SELECT Enable FROM mod_ptrtemplate_index WHERE ID={}", index); // TODO: Check keywords column for template...keywords.
        if(check)
        {
            uint8 enable = (*check)[0].Get<uint8>();
            if (enable == 1)
            {
                if (!player)
                    player = PlayerIdentifier::FromTargetOrSelf(handler);
                Player* target = player->GetConnectedPlayer();
                createTemplate::AddTemplateDeathKnight(target);
                createTemplate::AddTemplateLevel(target, index);
                createTemplate::AddTemplatePosition(target, index);
                createTemplate::AddTemplateHomebind(target, index);
                createTemplate::AddTemplateReputation(target, index);
                createTemplate::AddTemplateSkills(target, index);
                createTemplate::AddTemplateWornGear(target, index);
                std::this_thread::sleep_for(50ms); //  < - - - - - - -I absolutely despise this solution, but I have
                createTemplate::AddTemplateBagGear(target, index); // to make sure the bags are equipped before trying to add any gear to said bags.
                createTemplate::AddTemplateSpells(target, index); //  Open to better solutions, please.
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
