#include "ptr_template_loader.h"
#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"
#include "TaskScheduler.h"

using namespace Acore::ChatCommands;

TaskScheduler scheduler;

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

public:
    createTemplate() : PlayerScript("createTemplate") { }

    void HandleApply(Player* player, uint32 index)
    {
        LOG_DEBUG("module", "Applying template {} for character {}.", index, player->GetGUID().ToString());

        scheduler.Schedule(Milliseconds(APPLY_DELAY), [player, index](TaskContext context)
            {
                switch (context.GetRepeatCounter())
                {
                case 0:
                    if (sConfigMgr->GetOption<bool>("Template.dk", true))
                    {
                        AddTemplateDeathKnight(player);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying death knight case for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 1:
                    if (sConfigMgr->GetOption<bool>("Template.level", true))
                    {
                        AddTemplateLevel(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying level for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 2:
                    if (sConfigMgr->GetOption<bool>("Template.taximask", true))
                    {
                        AddTemplateTaxi(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying taximask for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 3:
                    if (sConfigMgr->GetOption<bool>("Template.homebind", true))
                    {
                        AddTemplateHomebind(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying homebind for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 4:
                    if (sConfigMgr->GetOption<bool>("Template.achievements", true))
                    {
                        AddTemplateAchievements(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying achievements for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 5:
                    if (sConfigMgr->GetOption<bool>("Template.quests", true))
                    {
                        AddTemplateQuests(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying quests for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 6:
                    if (sConfigMgr->GetOption<bool>("Template.reputation", true))
                    {
                        AddTemplateReputation(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying reputations for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 7:
                    if (sConfigMgr->GetOption<bool>("Template.skills", true))
                    {
                        AddTemplateSkills(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying skills for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 8:
                    if (sConfigMgr->GetOption<bool>("Template.equipgear", true))
                    {
                        AddTemplateWornGear(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying equipment for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 9:
                    if (sConfigMgr->GetOption<bool>("Template.baggear", true))
                    {
                        AddTemplateBagGear(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying inventory items for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 10:
                    if (sConfigMgr->GetOption<bool>("Template.spells", true))
                    {
                        AddTemplateSpells(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying spells for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 11:
                    if (sConfigMgr->GetOption<bool>("Template.hotbar", true))
                    {
                        AddTemplateHotbar(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying hotbar spells for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 12:
                    if (sConfigMgr->GetOption<bool>("Template.teleport", true))
                    {
                        AddTemplatePosition(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished teleporting template character {}.", player->GetGUID().ToString());
                    }
                    return;
                }
                context.Repeat(Milliseconds(APPLY_RATE));
            });
    }

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

private:

    enum TemplateEnums
    {
        APPLY_DELAY         = 25,
        APPLY_RATE          = 50,
        HORDE_SIMILAR       = -1,
        ACTION_BUTTON_BEGIN = 0,
        CONTAINER_BACKPACK  = 0,
        CONTAINER_END       = 5,
        ITEM_GOLD           = 8
    };

    static void AddTemplateLevel(Player* player, uint32 index)
    { //                                                  0
        QueryResult check = WorldDatabase.Query("SELECT Level FROM mod_ptrtemplate_index WHERE ID={}", index);
        if (check)
        {
            uint8 levelEntry = (*check)[0].Get<uint8>();
            player->GiveLevel(levelEntry);
            LOG_DEBUG("module", "Template character {} has been made level {}.", player->GetGUID().ToString(), levelEntry);
        }
    }

    static void AddTemplateTaxi(Player* player, uint32 index)
    { //                                                          0           1
        QueryResult taxiEntry = WorldDatabase.Query("SELECT TaxiAlliance, TaxiHorde FROM mod_ptrtemplate_index WHERE ID={}", index);
        if (taxiEntry)
        {
            if (player->GetTeamId() == TEAM_ALLIANCE || (*taxiEntry)[1].Get<std::string_view>() == "-1")
            {
                player->m_taxi.LoadTaxiMask((*taxiEntry)[0].Get<std::string_view>()); // Replaces existing m_taxi with template taximask (if exists)
                LOG_DEBUG("module", "Template character {} has been given alliance taxi mask.", player->GetGUID().ToString());
            }
            else
            {
                player->m_taxi.LoadTaxiMask((*taxiEntry)[1].Get<std::string_view>()); // Comes with the downside of having to create a taximask beforehand, but who cares it works
                LOG_DEBUG("module", "Template character {} has been given horde taxi mask.", player->GetGUID().ToString());
            }
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
            int16 mapHordeEntry = (*posEntry)[5].Get<int16>();
            float XHordeEntry = (*posEntry)[6].Get<float>();
            float YHordeEntry = (*posEntry)[7].Get<float>();
            float ZHordeEntry = (*posEntry)[8].Get<float>();
            float OHordeEntry = (*posEntry)[9].Get<float>();
            if (mapHordeEntry == HORDE_SIMILAR || player->GetTeamId() == TEAM_ALLIANCE)
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
            int16 hMapHordeEntry = (*homeEntry)[6].Get<int16>();
            uint16 hZoneHordeEntry = (*homeEntry)[7].Get<uint16>();
            float HXHordeEntry = (*homeEntry)[8].Get<float>();
            float HYHordeEntry = (*homeEntry)[9].Get<float>();
            float HZHordeEntry = (*homeEntry)[10].Get<float>();
            float HOHordeEntry = (*homeEntry)[11].Get<float>();
            WorldLocation homebinding;
            if (hMapHordeEntry == HORDE_SIMILAR || player->GetTeamId() == TEAM_ALLIANCE)
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
        for (uint8 j = ACTION_BUTTON_BEGIN; j <= MAX_ACTION_BUTTONS; j++) // This is supposed to go through every available action slot and remove what's there.
        { //                                                                 This doesn't work for spells added by AddTemplateSpells.
            player->removeActionButton(j); //                                I don't know why and I've tried everything I can think of, but nothing's worked.
        } //                                                                 And yes, I do want the hotbar cleared for characters that don't fit the requirements of the template.
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

    static void AddTemplateWornGear(Player* player, uint32 index) // Handles paper doll items and equipped bags.
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
                if ((slotEntry >= INVENTORY_SLOT_BAG_END && slotEntry < BANK_SLOT_BAG_START) || (slotEntry >= BANK_SLOT_BAG_END && slotEntry < PLAYER_SLOT_END) || bagEntry != CONTAINER_BACKPACK) // If item is not either an equipped armorpiece, weapon, or container.
                {
                    continue;
                }
                if (slotEntry >= PLAYER_SLOT_END)
                {
                    player->SetAmmo(itemEntry);
                }
                else
                player->EquipNewItem(slotEntry, itemEntry, true);
                if (slotEntry >= BANK_SLOT_BAG_START && slotEntry < BANK_SLOT_BAG_END)
                {
                    uint8 slotBuffer = slotEntry - (BANK_SLOT_BAG_START - 1);

                    if (player->GetBankBagSlotCount() < slotBuffer)
                    {
                        player->SetBankBagSlotCount(slotBuffer);
                    }
                }
                Item* item = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, slotEntry);
                if (item && item->GetEntry() != itemEntry)
                {
                    continue;
                }
                TemplateHelperItemEnchants(gearInfo, player, item, 3);
            } while (gearInfo->NextRow());
        }
    }

    static void AddTemplateBagGear(Player* player, uint32 index) // Handles bag items and currency.
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
                if (itemEntry == ITEM_GOLD)
                {
                    player->SetMoney(quantityEntry);
                    continue;
                }
                if ((slotEntry < INVENTORY_SLOT_BAG_END || slotEntry >= PLAYER_SLOT_END) && bagEntry == CONTAINER_BACKPACK) // If item is either an equipped armorpiece, weapon, or container.
                {
                    continue;
                }
                ItemPosCountVec dest;
                if (bagEntry > CONTAINER_BACKPACK && bagEntry < CONTAINER_END) // If bag is an equipped container.
                { // TODO: Make this whole section better.
                    do // Also TODO: Add support for adding to bank bag contents. Damn paladins.
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
                            if (item && item->GetEntry() != itemEntry)
                            {
                                continue;
                            }
                            TemplateHelperItemEnchants(bagInfo, player, item, 4);
                        }
                    } while (containerInfo->NextRow());
                }
                else if (bagEntry == CONTAINER_BACKPACK) // If bag is backpack
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
                        if (item && item->GetEntry() != itemEntry)
                        {
                            continue;
                        }
                        TemplateHelperItemEnchants(bagInfo, player, item, 4);
                    }
                }
                else
                {
                    uint8 validCheck = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemEntry, quantityEntry);
                    if (validCheck == EQUIP_ERR_OK)
                    {
                        player->StoreNewItem(dest, itemEntry, true); // Add to next available slot in backpack/equipped bags.
                    }
                    else if (validCheck == EQUIP_ERR_INVENTORY_FULL) // No available slots, post office's problem.
                    {
                        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
                        Item* itemBuffer = Item::CreateItem(itemEntry, quantityEntry, player, false);

                        TemplateHelperItemEnchants(bagInfo, player, itemBuffer, 4);

                        std::string subject = player->GetSession()->GetAcoreString(LANG_NOT_EQUIPPED_ITEM);

                        MailDraft draft(subject, "There were problems with equipping item(s).");
                        draft.AddItem(itemBuffer); // TODO: Make a damn queue and allow multiple items to a single message. Druids have so many items their inbox gets full immediately if sending one by one.
                        draft.SendMailTo(trans, player, MailSender(player, MAIL_STATIONERY_GM), MAIL_CHECK_MASK_COPIED);
                        CharacterDatabase.CommitTransaction(trans);
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
        }
    }

    static void TemplateHelperItemEnchants(QueryResult query, Player* player, Item* item, uint8 offset)
    {
        uint32 enchant0Entry = (*query)[offset].Get<uint32>();
        uint32 enchant1Entry = (*query)[offset + 1].Get<uint32>();
        uint32 enchant2Entry = (*query)[offset + 2].Get<uint32>();
        uint32 enchant3Entry = (*query)[offset + 3].Get<uint32>();
        uint32 enchant4Entry = (*query)[offset + 4].Get<uint32>();
        uint32 enchant5Entry = (*query)[offset + 5].Get<uint32>();
        uint32 enchant6Entry = (*query)[offset + 6].Get<uint32>();

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
};

class schedulediff : public WorldScript {

public:
    schedulediff() : WorldScript("schedulediff") { }

    void OnUpdate(uint32 diff)
    {
        scheduler.Update(diff);
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
            { "enable",  enableTemplate,  SEC_MODERATOR, Console::Yes },
            { "disable", disableTemplate, SEC_MODERATOR, Console::Yes },
            { "apply",   applyTemplate,   SEC_PLAYER,    Console::No },
            { "list",    listTemplate,    SEC_PLAYER,    Console::No },
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
        static createTemplate templatevar;
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
                if (!(templatevar.CheckTemplateQualifier(target, index)))
                {
                    handler->PSendSysMessage("You do not meet the requirements to apply this template."); // Probably a poorly-worded message, but w/e.
                    return true;
                }
                uint32 oldMSTime = getMSTime();
                templatevar.HandleApply(target, index);
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
        QueryResult enable = WorldDatabase.Query("SELECT ID, Enable, Comment FROM mod_ptrtemplate_index ORDER BY ID");
        if (enable)
        {
            do
            {
                uint8 indexEntry = (*enable)[0].Get<uint8>();
                uint8 enableEntry = (*enable)[1].Get<uint8>();
                std::string commentEntry = (*enable)[2].Get<std::string>();
                std::string enableText = enableEntry
                                         ? "Enabled"
                                         : "Disabled";
                if ((handler->GetSession()->GetSecurity() >= sConfigMgr->GetOption<int8>("Enable.security", true)) || (enableEntry))
                {
                    handler->PSendSysMessage("%u (%s): %s", indexEntry, commentEntry, enableText);
                }
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
    new schedulediff();
}

void AddSC_ptr_template_commandscript()
{
    new ptr_template_commandscript();
}
