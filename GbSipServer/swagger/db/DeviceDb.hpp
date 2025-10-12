



#ifndef _C_DEVICE_DB_H_
#define _C_DEVICE_DB_H_

#include "dto/UserDto.hpp"
#include "oatpp-sqlite/orm.hpp"
#include "swagger/dto/DeviceDto.hpp"
#include "oatpp/base/Log.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient) //<- Begin Codegen

/**
 * DeviceDb client definitions.
 */
class DeviceDb : public oatpp::orm::DbClient {
public:

  DeviceDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
    : oatpp::orm::DbClient(executor)
  {

    oatpp::orm::SchemaMigration migration(executor);
    migration.addFile(2 /* start from version 1 */, DATABASE_MIGRATIONS "/002_init_device.sql");
	//migration.addFile(2 /* start from version 1 */, DATABASE_MIGRATIONS "/001_init_device.sql");
    // TODO - Add more migrations here.
    migration.migrate(); // <-- run migrations. This guy will throw on error.

    auto version = executor->getSchemaVersion();
    OATPP_LOGd("DeviceDb", "Migration - OK. Version={}.", version);

  }
  //QUERY(createDevice,
//      "INSERT INTO t_device"
//      "(device_id, type, device_name, channel_count, password, catalog_subscribe, online, sip_transport, "
  //    "media_transport, media_transport_mode, last_register_at, last_keepalive_at, update_at, create_at, role) VALUES "
//      "(:device.deviceid, :device.type, :device.devicename, :device.channelcount , "
  //    ":device.password, :device:catalogsubscribe, :device.online, device:siptransport, "
  //    ":device.mediatransport, :device.mediatransportmode, device:lastregisterat, "
  //    ":device:lastkeepaliveat,  device:updateat, device:createat, :device.role);",
//      PARAM(oatpp::Object<DeviceDto>, device))


  QUERY(createDevice,
        "INSERT INTO t_device"
        "(deviceid, online, role) VALUES "
        "(:device.deviceid, :device.online, :device.role);",
        PARAM(oatpp::Object<DeviceDto>, device))

  QUERY(updateDevice,
        "UPDATE t_device "
        "SET "
        " online=:device.online, "
        "WHERE "
        " id=:device.id;",
        PARAM(oatpp::Object<DeviceDto>, device))

  QUERY(getDeviceById,
        "SELECT * FROM t_device WHERE id=:id;",
        PARAM(oatpp::Int32, id))

  QUERY(getAllDevices,
        "SELECT * FROM t_device LIMIT :limit OFFSET :offset;",
        PARAM(oatpp::UInt32, offset),
        PARAM(oatpp::UInt32, limit))

  QUERY(deleteDeviceById,
        "DELETE FROM t_device WHERE id=:id;",
        PARAM(oatpp::Int32, id))

};

#include OATPP_CODEGEN_END(DbClient) //<- End Codegen

#endif //CRUD_USERDB_HPP
