-- 记得创建数据库overflow，以及use该数据库

-- 创建fileInfo表
CREATE TABLE fileInfo (
    id INT PRIMARY KEY AUTO_INCREMENT, -- 文件ID，采用自增的方式赋予唯一标识符
    filename VARCHAR(255), -- 文件名，用于存储文件的名称
    type CHAR(1), -- 文件类型，用于指明文件的类型，d-目录，f-文件
    pre_id INT, -- 所属目录ID，用于记录文件/目录所属的上一级目录（如果有，-1表示根目录）
    username VARCHAR(255), -- 用户名，用于关联文件所属的用户
    path VARCHAR(255), -- 文件路径，用于存储文件在文件系统中的路径信息
    tomb INT DEFAULT 0, -- 删除标记，用于标记文件是否已删除，0表示未删除，1表示已删除
    md5 VARCHAR(32), -- 文件MD5码，表示文件内容，NULL表示为目录
    UNIQUE (username, path) -- 将username和path设置为唯一键
);


-- fileInfo表的初始化数据（执行完后请确保这些目录和文件也实际存在）
INSERT INTO `fileInfo`(`id`, `filename`, `type`, `pre_id`, `username`, `path`, `tomb`, `md5`) VALUES (1, 'dir1', 'd', -1, 'over', '/dir1', 0, NULL);
INSERT INTO `fileInfo`(`id`, `filename`, `type`, `pre_id`, `username`, `path`, `tomb`, `md5`) VALUES (2, 'dir2', 'd', -1, 'over', '/dir2', 0, NULL);
INSERT INTO `fileInfo`(`id`, `filename`, `type`, `pre_id`, `username`, `path`, `tomb`, `md5`) VALUES (3, 'file1', 'f', 1, 'over', '/dir1/file1', 0, NULL);
INSERT INTO `fileInfo`(`id`, `filename`, `type`, `pre_id`, `username`, `path`, `tomb`, `md5`) VALUES (4, 'dir3', 'd', 2, 'over', '/dir1/dir3', 0, NULL);
INSERT INTO `fileInfo`(`id`, `filename`, `type`, `pre_id`, `username`, `path`, `tomb`, `md5`) VALUES (5, 'file2', 'f', 2, 'over', '/dir2/file2', 0, NULL);
INSERT INTO `fileInfo`(`id`, `filename`, `type`, `pre_id`, `username`, `path`, `tomb`, `md5`) VALUES (6, 'dir4', 'd', 4, 'over', '/dir2/dir3/dir4', 0, NULL);
INSERT INTO `fileInfo`(`id`, `filename`, `type`, `pre_id`, `username`, `path`, `tomb`, `md5`) VALUES (7, 'dir1', 'd', -1, 'test', '/dir1', 0, NULL);
INSERT INTO `fileInfo`(`id`, `filename`, `type`, `pre_id`, `username`, `path`, `tomb`, `md5`) VALUES (8, 'dir2', 'd', -1, 'test', '/dir2', 0, NULL);
INSERT INTO `fileInfo`(`id`, `filename`, `type`, `pre_id`, `username`, `path`, `tomb`, `md5`) VALUES (9, 'file1', 'f', -1, 'test', '/file1', 0, NULL);
INSERT INTO `fileInfo`(`id`, `filename`, `type`, `pre_id`, `username`, `path`, `tomb`, `md5`) VALUES (10, 'dir5', 'd', 7, 'test', '/dir1/dir5', 0, NULL);
