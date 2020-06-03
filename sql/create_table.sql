-- 创建用户表
CREATE TABLE IF NOT EXISTS `user`(
    `id` INT UNSIGNED AUTO_INCREMENT,
    `user_id` INT UNSIGNED NOT NULL UNIQUE,
    `user_name` VARCHAR(50) NOT NULL,
    `user_password` VARCHAR(50) NOT NULL,
    PRIMARY KEY (`id`)
) ENGINE = InnoDB DEFAULT CHARSET = utf8;
-- 插入几个用户
INSERT INTO user (user_id, user_name, user_password)
VALUES (101, 'wxn', 'wxn'),
    (102, 'wzf', 'wzf'),
    (103, 'tl', 'tl');