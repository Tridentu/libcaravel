
#include "pk-backend.h"
#include "pk-shared.h"
#include <vector>
#include <caravel/CaravelDBContext.h>
#include <sqlite3.h>
#include <map>
#include <iostream>
#include <sstream>
#include "caravel-backend.h"
#include <glib/gmessages.h>




static bool done = false;
static std::vector<CaravelPM::CaravelPackageInfo> infos;
static std::vector<CaravelPM::CaravelPackageId> ids;
static sqlite3* m_DB;
static PkBackendCaravelPrivate* backendData;

void pk_backend_initialize(GKeyFile *conf, PkBackend* backend){
 backendData = g_new(PkBackendCaravelPrivate, 1);
    backendData->dots_enabled = FALSE;
    backendData->firm_enabled = FALSE;
    
}

static int dbCallback2(void* data, int argc, char **argv, char **colName){
  std::map<std::string, std::string> propMap;
  int packageRecIndex = 0;
  done = false;
  for(int i = 0; i < argc; i++){
    std::string column(colName[i]);
    if(column == "name" || column == "description" || column == "pkgType" || column == "category" || column ==  "architecture" || column == "pkgId"){
      propMap[column] = std::string(argv[i] ? argv[i] : "NULL");
      packageRecIndex++;
    }
    if(packageRecIndex % 6 == 0 && packageRecIndex > 0){
      CaravelPM::CaravelPackageInfo info;
      info.Namespace = propMap["architecture"];
      info.PackageType = propMap["pkgType"];
      info.Description = propMap["description"];
      info.PackageCategory = propMap["category"];
      info.PackageName = propMap["name"];
      info.Id = std::stoi(propMap["pkgId"]);
      infos.push_back(info);
      propMap.clear();
    }
    
  }
  done = true;
  return 0;
}


static int dbCallbackPkgId2(void* data, int argc, char **argv, char **colName){
  std::map<std::string, std::string> propMap;
  int packageRecIndex = 0;
  done = false;
  for(int i = 0; i < argc; i++){
    std::string column(colName[i]);
    if(column == "package_table_id" || column == "package_id"){
      propMap[column] = std::string(argv[i] ? argv[i] : "NULL");
      packageRecIndex++;
    }
    if(packageRecIndex % 2 == 0 && packageRecIndex > 0){
      CaravelPM::CaravelPackageId info;
      info.PackageId = propMap["package_id"];
      info.Id = std::stoi(propMap["package_table_id"]);
      ids.push_back(info);
      propMap.clear();
    }
    
  }
  done = true;
  return 0;
}


void pk_backend_destroy(PkBackend *backend){
    g_free(backendData);
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
    return pk_bitfield_from_enums (
		PK_FILTER_ENUM_SUPPORTED,
		PK_FILTER_ENUM_NOT_SUPPORTED,
		PK_FILTER_ENUM_INSTALLED,
		PK_FILTER_ENUM_NOT_INSTALLED,
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

void pk_backend_get_repo_list (PkBackend *backend, PkBackendJob *job, PkBitfield filters) 
{
        	pk_backend_job_set_status (job, PK_STATUS_ENUM_QUERY);
            g_debug("Adding the CMR...");
            pk_backend_job_repo_detail (job, (gchar*)"caravel_cmr",
 				    (gchar*)"Caravel Main Repository", TRUE);
            pk_backend_job_repo_detail (job, (gchar*)"caravel_dots",
 				    (gchar*)"Caravel Dotfiles Repository", backendData->dots_enabled);
            pk_backend_job_repo_detail (job, (gchar*)"caravel_cfr",
 				    (gchar*)"Caravel Firmware Repository", backendData->firm_enabled);
            pk_backend_job_finished (job);
}

static std::vector<std::vector<std::string>> findPackageIdsFromNameQuery(std::string query){
    int rc = sqlite3_open((std::string(getenv("HOME")) + std::string("/pman.caraveldb")).c_str(), &m_DB);
    std::vector<std::vector<std::string>> packageIds;
    std::vector<CaravelPM::CaravelPackageInfo> pids;
    {
        infos.clear();
        std::string data = std::string("CALLBACK FUNCTION").c_str();
        char* errMsg = 0;
        std::string result;
        std::string sql = "SELECT * FROM packageinfo WHERE name LIKE '%" + query + "%';";
        int rc = sqlite3_exec(m_DB, sql.c_str(), dbCallback2, (void*)data.c_str(), &errMsg);
        if(rc != SQLITE_OK ){
            std::cerr << "SQL error detected: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return {};
        } else {
            int i = 0;
            while(!done)
                i++;
        }
        pids = infos;
    }
    
    for (int i = 0; i < pids.size(); i++){
        CaravelPM::CaravelPackageId* res;
        {
            ids.clear();
            std::string data = std::string("CALLBACK FUNCTION").c_str();
            char* errMsg = 0;
            std::stringstream ss;
            ss << "SELECT * FROM packagekit_ids WHERE package_table_id=" << pids.at(i).Id << ";";
            std::string sql = ss.str();
            int rc = sqlite3_exec(m_DB, sql.c_str(), dbCallbackPkgId2, (void*)data.c_str(), &errMsg);
            if(rc != SQLITE_OK ){
                std::cerr << "SQL error detected: " << errMsg << std::endl;
                sqlite3_free(errMsg);
                return {};
            } else {
                int i = 0;
                while(!done)
                    i++;
            }
            res = &ids.at(0);
        }
        if (res){
            std::vector<std::string> data = {res->PackageId, pids.at(i).Description};
            packageIds.push_back(data);
        }
    }
    sqlite3_close(m_DB);
    return packageIds;
}

static void pk_backend_search_names_thread(PkBackendJob *job, GVariant *params, gpointer user_data){
    PkRoleEnum role;
	PkBitfield filters;	
    g_autofree gchar **search = NULL;
    role = pk_backend_job_get_role (job);
	if (role == PK_ROLE_ENUM_GET_PACKAGES) {
		g_variant_get (params, "(t)",
			       &filters);
	} else {
		g_variant_get (params, "(t^a&s)",
			       &filters,
			       &search);
	}
	if (search[0]){
       std::vector<std::vector<std::string>>  package_ids = findPackageIdsFromNameQuery(std::string(search[0]));
       for (int i = 0; i < package_ids.size(); i++){
            g_log(G_LOG_DOMAIN, G_LOG_LEVEL_INFO, "%s", package_ids.at(i).at(0).c_str());
            pk_backend_job_package(job, PK_INFO_ENUM_AVAILABLE, (const gchar*) package_ids.at(i).at(0).c_str(), (const gchar*) package_ids.at(i).at(1).c_str());
       }
    }

}


void pk_backend_search_names(PkBackend *backend, PkBackendJob *job, PkBitfield filters, gchar **values){
    pk_backend_job_set_percentage(job, PK_BACKEND_PERCENTAGE_INVALID);
    pk_backend_job_set_allow_cancel(job, TRUE);
    pk_backend_job_set_status(job, PK_STATUS_ENUM_QUERY);
    pk_backend_job_thread_create(job, pk_backend_search_names_thread, NULL, NULL);
    
}



gboolean pk_backend_supports_parallelization (PkBackend *backend)
{
	return TRUE;
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
                PK_ROLE_ENUM_REPAIR_SYSTEM,
                PK_ROLE_ENUM_INSTALL_FILES,
                PK_ROLE_ENUM_GET_REPO_LIST,
                -1);

    return roles;
}
