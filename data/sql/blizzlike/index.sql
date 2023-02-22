SET
@somsixty = 1,
@wotlkseventy = 2,
@wotlkeighty = 3;

DELETE FROM `mod_ptrtemplate_index` WHERE `ID` IN (@somsixty, @wotlkseventy, @wotlkeighty);
INSERT INTO `mod_ptrtemplate_index` (`ID`, `Keywords`, `Level`, `MapAlliance`, `XAlliance`, `YAlliance`, `ZAlliance`, `OAlliance`, `MapHorde`, `XHorde`, `YHorde`, `ZHorde`, `OHorde`, `HMapAlliance`, `HZoneAlliance`, `HXAlliance`, `HYAlliance`, `HZAlliance`, `HOAlliance`, `HMapHorde`, `HZoneHorde`, `HXHorde`, `HYHorde`, `HZHorde`, `HOHorde`, `TaxiAlliance`, `TaxiHorde`, `Enable`, `Comment`) VALUES
(@somsixty, NULL, 60, 0, -8865.09, 671.793, 97.9034, 2.16098, 1, 1633.8, -4440.47, 15.634, 3.08343, 0, 1537, -4840.65, -861.023, 501.915, 1.69675, 1, 1637, 1633.24, -4440.51, 15.6699, 5.71953, '3456411898 2148078929 49991 8 0 0 0 0 0 0 0 0 0 0', '830150144 315656864 56504 4 0 0 0 0 0 0 0 0 0 0', 0, 'Season of Mastery AQ40 Level 60'),
(@wotlkseventy, NULL, 70, 0, -8414.12, 1052.65, 68.3842, 1.35753, 1, 1529.99, -4412.64, 12.9766, 6.27874, 530, 3703, -1845.01, 5392.53, -12.428, 2.08404, -1, 0, 0, 0, 0, 0, '3456411898 2148078929 49991 2607808536 2284848129 262248 1052676 0 0 0 0 0 0 0', '830150144 315656864 449720 3869245476 2153780226 262180 1052672 0 0 0 0 0 0 0', 0, 'Wrath Classic Level 70'),
(@wotlkeighty, NULL, 80, 571, 5852.44, 692.975, 642.039, 0.856949, -1, 0, 0, 0, 0, 571, 4395, 5718.6, 682.702, 645.751, 3.23121, 571, 4395, 5887.89, 511.109, 641.57, 4.89159, NULL, NULL, 0, 'Wrath Classic Level 80');