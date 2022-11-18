DELETE FROM `command` WHERE `name` IN ('template enable', 'template disable', 'template apply', 'template list');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('template enable', 1, 'Syntax: .template enable $indexID'),
('template disable', 1, 'Syntax: .template disable $indexID'),
('template apply', 0, 'Syntax: .template apply $indexID'),
('template list', 0, 'Syntax: .template list');