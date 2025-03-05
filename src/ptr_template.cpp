#include "ptr_template_loader.h"

using namespace Acore::ChatCommands;

class createPTR : public WorldScript {

public:
    createPTR() : WorldScript("createPTR") { }

    void OnBeforeWorldInitialized() override
    {
        uint32 oldMSTime = getMSTime();
        uint32 count = 0;

        LOG_INFO("module", " "); //\n
        LOG_INFO("module", "Loading index entries for the PTR template module...");
        QueryResult result = WorldDatabase.Query("SELECT ID FROM mod_ptrtemplate_index");

        if (!result)
        {
            LOG_WARN("module", ">> Loaded 0 templates!");
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

    void HandleApply(Player* player, uint32 index, uint32 delayMultiplier = 1)
    {
        LOG_DEBUG("module", "Applying template {} for character {}.", index, player->GetGUID().ToString());

        uint8 itemRoutine = METHOD_BOOST;

        if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::DELETE_ITEMS))
            itemRoutine = METHOD_DELETE;

        scheduler.Schedule(Milliseconds(delayMultiplier * APPLY_DELAY), [player, index, itemRoutine](TaskContext context)
            {
                switch (context.GetRepeatCounter())
                {
                case 0:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_DEATH_KNIGHT))
                    {
                        if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_EQUIP_GEAR) && templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_BAG_GEAR))
                            TemplateHelperItemCleanup(player, SCOPE_ALL, itemRoutine);
                        else if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_BAG_GEAR))
                            TemplateHelperItemCleanup(player, SCOPE_BAGS, itemRoutine);
                        else if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_EQUIP_GEAR))
                            TemplateHelperItemCleanup(player, SCOPE_EQUIPPED, itemRoutine);

                        AddTemplateDeathKnight(player);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying death knight case for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 1:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_LEVEL))
                    {
                        AddTemplateLevel(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying level for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 2:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_TAXIMASK))
                    {
                        AddTemplateTaxi(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying taximask for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 3:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_HOMEBIND))
                    {
                        AddTemplateHomebind(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying homebind for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 4:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_ACHIEVEMENTS))
                    {
                        AddTemplateAchievements(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying achievements for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 5:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_QUESTS))
                    {
                        AddTemplateQuests(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying quests for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 6:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_REPUTATION))
                    {
                        AddTemplateReputation(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying reputations for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 7:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_SKILLS))
                    {
                        AddTemplateSkills(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying skills for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 8:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_EQUIP_GEAR))
                    {
                        TemplateHelperItemCleanup(player, SCOPE_EQUIPPED, itemRoutine);
                        AddTemplateWornGear(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying equipment for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 9:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_BAG_GEAR))
                    {
                        TemplateHelperItemCleanup(player, SCOPE_BAGS, itemRoutine);
                        AddTemplateBagGear(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying inventory items for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 10:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_SPELLS))
                    {
                        AddTemplateSpells(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying spells for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 11:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_HOTBAR))
                    {
                        AddTemplateHotbar(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying hotbar spells for template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 12:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_TELEPORT))
                    {
                        AddTemplatePosition(player, index);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished teleporting template character {}.", player->GetGUID().ToString());
                    }
                    break;
                case 13:
                    if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_RESOURCES))
                    {
                        AddTemplateResources(player);
                        player->SaveToDB(false, false);
                        LOG_DEBUG("module", "Finished applying full resources for template character {}.", player->GetGUID().ToString());
                    }
                    return;
                }
                context.Repeat(Milliseconds(APPLY_RATE));
            });
    }

    static uint8 CheckTemplateQualifier(Player* player, uint32 index, uint8 enable)
    {
        uint32 raceMask = player->getRaceMask();
        uint32 classMask = player->getClassMask();

        QueryResult queryCheck = WorldDatabase.Query( // These are used to figure out if a template has a race/class in mind.
            "SELECT COUNT(*) FROM(" //                   For example, the AQ40 blizzlike template doesn't apply to belfs, draenei, or DKs.
            "SELECT ID FROM mod_ptrtemplate_reputations WHERE(ID = {} AND RaceMask & {} AND ClassMask & {})"
            " UNION ALL "
            "SELECT ID FROM mod_ptrtemplate_action WHERE(ID = {} AND RaceMask & {} AND ClassMask & {})"
            " UNION ALL "
            "SELECT ID FROM mod_ptrtemplate_inventory WHERE(ID = {} AND RaceMask & {} AND ClassMask & {})"
            " UNION ALL "
            "SELECT ID FROM mod_ptrtemplate_skills WHERE(ID = {} AND RaceMask & {} AND ClassMask & {})"
            " UNION ALL "
            "SELECT ID FROM mod_ptrtemplate_spells WHERE(ID = {} AND RaceMask & {} AND ClassMask & {})"
            " UNION ALL "
            "SELECT ID FROM mod_ptrtemplate_achievements WHERE(ID = {} AND RaceMask & {} AND ClassMask & {})"
            " UNION ALL "
            "SELECT ID FROM mod_ptrtemplate_quests WHERE(ID = {} AND RaceMask & {} AND ClassMask & {})"
            ") AS combined",
            index, raceMask, classMask,
            index, raceMask, classMask,
            index, raceMask, classMask,
            index, raceMask, classMask,
            index, raceMask, classMask,
            index, raceMask, classMask,
            index, raceMask, classMask);

        uint8 security = player->GetSession()->GetSecurity();
        if ((security < templateConfigData.GetConfigValue<int8>(PTRTemplateConfig::ENABLE_APPLY_SECURITY)) && (security < templateConfigData.GetConfigValue<int8>(PTRTemplateConfig::DISABLE_APPLY_SECURITY)))
        {
            LOG_DEBUG("module", "Player {} tried to apply template {}, but does not meet security level.", player->GetGUID().ToString(), index);
            return INSUFFICIENT_SECURITY_LEVEL;
        }
        if ((security < templateConfigData.GetConfigValue<int8>(PTRTemplateConfig::DISABLE_APPLY_SECURITY)) && !enable)
        {
            LOG_DEBUG("module", "Player {} tried to apply template {}, but the template is disabled.", player->GetGUID().ToString(), index);
            return TEMPLATE_DISABLED_LOCAL;
        }
        if (!((*queryCheck)[0].Get<uint64>()))
        {
            LOG_DEBUG("module", "Template ID {} entered, but no template info available for player {}!", index, player->GetGUID().ToString());
            return MISSING_TEMPLATE_INFO;
        }
        if ((!(player->GetLevel() == (player->getClass() != CLASS_DEATH_KNIGHT
            ? sWorld->getIntConfig(CONFIG_START_PLAYER_LEVEL)
            : sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)))) && !(templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::LEVEL_ENABLE)))
        {
            LOG_DEBUG("module", "Player {} is not initial level, cannot apply template {}. Current level: {}", player->GetGUID().ToString(), index, player->GetLevel());
            return NOT_INITIAL_LEVEL;
        }
        if (!templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_ENABLE))
        {
            LOG_DEBUG("module", "Player {} tried to apply template {}, but templates are disabled.", player->GetGUID().ToString(), index);
            return TEMPLATE_DISABLED_GLOBAL;
        }
        else
        {
            LOG_DEBUG("module", "Player {} has passed qualification for template {}.", player->GetGUID().ToString(), index);
            return CHECK_PASSED;
        }
    }
private:
    static void AddTemplateAchievements(Player* player, uint32 index)
    { //                                                                0
        QueryResult achievementInfo = WorldDatabase.Query("SELECT AchievementID FROM mod_ptrtemplate_achievements WHERE (ID = {} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
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

    static void AddTemplateBagGear(Player* player, uint32 index) // Handles bag items and currency.
    { //                                                    0      1       2        3         4         5         6         7         8         9         10
        QueryResult bagInfo = WorldDatabase.Query("SELECT BagID, SlotID, ItemID, Quantity, Enchant0, Enchant1, Enchant2, Enchant3, Enchant4, Enchant5, Enchant6 FROM mod_ptrtemplate_inventory WHERE (ID = {} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (bagInfo)
        {
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            std::list<Item*> excessiveItems;

            do
            {   //                                                           0
                QueryResult containerInfo = CharacterDatabase.Query("SELECT slot FROM character_inventory WHERE (bag = 0 AND guid = {})", (player->GetGUID().GetCounter()));

                if (!containerInfo) // Apparently this can happen sometimes.
                    continue;

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
                    continue;

                ItemPosCountVec dest;
                if (bagEntry > CONTAINER_BACKPACK && bagEntry < CONTAINER_FINISH) // If bag is an equipped container.
                { // TODO: Make this whole section better.
                    do // Also TODO: Add support for adding to bank bag contents. Damn paladins.
                    {
                        if (!containerFields) // Apparently this can happen sometimes.
                            continue;

                        uint8 slotDBInfo = containerFields[0].Get<uint8>();

                        if (bagEntry != (slotDBInfo - 18)) // Check if equipped bag matches specified bag for module.
                            continue;

                        if (slotDBInfo < INVENTORY_SLOT_BAG_START || slotDBInfo >= INVENTORY_SLOT_ITEM_START)
                            continue; // Ignore any non-container slots (i.e. backpack gear, equipped gear)

                        uint8 validCheck = player->CanStoreNewItem(slotDBInfo, slotEntry, dest, itemEntry, quantityEntry);
                        if (validCheck == EQUIP_ERR_OK)
                        {
                            player->StoreNewItem(dest, itemEntry, true);
                            Item* item = player->GetUseableItemByPos(slotDBInfo, slotEntry);
                            player->SendNewItem(item, 1, false, true); // Broadcast item detail packet.
                            if (item && item->GetEntry() != itemEntry)
                                continue;

                            TemplateHelperItemEnchants(bagInfo, player, item, 4);
                        }
                    } while (containerInfo->NextRow());
                }
                else if (bagEntry == CONTAINER_BACKPACK)
                {
                    if (!containerFields) // Apparently this can happen sometimes.
                        continue;

                    if (slotEntry < INVENTORY_SLOT_BAG_END || slotEntry >= PLAYER_SLOT_END)
                        continue; // Ignore any equipped items or invalid slot items.

                    uint8 validCheck = player->CanStoreNewItem(INVENTORY_SLOT_BAG_0, slotEntry, dest, itemEntry, quantityEntry);
                    if (validCheck == EQUIP_ERR_OK)
                    {
                        player->StoreNewItem(dest, itemEntry, true);
                        Item* item = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, slotEntry); // TODO: Make this better and cooler.
                        player->SendNewItem(item, 1, false, true); // Broadcast item detail packet.
                        if (item && item->GetEntry() != itemEntry)
                            continue;

                        TemplateHelperItemEnchants(bagInfo, player, item, 4);
                    }
                }
                else
                {
                    uint8 validCheck = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemEntry, quantityEntry);
                    if (validCheck == EQUIP_ERR_OK)
                        player->StoreNewItem(dest, itemEntry, true); // Add to next available slot in backpack/equipped bags.
                                                                     // TODO: Create the item and make it usable for item enchant helper. Also packet broadcast.
                    else if (validCheck == EQUIP_ERR_INVENTORY_FULL) // No available slots, post office's problem.
                    {
                        Item* itemBuffer = Item::CreateItem(itemEntry, quantityEntry, player, false);

                        TemplateHelperItemEnchants(bagInfo, player, itemBuffer, 4);

                        excessiveItems.push_back(itemBuffer);
                    }
                }
            } while (bagInfo->NextRow());

            while (!excessiveItems.empty())
            {
                std::string subject = player->GetSession()->GetAcoreString(LANG_NOT_EQUIPPED_ITEM);
                std::string content = player->GetSession()->GetModuleString(module_string, MAIL_ERROR_EQUIP_BODY)[0];

                MailDraft draft(subject, content);
                for (uint8 i = 0; !excessiveItems.empty() && i < MAX_MAIL_ITEMS; ++i)
                {
                    draft.AddItem(excessiveItems.front());
                    excessiveItems.pop_front();
                }
                draft.SendMailTo(trans, player, MailSender(player, MAIL_STATIONERY_GM), MAIL_CHECK_MASK_COPIED);
            }

            CharacterDatabase.CommitTransaction(trans);
        }
    }

    static void AddTemplateDeathKnight(Player* player) // Pretty much all of this is copied from acidmanifesto's lovely work on the skip-dk-starting-area module.
    {
        if (!(player->getClass() == CLASS_DEATH_KNIGHT))
            return;

        int STARTER_QUESTS[33] = { 12593, 12619, 12842, 12848, 12636, 12641, 12657, 12678, 12679, 12680, 12687, 12698, 12701, 12706, 12716, 12719, 12720, 12722, 12724, 12725, 12727, 12733, -1, 12751, 12754, 12755, 12756, 12757, 12779, 12801, 13165, 13166 };
        // Blizz just dumped all of the special surprise quests on every DK template. Don't know yet if I want to do the same.
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
        STARTER_QUESTS[32] = player->GetTeamId() == TEAM_ALLIANCE
            ? 13188
            : 13189;

        for (int questId : STARTER_QUESTS)
        {
            if (player->GetQuestStatus(questId) == QUEST_STATUS_NONE)
            {
                player->AddQuest(sObjectMgr->GetQuestTemplate(questId), nullptr);
                player->RewardQuest(sObjectMgr->GetQuestTemplate(questId), 0, player, false);
            }
            if (player->GetItemByPos(INVENTORY_SLOT_BAG_0, INVENTORY_SLOT_ITEM_START))
            {
                TemplateHelperItemCleanup(player, SCOPE_BAGS, METHOD_DELETE); // Removes any items the DK is rewarded with during the process.
                //                                                               Guarantees player won't receive quest rewards in the mail.
                //                                                               This is done because I hate fun.
                //                                                               ^(;,;)^
            }
        }
    }

    static void AddTemplateHomebind(Player* player, uint32 index)
    { //                                                         0              1            2           3           4           5           6          7          8        9       10       11
        QueryResult homeEntry = WorldDatabase.Query("SELECT HMapAlliance, HZoneAlliance, HXAlliance, HYAlliance, HZAlliance, HOAlliance, HMapHorde, HZoneHorde, HXHorde, HYHorde, HZHorde, HOHorde FROM mod_ptrtemplate_index WHERE ID = {}", index);
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

    static void AddTemplateHotbar(Player* player, uint32 index)
    {
        for (uint8 j = ACTION_BUTTON_BEGIN; j <= MAX_ACTION_BUTTONS; j++)
            player->removeActionButton(j);

        //                                                  0       1      2
        QueryResult barInfo = WorldDatabase.Query("SELECT Button, Action, Type FROM mod_ptrtemplate_action WHERE (ID = {} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (barInfo)
        {
            do
            {
                uint8 buttonEntry = (*barInfo)[0].Get<uint8>();
                uint32 actionEntry = (*barInfo)[1].Get<uint32>();
                uint8 typeEntry = (*barInfo)[2].Get<uint8>();

                if (player->addActionButton(buttonEntry, actionEntry, typeEntry))
                    LOG_DEBUG("module", "Added hotbar spell {} on button {} with type {} for template character {}.", actionEntry, buttonEntry, typeEntry, player->GetGUID().ToString());
                else
                    LOG_ERROR("module", "Failed to add hotbar spell {} on button {} with type {} for template character {}.", actionEntry, buttonEntry, typeEntry, player->GetGUID().ToString());
            } while (barInfo->NextRow());
        }
        player->SaveToDB(false, false); // Commit action buttons.
        player->SendActionButtons(1);
    }

    static void AddTemplateLevel(Player* player, uint32 index)
    { //                                                  0
        QueryResult check = WorldDatabase.Query("SELECT Level FROM mod_ptrtemplate_index WHERE ID = {}", index);
        if (check)
        {
            uint8 levelEntry = (*check)[0].Get<uint8>();
            player->GiveLevel(levelEntry);
            LOG_DEBUG("module", "Template character {} has been made level {}.", player->GetGUID().ToString(), levelEntry);
        }
    }

    static void AddTemplatePosition(Player* player, uint32 index)
    { //                                                        0           1          2          3          4         5        6       7       8       9
        QueryResult posEntry = WorldDatabase.Query("SELECT MapAlliance, XAlliance, YAlliance, ZAlliance, OAlliance, MapHorde, XHorde, YHorde, ZHorde, OHorde FROM mod_ptrtemplate_index WHERE ID = {}", index);
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
            player->UpdatePositionData();
        }
    }

    static void AddTemplateQuests(Player* player, uint32 index)
    { //                                                       0
        QueryResult questInfo = WorldDatabase.Query("SELECT QuestID FROM mod_ptrtemplate_quests WHERE (ID = {} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (questInfo)
        {
            do
            {
                player->SetQuestStatus((*questInfo)[0].Get<uint32>(), QUEST_STATUS_COMPLETE);
                LOG_DEBUG("module", "Added quest {} to template character {}.", ((*questInfo)[0].Get<uint32>()), player->GetGUID().ToString());
            } while (questInfo->NextRow());
        }
    }

    static void AddTemplateReputation(Player* player, uint32 index)
    { //                                                      0         1
        QueryResult repInfo = WorldDatabase.Query("SELECT FactionID, Standing FROM mod_ptrtemplate_reputations WHERE (ID = {} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (repInfo)
        {
            do
            {
                Field* fields = repInfo->Fetch();
                uint16 factionEntry = fields[0].Get<uint16>();
                int32 standingEntry = fields[1].Get<int32>();
                FactionEntry const* factionId = sFactionStore.LookupEntry(factionEntry);

                if ((player->GetReputationMgr().GetReputation(factionEntry) >= standingEntry) && templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::MAINTAIN_IMPROVED_VALUES))
                    continue;

                player->GetReputationMgr().SetOneFactionReputation(factionId, float(standingEntry), false); // This was ripped from the `.modify reputation` command from base AC.
                player->GetReputationMgr().SendState(player->GetReputationMgr().GetState(factionId));
                LOG_DEBUG("module", "Added standing {} for faction {} for template character {}.", standingEntry, factionEntry, player->GetGUID().ToString());
            } while (repInfo->NextRow());
        }
    }

    static void AddTemplateResources(Player* player)
    {
        player->SetFullHealth();
        if (player->getPowerType() == POWER_MANA)
            player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));
        else if (player->getPowerType() == POWER_ENERGY)
            player->SetPower(POWER_ENERGY, player->GetMaxPower(POWER_ENERGY));

        LOG_DEBUG("module", "Template character {} has been given full health/power.", player->GetGUID().ToString());
    }

    static void AddTemplateSkills(Player* player, uint32 index)
    { //                                                       0       1     2
        QueryResult skillInfo = WorldDatabase.Query("SELECT SkillID, Value, Max FROM mod_ptrtemplate_skills WHERE (ID = {} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (skillInfo)
        {
            do
            {
                uint16 skillEntry = (*skillInfo)[0].Get<uint16>();
                uint16 valueEntry = (*skillInfo)[1].Get<uint16>();
                uint16 maxEntry = (*skillInfo)[2].Get<uint16>();

                if (((player->GetSkillValue(skillEntry) >= valueEntry) && (player->GetMaxSkillValue(skillEntry) >= maxEntry)) && templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::MAINTAIN_IMPROVED_VALUES))
                    continue;

                player->SetSkill(skillEntry, 0, valueEntry, maxEntry); // Don't know what step overload is used for, being zeroed here.
                LOG_DEBUG("module", "Added skill {} to template character {} with curvalue {} and maxvalue {}.", skillEntry, player->GetGUID().ToString(), valueEntry, maxEntry);
            } while (skillInfo->NextRow());
            player->SaveToDB(false, false);
        }
    }

    static void AddTemplateSpells(Player* player, uint32 index)
    { //                                                       0
        QueryResult spellInfo = WorldDatabase.Query("SELECT SpellID FROM mod_ptrtemplate_spells WHERE (ID = {} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (spellInfo)
        {
            do
            {
                uint64 spellEntry = (*spellInfo)[0].Get<uint64>();

                if (player->HasSpell(spellEntry))
                    continue;

                player->learnSpell(spellEntry);
                LOG_DEBUG("module", "Added spell {} to template character {}.", spellEntry, player->GetGUID().ToString());
            } while (spellInfo->NextRow());

            SendTalentReset(player);
        }
    }

    static void AddTemplateTaxi(Player* player, uint32 index)
    { //                                                          0           1
        QueryResult taxiEntry = WorldDatabase.Query("SELECT TaxiAlliance, TaxiHorde FROM mod_ptrtemplate_index WHERE ID = {}", index);
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

    static void AddTemplateWornGear(Player* player, uint32 index) // Handles paper doll items and equipped bags.
    { //                                                     0      1       2        3         4         5         6         7         8         9
        QueryResult gearInfo = WorldDatabase.Query("SELECT BagID, SlotID, ItemID, Enchant0, Enchant1, Enchant2, Enchant3, Enchant4, Enchant5, Enchant6 FROM mod_ptrtemplate_inventory WHERE (ID = {} AND RaceMask & {} AND ClassMask & {})", index, player->getRaceMask(), player->getClassMask());
        if (gearInfo)
        {
            do
            {
                uint32 bagEntry = (*gearInfo)[0].Get<uint32>();
                uint8 slotEntry = (*gearInfo)[1].Get<uint8>();
                uint32 itemEntry = (*gearInfo)[2].Get<uint32>();

                if ((slotEntry >= INVENTORY_SLOT_BAG_END && slotEntry < BANK_SLOT_BAG_START) || (slotEntry >= BANK_SLOT_BAG_END && slotEntry < PLAYER_SLOT_END) || bagEntry != CONTAINER_BACKPACK) // If item is not either an equipped armorpiece, weapon, or container.
                    continue;

                if (slotEntry >= PLAYER_SLOT_END)
                    player->SetAmmo(itemEntry);
                else
                    player->EquipNewItem(slotEntry, itemEntry, true);

                if (slotEntry >= BANK_SLOT_BAG_START && slotEntry < BANK_SLOT_BAG_END)
                {
                    uint8 slotBuffer = slotEntry - (BANK_SLOT_BAG_START - 1);

                    if (player->GetBankBagSlotCount() < slotBuffer)
                        player->SetBankBagSlotCount(slotBuffer);
                }
                Item* item = player->GetUseableItemByPos(INVENTORY_SLOT_BAG_0, slotEntry);
                player->SendNewItem(item, 1, false, true); // Broadcast item detail packet.
                if (item && item->GetEntry() != itemEntry)
                    continue;

                TemplateHelperItemEnchants(gearInfo, player, item, 3);
            } while (gearInfo->NextRow());
        }
    }

    static void TemplateHelperItemCleanup(Player* player, uint8 scope, uint8 method)
    {
        if (scope == SCOPE_BAGS || scope == SCOPE_ALL)
        {
            if (method != METHOD_DELETE)
            {
                CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
                std::list<Item*> vestigialBagItems;

                for (uint8 b = INVENTORY_SLOT_ITEM_START; b < INVENTORY_SLOT_ITEM_END; b++)
                {
                    Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, b);
                    if (!item)
                        continue;

                    player->MoveItemFromInventory(INVENTORY_SLOT_BAG_0, b, true);
                    item->DeleteFromInventoryDB(trans);
                    vestigialBagItems.push_back(item);
                }
                for (uint8 c = INVENTORY_SLOT_BAG_START; c < INVENTORY_SLOT_BAG_END; c++)
                {
                    for (uint8 i = INVENTORY_SLOT_START; i < MAX_BAG_SIZE; i++)
                    {
                        Item* item = player->GetItemByPos(c, i);
                        if (!item)
                            continue;

                        player->MoveItemFromInventory(c, i, true);
                        item->DeleteFromInventoryDB(trans);
                        vestigialBagItems.push_back(item);
                    }
                }

                while (!vestigialBagItems.empty())
                {
                    std::string subject = player->GetSession()->GetModuleString(module_string, MAIL_BOOST_SUBJECT)[0];
                    std::string content = player->GetSession()->GetModuleString(module_string, MAIL_BOOST_BODY)[0];

                    if (method == METHOD_SCROLL)
                    {
                        subject = player->GetSession()->GetModuleString(module_string, MAIL_RESURRECTION_SUBJECT)[0];
                        content = player->GetSession()->GetModuleString(module_string, MAIL_RESURRECTION_BODY)[0];
                    }

                    MailDraft draft(subject, content);
                    for (uint8 i = 0; !vestigialBagItems.empty() && i < MAX_MAIL_ITEMS; ++i)
                    {
                        draft.AddItem(vestigialBagItems.front());
                        vestigialBagItems.pop_front();
                    }
                    draft.SendMailTo(trans, player, MailSender(player), MAIL_CHECK_MASK_COPIED, 0U, MAILED_ITEM_DELAY);
                }

                CharacterDatabase.CommitTransaction(trans);
            }
            else
            {
                for (uint8 b = INVENTORY_SLOT_ITEM_START; b < INVENTORY_SLOT_ITEM_END; b++) // Iterate each backpack slot.
                    player->DestroyItem(INVENTORY_SLOT_BAG_0, b, true); // Kill.

                for (uint8 c = INVENTORY_SLOT_BAG_START; c < INVENTORY_SLOT_BAG_END; c++) // Iterate each equipped container.
                {
                    for (uint8 i = INVENTORY_SLOT_START; i < MAX_BAG_SIZE; i++) // Iterate each possible container slot.
                        player->DestroyItem(c, i, true); // Kill.
                }
            }
        }
        if (scope == SCOPE_EQUIPPED || scope == SCOPE_ALL)
        {
            if (method != METHOD_DELETE)
            {
                CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
                std::list<Item*> vestigialEquipItems;

                for (uint8 e = EQUIPMENT_SLOT_START; e < INVENTORY_SLOT_BAG_END; e++)
                {
                    Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, e);
                    if (!item)
                        continue;

                    player->MoveItemFromInventory(INVENTORY_SLOT_BAG_0, e, true);
                    item->DeleteFromInventoryDB(trans);
                    vestigialEquipItems.push_back(item);
                }

                while (!vestigialEquipItems.empty())
                {
                    std::string subject = player->GetSession()->GetModuleString(module_string, MAIL_BOOST_SUBJECT)[0];
                    std::string content = player->GetSession()->GetModuleString(module_string, MAIL_BOOST_BODY)[0];

                    if (method == METHOD_SCROLL)
                    {
                        subject = player->GetSession()->GetModuleString(module_string, MAIL_RESURRECTION_SUBJECT)[0];
                        content = player->GetSession()->GetModuleString(module_string, MAIL_RESURRECTION_BODY)[0];
                    }

                    MailDraft draft(subject, content);
                    for (uint8 i = 0; !vestigialEquipItems.empty() && i < MAX_MAIL_ITEMS; ++i)
                    {
                        draft.AddItem(vestigialEquipItems.front());
                        vestigialEquipItems.pop_front();
                    }
                    draft.SendMailTo(trans, player, MailSender(player), MAIL_CHECK_MASK_COPIED, 0U, MAILED_ITEM_DELAY);
                }

                CharacterDatabase.CommitTransaction(trans);
            }
            else
            {
                for (uint8 e = EQUIPMENT_SLOT_START; e < INVENTORY_SLOT_BAG_END; e++) // Iterate through equip slots.
                    player->DestroyItem(INVENTORY_SLOT_BAG_0, e, true); // Kill.
            }
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

    void OnPlayerLogin(Player* player) override
    {
        static createTemplate templatevar;

        if (!templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_ENABLE))
        {
            return;
        }

        if (templateConfigData.GetConfigValue<bool>(PTRTemplateConfig::ANNOUNCE_ENABLE))
            ChatHandler(player->GetSession()).PSendModuleSysMessage(module_string, ALERT_MODULE_PRESENCE);

        uint32 templateIndex = templateConfigData.GetConfigValue<uint32>(PTRTemplateConfig::LOGIN_TEMPLATE_INDEX);
        if (!templateIndex || !player->HasAtLoginFlag(AT_LOGIN_FIRST))
            return;

        uint32 oldMSTime = getMSTime();
        player->GetCinematicMgr()->EndCinematic();
        templatevar.HandleApply(player, templateIndex, 5);
        LOG_DEBUG("module", "Handled template apply for character {} in {} ms.", player->GetGUID().ToString(), (GetMSTimeDiffToNow(oldMSTime) - 100));
        return;
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
            { "apply",   applyTemplate,   SEC_PLAYER,        Console::No },
            { "disable", disableTemplate, SEC_ADMINISTRATOR, Console::Yes },
            { "enable",  enableTemplate,  SEC_ADMINISTRATOR, Console::Yes },
            { "list",    listTemplate,    SEC_PLAYER,        Console::Yes },
        };

        static ChatCommandTable commandTable =
        {
            { "template", templateTable },
        };
        return commandTable;
    }

    static bool enableTemplate(ChatHandler* handler, uint32 index)
    { //                                                   0
        QueryResult result = WorldDatabase.Query("SELECT Enable FROM mod_ptrtemplate_index WHERE ID = {}", index);
        if (result)
        {
            WorldDatabase.Execute("UPDATE mod_ptrtemplate_index SET Enable=1 WHERE ID = {}", index);

            std::string templateName = GetTemplateName(handler, index);

            handler->PSendModuleSysMessage(module_string, FEEDBACK_TEMPLATE_ENABLE, index, templateName);
            return true;
        }
        else
        {
            handler->PSendModuleSysMessage(module_string, FEEDBACK_TEMPLATE_MISSING);
            return false;
        }
    }

    static bool disableTemplate(ChatHandler* handler, uint32 index)
    { //                                                   0
        QueryResult result = WorldDatabase.Query("SELECT Enable FROM mod_ptrtemplate_index WHERE ID = {}", index);
        if (result)
        {
            WorldDatabase.Execute("UPDATE mod_ptrtemplate_index SET Enable=0 WHERE ID = {}", index);

            std::string templateName = GetTemplateName(handler, index);

            handler->PSendModuleSysMessage(module_string, FEEDBACK_TEMPLATE_DISABLE, index, templateName);
            return true;
        }
        else
        {
            handler->PSendModuleSysMessage(module_string, FEEDBACK_TEMPLATE_MISSING);
            return false;
        }
    }

    static bool applyTemplate(ChatHandler* handler, Optional<PlayerIdentifier> player, uint32 index) // TODO: Allow the command to use a target instead of always targetting self.
    { //                                                  0
        QueryResult check = WorldDatabase.Query("SELECT Enable FROM mod_ptrtemplate_index WHERE ID = {}", index); // TODO: Check keywords column for template...keywords.
        static createTemplate templatevar;

        if (check)
        {
            uint8 enable = (*check)[0].Get<uint8>();
            if (!player)
                player = PlayerIdentifier::FromTargetOrSelf(handler);

            Player* target = player->GetConnectedPlayer();

            switch(templatevar.CheckTemplateQualifier(target, index, enable))
            {
                case MISSING_TEMPLATE_INFO:
                    handler->PSendModuleSysMessage(module_string, ERROR_TEMPLATE_INFO);
                    return true;
                case NOT_INITIAL_LEVEL:
                    handler->PSendModuleSysMessage(module_string, ERROR_TEMPLATE_LEVEL);
                    return true;
                case TEMPLATE_DISABLED_GLOBAL:
                    handler->PSendModuleSysMessage(module_string, ERROR_TEMPLATE_DIABLE_GLOBAL);
                    return true;
                case INSUFFICIENT_SECURITY_LEVEL:
                    handler->PSendModuleSysMessage(module_string, ERROR_TEMPLATE_SECURITY);
                    return true;
                case TEMPLATE_DISABLED_LOCAL:
                    handler->PSendModuleSysMessage(module_string, ERROR_TEMPLATE_DISABLE_LOCAL);
                    return true;
                default:
                    break;
            }
            uint32 oldMSTime = getMSTime();
            templatevar.HandleApply(target, index);
            LOG_DEBUG("module", "Handled template apply for character {} in {} ms.", player->GetGUID().ToString(), (GetMSTimeDiffToNow(oldMSTime) - 100));
            return true;
        }
        else
        {
            handler->PSendModuleSysMessage(module_string, FEEDBACK_TEMPLATE_MISSING);
            return true;
        }
    }

    static bool listTemplate(ChatHandler* handler)
    { //                                                 0     1
        QueryResult index = WorldDatabase.Query("SELECT ID, Enable FROM mod_ptrtemplate_index ORDER BY ID");
        if (index)
        {
            handler->PSendModuleSysMessage(module_string, MESSAGE_TEMPLATE_LIST);

            int8 playerSecurity = handler->IsConsole()
                ? SEC_CONSOLE
                : handler->GetSession()->GetSecurity();

            do
            {
                uint8 indexEntry = (*index)[0].Get<uint8>();
                uint8 enableEntry = (*index)[1].Get<uint8>();
                std::string templateName = GetTemplateName(handler, indexEntry);

                if ((playerSecurity >= templateConfigData.GetConfigValue<int8>(PTRTemplateConfig::ENABLE_LIST_SECURITY) && enableEntry) || (playerSecurity >= templateConfigData.GetConfigValue<int8>(PTRTemplateConfig::DISABLE_LIST_SECURITY) && !enableEntry))
                {
                    if (playerSecurity >= templateConfigData.GetConfigValue<int8>(PTRTemplateConfig::STATUS_SECURITY_TEXT))
                    {
                        std::string enableText = enableEntry
                            ? handler->GetModuleString(module_string, DETAIL_ENABLE)[0]
                            : handler->GetModuleString(module_string, DETAIL_DISABLE)[0];

                        handler->PSendModuleSysMessage(module_string, MESSAGE_TEMPLATE_LIST_DETAIL, indexEntry, templateName, enableText);
                    }
                    else
                        handler->PSendModuleSysMessage(module_string, MESSAGE_TEMPLATE_LIST_SIMPLE, indexEntry, templateName);
                }
            } while (index->NextRow());
        }
        else
            handler->PSendModuleSysMessage(module_string, MESSAGE_TEMPLATE_LIST_EMPTY);

        return true;
    }

private:
    static std::string GetTemplateName(ChatHandler* handler, uint8 index)
    {
        LocaleConstant locale = LOCALE_enUS;

        if (!handler->IsConsole())
            locale = handler->GetSession()->GetSessionDbLocaleIndex();

        //                                                   0
        QueryResult defQuery = WorldDatabase.Query("SELECT Comment FROM mod_ptrtemplate_index WHERE ID = {}", index);

        std::string defName = (*defQuery)[0].Get<std::string>();

        if (locale != LOCALE_enUS)
        { //                                                    0     1     2     3     4     5     6     7
            QueryResult locQuery = WorldDatabase.Query("SELECT koKR, frFR, deDE, zhCN, zhTW, esES, esMX, ruRU FROM mod_ptrtemplate_locale WHERE ID = {}", index);
            if (locQuery)
            {
                std::string locName = (*locQuery)[locale - 1].Get<std::string>();

                if (!locName.empty())
                    return locName;
            }
        }

        return defName; // If locale == enUS || no localized text found, send enUS
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
