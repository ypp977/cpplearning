-- 创建log表
CREATE TABLE log (
    id INT PRIMARY KEY AUTO_INCREMENT, -- 日志ID
    file VARCHAR(255), -- 文件名
    function_name VARCHAR(255), -- 函数名
    line INT, -- 行数
    time VARCHAR(255), -- 时间
    info VARCHAR(255), -- info
    type INT -- 1 - 表示操作信息 0 - 表示报错信息
);
