DELETE FROM `item_template` WHERE `entry` = 189427;
INSERT INTO `item_template` (`entry`, `class`, `subclass`, `SoundOverrideSubclass`, `name`, `displayid`, `Quality`, `Flags`, `FlagsExtra`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `StatsCount`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `ScalingStatDistribution`, `ScalingStatValue`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `RequiredDisenchantSkill`, `ArmorDamageModifier`, `duration`, `ItemLimitCategory`, `HolidayId`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`, `flagsCustom`, `VerifiedBuild`) VALUES
(189427, 0, 0, -1, 'More Raid Consumables', 0, 1, 4, 24580, 1, 0, 0, 0, -1, -1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, 1000, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, '', 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, '', 0, 0, 0, 0, 0, 45437);


DELETE FROM `item_loot_template` WHERE `Entry` = 189427;
INSERT INTO `item_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(189427, 12457, 0, 100, 0, 1, 1, 20, 20, 'More Raid Consumables - Juju Chill'),
(189427, 13446, 0, 100, 0, 1, 1, 5, 5, 'More Raid Consumables - Major Healing Potion'),
(189427, 13455, 0, 100, 0, 1, 1, 5, 5, 'More Raid Consumables - Greater Stoneshield Potion'),
(189427, 5634, 0, 100, 0, 1, 1, 5, 5, 'More Raid Consumables - Free Action Potion'),
(189427, 9088, 0, 100, 0, 1, 1, 5, 5, 'More Raid Consumables - Gift of Arthas'),
(189427, 9206, 0, 100, 0, 1, 1, 5, 5, 'More Raid Consumables - Elixir of Giants'),
(189427, 12455, 0, 100, 0, 1, 1, 20, 20, 'More Raid Consumables - Juju Ember'),
(189427, 12460, 0, 100, 0, 1, 1, 20, 20, 'More Raid Consumables - Juju Might'),
(189427, 3387, 0, 100, 0, 1, 1, 5, 5, 'More Raid Consumables - Limited Invulnerability Potion'),
(189427, 3826, 0, 100, 0, 1, 1, 5, 5, 'More Raid Consumables - Mighty Troll\'s Blood Potion'),
(189427, 12451, 0, 100, 0, 1, 1, 20, 20, 'More Raid Consumables - Juju Power'),
(189427, 12384, 0, 100, 0, 1, 1, 1, 1, 'More Raid Consumables - Cache of Mau\'ari'),
(189427, 12404, 0, 100, 0, 1, 1, 20, 20, 'More Raid Consumables - Dense Sharpening Stone'),
(189427, 3825, 0, 100, 0, 1, 1, 5, 5, 'More Raid Consumables - Elixir of Fortitude'),
(189427, 13442, 0, 100, 0, 1, 1, 5, 5, 'More Raid Consumables - Mighty Rage Potion'),
(189427, 12820, 0, 100, 0, 1, 1, 5, 5, 'More Raid Consumables - Winterfall Firewater'),
(189427, 9264, 0, 100, 0, 1, 1, 5, 5, 'More Raid Consumables - Elixir of Shadow Power'),
(189427, 6657, 0, 100, 0, 1, 1, 20, 20, 'More Raid Consumables - Savory Deviate Delight'),
(189427, 12190, 0, 100, 0, 1, 1, 5, 5, 'More Raid Consumables - Dreamless Sleep Potion'),
(189427, 12662, 0, 100, 0, 1, 1, 20, 20, 'More Raid Consumables - Demonic Rune');
