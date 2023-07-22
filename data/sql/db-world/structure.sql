DROP TABLE IF EXISTS `mod_ptrtemplate_index`;
CREATE TABLE IF NOT EXISTS `mod_ptrtemplate_index` (
	`ID` TINYINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Template index ID',
	`Keywords` TEXT NULL DEFAULT NULL,
	`Level` TINYINT(3) UNSIGNED NOT NULL DEFAULT '1' COMMENT 'Level to set for characters',
	`MapAlliance` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Map ID for Alliance characters to be teleported to',
	`XAlliance` FLOAT NOT NULL DEFAULT '0' COMMENT 'X coordinate for Alliance characters to be teleported to',
	`YAlliance` FLOAT NOT NULL DEFAULT '0' COMMENT 'Y coordinate for Alliance characters to be teleported to',
	`ZAlliance` FLOAT NOT NULL DEFAULT '0' COMMENT 'Z coordinate for Alliance characters to be teleported to',
	`OAlliance` FLOAT NOT NULL DEFAULT '0' COMMENT 'Orientation for Alliance characters to be turned to',
	`MapHorde` SMALLINT(5) NOT NULL DEFAULT '0' COMMENT 'Map ID for Horde characters to be teleported to',
	`XHorde` FLOAT NOT NULL DEFAULT '0' COMMENT 'X coordinate for Horde characters to be teleported to',
	`YHorde` FLOAT NOT NULL DEFAULT '0' COMMENT 'Y coordinate for Horde characters to be teleported to',
	`ZHorde` FLOAT NOT NULL DEFAULT '0' COMMENT 'Z coordinate for Horde characters to be teleported to',
	`OHorde` FLOAT NOT NULL DEFAULT '0' COMMENT 'Orientation for Horde characters to be turned to',
	`HMapAlliance` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Map ID for Alliance characters'' hearthstones',
	`HZoneAlliance` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Zone ID for Alliance characters'' hearthstones',
	`HXAlliance` FLOAT NOT NULL DEFAULT '0' COMMENT 'X coordinate for Alliance characters'' hearthstones',
	`HYAlliance` FLOAT NOT NULL DEFAULT '0' COMMENT 'Y coordinate for Alliance characters'' hearthstones',
	`HZAlliance` FLOAT NOT NULL DEFAULT '0' COMMENT 'Z coordinate for Alliance characters'' hearthstones',
	`HOAlliance` FLOAT NOT NULL DEFAULT '0' COMMENT 'Orientation for Alliance characters'' hearthstones',
	`HMapHorde` SMALLINT(5) NOT NULL DEFAULT '0' COMMENT 'Map ID for Horde characters'' hearthstones',
	`HZoneHorde` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Zone ID for Horde characters'' hearthstones',
	`HXHorde` FLOAT NOT NULL DEFAULT '0' COMMENT 'X coordinate for Horde characters'' hearthstones',
	`HYHorde` FLOAT NOT NULL DEFAULT '0' COMMENT 'Y coordinate for Horde characters'' hearthstones',
	`HZHorde` FLOAT NOT NULL DEFAULT '0' COMMENT 'Z coordinate for Horde characters'' hearthstones',
	`HOHorde` FLOAT NOT NULL DEFAULT '0' COMMENT 'Orientation for Horde characters'' hearthstones',
	`TaxiAlliance` TEXT NULL DEFAULT NULL COMMENT 'TaxiMask for Alliance characters',
	`TaxiHorde` TEXT NULL DEFAULT NULL COMMENT 'TaxiMask for Horde characters',
	`Enable` TINYINT(1) NOT NULL DEFAULT '0' COMMENT 'Determines if the template can be applied or not',
	`Comment` TEXT NULL DEFAULT NULL COMMENT 'Name for the template',
	PRIMARY KEY (`ID`)
) COLLATE='utf8mb4_general_ci';

DROP TABLE IF EXISTS `mod_ptrtemplate_achievements`;
CREATE TABLE IF NOT EXISTS `mod_ptrtemplate_achievements` (
	`ID` TINYINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Template index ID',
	`RaceMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable races',
	`ClassMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable classes',
	`AchievementID` INT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Achievement entry',
	`Comment` TEXT NULL DEFAULT NULL
) COLLATE='utf8mb4_general_ci';

DROP TABLE IF EXISTS `mod_ptrtemplate_action`;
CREATE TABLE IF NOT EXISTS `mod_ptrtemplate_action` (
	`ID` TINYINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Template index ID',
	`RaceMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable races',
	`ClassMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable classes',
	`Button` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
	`Action` INT(10) UNSIGNED NOT NULL DEFAULT '0',
	`Type` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0',
	`Comment` TEXT NULL DEFAULT NULL
) COLLATE='utf8mb4_general_ci';

DROP TABLE IF EXISTS `mod_ptrtemplate_inventory`;
CREATE TABLE IF NOT EXISTS `mod_ptrtemplate_inventory` (
	`ID` TINYINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Template index ID',
	`RaceMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable races',
	`ClassMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable classes',
	`BagID` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bag ID the item should be placed in',
	`SlotID` TINYINT(3) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Slot ID the item should be placed in',
	`ItemID` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Item entry',
	`Quantity` INT(10) UNSIGNED NOT NULL DEFAULT '1' COMMENT 'Number of items to add',
	`Enchant0` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Permanent enchant',
	`Enchant1` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Temporary enchant',
	`Enchant2` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'First socket enchant',
	`Enchant3` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Second socket enchant',
	`Enchant4` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Third socket enchant',
	`Enchant5` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bonus enchant',
	`Enchant6` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Prismatic enchant',
	`Comment` TEXT NULL DEFAULT NULL
) COLLATE='utf8mb4_general_ci';

DROP TABLE IF EXISTS `mod_ptrtemplate_locale`;
CREATE TABLE IF NOT EXISTS `mod_ptrtemplate_locale` (
	`ID` TINYINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Template index ID',
	`koKR` TEXT NULL DEFAULT NULL COMMENT 'Korean template name translation',
	`frFR` TEXT NULL DEFAULT NULL COMMENT 'French template name translation',
	`deDE` TEXT NULL DEFAULT NULL COMMENT 'German template name translation',
	`zhCN` TEXT NULL DEFAULT NULL COMMENT 'Simplified Chinese template name translation',
	`zhTW` TEXT NULL DEFAULT NULL COMMENT 'Traditional Chinese template name translation',
	`esES` TEXT NULL DEFAULT NULL COMMENT 'Spanish (EU) template name translation',
	`esMX` TEXT NULL DEFAULT NULL COMMENT 'Spanish (Latin American) template name translation',
	`ruRU` TEXT NULL DEFAULT NULL COMMENT 'Russian template name translation',
	PRIMARY KEY (`ID`)
) COLLATE='utf8mb4_general_ci';

DROP TABLE IF EXISTS `mod_ptrtemplate_quests`;
CREATE TABLE IF NOT EXISTS `mod_ptrtemplate_quests` (
	`ID` TINYINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Template index ID',
	`RaceMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable races',
	`ClassMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable classes',
	`QuestID` MEDIUMINT(7) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'quest_template entry ID',
	`Comment` TEXT NULL DEFAULT NULL
) COLLATE='utf8mb4_general_ci';

DROP TABLE IF EXISTS `mod_ptrtemplate_reputations`;
CREATE TABLE IF NOT EXISTS `mod_ptrtemplate_reputations` (
	`ID` TINYINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Template index ID',
	`RaceMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable races',
	`ClassMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable classes',
	`FactionID` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Faction entry',
	`Standing` INT(10) NOT NULL DEFAULT '0' COMMENT 'Reputation value',
	`Comment` TEXT NULL DEFAULT NULL
) COLLATE='utf8mb4_general_ci';

DROP TABLE IF EXISTS `mod_ptrtemplate_skills`;
CREATE TABLE IF NOT EXISTS `mod_ptrtemplate_skills` (
	`ID` TINYINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Template index ID',
	`RaceMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable races',
	`ClassMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable classes',
	`SkillID` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'SkillLine entry',
	`Value` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Skill level',
	`Max` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Max value of the skill',
	`Comment` TEXT NULL DEFAULT NULL
) COLLATE='utf8mb4_general_ci';

DROP TABLE IF EXISTS `mod_ptrtemplate_spells`;
CREATE TABLE IF NOT EXISTS `mod_ptrtemplate_spells` (
	`ID` TINYINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Template index ID',
	`RaceMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable races',
	`ClassMask` SMALLINT UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Bitmask for applicable classes',
	`SpellID` MEDIUMINT(7) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Spell entry',
	`Comment` TEXT NULL DEFAULT NULL
) COLLATE='utf8mb4_general_ci';
