#include <boost/uuid/random_generator.hpp>
#include <string.h>
#include "rpcdce.h"

// https://docs.microsoft.com/en-us/windows/win32/api/rpcdce/nf-rpcdce-uuidcreate
RPC_STATUS UuidCreate(UUID* Uuid) {
    if (Uuid == nullptr) {
        return -1;
    }
    auto uuid = boost::uuids::random_generator()();
    memcpy(&Uuid->Data1, &uuid.data[0], sizeof(Uuid->Data1));
    memcpy(
        &Uuid->Data2, &uuid.data[0] + sizeof(Uuid->Data1), sizeof(Uuid->Data2));
    memcpy(&Uuid->Data3,
        &uuid.data[0] + sizeof(Uuid->Data1) + sizeof(Uuid->Data2),
        sizeof(Uuid->Data3));
    memcpy(&Uuid->Data4,
        &uuid.data[0] + sizeof(Uuid->Data1) + sizeof(Uuid->Data2) + sizeof(Uuid->Data2),
        sizeof(Uuid->Data4));
    // return value not used
    return 0;
}
