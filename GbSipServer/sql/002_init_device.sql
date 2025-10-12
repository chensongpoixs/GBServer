





CREATE TABLE t_device (
    id          INTEGER PRIMARY KEY,
    deviceid    VARCHAR,
    online        INTEGER,
     role        VARCHAR
);


INSERT INTO t_device
(deviceid, online,  role) VALUES ('admin', 0, 'ROLE_ADMIN');