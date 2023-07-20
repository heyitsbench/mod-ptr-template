SET @string = 40000;

DELETE FROM `acore_string` WHERE `entry` BETWEEN @string AND @string+16;
INSERT INTO `acore_string` (`entry`, `content_default`,             `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(@string, 'Enabled template %u (%s).',                              NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+1, 'This template has not been added.',                    NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+2, 'Disabled template %u (%s).',                           NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+3, 'The selected template does not apply to you.',         NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+4, 'You must be a new character to apply this template.',  NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+5, 'Templates currently cannot be applied.',               NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+6, 'You do not meet the security to apply templates.',     NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+7, 'This template is disabled.',                           NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+8, 'Please logout for the template to fully apply.',       NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+9, 'Available template sets:',                             NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+10, '%u (%s): %s',                                         NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+11, '%u (%s)',                                             NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+12, 'There are no added templates.',                       NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+13, 'Enabled',                                             NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+14, 'Disabled',                                            NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+15, 'This server is running the PTR Template module.',     NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL),
(@string+16, 'There was a problem equipping the attached item(s).', NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          NULL);
