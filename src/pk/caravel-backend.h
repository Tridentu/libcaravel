

#pragma once

#include <glib-2.0/gmodule.h>
#include <glib-2.0/glib.h>
#include <cstring>
#include <cstdlib>
#include <glib-2.0/gio/gio.h>
#include <gio-unix-2.0/gio/gunixsocketaddress.h>

#include "pk-backend.h"
#include "pk-shared.h"
#include "pk-backend-job.h"




    void pk_backend_initialize(GKeyFile *conf, PkBackend* backend);
    void pk_backend_destroy(PkBackend* backend);
    PkBitfield pk_backend_get_groups(PkBackend* backend);
    PkBitfield pk_backend_get_filters(PkBackend* backend);
    gchar** pk_backend_get_mime_types(PkBackend* backend);
    const gchar* pk_backend_get_description(PkBackend* backend);
    PkBitfield pk_backend_get_roles(PkBackend* backend);


