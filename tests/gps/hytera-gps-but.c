#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

typedef uint8_t flag_t;

typedef struct  {
	double latitude;
	char latitude_ch;
	double longitude;
	char longitude_ch;
	uint16_t speed;
	uint16_t heading;
	flag_t speed_valid		: 1;
	flag_t heading_valid	: 1;
} dmr_data_gpspos_t;

dmr_data_gpspos_t *dmr_data_decode_hytera_gps_button(uint8_t *message_data, uint16_t message_data_length) {
	static dmr_data_gpspos_t result;
	char tmp[10];
	char *endptr;

	if (message_data_length < 60 || message_data == NULL)
		return NULL;

	// Latitude
	result.latitude_ch = message_data[30];
	if (result.latitude_ch != 'S' && result.latitude_ch != 'N')
		return NULL;

	snprintf(tmp, sizeof(tmp), "%c%c%c%c.%c%c%c", message_data[31], message_data[32], message_data[33],
		message_data[34], message_data[36], message_data[37], message_data[38]);
	errno = 0;
	result.latitude = strtod(tmp, &endptr);
	if (errno != 0 || *endptr != 0)
		return NULL;

	// Longitude
	result.longitude_ch = message_data[40];
	if (result.longitude_ch != 'E' && result.longitude_ch != 'W')
		return NULL;

	snprintf(tmp, sizeof(tmp), "%c%c%c%c.%c%c%c%c", message_data[42], message_data[43], message_data[44],
		message_data[45], message_data[47], message_data[48], message_data[49], message_data[50]);
	errno = 0;
	result.longitude = strtod(tmp, &endptr);
	if (errno != 0 || *endptr != 0)
		return NULL;

	// Speed
	snprintf(tmp, sizeof(tmp), "%c%c%c", message_data[51] == '.' ? ' ' : message_data[51], message_data[52] == '.' ? ' ' : message_data[52],
		message_data[53] == '.' ? ' ' : message_data[53]);
	errno = 0;
	result.speed = strtol(tmp, &endptr, 10);
	result.speed_valid = (errno == 0); // We don't care about *endptr being 0 as the string can contain dot chars at the end.

	// Heading
	snprintf(tmp, sizeof(tmp), "%c%c%c", message_data[54], message_data[55], message_data[56]);
	errno = 0;
	result.heading = strtol(tmp, &endptr, 10);
	result.heading_valid = (errno == 0 && *endptr == 0);

	return &result;
}

char *dmr_data_get_gps_string(dmr_data_gpspos_t *gpspos) {
	static char result[100];
	char latitude[9];
	char longitude[10];
	char speed[4];
	char heading[4];

	if (gpspos == NULL)
		return NULL;

	snprintf(latitude, sizeof(latitude), "%04.0f.%03.0f", floor(gpspos->latitude), (gpspos->latitude-floor(gpspos->latitude))*1000);
	snprintf(longitude, sizeof(longitude), "%05.0f.%03.0f", floor(gpspos->longitude), (gpspos->longitude-floor(gpspos->longitude))*1000);
	if (gpspos->speed_valid)
		snprintf(speed, sizeof(speed), "%3u", gpspos->speed);
	else
		snprintf(speed, sizeof(speed), "???");
	if (gpspos->heading_valid)
		snprintf(heading, sizeof(heading), "%3u", gpspos->heading);
	else
		snprintf(heading, sizeof(heading), "???");

	snprintf(result, sizeof(result), "%c%c°%c%c.%c%c%c' %c %c%c%c°%c%c.%c%c%c' %c speed: %skm/h heading: %s",
		latitude[0], latitude[1], latitude[2], latitude[3], latitude[5], latitude[6], latitude[7], gpspos->latitude_ch,
		longitude[0], longitude[1], longitude[2], longitude[3], longitude[4], longitude[6], longitude[7], longitude[8], gpspos->longitude_ch,
		speed, heading);

	return result;
}

int main(void) {
	// 47 18 811, 18 54 4350, 1, 202
	uint8_t d[] = { 01, 0x01, 0x08, 0xa0, 0x02, 0x00, 0x32, 0x00, 0x00, 0x00, 0x01, 0x0a, 0x21, 0x10, 0xdd, 0x00, 0x00, 0x41, 0x31, 0x38, 0x33, 0x36, 0x34, 0x38, 0x32, 0x36, 0x31, 0x30, 0x31, 0x35, 0x4e, 0x34, 0x37, 0x31, 0x38, 0x2e, 0x38, 0x30, 0x35, 0x31, 0x45, 0x30, 0x31, 0x38, 0x35, 0x34, 0x2e, 0x34, 0x33, 0x38, 0x37, 0x30, 0x2e, 0x31, 0x31, 0x32, 0x31, 0x0b, 0x03, 0x00 };

	dmr_data_gpspos_t *gpspos = dmr_data_decode_hytera_gps_button(d, sizeof(d));
	char *a = dmr_data_get_gps_string(gpspos);
	if (a != NULL)
		printf("%s\n", a);
	return 0;
}
