#include "Chat.h"
#include "Config.h"
#include "ConfigData.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"
#include "TaskScheduler.h"

#define module_string "ptr-template"

TaskScheduler scheduler;

enum checkCodes
{
    MISSING_TEMPLATE_INFO       = 1,
    NOT_INITIAL_LEVEL           = 2,
    TEMPLATE_DISABLED_GLOBAL    = 3,
    INSUFFICIENT_SECURITY_LEVEL = 4,
    TEMPLATE_DISABLED_LOCAL     = 5,
    CHECK_PASSED                = 0
};

enum templateStrings
{
    FEEDBACK_TEMPLATE_ENABLE     = 0,
    FEEDBACK_TEMPLATE_MISSING    = 1,
    FEEDBACK_TEMPLATE_DISABLE    = 2,
    ERROR_TEMPLATE_INFO          = 3,
    ERROR_TEMPLATE_LEVEL         = 4,
    ERROR_TEMPLATE_DIABLE_GLOBAL = 5,
    ERROR_TEMPLATE_SECURITY      = 6,
    ERROR_TEMPLATE_DISABLE_LOCAL = 7,
    MESSAGE_TEMPLATE_LIST        = 8,
    MESSAGE_TEMPLATE_LIST_DETAIL = 9,
    MESSAGE_TEMPLATE_LIST_SIMPLE = 10,
    MESSAGE_TEMPLATE_LIST_EMPTY  = 11,
    DETAIL_ENABLE                = 12,
    DETAIL_DISABLE               = 13,
    ALERT_MODULE_PRESENCE        = 14,
    MAIL_ERROR_EQUIP_BODY        = 15,
    MAIL_BOOST_SUBJECT           = 16,
    MAIL_BOOST_BODY              = 17,
    MAIL_RESURRECTION_SUBJECT    = 18,
    MAIL_RESURRECTION_BODY       = 19
};

enum templateMail
{
    MAIL_BOOST        = 403,
    MAIL_RESURRECTION = 344
};

enum itemCleanupCodes
{
    METHOD_DELETE  = 0,
    METHOD_BOOST   = 1,
    METHOD_SCROLL  = 2,
    SCOPE_EQUIPPED = 0,
    SCOPE_BAGS     = 1,
    SCOPE_ALL      = 2
};

enum TemplateEnums
{
    APPLY_DELAY          = 25,
    APPLY_RATE           = 50,
    HORDE_SIMILAR        = -1,
    ACTION_BUTTON_BEGIN  = 0,
    CONTAINER_BACKPACK   = 0,
    CONTAINER_FINISH     = 5,
    ITEM_GOLD            = 8,
    INVENTORY_SLOT_START = 0,
    MAILED_ITEM_DELAY    = 180
};

enum class PTRTemplateConfig
{
    ANNOUNCE_ENABLE,
    TEMPLATE_ENABLE,
    LEVEL_ENABLE,
    TEMPLATE_ACHIEVEMENTS,
    TEMPLATE_BAG_GEAR,
    TEMPLATE_DEATH_KNIGHT,
    TEMPLATE_EQUIP_GEAR,
    TEMPLATE_HOMEBIND,
    TEMPLATE_HOTBAR,
    TEMPLATE_LEVEL,
    TEMPLATE_QUESTS,
    TEMPLATE_REPUTATION,
    TEMPLATE_RESOURCES,
    TEMPLATE_SKILLS,
    TEMPLATE_SPELLS,
    TEMPLATE_TAXIMASK,
    TEMPLATE_TELEPORT,
    DELETE_ITEMS,
    LOGIN_TEMPLATE_INDEX,
    MAINTAIN_IMPROVED_VALUES,
    ENABLE_APPLY_SECURITY,
    DISABLE_APPLY_SECURITY,
    ENABLE_LIST_SECURITY,
    DISABLE_LIST_SECURITY,
    STATUS_SECURITY_TEXT,

    NUM_CONFIGS,
};

class PTRTemplateConfigData : public ConfigData<PTRTemplateConfig>
{
public:
    PTRTemplateConfigData() : ConfigData(PTRTemplateConfig::NUM_CONFIGS) { };

    void BuildConfigCache() override
    {
        SetConfigValue<bool>(PTRTemplateConfig::ANNOUNCE_ENABLE,            "AnnounceEnable",           true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_ENABLE,            "TemplateEnable",           true);
        SetConfigValue<bool>(PTRTemplateConfig::LEVEL_ENABLE,               "LevelEnable",              true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_ACHIEVEMENTS,      "TemplateAchievements",     true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_BAG_GEAR,          "TemplateBagGear",          true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_DEATH_KNIGHT,      "TemplateDK",               true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_EQUIP_GEAR,        "TemplateEquipGear",        true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_HOMEBIND,          "TemplateHomebind",         true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_HOTBAR,            "TemplateHotbar",           true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_LEVEL,             "TemplateLevel",            true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_QUESTS,            "TemplateQuests",           false);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_REPUTATION,        "TemplateReputation",       true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_RESOURCES,         "TemplateResources",        true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_SKILLS,            "TemplateSkills",           true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_SPELLS,            "TemplateSpells",           true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_TAXIMASK,          "TemplateTaximask",         true);
        SetConfigValue<bool>(PTRTemplateConfig::TEMPLATE_TELEPORT,          "TemplateTeleport",         true);
        SetConfigValue<bool>(PTRTemplateConfig::DELETE_ITEMS,               "DeleteItems",              true);
        SetConfigValue<uint32>(PTRTemplateConfig::LOGIN_TEMPLATE_INDEX,     "LoginTemplateIndex",       0);
        SetConfigValue<bool>(PTRTemplateConfig::MAINTAIN_IMPROVED_VALUES,   "MaintainImprovedValues",   true);
        SetConfigValue<int8>(PTRTemplateConfig::ENABLE_APPLY_SECURITY,      "EnableApplySecurity",      0);
        SetConfigValue<int8>(PTRTemplateConfig::DISABLE_APPLY_SECURITY,     "DisableApplySecurity",     3);
        SetConfigValue<int8>(PTRTemplateConfig::ENABLE_LIST_SECURITY,       "EnableListSecurity",       0);
        SetConfigValue<int8>(PTRTemplateConfig::DISABLE_LIST_SECURITY,      "DisableListSecurity",      2);
        SetConfigValue<int8>(PTRTemplateConfig::STATUS_SECURITY_TEXT,       "StatusSecurityText",       2);
    }
};

static PTRTemplateConfigData templateConfigData;

void SendTalentReset(Player* player)
{
    WorldPacket data(SMSG_TALENTS_INVOLUNTARILY_RESET, 1);
    data << uint8(0);
    player->SendMessageToSet(&data, true);
}

void Add_ptr_template();
void AddSC_ptr_template_commandscript();

void Addmod_ptr_templateScripts()
{
    Add_ptr_template();
    AddSC_ptr_template_commandscript();
}
