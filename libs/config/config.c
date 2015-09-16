/*
 * This file is part of dmrshark.
 *
 * dmrshark is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * dmrshark is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dmrshark.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <config/defaults.h>

#include "config.h"
#include "config-voicestreams.h"

#include <libs/base/types.h>
#include <libs/daemon/daemon.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static pthread_mutex_t config_mutex = PTHREAD_MUTEX_INITIALIZER;
static GKeyFile *keyfile = NULL;
static GKeyFileFlags flags;
static char *config_configfilename = NULL;

GKeyFile *config_get_keyfile(void) {
	return keyfile;
}

pthread_mutex_t *config_get_mutex(void) {
	return &config_mutex;
}

char **config_get_groups(int *length) {
	gchar **result;
	gsize length_s;

	*length = 0;
	result = g_key_file_get_groups(keyfile, &length_s);
	if (!length_s || !result)
	    return NULL;
	*length = length_s;

	return result;
}

void config_free_groups(char **config_groups) {
    g_strfreev(config_groups);
}

void config_writeconfigfile(void) {
	GError *error = NULL;
	FILE *f;
	gsize size;
	char *data = NULL;

	if (config_configfilename == NULL) {
		console_log("config error: no config file name given\n");
		return;
	}

	pthread_mutex_lock(&config_mutex);

	data = g_key_file_to_data(keyfile, &size, &error);
	if (!error && data != NULL) {
		f = fopen(config_configfilename, "w");
		if (f) {
			fwrite(data, 1, size, f);
			fclose(f);
		} else
			console_log("config error: can't save, file %s is not writable\n", config_configfilename);
	} else
		console_log("config error: can't save\n");

	if (data)
		free(data);

	pthread_mutex_unlock(&config_mutex);
}

void config_set_loglevel(loglevel_t *loglevel) {
	pthread_mutex_lock(&config_mutex);
	g_key_file_set_integer(keyfile, "main", "loglevel", loglevel->raw);
	pthread_mutex_unlock(&config_mutex);
	config_writeconfigfile();
}

int config_get_loglevel(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "loglevel";
	int defaultvalue = 0;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 0;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

char *config_get_logfilename(void) {
	GError *error = NULL;
	char *value = NULL;
	char *key = "logfile";
	char *defaultvalue = NULL;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = APPNAME ".log";
	value = g_key_file_get_string(keyfile, "main", key, &error);
	if (error || value == NULL) {
		value = strdup(defaultvalue);
		if (value)
			g_key_file_set_string(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

char *config_get_pidfilename(void) {
	GError *error = NULL;
	char *value = NULL;
	char *key = "pidfile";
	char *defaultvalue = NULL;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = APPNAME ".pid";
	value = g_key_file_get_string(keyfile, "main", key, &error);
	if (error || value == NULL) {
		value = strdup(defaultvalue);
		if (value)
			g_key_file_set_string(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

char *config_get_daemonctlfile(void) {
	GError *error = NULL;
	char *value = NULL;
	char *key = "daemonctlfile";
	char *defaultvalue = NULL;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = "/tmp/" APPNAME ".ctl";
	value = g_key_file_get_string(keyfile, "main", key, &error);
	if (error || value == NULL) {
		value = strdup(defaultvalue);
		if (value)
			g_key_file_set_string(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

char *config_get_ttyconsoledev(void) {
	GError *error = NULL;
	char *value = NULL;
	char *key = "ttyconsoledev";
	char *defaultvalue = NULL;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = "/dev/ttyUSB0";
	value = g_key_file_get_string(keyfile, "main", key, &error);
	if (error || value == NULL) {
		value = strdup(defaultvalue);
		if (value)
			g_key_file_set_string(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

flag_t config_get_ttyconsoleenabled(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "ttyconsoleenabled";
	int defaultvalue = 0;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 0;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return (value != 0 ? 1 : 0);
}

int config_get_ttyconsolebaudrate(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "ttyconsolebaudrate";
	int defaultvalue = 0;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 115200;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

char *config_get_netdevicename(void) {
	GError *error = NULL;
	char *value = NULL;
	char *key = "netdevicename";
	char *defaultvalue = NULL;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = "any";
	value = g_key_file_get_string(keyfile, "main", key, &error);
	if (error || value == NULL) {
		value = strdup(defaultvalue);
		if (value)
			g_key_file_set_string(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

int config_get_repeaterinfoupdateinsec(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "repeaterinfoupdateinsec";
	int defaultvalue = 0;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 300;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

int config_get_repeaterinactivetimeoutinsec(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "repeaterinactivetimeoutinsec";
	int defaultvalue = 0;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 30;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

int config_get_rssiupdateduringcallinmsec(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "rssiupdateduringcallinmsec";
	int defaultvalue = 0;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 500;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

int config_get_calltimeoutinsec(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "calltimeoutinsec";
	int defaultvalue = 0;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 1;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

int config_get_datatimeoutinsec(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "datatimeoutinsec";
	int defaultvalue = 0;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 3;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

char *config_get_ignoredsnmprepeaterhosts(void) {
	GError *error = NULL;
	char *value = NULL;
	char *key = "ignoredsnmprepeaterhosts";
	char *defaultvalue = NULL;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = "";
	value = g_key_file_get_string(keyfile, "main", key, &error);
	if (error || value == NULL) {
		value = strdup(defaultvalue);
		if (value)
			g_key_file_set_string(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

char *config_get_ignoredhosts(void) {
	GError *error = NULL;
	char *value = NULL;
	char *key = "ignoredhosts";
	char *defaultvalue = NULL;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = "";
	value = g_key_file_get_string(keyfile, "main", key, &error);
	if (error || value == NULL) {
		value = strdup(defaultvalue);
		if (value)
			g_key_file_set_string(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

char *config_get_remotedbhost(void) {
	GError *error = NULL;
	char *defaultvalue = NULL;
	char *key = "remotedbhost";
	char *value = NULL;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = "";
	value = g_key_file_get_string(keyfile, "main", key, &error);
	if (error || value == NULL) {
		value = strdup(defaultvalue);
		if (value)
			g_key_file_set_string(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

char *config_get_remotedbuser(void) {
	GError *error = NULL;
	char *value = NULL;
	char *key = "remotedbuser";
	char *defaultvalue = NULL;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = APPNAME;
	value = g_key_file_get_string(keyfile, "main", key, &error);
	if (error || value == NULL) {
		value = strdup(defaultvalue);
		if (value)
			g_key_file_set_string(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

char *config_get_remotedbpass(void) {
	GError *error = NULL;
	char *value = NULL;
	char *key = "remotedbpass";
	char *defaultvalue = NULL;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = "";
	value = g_key_file_get_string(keyfile, "main", key, &error);
	if (error || value == NULL) {
		value = strdup(defaultvalue);
		if (value)
			g_key_file_set_string(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

char *config_get_remotedbname(void) {
	GError *error = NULL;
	char *value = NULL;
	char *key = "remotedbname";
	char *defaultvalue = NULL;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = APPNAME;
	value = g_key_file_get_string(keyfile, "main", key, &error);
	if (error || value == NULL) {
		value = strdup(defaultvalue);
		if (value)
			g_key_file_set_string(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

char *config_get_remotedbtableprefix(void) {
	GError *error = NULL;
	char *value = NULL;
	char *key = "remotedbtableprefix";
	char *defaultvalue = NULL;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = APPNAME "-";
	value = g_key_file_get_string(keyfile, "main", key, &error);
	if (error || value == NULL) {
		value = strdup(defaultvalue);
		if (value)
			g_key_file_set_string(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

int config_get_remotedbreconnecttrytimeoutinsec(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "remotedbreconnecttrytimeoutinsec";
	int defaultvalue = 0;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 5;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

int config_get_remotedbmaintenanceperiodinsec(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "remotedbmaintenanceperiodinsec";
	int defaultvalue = 0;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 60;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}


int config_get_remotedbdeleteolderthansec(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "remotedbdeleteolderthansec";
	int defaultvalue = 0;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 86400;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

int config_get_updatestatstableenabled(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "updatestatstableenabled";
	int defaultvalue = 0;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 1;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

int config_get_httpserverenabled(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "httpserverenabled";
	int defaultvalue = 0;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 1;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

int config_get_httpserverport(void) {
	GError *error = NULL;
	int value = 0;
	char *key = "httpserverenabled";
	int defaultvalue = 8080;

	pthread_mutex_lock(&config_mutex);
	defaultvalue = 1;
	value = g_key_file_get_integer(keyfile, "main", key, &error);
	if (error) {
		value = defaultvalue;
		g_key_file_set_integer(keyfile, "main", key, value);
	}
	pthread_mutex_unlock(&config_mutex);
	return value;
}

void config_init(char *configfilename) {
	GError *error = NULL;

	console_log("config: init\n");

	if (configfilename != NULL)
		config_configfilename = configfilename;

	FILE *f = fopen(config_configfilename, "r");
	if (f == NULL) {
		console_log("config: config file %s doesn't exist, creating\n", config_configfilename);
		f = fopen(config_configfilename, "w");
		if (!f) {
			console_log("config error: can't save, file is not writable\n");
			return;
		}
		fputs("[main]\n", f);
	}
	fclose(f);

	if (keyfile != NULL) {
		g_key_file_free(keyfile);
		keyfile = NULL;
	}

	keyfile = g_key_file_new();

	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
	if (!g_key_file_load_from_file(keyfile, config_configfilename, flags, &error)) {
		console_log("config: error loading file\n");
		g_key_file_free(keyfile);
		keyfile = NULL;
	}

	// We read everything, a default value will be set for non-existent keys in the config file.
	config_get_loglevel();
	char *temp = config_get_logfilename();
	free(temp);
	temp = config_get_pidfilename();
	free(temp);
	temp = config_get_daemonctlfile();
	free(temp);
	temp = config_get_ttyconsoledev();
	free(temp);
	config_get_ttyconsoleenabled();
	config_get_ttyconsolebaudrate();
	temp = config_get_netdevicename();
	free(temp);
	config_get_repeaterinfoupdateinsec();
	config_get_repeaterinactivetimeoutinsec();
	config_get_rssiupdateduringcallinmsec();
	config_get_calltimeoutinsec();
	config_get_datatimeoutinsec();
	temp = config_get_ignoredsnmprepeaterhosts();
	free(temp);
	temp = config_get_ignoredhosts();
	free(temp);
	temp = config_get_remotedbhost();
	free(temp);
	temp = config_get_remotedbuser();
	free(temp);
	temp = config_get_remotedbpass();
	free(temp);
	temp = config_get_remotedbname();
	free(temp);
	temp = config_get_remotedbtableprefix();
	free(temp);
	config_get_remotedbreconnecttrytimeoutinsec();
	config_get_remotedbdeleteolderthansec();
	config_get_updatestatstableenabled();
	config_get_httpserverenabled();
	config_get_httpserverport();

	config_writeconfigfile();
}

void config_deinit(void) {
	console_log("config: deinit\n");

	if (keyfile != NULL) {
		g_key_file_free(keyfile);
		keyfile = NULL;
	}

	pthread_mutex_destroy(&config_mutex);
}
