SET
@entry = 184320, -- Blizzlike
@guid = 4000000, -- ¯\_(ツ)_/¯
@text = 64000, -- ¯\_(ツ)_/¯
@menu = 27525; -- Blizzlike (I think)

DELETE FROM `creature_template` WHERE `entry` = @entry; 
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `speed_swim`, `speed_flight`, `detection_range`, `scale`, `rank`, `dmgschool`, `DamageModifier`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `ExperienceModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(@entry, 0, 0, 0, 0, 0, 127, 0, 0, 0, 'Cap\'n Placeholder', 'Beta Vendorrr and Teleporterrr', 'Speak', 27525, 60, 60, 0, 35, 4225, 2.5, 8, 4.72221994400024414, 7, 20, 2, 0, 0, 1, 2000, 2000, 1, 1, 1, 768, 2048, 0, 0, 0, 0, 0, 0, 7, 134217728, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, '', 45437); -- Somewhat blizzlike

DELETE FROM `creature` WHERE `id1` = @entry;
INSERT INTO `creature` (`guid`, `id1`, `id2`, `id3`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`, `ScriptName`, `VerifiedBuild`) VALUES
(@guid, @entry, 0, 0, 0, 0, 0, 1, 1, 0, -8826.973, 633.6285, 94.33641, 3.773838758468627929, 300, 0, 0, 3052, 0, 0, 0, 0, 0, '', 45437), -- Mostly blizzlike hopefully
(@guid+1, @entry, 0, 0, 1, 0, 0, 1, 1, 0, 1563.5122, -4403.127, 6.785325, 3.387704849243164062, 300, 0, 0, 3052, 0, 0, 0, 0, 0, '', 45437);

DELETE FROM `npc_text` WHERE `ID` = @text;
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`, `VerifiedBuild`) VALUES
(@text, 10117, 10117, 45437);

DELETE FROM `gossip_menu` WHERE `MenuID` = @menu;
INSERT INTO `gossip_menu` (`MenuID`, `TextID`) VALUES
(@menu, @text);
