DROP TABLE IF EXISTS `mod_ptrtemplate_achievements`;
DROP TABLE IF EXISTS `mod_ptrtemplate_action`;
DROP TABLE IF EXISTS `mod_ptrtemplate_index`;
DROP TABLE IF EXISTS `mod_ptrtemplate_inventory`;
DROP TABLE IF EXISTS `mod_ptrtemplate_quests`;
DROP TABLE IF EXISTS `mod_ptrtemplate_reputations`;
DROP TABLE IF EXISTS `mod_ptrtemplate_skills`;
DROP TABLE IF EXISTS `mod_ptrtemplate_spells`;

DELETE FROM `command` WHERE `name` IN ('template apply', 'template disable', 'template enable', 'template list');
