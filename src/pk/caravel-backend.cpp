
#include "pk-backend.h"
#include "pk-shared.h"
#include "caravel-backend.h"


void pk_backend_initialize(GKeyFile *conf, PkBackend* backend){

    
}

void pk_backend_destroy(PkBackend *backend){

    
}

PkBitfield pk_backend_get_groups ( PkBackend* backend )
{
    return pk_bitfield_from_enums (PK_GROUP_ENUM_ACCESSIBILITY,
                                   PK_GROUP_ENUM_ADMIN_TOOLS,
                                   PK_GROUP_ENUM_INTERNET,
                                   PK_GROUP_ENUM_PROGRAMMING,
                                   PK_GROUP_ENUM_SYSTEM,
                                   PK_GROUP_ENUM_GAMES,
                                   -1
                                  );
}

PkBitfield pk_backend_get_filters ( PkBackend* backend )
{
    return pk_bitfield_from_enums(
        PK_FILTER_ENUM_INSTALLED,
        PK_FILTER_ENUM_NEWEST,
        PK_FILTER_ENUM_SUPPORTED,
         -1
    );
}

gchar** pk_backend_get_mime_types(PkBackend* backend) {
    const gchar *mime_types[] = {
        "application/x-caravel",
        NULL
    } ;
    return g_strdupv((gchar**)mime_types);
}

const gchar * pk_backend_get_description ( PkBackend* backend )
{
    return "Caravel PM";
}

const gchar * pk_backend_get_author (PkBackend *backend)
{
	return "Tridentu Group <tridentuorg@protonmail.com>";
}

PkBitfield pk_backend_get_roles(PkBackend *backend)
{
    PkBitfield roles;
    roles = pk_bitfield_from_enums(
                PK_ROLE_ENUM_CANCEL,
                PK_ROLE_ENUM_DEPENDS_ON,
                PK_ROLE_ENUM_GET_DETAILS,
                PK_ROLE_ENUM_GET_DETAILS_LOCAL,
                PK_ROLE_ENUM_GET_FILES,
                PK_ROLE_ENUM_GET_FILES_LOCAL,
                PK_ROLE_ENUM_REQUIRED_BY,
                PK_ROLE_ENUM_GET_PACKAGES,
                PK_ROLE_ENUM_WHAT_PROVIDES,
                PK_ROLE_ENUM_GET_UPDATES,
                PK_ROLE_ENUM_GET_UPDATE_DETAIL,
                PK_ROLE_ENUM_INSTALL_PACKAGES,
                PK_ROLE_ENUM_INSTALL_SIGNATURE,
                PK_ROLE_ENUM_REFRESH_CACHE,
                PK_ROLE_ENUM_REMOVE_PACKAGES,
                PK_ROLE_ENUM_DOWNLOAD_PACKAGES,
                PK_ROLE_ENUM_RESOLVE,
                PK_ROLE_ENUM_SEARCH_DETAILS,
                PK_ROLE_ENUM_SEARCH_FILE,
                PK_ROLE_ENUM_SEARCH_GROUP,
                PK_ROLE_ENUM_SEARCH_NAME,
                PK_ROLE_ENUM_UPDATE_PACKAGES,
                PK_ROLE_ENUM_GET_REPO_LIST,
                PK_ROLE_ENUM_REPO_ENABLE,
                PK_ROLE_ENUM_REPAIR_SYSTEM,
                PK_ROLE_ENUM_REPO_REMOVE,
                PK_ROLE_ENUM_INSTALL_FILES,
                -1);

    return roles;
}
