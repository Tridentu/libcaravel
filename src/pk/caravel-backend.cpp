#include <glib-2.0/gmodule.h>
#include <glib-2.0/glib.h>
#include <cstring>
#include <cstdlib>
#include <glib-2.0/gio/gio.h>
#include <gio-unix-2.0/gio/gunixsocketaddress.h>
#include "pk-backend.h"
#include "pk-backend-job.h"
#include "pk-shared.h"
#include <vector>
#include <caravel/CaravelDBContext.h>
#include <sqlite3.h>
#include <map>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <glib/gmessages.h>
#include <string>


typedef struct {
    gboolean dots_enabled;
    gboolean firm_enabled;
} PkBackendCaravelPrivate;
typedef struct {
	guint progress_percentage;
	GSocket *socket;
	guint socket_listen_id;
	GCancellable *cancellable;
    gulong signal_timeout;
} PkBackendCaravelJobData;

static std::vector<CaravelPM::CaravelPackageInfo> infos;
static std::vector<CaravelPM::CaravelPackageId> ids;
static PkBackendCaravelPrivate* backendData;

void pk_backend_initialize(GKeyFile *conf, PkBackend* backend){
 backendData = g_new(PkBackendCaravelPrivate, 1);
    backendData->dots_enabled = FALSE;
    backendData->firm_enabled = FALSE;
    CaravelPM::CaravelDBContext::InitDB(std::string("https://tridentu.github.io/acquirium/pman.caraveldb"), true);

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
        PK_FILTER_ENUM_FREE,
        PK_FILTER_ENUM_VISIBLE,
		-1
	);
}

void pk_backend_get_categories(PkBackend* backend, PkBackendJob	*job){
    pk_backend_job_category(job, "accessories", "shells", "Shells", "Used for running commands in a pty.", "terminal");
}

PkBitfield
pk_backend_get_roles (PkBackend *backend)
{
	PkBitfield roles;
	roles = pk_bitfield_from_enums (
		PK_ROLE_ENUM_CANCEL,
		PK_ROLE_ENUM_DEPENDS_ON,
		PK_ROLE_ENUM_GET_DETAILS,
		PK_ROLE_ENUM_GET_FILES,
		PK_ROLE_ENUM_REQUIRED_BY,
		PK_ROLE_ENUM_GET_PACKAGES,
		PK_ROLE_ENUM_WHAT_PROVIDES,
		PK_ROLE_ENUM_GET_UPDATES,
		PK_ROLE_ENUM_GET_UPDATE_DETAIL,
		PK_ROLE_ENUM_INSTALL_PACKAGES,
		PK_ROLE_ENUM_INSTALL_FILES,
		//PK_ROLE_ENUM_INSTALL_SIGNATURE,
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
		//PK_ROLE_ENUM_REPO_SET_DATA,
		PK_ROLE_ENUM_GET_CATEGORIES,
		-1);

	return roles;
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
            pk_backend_job_repo_detail (job, (gchar*)"caravel-cmr",
 				    (gchar*)"Caravel Main Repository", TRUE);
            pk_backend_job_repo_detail (job, (gchar*)"caravel_cmr_dots",
 				    (gchar*)"CMR - Dotfiles", backendData->dots_enabled);
            pk_backend_job_repo_detail (job, (gchar*)"caravel_cfr",
 				    (gchar*)"Caravel Firmware Repository", backendData->firm_enabled);
            pk_backend_job_finished (job);
}

static std::vector<std::vector<std::string>> findPackageIdsFromNameQuery(std::string query){
    std::vector<std::vector<std::string>> packageIds =  CaravelPM::CaravelDBContext::GetDB()->FindPackageIdsFromNameQuery(query);
    return packageIds;
}

static PkGroupEnum pk_backend_caravel_get_group(std::string cat){
  if(cat == "shells"){
    return PK_GROUP_ENUM_SYSTEM;
   } else if(cat == "utilites"){
    return PK_GROUP_ENUM_ACCESSORIES;
  }
   return PK_GROUP_ENUM_UNKNOWN;
}

void pk_backend_get_details (PkBackend *backend, PkBackendJob *job, gchar **package_ids){
    
    guint i;
	guint len;
	const gchar *package_id;

	pk_backend_job_set_status (job, PK_STATUS_ENUM_QUERY);
	pk_backend_job_set_percentage (job, 0);

	/* each one has a different detail for testing */
	len = g_strv_length (package_ids);
    std::vector<CaravelPM::CaravelPackageInfo> pids = CaravelPM::CaravelDBContext::GetDB()->GetAllPackages();
            CaravelPM::CaravelPackageInfo* package = nullptr;
             CaravelPM::CaravelPackageId* packageIDObj  = nullptr;
    for (int i = 0; i < len; i++){
        for (auto& packageID: pids){
          packageIDObj = CaravelPM::CaravelDBContext::GetDB()->FindPackageId(packageID);
            if (packageIDObj) {
                std::string idStr =  packageIDObj->PackageId.substr(0, packageIDObj->PackageId.size() - 1);
                if(packageIDObj->Id == packageID.Id && idStr == std::string(package_ids[i])){
                    package = &packageID;
                    break;
                }
            }
            packageIDObj = nullptr;
        }
        if (package) {
            pk_backend_job_details(job, package_ids[i], package->Description.c_str(), "GPL", pk_backend_caravel_get_group(package->PackageCategory), package->Description.c_str(), "", package->Id * 1024*1024);
        }
        package = nullptr;
        pk_backend_job_set_percentage (job, (i/len) * 100);
    }
    pk_backend_job_set_percentage (job, 100);
	pk_backend_job_finished (job);
}

static void
pk_backend_resolve_thread (PkBackendJob *job, GVariant *params, gpointer user_data)
{
    PkRoleEnum role;
	PkBitfield filters;
    
    g_autofree gchar **search = NULL;

	role = pk_backend_job_get_role (job);
	
		g_variant_get (params, "(t^a&s)",
			       &filters,
			       &search);
    std::string idStr;
    bool isInstalled = false;
	guint len = g_strv_length (search);
    std::vector<CaravelPM::CaravelInstalledPackage> installed = CaravelPM::CaravelDBContext::GetDB()->GetInstalledPackages();
    std::vector<CaravelPM::CaravelPackageInfo> pids = CaravelPM::CaravelDBContext::GetDB()->GetAllPackages();
    for (size_t i2 = 0; i2 < len; i2++){
        for (size_t i3 = 0; i3 < pids.size(); i3++){
        CaravelPM::CaravelPackageId* res = CaravelPM::CaravelDBContext::GetDB()->FindPackageId(pids.at(i3));
        if (res){
          for(auto ip = installed.begin(); ip != installed.end(); ip++){
                int pos = pids.at(i3).PackageName.find(ip->name);
                if(pos != std::string::npos){
                    isInstalled = true;
                    break;
                }
            }
            if (pids.at(i3).PackageName.find(search[i2]) == std::string::npos)
                continue;
            idStr = res->PackageId;
            pk_backend_job_package(job, (isInstalled) ? PK_INFO_ENUM_INSTALLED : PK_INFO_ENUM_AVAILABLE, idStr.substr(0,idStr.size() - 1).c_str(), pids.at(i3).Description.c_str());
            isInstalled = false;
        }
        }
    }
}

void
pk_backend_resolve (PkBackend *backend, PkBackendJob *job, PkBitfield filters, gchar **packages)
{
	pk_backend_job_thread_create (job, pk_backend_resolve_thread, NULL, NULL);
}

static void pk_backend_search_names_thread(PkBackendJob *job, GVariant *params, gpointer user_data){
    PkRoleEnum role;
	PkBitfield filters;
    g_autofree gchar **search = NULL;

	role = pk_backend_job_get_role (job);
	
		g_variant_get (params, "(t^a&s)",
			       &filters,
			       &search);

    if (search){
        std::vector<std::vector<std::string>>  package_ids = findPackageIdsFromNameQuery(std::string(search[0]));

       if(package_ids.size() <= 0){
        pk_backend_job_error_code (job, PK_ERROR_ENUM_PACKAGE_NOT_FOUND, "No packages found (packages were non-existent from query %s).", search[0]);
         pk_backend_job_finished (job);

         return;
       }
       std::ofstream fs1;
       fs1.open("/tmp/caravel-pk.log");
       fs1 << "Starting results..." << std::endl;
       for (int i = 0; i < package_ids.size(); i++){
        
            fs1 << package_ids.at(i).at(0).c_str() << std::endl;
            std::string idStr = package_ids.at(i).at(0);
            fs1 << idStr.substr(0,idStr.size() - 1).c_str() << std::endl;
            pk_backend_job_package(job, PK_INFO_ENUM_AVAILABLE, (const gchar*)idStr.substr(0,idStr.size() - 1).c_str(), (const gchar*)package_ids.at(i).at(1).c_str());
       }
       fs1.close();
    } else {
         pk_backend_job_error_code (job, PK_ERROR_ENUM_PACKAGE_NOT_FOUND, "No packages found (search was empty)."); 

    }


}


void pk_backend_search_names(PkBackend *backend, PkBackendJob *job, PkBitfield filters, gchar **values){
    pk_backend_job_set_allow_cancel(job, TRUE);
    pk_backend_job_set_status(job, PK_STATUS_ENUM_QUERY);
    pk_backend_job_thread_create(job, pk_backend_search_names_thread, NULL, NULL);
    
}



static gboolean
pk_backend_get_updates_timeout (gpointer data)
{
	PkBackendJob *job = (PkBackendJob *) data;
    pk_backend_job_finished (job);
	return FALSE;
}

void
pk_backend_get_updates (PkBackend *backend, PkBackendJob *job, PkBitfield filters)
{
	pk_backend_job_set_status (job, PK_STATUS_ENUM_QUERY);
    pk_backend_job_set_percentage (job, PK_BACKEND_PERCENTAGE_INVALID);
    if (!pk_backend_is_online (backend)) {
		pk_backend_job_error_code (job, PK_ERROR_ENUM_NO_NETWORK, "Cannot check when offline");
		pk_backend_job_finished (job);
		return;
	}
    g_timeout_add (1000, pk_backend_get_updates_timeout, job);
}

gboolean pk_backend_supports_parallelization (PkBackend *backend)
{
	return TRUE;
}

static void
pk_backend_refresh_cache_thread (PkBackendJob *job, GVariant *params, gpointer user_data)
{
	gboolean force;
	g_variant_get (params, "(b)",
		       &force);

    if(CaravelPM::CaravelDBContext::GetDB())
        CaravelPM::CaravelDBContext::Cleanup();
    pk_backend_job_set_percentage (job, 50);
    CaravelPM::CaravelDBContext::InitDB(std::string("https://tridentu.github.io/acquirium/pman.caraveldb"), true);
    g_usleep(10 * 1000);
    pk_backend_job_set_percentage (job, 100);

	pk_backend_job_set_locked (job, FALSE);
}

void pk_backend_get_packages(PkBackend *backend, PkBackendJob *job, PkBitfield filters){
    pk_backend_job_set_status(job, PK_STATUS_ENUM_REQUEST);
    std::vector<CaravelPM::CaravelPackageInfo> pids = CaravelPM::CaravelDBContext::GetDB()->GetAllPackages();
    std::vector<CaravelPM::CaravelInstalledPackage> installed = CaravelPM::CaravelDBContext::GetDB()->GetInstalledPackages();
   
    std::ofstream fs1;
    fs1.open("/tmp/caravel-pk.log");
    fs1 << "Starting results..." << std::endl;
    std::string idStr;
    bool isInstalled = false;

    for (size_t i2 = 0; i2 < pids.size(); i2++){
        fs1 << pids.at(i2).PackageName << " " << i2 << std::endl;
        CaravelPM::CaravelPackageId* res = CaravelPM::CaravelDBContext::GetDB()->FindPackageId(pids.at(i2));
        
        if (res){
            idStr = res->PackageId;
            fs1 << res->PackageId << std::endl;
            for(auto ip = installed.begin(); ip != installed.end(); ip++){
                int pos = pids.at(i2).PackageName.find(ip->name);
                if(pos != std::string::npos){
                    isInstalled = true;
                    break;
                }
            }
            fs1 << isInstalled << std::endl;
            pk_backend_job_package(job, (isInstalled) ? PK_INFO_ENUM_INSTALLED : PK_INFO_ENUM_AVAILABLE, idStr.substr(0,idStr.size() - 1).c_str(), pids.at(i2).Description.c_str());
            isInstalled = false;
        }
    }
    fs1.close();
    pk_backend_job_finished(job);
}

void
pk_backend_refresh_cache (PkBackend *backend, PkBackendJob *job, gboolean force)
{

	
	pk_backend_job_set_allow_cancel (job, TRUE);
	pk_backend_job_set_status (job, PK_STATUS_ENUM_REFRESH_CACHE);

	
	pk_backend_job_set_locked (job, TRUE);

	pk_backend_job_thread_create (job, pk_backend_refresh_cache_thread, NULL, NULL);
}

