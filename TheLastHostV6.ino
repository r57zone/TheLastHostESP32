#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <DNSServer.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <vector>

// HTTPS Server libraries / библиотеки
#include <HTTPSServer.hpp>
#include <HTTPServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>

// Upload
#include <HTTPURLEncodedBodyParser.hpp>
#include <HTTPBodyParser.hpp>
#include <HTTPMultipartBodyParser.hpp>

using namespace httpsserver;

// https://github.com/fhessel/esp32_https_server_compat/blob/master/examples/HelloServerSecure/cert.h
unsigned char crt_DER[] = {
  0x30, 0x82, 0x02, 0x18, 0x30, 0x82, 0x01, 0x81, 0x02, 0x01, 0x02, 0x30,
  0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b,
  0x05, 0x00, 0x30, 0x54, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04,
  0x06, 0x13, 0x02, 0x44, 0x45, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55,
  0x04, 0x08, 0x0c, 0x02, 0x42, 0x45, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03,
  0x55, 0x04, 0x07, 0x0c, 0x06, 0x42, 0x65, 0x72, 0x6c, 0x69, 0x6e, 0x31,
  0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x09, 0x4d, 0x79,
  0x43, 0x6f, 0x6d, 0x70, 0x61, 0x6e, 0x79, 0x31, 0x13, 0x30, 0x11, 0x06,
  0x03, 0x55, 0x04, 0x03, 0x0c, 0x0a, 0x6d, 0x79, 0x63, 0x61, 0x2e, 0x6c,
  0x6f, 0x63, 0x61, 0x6c, 0x30, 0x1e, 0x17, 0x0d, 0x32, 0x30, 0x30, 0x36,
  0x30, 0x36, 0x32, 0x31, 0x34, 0x33, 0x35, 0x33, 0x5a, 0x17, 0x0d, 0x33,
  0x30, 0x30, 0x36, 0x30, 0x34, 0x32, 0x31, 0x34, 0x33, 0x35, 0x33, 0x5a,
  0x30, 0x55, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
  0x02, 0x44, 0x45, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08,
  0x0c, 0x02, 0x42, 0x45, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04,
  0x07, 0x0c, 0x06, 0x42, 0x65, 0x72, 0x6c, 0x69, 0x6e, 0x31, 0x12, 0x30,
  0x10, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x09, 0x4d, 0x79, 0x43, 0x6f,
  0x6d, 0x70, 0x61, 0x6e, 0x79, 0x31, 0x14, 0x30, 0x12, 0x06, 0x03, 0x55,
  0x04, 0x03, 0x0c, 0x0b, 0x65, 0x73, 0x70, 0x33, 0x32, 0x2e, 0x6c, 0x6f,
  0x63, 0x61, 0x6c, 0x30, 0x81, 0x9f, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86,
  0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81, 0x8d,
  0x00, 0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xce, 0xe6, 0x25, 0x6f,
  0x3c, 0xfb, 0xda, 0x38, 0x2b, 0xd7, 0x62, 0xcc, 0x04, 0x9c, 0x58, 0xc8,
  0xc8, 0x91, 0x14, 0x93, 0xb4, 0x0e, 0xa4, 0x09, 0x78, 0xd8, 0xcb, 0xfe,
  0x13, 0x30, 0x6d, 0xd8, 0x22, 0x21, 0xf6, 0xdf, 0x7e, 0xc3, 0xc6, 0x92,
  0x2f, 0xc6, 0x84, 0x43, 0xff, 0xb4, 0xa1, 0x6b, 0x13, 0x5f, 0x36, 0xa7,
  0xc3, 0x99, 0x32, 0x03, 0x89, 0x50, 0x30, 0x1e, 0xad, 0xc6, 0x36, 0xe7,
  0x73, 0x48, 0x09, 0x28, 0x4f, 0xb9, 0x46, 0xbf, 0xdd, 0x76, 0x10, 0xc3,
  0x07, 0x33, 0x9b, 0xd0, 0x8d, 0xb2, 0x24, 0xd4, 0xcb, 0x2e, 0x90, 0x06,
  0xbf, 0xf1, 0xfa, 0xae, 0x06, 0x5c, 0xec, 0x5d, 0xe8, 0x61, 0x06, 0x4a,
  0x3a, 0x2e, 0x2b, 0x1e, 0x60, 0xf2, 0xc4, 0x09, 0xca, 0xe6, 0x27, 0x64,
  0x31, 0x9c, 0xbd, 0x2d, 0x3a, 0x56, 0x27, 0x6d, 0x23, 0x67, 0x21, 0x11,
  0x6f, 0x50, 0xca, 0x11, 0x02, 0x03, 0x01, 0x00, 0x01, 0x30, 0x0d, 0x06,
  0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00,
  0x03, 0x81, 0x81, 0x00, 0x82, 0x74, 0x62, 0xfb, 0xce, 0xca, 0xee, 0xfb,
  0x01, 0x16, 0x91, 0xf8, 0x5d, 0x56, 0xb5, 0x70, 0x6e, 0xc9, 0x12, 0x78,
  0x5f, 0x9f, 0xbd, 0x16, 0x13, 0xe3, 0x23, 0xa0, 0x2d, 0x52, 0x02, 0xad,
  0x08, 0xd5, 0x7b, 0xcb, 0xc3, 0x5a, 0x13, 0x4f, 0x3d, 0x1c, 0x93, 0x95,
  0x2b, 0x61, 0xf9, 0xe6, 0xa2, 0x62, 0xb7, 0x54, 0x1f, 0x06, 0x65, 0xe2,
  0x30, 0x54, 0xf6, 0x72, 0x4b, 0x87, 0xe8, 0xb7, 0x34, 0xee, 0xad, 0x12,
  0x90, 0x30, 0xf7, 0x13, 0x36, 0x4e, 0x32, 0xb1, 0x06, 0xf3, 0xfa, 0x37,
  0x8b, 0x8c, 0xb7, 0x30, 0x2a, 0x04, 0x3a, 0x47, 0xd5, 0x99, 0x67, 0x06,
  0x42, 0x40, 0x41, 0xbe, 0xbb, 0x59, 0x48, 0xcb, 0xe7, 0xef, 0x1c, 0xed,
  0x22, 0x1a, 0xe8, 0x25, 0x83, 0x7f, 0x3d, 0x40, 0x05, 0x8d, 0x5b, 0x0b,
  0x6a, 0x69, 0x2b, 0xea, 0x4b, 0xf4, 0xd4, 0x88, 0xdb, 0xd2, 0xcf, 0x7e
};
unsigned int crt_DER_len = 540;

// https://github.com/fhessel/esp32_https_server_compat/blob/master/examples/HelloServerSecure/private_key.h
unsigned char key_DER[] = {
  0x30, 0x82, 0x02, 0x5d, 0x02, 0x01, 0x00, 0x02, 0x81, 0x81, 0x00, 0xce,
  0xe6, 0x25, 0x6f, 0x3c, 0xfb, 0xda, 0x38, 0x2b, 0xd7, 0x62, 0xcc, 0x04,
  0x9c, 0x58, 0xc8, 0xc8, 0x91, 0x14, 0x93, 0xb4, 0x0e, 0xa4, 0x09, 0x78,
  0xd8, 0xcb, 0xfe, 0x13, 0x30, 0x6d, 0xd8, 0x22, 0x21, 0xf6, 0xdf, 0x7e,
  0xc3, 0xc6, 0x92, 0x2f, 0xc6, 0x84, 0x43, 0xff, 0xb4, 0xa1, 0x6b, 0x13,
  0x5f, 0x36, 0xa7, 0xc3, 0x99, 0x32, 0x03, 0x89, 0x50, 0x30, 0x1e, 0xad,
  0xc6, 0x36, 0xe7, 0x73, 0x48, 0x09, 0x28, 0x4f, 0xb9, 0x46, 0xbf, 0xdd,
  0x76, 0x10, 0xc3, 0x07, 0x33, 0x9b, 0xd0, 0x8d, 0xb2, 0x24, 0xd4, 0xcb,
  0x2e, 0x90, 0x06, 0xbf, 0xf1, 0xfa, 0xae, 0x06, 0x5c, 0xec, 0x5d, 0xe8,
  0x61, 0x06, 0x4a, 0x3a, 0x2e, 0x2b, 0x1e, 0x60, 0xf2, 0xc4, 0x09, 0xca,
  0xe6, 0x27, 0x64, 0x31, 0x9c, 0xbd, 0x2d, 0x3a, 0x56, 0x27, 0x6d, 0x23,
  0x67, 0x21, 0x11, 0x6f, 0x50, 0xca, 0x11, 0x02, 0x03, 0x01, 0x00, 0x01,
  0x02, 0x81, 0x80, 0x7b, 0x28, 0x2e, 0x12, 0x58, 0x27, 0xc6, 0xce, 0xf6,
  0xf1, 0xe0, 0x02, 0x77, 0xa0, 0x25, 0x8d, 0x67, 0x2e, 0x4d, 0x24, 0x5a,
  0xe2, 0xf8, 0x2c, 0x17, 0x3e, 0x5d, 0xb7, 0x60, 0xee, 0xcc, 0x04, 0x02,
  0xd5, 0x5a, 0xe1, 0xd0, 0xd0, 0x72, 0xcc, 0x24, 0x1a, 0x34, 0x33, 0x51,
  0xeb, 0xd0, 0xc6, 0x2f, 0x22, 0xd7, 0x22, 0xe7, 0xe0, 0xb2, 0x0f, 0xbe,
  0xd5, 0xf7, 0xbe, 0xdb, 0x4c, 0x08, 0xf3, 0x8b, 0xb2, 0x04, 0x7e, 0x45,
  0x2d, 0x7e, 0xff, 0x98, 0xc2, 0x4f, 0xce, 0xa4, 0x98, 0x06, 0x08, 0x36,
  0x2e, 0x6c, 0xd3, 0xc6, 0x1c, 0x29, 0x26, 0x96, 0xcd, 0xeb, 0x40, 0xa5,
  0xf5, 0xf1, 0x1c, 0xd6, 0x21, 0xbd, 0x1b, 0x2b, 0xba, 0x0f, 0xba, 0x69,
  0xf4, 0xb9, 0x39, 0x78, 0xbc, 0xfe, 0x95, 0x3a, 0xb9, 0xbf, 0x85, 0x9e,
  0x86, 0xfb, 0x39, 0x5c, 0xd1, 0xf0, 0x37, 0xbc, 0x40, 0xfc, 0x51, 0x02,
  0x41, 0x00, 0xfd, 0xd7, 0x02, 0xf9, 0xc6, 0xf6, 0x24, 0x0e, 0x57, 0x9e,
  0xb1, 0xf0, 0x55, 0x9a, 0x10, 0xa8, 0x65, 0xf9, 0x55, 0x54, 0xe7, 0x99,
  0x0f, 0xd5, 0x5d, 0xe4, 0xff, 0x70, 0x68, 0xc1, 0xbe, 0x58, 0x78, 0x2c,
  0x84, 0xb7, 0xd2, 0x8a, 0xde, 0xa5, 0x6e, 0x01, 0x12, 0xc8, 0x58, 0x02,
  0x46, 0x7d, 0x43, 0xd7, 0x5b, 0x43, 0xfb, 0x97, 0x20, 0x22, 0x87, 0x71,
  0x87, 0xd3, 0x44, 0x8c, 0xfd, 0xbd, 0x02, 0x41, 0x00, 0xd0, 0xa8, 0xdf,
  0xcd, 0xc8, 0x55, 0x12, 0x80, 0xf6, 0xb8, 0x1c, 0x55, 0xa7, 0x6a, 0xd7,
  0xad, 0x7f, 0xab, 0xed, 0xc5, 0x19, 0xfa, 0x9a, 0x89, 0x11, 0x6f, 0xc9,
  0xf2, 0xa9, 0x03, 0x99, 0x0b, 0xe4, 0xda, 0x17, 0x02, 0x11, 0xb7, 0x80,
  0x3b, 0x7d, 0x30, 0xae, 0xa9, 0x8b, 0xc8, 0xc6, 0x39, 0x9c, 0x73, 0xa5,
  0xe3, 0x16, 0xe2, 0x15, 0xed, 0xf8, 0x38, 0xff, 0xce, 0x71, 0x0e, 0x10,
  0xe5, 0x02, 0x41, 0x00, 0xb1, 0xc8, 0xfe, 0xf7, 0x8c, 0x47, 0x66, 0xf7,
  0x78, 0x9c, 0xd8, 0x89, 0xb8, 0x9a, 0xc0, 0x62, 0x01, 0x92, 0x01, 0x17,
  0x07, 0x62, 0xa7, 0xb9, 0x4c, 0x1b, 0x10, 0x61, 0x5d, 0xad, 0x9c, 0xb0,
  0x7f, 0xf2, 0xc6, 0x3d, 0xad, 0x43, 0xc0, 0x2e, 0xe3, 0x7d, 0xf2, 0xf6,
  0xc8, 0xd5, 0x47, 0x23, 0x82, 0xf9, 0x79, 0x9d, 0x82, 0xbf, 0xd5, 0x2c,
  0xf9, 0xea, 0x25, 0x34, 0x6e, 0x45, 0xc5, 0x8d, 0x02, 0x40, 0x5d, 0x25,
  0x86, 0x03, 0x0f, 0x13, 0x2b, 0x17, 0x77, 0x0b, 0xe9, 0x5a, 0x33, 0x4a,
  0x76, 0xcd, 0x74, 0xd9, 0x03, 0x63, 0xa1, 0x9d, 0x45, 0xaf, 0x3a, 0xa1,
  0x74, 0xbd, 0x66, 0xc5, 0xbc, 0x64, 0x9a, 0xdc, 0xe0, 0xb8, 0x83, 0xc0,
  0x2e, 0xf6, 0x5f, 0x84, 0x83, 0xf4, 0x1b, 0xfa, 0x9c, 0xc2, 0xcb, 0x1c,
  0xb5, 0x49, 0x12, 0xc6, 0x0a, 0x94, 0x18, 0xe3, 0x19, 0x0e, 0xc7, 0x59,
  0x48, 0x21, 0x02, 0x41, 0x00, 0xaa, 0x5d, 0x55, 0xc3, 0xee, 0xf7, 0x45,
  0xbd, 0xa5, 0x00, 0x32, 0xb9, 0xa1, 0x71, 0x49, 0xd5, 0x8c, 0x32, 0xe0,
  0xc7, 0xd5, 0xf0, 0x64, 0xa9, 0xb5, 0xaf, 0x1b, 0x25, 0xdf, 0x34, 0xed,
  0xd4, 0xa6, 0xe1, 0x77, 0xfe, 0x9b, 0xc3, 0xed, 0x9b, 0x74, 0xca, 0xbf,
  0x6d, 0xa4, 0x85, 0x5a, 0x37, 0xd8, 0xf3, 0xad, 0xae, 0x91, 0x4f, 0xa1,
  0x30, 0x24, 0xef, 0x3c, 0x4f, 0x49, 0xec, 0x34, 0xa5
};
unsigned int key_DER_len = 609;

SSLCert cert = SSLCert(
  crt_DER, crt_DER_len,
  key_DER, key_DER_len
);

#define SD_CS 7
#define SD_MOSI 6
#define SD_MISO 5
#define SD_SCK 4

HTTPSServer secureServer(&cert);
HTTPServer insecureServer;
DNSServer dnsServer;
bool sdCardAttached = true;

String ssid = "ESP32-AccessPoint";
String password = "";

bool Logs = false;

// Web server
struct HostMapping {
    String domain;
    String folder;
};
std::vector<HostMapping> hostMappings;

// Pages emulation
struct PageMapping {
    String page;
    int status;
    String mimeType;
    String filePath;
};
std::vector<PageMapping> pageMappings;

const char style[] PROGMEM =
  "body{font-family:sans-serif;}"
  "a{text-decoration:none}"
  "button{margin:0;}"
  ".header{font-size:16px;}"
  ".controls{font-size:16px;padding:8px 0;border-bottom:1px solid grey;}"
  ".table{display:flex;flex-direction:column;margin-top:4px;max-width:540px;}"
  ".row{display:flex;align-items:center;gap:1em;padding:.5em;white-space:nowrap;overflow:auto}"
  ".row:nth-child(odd){background:#eee}"
  ".icon{width:24px;height:24px}"
  ".name{flex:1;min-width:100px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;}"
  ".actions{display:flex;gap:.5em}";

void logRequest(const String &host, const String &request, const String &method, const String &userAgent, const String &body) {
    File logFile = SD.open("/Logs.txt", FILE_APPEND);
    if (logFile) {
        logFile.print("Host: " + host + ", Method: " + method + ", Uptime: " + String(millis()) + "\n");
        logFile.print("URL: " + host + request + "\n");
        logFile.print("User-Agent: " + userAgent + "\n");
        if (body.length() > 0)
            logFile.print("Body:\n" + body + "\n");
		logFile.print("\n");
        logFile.close();
    }
}

void loadConfig() {
    File f = SD.open("/Setup.ini");
    if (!f) return;
    while (f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();
        int sep = line.indexOf('=');
        if (sep > 0) {
            String key = line.substring(0, sep);
            String val = line.substring(sep + 1);
            key.trim(); val.trim();
            if (key == "ssid")
                ssid = val;
            else if (key == "password")
                password = val;
            else if (key == "logs")
                Logs = val == "1";
        }
    }
    f.close();
}

void loadHosts() {
    File file = SD.open("/Hosts.txt");
    if (!file) return;
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.length() == 0) continue;
        int spaceIndex = line.indexOf(' ');
        if (spaceIndex > 0) {
            String domain = line.substring(0, spaceIndex);
            String folder = line.substring(spaceIndex + 1);
            folder.trim();
            
            hostMappings.push_back({domain, folder});
        }
    }
    file.close();
}

void loadPagesEmulation() {
    File file = SD.open("/Emulation.txt");
    if (!file) return;

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.length() == 0) continue;

        int first = line.indexOf('|');
        if (first < 0) continue;
        int second = line.indexOf('|', first + 1);
        if (second < 0) continue;
        int third = line.indexOf('|', second + 1);
        if (third < 0) continue;

        String page = line.substring(0, first);
		page.trim();
        String statusStr = line.substring(first + 1, second);
		statusStr.trim();
        String MIMEType = line.substring(second + 1, third);
		MIMEType.trim();
        String filePath = line.substring(third + 1);
		filePath.trim();

        pageMappings.push_back({page, statusStr.toInt(), MIMEType, filePath});
    }

    file.close();
}

String getFolderForHost(const String &host) {
    for (const auto &mapping : hostMappings) {
        if (host.equalsIgnoreCase(mapping.domain))
            return mapping.folder;
    }
    return "";
}

String bytesToSize(uint64_t bytes){ // size_t
	if (bytes < 1024)
		return String(bytes) + " B";
	else if (bytes < (1048576)) // 1024 * 1024
		return String(bytes / 1024.0) + " KB";
	else if(bytes < (1073741824)) // 1024 * 1024 * 1024
		return String(bytes / 1048576.0) + " MB"; // 1024.0 / 1024.0
	else 
		return String(bytes / 1073741824.0) + " GB"; // 1024.0 / 1024.0 / 1024.0
}

// https://circuits4you.com/2019/03/21/esp8266-url-encode-decode-example/
String urlEncode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
    
}

unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

String urlDecode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str.length(); i++){
        c=str.charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str.charAt(i);
        i++;
        code1=str.charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
        
        encodedString+=c;  
      }
      
      yield();
    }
    
   return encodedString;
}

void folderList(fs::FS &fs, const char *folderName, HTTPResponse *res, String request) {
	String folderNameStr(folderName);
	res->setStatusCode(200);
	res->setHeader("Content-Type", "text/html");
    res->println("<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    res->print("<title>Index of " + folderNameStr + "</title><style>");
    res->print(style);
    res->println("</style></head><body><h2>Index of " + folderNameStr + "</h2><hr>");

    File root = fs.open(folderName);
    if (!root || !root.isDirectory()) {
		//res->println("<p>The requested URL \"" + url + "\" is not a directory.<br>Path: \"" + folderNameStr + "\".</p></body></html>");
		res->println("<p>The requested URL \"" + request + "\" is either missing or not a directory on the SD card.<br>Requested path: \"" + folderNameStr + "\".</p></body></html>");
        return;
    }
	
    String clientPath = String(folderName);
    if (!clientPath.endsWith("/"))
        clientPath += "/";
	
	//res->println(request);
	
	res->println("<div class='controls'><button onclick=\"let name=prompt('New folder:'); if(name && name.trim() != '') document.location='/?mkdir=" + urlEncode(clientPath) + "' + encodeURI(name) + '&url=' + '" + request + "';\">New folder</button>");
	res->println(
	  "<form style='display:inline-block; vertical-align: middle; margin: 0;' method='POST' enctype='multipart/form-data' action='/upload=" + urlEncode(clientPath + "&url=" + request) + "'>"
	  "<input type='file' name='upload' id='upload' style='display:none' onchange='this.form.submit()'>"
	  "<label for='upload' id='uploadLabel' style='display:none'></label>"
	  "<button type='button' onclick='document.getElementById(\"uploadLabel\").click()'>Upload</button>"
	  "</form>"
	);

    res->println("<div class='table'>");
    File file = root.openNextFile();
    while (file) {
        res->print("<div class='row'>");
        res->print("<div class='name'><a href=\"" + request);
        res->print(file.name());
        if (file.isDirectory())
            res->print("/");
        res->print("\">");
        res->print(file.name());
        res->print("</a></div><div class='date'>");
		if (!file.isDirectory())
			res->print(bytesToSize(file.size()));
		String deletePath = clientPath + file.name();
		res->print("</div><div class='actions'><a href=\"/?delete=" + urlEncode(deletePath + "&url=" + request) + "\" ");
		res->print("onclick=\"return confirm('Remove &quot;");
		res->print(file.name());
		res->print("&quot;?');\">❌</a></div></div>");
        file = root.openNextFile();
    }
    res->println("</div></body></html>");
}

String getContentType(const String path) {
	if (path.endsWith(".html") || path.endsWith(".htm")) return "text/html";
	else if (path.endsWith(".css")) return "text/css";
	else if (path.endsWith(".js")) return "application/javascript";
	else if (path.endsWith(".json")) return "application/json";
	else if (path.endsWith(".xml")) return "text/xml";
	else if (path.endsWith(".txt") || path.endsWith(".ini")) return "text/plain";
	else if (path.endsWith(".jpg") || path.endsWith(".jpeg")) return "image/jpeg";
	else if (path.endsWith(".ico")) return "image/x-icon";
	else if (path.endsWith(".png")) return "image/png";
	else if (path.endsWith(".gif")) return "image/gif";
	else if (path.endsWith(".svg")) return "image/svg+xml";
	else if (path.endsWith(".webp")) return "image/webp";
	else if (path.endsWith(".mp3")) return "audio/mpeg";
	else if (path.endsWith(".wav")) return "audio/wav";
	else if (path.endsWith(".ogg")) return "audio/ogg";
	else if (path.endsWith(".mp4")) return "video/mp4";
	else if (path.endsWith(".webm")) return "video/webm";
	else if (path.endsWith(".pdf")) return "application/pdf";
	else if (path.endsWith(".manifest")) return "text/cache-manifest";
	else return "application/octet-stream"; // contentType = "text/html"
}

void sendFile(HTTPResponse *res, int resStatus, String contentType, String pathOrValue, String request) {
	// If the emulation simply returns a string / Если при эмуляции просто отдается строка
    if (pathOrValue.startsWith("\"")) {
		res->setStatusCode(resStatus);
		res->setHeader("Content-Type", contentType.c_str());
		pathOrValue = pathOrValue.substring(1, pathOrValue.length() - 1);
		pathOrValue.replace("\\n", "\n");
		res->println(pathOrValue);
		return;
	}
	
	// Normalizing paths / Приводим пути в порядок
	while (pathOrValue.indexOf("//") != -1)
        pathOrValue.replace("//", "/");
	if (pathOrValue != "/" && pathOrValue.endsWith("/")) // Folders are recognized only without the last slash (!file) / Папки распознаются только без последнего слеша (!file)
		pathOrValue.remove(pathOrValue.length() - 1); 

	
    File file = SD.open(pathOrValue);
	
	// If there is nothing / Если ничего нет
    if (!file) {
		res->setStatusCode(404);
		res->setHeader("Content-Type", "text/html");
        //res->println("<html><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL was not found on this server.<br>Host: \"" + host + "\".<br>Path: \"" + pathOrValue + "\".</p></body></html>");
        res->println("<html><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL \"" + request + "\" was not found on this server.<br>Requested path: \"" + pathOrValue + "\".</p></body></html>");
        return;
    }

	// If folder / Если папка
    if (file.isDirectory()) {
		file.close();
        folderList(SD, pathOrValue.c_str(), res, request);
		return;
    }

	// If file / Если файл
    if (contentType == "")
		contentType = getContentType(pathOrValue);

	res->setStatusCode(resStatus);
	res->setHeader("Content-Type", contentType.c_str());

    uint8_t buf[1024];
    while (file.available()) {
        size_t len = file.read(buf, sizeof(buf));
        res->write(buf, len);
    }
    file.close();
}

void deleteDirRecursively(const String &path) {
    File dir = SD.open(path);
    if (!dir || !dir.isDirectory()) {
        dir.close();
        return;
    }

    File entry = dir.openNextFile();
    while (entry) {
        String entryName = entry.name();
        String filePath = path + "/" + entryName;

        if (entry.isDirectory()) {
            entry.close();  // Close before recursion / Закрываем перед рекурсией
            deleteDirRecursively(filePath);
            SD.rmdir(filePath);  // Delete the folder itself / Удаляем саму папку
        } else {
            entry.close();  // Close the file before deleting Закрываем файл перед удалением
            SD.remove(filePath);
        }

        entry = dir.openNextFile();  // Next file / Следующий файл
    }

    dir.close();
    SD.rmdir(path);  // We delete the already empty directory / Удаляем уже пустую директорию
}

// https://github.com/bdash9/ESP32-FileServer-HTTP-HTTPS/blob/main/ESP32-File_server-AP-web-DEMO/ESP32-File_server-AP-web-DEMO.ino
void handleUpload(HTTPRequest *req, HTTPResponse *res, String request)
{
	String pathUpload = request.substring(request.indexOf("/upload=") + 8);
	
	String basePath = "/";
	int urlParamIndex = pathUpload.indexOf("&url=");
	if (urlParamIndex != -1) {
		basePath = pathUpload.substring(urlParamIndex + 5);
		if (!basePath.startsWith("/")) basePath = "/" + basePath;
		pathUpload = pathUpload.substring(0, urlParamIndex);
	}

	HTTPMultipartBodyParser *parser = new HTTPMultipartBodyParser(req);
	while (parser->nextField()) {
		String filename = parser->getFieldFilename().c_str();
		if (filename.length() == 0) continue;

		filename.replace("\\", "/");
		while (filename.startsWith("/")) filename = filename.substring(1);
		if (filename.indexOf("..") >= 0) continue;

		File upFile = SD.open(pathUpload + filename, "w");
		if (!upFile) continue;

		while (!parser->endOfField()) {
			byte buf[1024];
			size_t readLength = parser->read(buf, sizeof(buf));
			upFile.write(buf, readLength);
		}
		upFile.close();
	}
	delete parser;

	res->setStatusCode(303); // No caching / Без кеширования
	res->setStatusText("See Other");
	basePath = urlEncode(basePath);
	res->setHeader("Location", basePath.c_str());
}

String readRequestBody(HTTPRequest *req) {
	int len = req->getContentLength();
	String body = "";
	if (len > 0) {
		byte *buf = new byte[len + 1]; // используем byte вместо char
		int readLen = req->readBytes(buf, len);
		buf[readLen] = 0; // завершаем строку

		body = String((char*)buf); // приводим к char* при создании строки
		delete[] buf;
	}
	return body;
}

void handleClient(HTTPRequest * req, HTTPResponse * res) {
	String protocol = String(req->isSecure() ? "https://" : "http://");
	String method = String(req->getMethod().c_str());
	String host = String(req->getHeader("Host").c_str());
	String request = urlDecode(String(req->getRequestString().c_str()));
	String userAgent = String(req->getHeader("User-Agent").c_str());
	String body = "";
	if (method == "POST" || method == "PUT")
		body = readRequestBody(req);
	unsigned long timeout = millis() + 1000;

	host.trim();
	request.trim();

	if (host.length() == 0 || request.length() == 0) return;
	while (request.indexOf("//") != -1) // We bring the requests in order site//file.txt -> site/file.txt / Приводим запросы в порядок site//file.txt -> site/file.txt
		request.replace("//", "/");
		
	if (method == "POST" && request.startsWith("/upload")) {
		handleUpload(req, res, request);
		return;
	}
	
	// Android WiFi activation skip
	//if (host == "connectivitycheck.gstatic.com") {
		//res->setStatusCode(404);
		//return;
	//}
	
    if (Logs)
        logRequest(host, request, method, userAgent, body);
	
	// Create a folder / Создание папки
	int mkDirIndex = request.indexOf("/?mkdir=");
	if (mkDirIndex != -1) {
		String newFolder = request.substring(mkDirIndex + 8);
		String basePath = "/";

		int urlParamIndex = newFolder.indexOf("&url=");
		if (urlParamIndex != -1) {
			basePath = newFolder.substring(urlParamIndex + 5);
			if (!basePath.startsWith("/")) basePath = "/" + basePath;
			newFolder = newFolder.substring(0, urlParamIndex);
		}
		
		newFolder.trim();
		//newFolder = urlDecode(newFolder);

		if (!SD.exists(newFolder))
			SD.mkdir(newFolder);
		
		/*res->setStatusCode(404);
		res->setHeader("Content-Type", "text/html");
		res->println("\"" + basePath + "\"<br>" + "\"" + newFolder + "\"");*/

		// Forwarding back / Переадресация обратно
		basePath = urlEncode(basePath);
		res->setHeader("Location", basePath.c_str());
		res->setStatusCode(303);
		res->setHeader("Content-Type", "text/html");
		return;
	}
	
	// Deleting / Удаление
	int deleteIndex = request.indexOf("/?delete=");
	if (deleteIndex != -1) {
		String toDelete = request.substring(deleteIndex + 9);
		String basePath = "/";
		
		int urlParamIndex = toDelete.indexOf("&url=");
		if (urlParamIndex != -1) {
			basePath = toDelete.substring(urlParamIndex + 5);
			if (!basePath.startsWith("/")) basePath = "/" + basePath;
			toDelete = toDelete.substring(0, urlParamIndex);
		}
		
		toDelete.trim();
		if (toDelete == "/") return;
		if (toDelete.endsWith("/")) // Folders are recognized only without the "/" (!file) / Папки распознаются только без "/" (!file)
			toDelete.remove(toDelete.length() - 1); 

		if (SD.exists(toDelete)) {
			File f = SD.open(toDelete);
			if (f) {
				bool isDir = f.isDirectory();
				f.close(); // Close before deleting / Закрываем перед удалением

				if (isDir)
					deleteDirRecursively(toDelete);
				else
					SD.remove(toDelete);
			}
		}
		
		/*res->setStatusCode(404);
		res->setHeader("Content-Type", "text/html");
		res->println("\"" + basePath + "\"<br>" + "\"" + toDelete + "\"");*/
		
		// Forwarding back / Переадресация обратно
		//String location = String(req->isSecure() ? "https://" : "http://") + host + basePath;
		basePath = urlEncode(basePath);
		res->setHeader("Location", basePath.c_str()); //pathUpload.c_str()
		res->setStatusCode(303);
		res->setHeader("Content-Type", "text/html");
		return;
	}
	
    // Page emulation
    for (const auto &mapping : pageMappings)
            if (mapping.page == host + request || (request == "/" && mapping.page == host)) { // For both files and the homepage / Как файлы, так и главная страница
			if (mapping.filePath != "-")
				sendFile(res, mapping.status, mapping.mimeType, mapping.filePath, "");
			else {
				res->setStatusCode(404);
				res->setHeader("Content-Type", "text/html");
				res->println("");
				//req->abort ????
			}
			
			/*res->setStatusCode(404); // Debug
			res->setHeader("Content-Type", "text/html");
			res->println("<html><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>404 Not Found</title></head><body>");
			
			res->println("Page: " + mapping.page + "<br>");
			res->println("Host: " + host + "<br>");
			res->println("Host + Request: " + host + request + "<br>");
			res->println("Status: " + String(mapping.status) + "<br>");
			res->println("Type: " + mapping.mimeType + "<br>");
			res->println("File: " + mapping.filePath + "<br>");
			res->println("</body></html>");*/
		
            return;
        }

    if (host == WiFi.softAPIP().toString() || host == "i.me") {
        res->setStatusCode(200);
		res->setHeader("Content-Type", "text/html");
        res->println("<html><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Sites</title><style>");
		res->print(style);
		res->println("</style></head><body><h2>Sites</h2><hr><div class='table'>");

        for (const auto &mapping : hostMappings)
			res->print("<div class='row'><div class='name'><a href=\"" + protocol + mapping.domain + "/\">" + mapping.domain + "</a>" + "</div></div>");
		res->print("</div>");
		
		res->println("<h2>Emulation</h2><hr>");
		res->print("<div class='table'>");
		for (const auto &mapping : pageMappings) {
			res->print("<div class='row'><div class='name'><a href=\"" + protocol + mapping.page + "\">" + mapping.page + "</a></div>");
            res->print("<div class='filePath'>" + mapping.filePath + "</div><div class='mimeType'>" + mapping.mimeType + "</div></div>");
		}
		res->print("</div>");
		
		res->println("<h2>Debug</h2><hr>");
		res->println("Host: \"" + host + "\"");
		res->println("<br>Request: \"" + request + "\"");
		res->println("<br>Protocol: " + String(req->isSecure() ? "HTTPS" : "HTTP") + ", go to <a href=\"" + String(req->isSecure() ? "http://" : "https://") + WiFi.softAPIP().toString() + "/\">HTTPS</a>.");
		res->println("<h2>ESP32</h2><hr>");
		res->println("CPU frequency: " + String(ESP.getCpuFreqMHz()) + "MHz<br>");
		res->println("Ram size: " + bytesToSize(ESP.getHeapSize()) + "<br>");
		res->println("Free ram: " + bytesToSize(ESP.getFreeHeap()) + "<br>");
		res->println("Max alloc ram: " + bytesToSize(ESP.getMaxAllocHeap()) + "<br>");
		res->println("Sketch size: " + bytesToSize(ESP.getSketchSize()) + "<br>");
		res->println("Free space available: " + bytesToSize(ESP.getFreeSketchSpace() - ESP.getSketchSize()) + "<br>");
		if (sdCardAttached) {
			res->println("SD card Size: " + bytesToSize(SD.cardSize()));
			res->println("<br>SD card used space: " + bytesToSize(SD.usedBytes()));
			res->println("<br>SD card free space: " + bytesToSize(SD.totalBytes() - SD.usedBytes()));
		} else
			res->println("SD Card: detached");
        res->println("</body></html>");
        return;
    }

	// If Host is not found, then folder length is 0 / Если Host не найден, то folder length - 0
    String hostFolder = getFolderForHost(host);
    if (hostFolder.length() == 0) {
        res->setStatusCode(404);
		res->setHeader("Content-Type", "text/html");
        res->println("<html><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested host was not found on this server.</p>Host: \"" + host + "\".</body></html>");
		//req->abort????
        return;
    }
	
	if (request == "/" || request.length() == 0) {
		// We search for "index.html", otherwise we output the file / folder / Ищем "index.html", иначе выводим файл / папку
		File indexFile = SD.open(hostFolder + "index.html");
        if (indexFile) {
            indexFile.close();
            sendFile(res, 200, "", hostFolder + "index.html", "");
        } else
			sendFile(res, 200, "", hostFolder, request);
    } else
		sendFile(res, 200, "", hostFolder + request, request);
	return;
}

void handleHTTPClient(HTTPRequest * req, HTTPResponse * res) {
	handleClient(req, res);
}

void handleHTTPSClient(HTTPRequest * req, HTTPResponse * res) {
	handleClient(req, res);
}

void setup() {
	//Serial.begin(115200); Not working ???

	SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
	//delay(100);
	if (!SD.begin(SD_CS)) {
		sdCardAttached = false;
		return;
	}
	//delay(100);
	loadConfig();
	//delay(10);
	loadHosts();
	//delay(10);
	loadPagesEmulation();
	WiFi.softAP(ssid.c_str(), password.c_str());

	IPAddress myIP = WiFi.softAPIP();
	dnsServer.start(53, "*", myIP);

	ResourceNode *anyHTTPGetHandler = new ResourceNode("", "ANY", &handleHTTPClient);
	ResourceNode *anyHTTPSGetHandler = new ResourceNode("", "ANY", &handleHTTPSClient);

	insecureServer.setDefaultNode(anyHTTPGetHandler);
	secureServer.setDefaultNode(anyHTTPSGetHandler);

	insecureServer.start();
	secureServer.start();
}

void loop() {
	dnsServer.processNextRequest();
	secureServer.loop();
	insecureServer.loop();
	delay(1);
}
