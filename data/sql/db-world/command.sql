DELETE FROM `command` WHERE `name` IN ('template apply', 'template disable', 'template enable', 'template list');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('template apply', 0, 'Syntax: .template apply $IndexID\nApplies the given template to your character.'),
('template disable', 3, 'Syntax: .template disable $IndexID\nDisables the given template from being applied.'),
('template enable', 3, 'Syntax: .template enable $IndexID\nEnables the given template to be applied.'),
('template list', 0, 'Syntax: .template list\nList character template sets.');
