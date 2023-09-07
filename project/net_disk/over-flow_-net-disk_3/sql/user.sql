-- 创建user表
CREATE TABLE user (
    id INT PRIMARY KEY AUTO_INCREMENT, -- 用户ID
    username VARCHAR(255), -- 用户名
    salt VARCHAR(20), -- 密码盐值
    password VARCHAR(255) -- 加密后的密码
);
