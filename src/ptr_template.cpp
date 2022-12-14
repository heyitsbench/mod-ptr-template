#include "Chat.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "ReputationMgr.h"
#include "ptr_template_loader.h"

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

class createTemplate : public PlayerScript { // TODO: Add logging stuff everywhere. I love me some logs.
    //     Also TODO: Add taximask function because golly me do I love taxis.
public: // Probably gonna use SetTaximaskNode. Looks like it sucks, but that's a problem I get for later. :)
    createTemplate() : PlayerScript("createTemplate") { }

    static bool CheckTemplateQualifier(Player* player, uint32 index)
    {
        QueryResult repInfo = WorldDatabase.Query("SELECT * FROM mod_ptrtemplate_reputations WHERE (ID={} AND RaceMask & {} AND ClassMask & {}) LIMIT 1", index, player->getRaceMask(), player->getClassMask()); // These are used to figure out if a template has a race/class in mind.
        QueryResult barInfo = WorldDatabase.Query("SELECT * FROM mod_ptrtemplate_action WHERE (ID={} AND RaceMask & {} AND ClassMask & {}) LIMIT 1", index, player->getRaceMask(), player->getClassMask()); //      For example, the AQ40 blizzlike template doesn't apply to belfs, draenei, or DKs.
        QueryResult itemInfo = WorldDatabase.Query("SELECT * FROM mod_ptrtemplate_inventory WHERE (ID={} AND RaceMask & {} AND ClassMask & {}) LIMIT 1", index, player->getRaceMask(), player->getClassMask());
        QueryResult skillInfo = WorldDatabase.Query("SELECT * FROM mod_ptrtemplate_skills WHERE (ID={} AND RaceMask & {} AND ClassMask & {}) LIMIT 1", index, player->getRaceMask(), player->getClassMask());
        QueryResult spellInfo = WorldDatabase.Query("SELECT * FROM mod_ptrtemplate_spells WHERE (ID={} AND RaceMask & {} AND ClassMask & {}) LIMIT 1", index, player->getRaceMask(), player->getClassMask());
        if (!repInfo && !barInfo && !itemInfo && !skillInfo && !spellInfo)
        {
            return false;
        }
        if ((!(player->getLevel() == (player->getClass() != CLASS_DEATH_KNIGHT
            ? sWorld->getIntConfig(CONFIG_START_PLAYER_LEVEL)
            : sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)))) && !(sConfigMgr->GetOption<bool>("Level.enable", true)))
        {
            return false;
        }
        if (!sConfigMgr->GetOption<bool>("Template.enable", true))
        {
            return false;
        }
        if (!(player->GetSession()->GetSecurity() >= sConfigMgr->GetOption<int8>("Enable.security", true)))
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    static void AddTemplateLevel(Player* player, uint32 index)
    { //                                                  0
        QueryResult check = WorldDatabase.Query("SELECT Level FROM mod_ptrtemplate_index WHERE ID={}", index);
        if (check)
        {
            uint8 levelEntry = (*check)[0].Get<uint8>();
            player->GiveLevel(levelEntry);
        }
    }

    static void AddTemplatePosition(Player* player, uint32 index)
    { //                                                        0           1          2          3          4         5        6       7       8       9
        QueryResult posEntry = WorldDatabase.Query("SELECT MapAlliance, XAlliance, YAlliance, ZAlliance, OAlliance, MapHorde, XHorde, YHorde, ZHorde, OHorde FROM mod_ptrtemplate_index WHERE ID={}", index);
        if (posEntry)
        {
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
            {
                player->TeleportTo(mapAllianceEntry, XAllianceEntry, YAllianceEntry, ZAllianceEntry, OAllianceEntry);
                LOG_DEBUG("module", "Template character {} has been teleported to alliance position.", player->GetGUID().ToString());
            }
            else
            {
                player->TeleportTo(mapHordeEntry, XHordeEntry, YHordeEntry, ZHordeEntry, OHordeEntry);
                LOG_DEBUG("module", "Template character {} has been teleported to horde position.", player->GetGUID().ToString());
            }
        }
    }

    static void AddTemplateHomebind(Player* player, uint32 index)
    { //                                                         0              1            2           3           4           5           6          7          8        9       10       11
        QueryResult homeEntry = WorldDatabase.Query("SELECT HMapAlliance, HZoneAlliance, HXAlliance, HYAlliance, HZAlliance, HOAlliance, HMapHorde, HZoneHorde, HXHorde, HYHorde, HZHorde, HOHorde FROM mod_ptrtemplate_index WHERE ID={}", index);
        if (homeEntry)
        {
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
                LOG_DEBUG("module", "Template character {} has had their homebind replaced with alliance.", player->GetGUID().ToString());
            }
            else
            {
                homebinding = WorldLocation(hMapHordeEntry, HXHordeEntry, HYHordeEntry, HZHordeEntry, HOHordeEntry);
                player->SetHomebind(homebinding, hZoneHordeEntry);
                LOG_DEBUG("module", "Template character {} has had their homebind replaced with horde.", player->GetGUID().ToString());
            }
        }
    }

    static void AddTemplateReputation(Player* player, uint32 index)
    { //                                                      0         1
        QueryResult repInfo = WorldDatabase.Query("SELECT FactionID, Standing FROM mod_ptrtemplate_reputations WHERE (ID={} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (repInfo)
        {
            do
            {
                Field* fields = repInfo->Fetch();
                uint16 factionEntry = fields[0].Get<uint16>();
                int32 standingEntry = fields[1].Get<int32>();
                FactionEntry const* factionId = sFactionStore.LookupEntry(factionEntry);
                player->GetReputationMgr().SetOneFactionReputation(factionId, float(standingEntry), false); // This was ripped from the `.modify reputation` command from base AC.
                player->GetReputationMgr().SendState(player->GetReputationMgr().GetState(factionId));
                LOG_DEBUG("module", "Added standing {} for faction {} for template character {}.", standingEntry, factionEntry, player->GetGUID().ToString());
            } while (repInfo->NextRow());
        }
    }

    static void AddTemplateHotbar(Player* player, uint32 index) // Someone smarter than me needs to fix this.
    { //                                                    0       1      2
        QueryResult barInfo = WorldDatabase.Query("SELECT Button, Action, Type FROM mod_ptrtemplate_action WHERE (ID={} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        for (uint8 j = 0; j <= MAX_ACTION_BUTTONS; j++) // This is supposed to go through every available action slot and remove what's there.
        { //                                               This doesn't work for spells added by AddTemplateSpells.
            player->removeActionButton(j); //              I don't know why and I've tried everything I can think of, but nothing's worked.
        } //                                               And yes, I do want the hotbar cleared for characters that don't fit the requirements of the template.
        if (barInfo)
        {
            do
            {
                uint8 buttonEntry = (*barInfo)[0].Get<uint8>();
                uint32 actionEntry = (*barInfo)[1].Get<uint32>();
                uint8 typeEntry = (*barInfo)[2].Get<uint8>();
                player->addActionButton(buttonEntry, actionEntry, typeEntry);
                LOG_DEBUG("module", "Added hotbar spell {} on button {} with type {} for template character {}.", actionEntry, buttonEntry, typeEntry, player->GetGUID().ToString());
            } while (barInfo->NextRow());
        }
        player->SendActionButtons(2);
    }

    static void AddTemplateWornGear(Player* player, uint32 index)
    { //                                                     0      1       2        3         4         5         6         7         8         9
        QueryResult gearInfo = WorldDatabase.Query("SELECT BagID, SlotID, ItemID, Enchant0, Enchant1, Enchant2, Enchant3, Enchant4, Enchant5, Enchant6 FROM mod_ptrtemplate_inventory WHERE (ID={} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (gearInfo)
        {
            for (uint8 j = EQUIPMENT_SLOT_START; j < EQUIPMENT_SLOT_END; j++)
            {
                player->DestroyItem(INVENTORY_SLOT_BAG_0, j, true);
            }
            do
            {
                uint32 bagEntry = (*gearInfo)[0].Get<uint32>();
                uint8 slotEntry = (*gearInfo)[1].Get<uint8>();
                uint32 itemEntry = (*gearInfo)[2].Get<uint32>();
                uint32 enchant0Entry = (*gearInfo)[3].Get<uint32>();
                uint32 enchant1Entry = (*gearInfo)[4].Get<uint32>();
                uint32 enchant2Entry = (*gearInfo)[5].Get<uint32>();
                uint32 enchant3Entry = (*gearInfo)[6].Get<uint32>();
                uint32 enchant4Entry = (*gearInfo)[7].Get<uint32>();
                uint32 enchant5Entry = (*gearInfo)[8].Get<uint32>();
                uint32 enchant6Entry = (*gearInfo)[9].Get<uint32>();
                if ((slotEntry >= INVENTORY_SLOT_BAG_END && slotEntry < PLAYER_SLOT_END) || bagEntry != 0) // If item is not either an equipped armorpiece, weapon, or container.
                {
                    continue;
                }
                if (slotEntry >= PLAYER_SLOT_END)
                {
                    player->SetAmmo(itemEntry);
                }
                else
                player->EquipNewItem(slotEntry, itemEntry, true); // For some reason this straight up overwrites anything occupying the slot, so no need for DestroyItem.
                Item* item = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, slotEntry); // TODO: Make this better.
                if (enchant0Entry)
                {
                    player->ApplyEnchantment(item, false);
                    item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchant0Entry, 0, 0);
                    player->ApplyEnchantment(item, true);
                }
                if (enchant1Entry)
                {
                    player->ApplyEnchantment(item, false);
                    item->SetEnchantment(TEMP_ENCHANTMENT_SLOT, enchant1Entry, 0, 0);
                    player->ApplyEnchantment(item, true);
                }
                if (enchant2Entry)
                {
                    player->ApplyEnchantment(item, false);
                    item->SetEnchantment(SOCK_ENCHANTMENT_SLOT, enchant2Entry, 0, 0);
                    player->ApplyEnchantment(item, true);
                }
                if (enchant3Entry)
                {
                    player->ApplyEnchantment(item, false);
                    item->SetEnchantment(SOCK_ENCHANTMENT_SLOT_2, enchant3Entry, 0, 0);
                    player->ApplyEnchantment(item, true);
                }
                if (enchant4Entry)
                {
                    player->ApplyEnchantment(item, false);
                    item->SetEnchantment(SOCK_ENCHANTMENT_SLOT_3, enchant4Entry, 0, 0);
                    player->ApplyEnchantment(item, true);
                }
                if (enchant5Entry)
                {
                    player->ApplyEnchantment(item, false);
                    item->SetEnchantment(BONUS_ENCHANTMENT_SLOT, enchant5Entry, 0, 0);
                    player->ApplyEnchantment(item, true);
                }
                if (enchant6Entry)
                {
                    player->ApplyEnchantment(item, false);
                    item->SetEnchantment(PRISMATIC_ENCHANTMENT_SLOT, enchant6Entry, 0, 0);
                    player->ApplyEnchantment(item, true);
                }
            } while (gearInfo->NextRow());
        }
        player->SaveToDB(false, false);
    }

    static void AddTemplateBagGear(Player* player, uint32 index)
    { //                                                    0      1       2        3         4         5         6         7         8         9         10
        QueryResult bagInfo = WorldDatabase.Query("SELECT BagID, SlotID, ItemID, Quantity, Enchant0, Enchant1, Enchant2, Enchant3, Enchant4, Enchant5, Enchant6 FROM mod_ptrtemplate_inventory WHERE (ID={} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (bagInfo)
        {
            do
            {   //                                                           0
                QueryResult containerInfo = CharacterDatabase.Query("SELECT slot FROM character_inventory WHERE (bag = 0 AND guid={})", (player->GetGUID().GetCounter()));
                Field* bagFields = bagInfo->Fetch();
                Field* containerFields = containerInfo->Fetch();
                uint32 bagEntry = bagFields[0].Get<uint32>();
                uint8 slotEntry = bagFields[1].Get<uint8>();
                uint32 itemEntry = bagFields[2].Get<uint32>();
                uint32 quantityEntry = bagFields[3].Get<uint32>();
                uint32 enchant0Entry = bagFields[4].Get<uint32>();
                uint32 enchant1Entry = bagFields[5].Get<uint32>();
                uint32 enchant2Entry = bagFields[6].Get<uint32>();
                uint32 enchant3Entry = bagFields[7].Get<uint32>();
                uint32 enchant4Entry = bagFields[8].Get<uint32>();
                uint32 enchant5Entry = bagFields[9].Get<uint32>();
                uint32 enchant6Entry = bagFields[10].Get<uint32>();
                if (itemEntry == 8) // Arbitrary non-existent itemID value (Used for gold)
                { //                   This should probably be a slotID and not an itemEntry, similar to ammo.
                    player->SetMoney(quantityEntry);
                    continue;
                }
                if ((slotEntry < INVENTORY_SLOT_BAG_END || slotEntry >= PLAYER_SLOT_END) && bagEntry == 0) // If item is either an equipped armorpiece, weapon, or container.
                {
                    continue;
                }
                ItemPosCountVec dest;
                if (bagEntry > 0 && bagEntry < 5) // If bag is an equipped container.
                { // TODO: Make this whole section better.
                    do
                    {
                        if (!containerFields) // Apparently this can happen sometimes.
                        {
                            continue;
                        }
                        uint8 slotDBInfo = containerFields[0].Get<uint8>();
                        if (bagEntry != (slotDBInfo - 18)) // Check if equipped bag matches specified bag for module.
                        {
                            continue;
                        }
                        if (slotDBInfo < INVENTORY_SLOT_BAG_START || slotDBInfo >= INVENTORY_SLOT_ITEM_START)
                        {
                            continue; // Ignore any non-container slots (i.e. backpack gear, equipped gear)
                        }
                        uint8 validCheck = player->CanStoreNewItem(slotDBInfo, slotEntry, dest, itemEntry, quantityEntry);
                        if (validCheck == EQUIP_ERR_OK)
                        {
                            player->StoreNewItem(dest, itemEntry, true);
                            Item* item = player->GetUseableItemByPos(slotDBInfo, slotEntry);
                            if (enchant0Entry)
                            {
                                player->ApplyEnchantment(item, false);
                                item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchant0Entry, 0, 0);
                                player->ApplyEnchantment(item, true);
                            }
                            if (enchant1Entry)
                            {
                                player->ApplyEnchantment(item, false);
                                item->SetEnchantment(TEMP_ENCHANTMENT_SLOT, enchant1Entry, 0, 0);
                                player->ApplyEnchantment(item, true);
                            }
                            if (enchant2Entry)
                            {
                                player->ApplyEnchantment(item, false);
                                item->SetEnchantment(SOCK_ENCHANTMENT_SLOT, enchant2Entry, 0, 0);
                                player->ApplyEnchantment(item, true);
                            }
                            if (enchant3Entry)
                            {
                                player->ApplyEnchantment(item, false);
                                item->SetEnchantment(SOCK_ENCHANTMENT_SLOT_2, enchant3Entry, 0, 0);
                                player->ApplyEnchantment(item, true);
                            }
                            if (enchant4Entry)
                            {
                                player->ApplyEnchantment(item, false);
                                item->SetEnchantment(SOCK_ENCHANTMENT_SLOT_3, enchant4Entry, 0, 0);
                                player->ApplyEnchantment(item, true);
                            }
                            if (enchant5Entry)
                            {
                                player->ApplyEnchantment(item, false);
                                item->SetEnchantment(BONUS_ENCHANTMENT_SLOT, enchant5Entry, 0, 0);
                                player->ApplyEnchantment(item, true);
                            }
                            if (enchant6Entry)
                            {
                                player->ApplyEnchantment(item, false);
                                item->SetEnchantment(PRISMATIC_ENCHANTMENT_SLOT, enchant6Entry, 0, 0);
                                player->ApplyEnchantment(item, true);
                            }
                        }
                    } while (containerInfo->NextRow());
                }
                else if (bagEntry == 0) // If bag is backpack
                {
                    if (!containerFields) // Apparently this can happen sometimes.
                    {
                        continue;
                    }
                    if (slotEntry < INVENTORY_SLOT_BAG_END || slotEntry >= PLAYER_SLOT_END)
                    {
                        continue; // Ignore any equipped items or invalid slot items.
                    }
                    player->DestroyItem(INVENTORY_SLOT_BAG_0, slotEntry, true);
                    uint8 validCheck = player->CanStoreNewItem(INVENTORY_SLOT_BAG_0, slotEntry, dest, itemEntry, quantityEntry);
                    if (validCheck == EQUIP_ERR_OK)
                    {
                        player->StoreNewItem(dest, itemEntry, true);
                        Item* item = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, slotEntry); // TODO: Make this better and cooler.
                        if (enchant0Entry)
                        {
                            player->ApplyEnchantment(item, false);
                            item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchant0Entry, 0, 0);
                            player->ApplyEnchantment(item, true);
                        }
                        if (enchant1Entry)
                        {
                            player->ApplyEnchantment(item, false);
                            item->SetEnchantment(TEMP_ENCHANTMENT_SLOT, enchant1Entry, 0, 0);
                            player->ApplyEnchantment(item, true);
                        }
                        if (enchant2Entry)
                        {
                            player->ApplyEnchantment(item, false);
                            item->SetEnchantment(SOCK_ENCHANTMENT_SLOT, enchant2Entry, 0, 0);
                            player->ApplyEnchantment(item, true);
                        }
                        if (enchant3Entry)
                        {
                            player->ApplyEnchantment(item, false);
                            item->SetEnchantment(SOCK_ENCHANTMENT_SLOT_2, enchant3Entry, 0, 0);
                            player->ApplyEnchantment(item, true);
                        }
                        if (enchant4Entry)
                        {
                            player->ApplyEnchantment(item, false);
                            item->SetEnchantment(SOCK_ENCHANTMENT_SLOT_3, enchant4Entry, 0, 0);
                            player->ApplyEnchantment(item, true);
                        }
                        if (enchant5Entry)
                        {
                            player->ApplyEnchantment(item, false);
                            item->SetEnchantment(BONUS_ENCHANTMENT_SLOT, enchant5Entry, 0, 0);
                            player->ApplyEnchantment(item, true);
                        }
                        if (enchant6Entry)
                        {
                            player->ApplyEnchantment(item, false);
                            item->SetEnchantment(PRISMATIC_ENCHANTMENT_SLOT, enchant6Entry, 0, 0);
                            player->ApplyEnchantment(item, true);
                        }
                    }
                }
                else if (bagEntry >= 5) // Basically just used for random sorting.
                {
                    uint8 validCheck = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemEntry, quantityEntry);
                    if (validCheck == EQUIP_ERR_OK)
                    {
                        player->StoreNewItem(dest, itemEntry, true);
                    }
                }
            } while (bagInfo->NextRow());
        }
    }

    static void AddTemplateQuests(Player* player, uint32 index)
    { //                                                       0
        QueryResult questInfo = WorldDatabase.Query("SELECT QuestID FROM mod_ptrtemplate_quests WHERE (ID={} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (questInfo)
        {
            do
            {
                player->SetQuestStatus((*questInfo)[0].Get<uint32>(), QUEST_STATUS_COMPLETE);
                LOG_DEBUG("module", "Added quest {} to template character {}.", ((*questInfo)[0].Get<uint32>()), player->GetGUID().ToString());
            } while (questInfo->NextRow());
        }
    }

    static void AddTemplateAchievements(Player* player, uint32 index)
    { //                                                                0
        QueryResult achievementInfo = WorldDatabase.Query("SELECT AchievementID FROM mod_ptrtemplate_achievements WHERE (ID={} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (achievementInfo)
        {
            AchievementEntry const* achievementID;
            do
            {
                achievementID = sAchievementStore.LookupEntry((*achievementInfo)[0].Get<uint32>());
                player->CompletedAchievement(achievementID);
                LOG_DEBUG("module", "Added achievement {} to template character {}.", (*achievementInfo)[0].Get<uint32>(), player->GetGUID().ToString());
            } while (achievementInfo->NextRow());
        }
    }

    static void AddTemplateSkills(Player* player, uint32 index)
    { //                                                       0       1     2
        QueryResult skillInfo = WorldDatabase.Query("SELECT SkillID, Value, Max FROM mod_ptrtemplate_skills WHERE (ID={} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (skillInfo)
        {
            do
            {
                uint16 skillEntry = (*skillInfo)[0].Get<uint16>();
                uint16 valueEntry = (*skillInfo)[1].Get<uint16>();
                uint16 maxEntry = (*skillInfo)[2].Get<uint16>();
                player->SetSkill(skillEntry, 0, valueEntry, maxEntry); // Don't know what step overload is used for, being zeroed here.
                LOG_DEBUG("module", "Added skill {} to template character {} with curvalue {} and maxvalue {}.", skillEntry, player->GetGUID().ToString(), valueEntry, maxEntry);
            } while (skillInfo->NextRow());
            player->SaveToDB(false, false);
        }
    }

    static void AddTemplateSpells(Player* player, uint32 index)
    { //                                                       0
        QueryResult spellInfo = WorldDatabase.Query("SELECT SpellID FROM mod_ptrtemplate_spells WHERE (ID={} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (spellInfo)
        {
            do
            {
                uint64 spellEntry = (*spellInfo)[0].Get<uint64>();
                player->learnSpell(spellEntry);
                LOG_DEBUG("module", "Added spell {} to template character {}.", spellEntry, player->GetGUID().ToString());
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

            for (uint8 j = INVENTORY_SLOT_BAG_START; j < INVENTORY_SLOT_ITEM_END; j++) // Removes any items the DK is carrying at the end of the process.
            { //                                                                          Includes starting gear as well as quest rewards.
                player->DestroyItem(INVENTORY_SLOT_BAG_0, j, true); //                    This is done because I hate fun.
            } //                                                                          ^(;,;)^
            LOG_DEBUG("module", "Handled death knight case for template character {}.", player->GetGUID().ToString());
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
            { "list", listTemplate, SEC_PLAYER, Console::No },
        };

        static ChatCommandTable commandTable =
        {
            { "template", templateTable },
        };
        return commandTable;
    }

    static bool enableTemplate(ChatHandler* handler, uint32 index)
    { //                                                    0
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
            handler->PSendSysMessage("This template has not been added.");
            return false;
        }
    }

    static bool disableTemplate(ChatHandler* handler, uint32 index)
    { //                                                    0
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
            handler->PSendSysMessage("This template has not been added.");
            return false;
        }
    }

    static bool applyTemplate(ChatHandler* handler, Optional<PlayerIdentifier> player, uint32 index) // TODO: Allow the command to use a target instead of always targetting self.
    { //                                                  0
        QueryResult check = WorldDatabase.Query("SELECT Enable FROM mod_ptrtemplate_index WHERE ID={}", index); // TODO: Check keywords column for template...keywords.
        if(check)
        {
            uint8 enable = (*check)[0].Get<uint8>();
            if (enable)
            {
                if (!player)
                {
                    player = PlayerIdentifier::FromTargetOrSelf(handler);
                }
                Player* target = player->GetConnectedPlayer();
                if (!(createTemplate::CheckTemplateQualifier(target, index)))
                {
                    handler->PSendSysMessage("You do not meet the requirements to apply this template."); // Probably a poorly-worded message, but w/e.
                    return true;
                }
                uint32 oldMSTime = getMSTime();
                createTemplate::AddTemplateDeathKnight(target);
                createTemplate::AddTemplateLevel(target, index);
                createTemplate::AddTemplateHomebind(target, index);
                createTemplate::AddTemplateAchievements(target, index);
                createTemplate::AddTemplateQuests(target, index);
                createTemplate::AddTemplateReputation(target, index);
                createTemplate::AddTemplateSkills(target, index);
                createTemplate::AddTemplateWornGear(target, index);
                std::this_thread::sleep_for(50ms); //  < - - - - - - -I absolutely despise this solution, but I have
                createTemplate::AddTemplateBagGear(target, index); // to make sure the bags are equipped before trying to add any gear to said bags.
                createTemplate::AddTemplateSpells(target, index); //  Might be able to use TaskScheduler for this.
                createTemplate::AddTemplateHotbar(target, index);
                createTemplate::AddTemplatePosition(target, index);
                std::this_thread::sleep_for(50ms); //                 My opinion still hasn't changed five lines later.
                LOG_DEBUG("module", "Handled template apply for character {} in {} ms.", player->GetGUID().ToString(), (GetMSTimeDiffToNow(oldMSTime) - 100));
                handler->PSendSysMessage("Please logout for the template to fully apply."); // This is a dumb message that I feel obligated to add because the hotbar changes when you log back in,
            } //                                                                               because I will never ever ever figure out how to do the hotbar correctly.
            else
            {
                handler->PSendSysMessage("This template is disabled.");
            }
            return true;
        }
        else
        {
            handler->PSendSysMessage("This template has not been added.");
            return true;
        }
    }

    static bool listTemplate(ChatHandler* handler)
    { //                                                 0     1        2
        QueryResult enable = WorldDatabase.Query("SELECT ID, Enable, Comment FROM mod_ptrtemplate_index");
        if (enable)
        {
            do
            {
                uint8 indexEntry = (*enable)[0].Get<uint8>();
                uint8 enableEntry = (*enable)[1].Get<uint8>();
                std::string commentEntry = (*enable)[2].Get<std::string>();
                if ((handler->GetSession()->GetSecurity() >= sConfigMgr->GetOption<int8>("Enable.security", true)) || (enableEntry))
                    handler->PSendSysMessage("%u (%s): %u", indexEntry, commentEntry, enableEntry);
            } while (enable->NextRow());
        }
        else
        {
            handler->PSendSysMessage("There are no added templates.");
        }
        return true;
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
