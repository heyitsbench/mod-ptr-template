DELETE FROM `command` WHERE `name` IN ('template apply', 'template disable', 'template enable', 'template list');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('template apply', 0, 'Syntax: .template apply $IndexID'),
('template disable', 1, 'Syntax: .template disable $IndexID'),
('template enable', 1, 'Syntax: .template enable $IndexID'),
('template list', 0, 'Syntax: .template list');
