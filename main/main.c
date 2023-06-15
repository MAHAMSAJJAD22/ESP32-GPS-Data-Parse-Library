#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    char time[10];
    double latitude;
    double longitude;
    int fixQuality;
    int numSatellites;
    double hdop;
    double altitude;
    char altitudeUnit;
} GPSData;

bool validate_checksum(const char* packet) {
    int len = strlen(packet);
    int checksum = 0;
    int i = 1;

    // Calculate the checksum excluding the leading '$'
    while (packet[i] != '*' && i < len) {
        checksum ^= packet[i];
        i++;
    }

    // Convert the checksum from hexadecimal to decimal
    char checksumStr[3];
    strncpy(checksumStr, &packet[i + 1], 2);
    checksumStr[2] = '\0';
    int packetChecksum = (int)strtol(checksumStr, NULL, 16);

    return (checksum == packetChecksum);
}

void parse_gps_data(const char* packet, GPSData* data) {
    if (strlen(packet) < 10 || packet[0] != '$') {
        // Invalid packet or empty string
        return;
    }

    if (!validate_checksum(packet)) {
        // Invalid checksum
        return;
    }

    char* packetCopy = strdup(packet);  // Create a mutable copy of the packet string

    char* token = strtok(packetCopy, ",");
    int i = 0;

    while (token != NULL) {
        switch (i) {
            case 1: // Time
                strncpy(data->time, token, sizeof(data->time) - 1);
                data->time[sizeof(data->time) - 1] = '\0';
                break;
            case 2: // Latitude
                data->latitude = atof(token);
                break;
            case 3: // Latitude direction (N/S)
                if (strcmp(token, "S") == 0) {
                    data->latitude = -data->latitude;
                }
                break;
            case 4: // Longitude
                data->longitude = atof(token);
                break;
            case 5: // Longitude direction (E/W)
                if (strcmp(token, "W") == 0) {
                    data->longitude = -data->longitude;
                }
                break;
            case 6: // Fix quality
                data->fixQuality = atoi(token);
                break;
            case 7: // Number of satellites
                data->numSatellites = atoi(token);
                break;
            case 8: // HDOP
                data->hdop = atof(token);
                break;
            case 9: // Altitude
                data->altitude = atof(token);
                break;
            case 10: // Altitude unit
                data->altitudeUnit = token[0];
                break;
        }

        token = strtok(NULL, ",");
        i++;
    }

    free(packetCopy);  // Free the memory allocated for the packet copy
}



int main() 
{
    const char* packet = "$GPGGA,002153.000,3342.6618,N,11751.3858,W,1,10,1.2,27.0,M,-34.2,M,,0000*5E";
    GPSData gpsData;
    parse_gps_data(packet, &gpsData);

    printf("Time: %s\n", gpsData.time);
    printf("Latitude: %.4lf\n", gpsData.latitude);
    printf("Longitude: %.4lf\n", gpsData.longitude);
    printf("Fix Quality: %d\n", gpsData.fixQuality);
    printf("Number of Satellites: %d\n", gpsData.numSatellites);
    printf("HDOP: %.1lf\n", gpsData.hdop);
    printf("Altitude: %.1lf %c\n", gpsData.altitude, gpsData.altitudeUnit);

    return 0;
}

