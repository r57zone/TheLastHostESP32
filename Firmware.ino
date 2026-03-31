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

#define PIN_SD_CS   7
#define PIN_SD_MOSI 6
#define PIN_SD_MISO 5
#define PIN_SD_SCK  4

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

struct Config {
    String ssid = "ESP32-AccessPoint";
    String password = "";
    bool logsEnabled = false;
};

struct HostRoute {
    String domain;
    String directory;
};

struct PageEmulation {
    String url;
    int statusCode;
    String contentType;
    String response;
};

DNSServer dnsServer;
HTTPServer httpServer;
HTTPSServer httpsServer(&cert);

Config config;
std::vector<HostRoute> hostRoutes;
std::vector<PageEmulation> pageEmulations;
bool sdReady = false;

// CSS
const char CSS[] PROGMEM = R"(
body{font-family:Arial,sans-serif;margin:10px;background:#f5f5f5}
a{color:#0066cc;text-decoration:none}
button{padding:8px 16px;margin:2px;background:#4CAF50;color:white;border:none;border-radius:4px;cursor:pointer}
button:hover{background:#45a049}
.controls{background:white;padding:15px;margin:10px 0;border-radius:5px;box-shadow:0 2px 4px rgba(0,0,0,0.1)}
.table{background:white;margin:10px 0;border:1px solid #d3d3d3;border-radius:5px;box-shadow:0 2px 4px rgba(0,0,0,0.1)}
.row{display:flex;align-items:center;padding:10px 6px;border-bottom:1px solid #eee}
.row:first-child{border-top:none;}
.row:hover{background:#f9f9f9}
.row:last-child{border-bottom:none}
.row:nth-child(odd){background:#eee}
.name{flex:1;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;}
.size{width:100px;text-align:right;color:#666}
.actions{width:50px;text-align:right}
)";

String formatBytes(uint64_t bytes) {
    if (bytes < 1024) return String(bytes) + " B";
    if (bytes < 1048576) return String(bytes / 1024.0, 1) + " KB";
    if (bytes < 1073741824) return String(bytes / 1048576.0, 1) + " MB";
    return String(bytes / 1073741824.0, 2) + " GB";
}


// URL decode (UTF-8)
String urlDecode(String str) {
    String decoded = "";
    char a, b;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%') {
            if (i + 2 < str.length()) {
                a = str[i + 1];
                b = str[i + 2];
                if (isxdigit(a) && isxdigit(b)) {
                    if (a >= 'a') a -= 'a' - 'A';
                    if (a >= 'A') a -= ('A' - 10);
                    else a -= '0';
                    if (b >= 'a') b -= 'a' - 'A';
                    if (b >= 'A') b -= ('A' - 10);
                    else b -= '0';
                    decoded += char(16 * a + b);
                    i += 2;
                    continue;
                }
            }
        } else if (str[i] == '+') {
            decoded += ' ';
            continue;
        }
        decoded += str[i];
    }
	decoded.replace("+", " ");
	decoded.replace("%20", " ");
    return decoded;
}

String urlEncode(String str) {
    const char *hex = "0123456789ABCDEF";
    String encoded = "";
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || 
            c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += c;
        } else if (c == ' ') {
            encoded += '+';
        } else {
            encoded += '%';
            encoded += hex[(c >> 4) & 15];
            encoded += hex[c & 15];
        }
    }
    return encoded;
}

String normalizePath(String path) {
    path.trim();
    while (path.indexOf("//") != -1) path.replace("//", "/");
    if (path.length() > 1 && path.endsWith("/")) {
        path.remove(path.length() - 1);
    }
    if (!path.startsWith("/")) path = "/" + path;
    return path;
}

bool isSafePath(const String& path) {
    return path.indexOf("..") == -1;
}

void loadConfig() {
    File file = SD.open("/Setup.ini");
    if (!file) return;

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        int sep = line.indexOf('=');
        if (sep > 0) {
            String key = line.substring(0, sep);
            String value = line.substring(sep + 1);
            key.trim();
            value.trim();
            
            if (key == "AP_SSID")
				config.ssid = value;
            else if (key == "AP_PASSWORD")
				config.password = value;
            else if (key == "ENABLE_LOGS")
				config.logsEnabled = (value == "1");
        }
    }
    file.close();
}

void loadHosts() {
    File file = SD.open("/Hosts.txt");
    if (!file) return;

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.length() == 0) continue;
        
        int space = line.indexOf(' ');
        if (space > 0) {
            HostRoute route;
            route.domain = line.substring(0, space);
            route.directory = line.substring(space + 1);
            route.directory.trim();
			
            hostRoutes.push_back(route);
        }
    }
    file.close();
}

void loadEmulation() {
    File file = SD.open("/Emulation.txt");
    if (!file) return;

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.length() == 0) continue;

        int firstPos = line.indexOf('|');
        if (firstPos < 0) continue;
        int secondPos = line.indexOf('|', firstPos + 1);
        if (secondPos < 0) continue;
        int thirdPos = line.indexOf('|', secondPos + 1);
        if (thirdPos < 0) continue;

        PageEmulation pageEmu;
        pageEmu.url = line.substring(0, firstPos);
        pageEmu.statusCode = line.substring(firstPos + 1, secondPos).toInt();
        pageEmu.contentType = line.substring(secondPos + 1, thirdPos);
        pageEmu.response = line.substring(thirdPos + 1);
        
        pageEmu.url.trim();
        pageEmu.contentType.trim();
        pageEmu.response.trim();
        
        pageEmulations.push_back(pageEmu);
    }
    file.close();
}

void logRequest(const String& protocol, const String& host, const String& path, 
                const String& method, const String& userAgent, const String& body) {
    File log = SD.open("/Logs.txt", FILE_APPEND);
    if (!log) return;
    
	log.println("Uptime: " + String(millis()));
	log.println("URL: " + protocol + host + path);
    log.println("Method: " + method);
    log.println("User-Agent: " + userAgent);
    if (body.length() > 0)
        log.println("Body: " + body);

    log.println();
    log.close();
}

String getMimeType(const String& origPath) {
	String path = origPath;
	path.toLowerCase();
    if (path.endsWith(".html") || path.endsWith(".htm")) return "text/html";
    if (path.endsWith(".css")) return "text/css";
    if (path.endsWith(".js")) return "application/javascript";
    if (path.endsWith(".json")) return "application/json";
    if (path.endsWith(".xml")) return "text/xml";
    if (path.endsWith(".txt") || path.endsWith(".ini")) return "text/plain";
    if (path.endsWith(".jpg") || path.endsWith(".jpeg")) return "image/jpeg";
	if (path.endsWith(".ico")) return "image/x-icon";
    if (path.endsWith(".png")) return "image/png";
    if (path.endsWith(".gif")) return "image/gif";
    if (path.endsWith(".svg")) return "image/svg+xml";
    if (path.endsWith(".webp")) return "image/webp";
    if (path.endsWith(".mp3")) return "audio/mpeg";
	if (path.endsWith(".wav")) return "audio/wav";
	if (path.endsWith(".ogg")) return "audio/ogg";
    if (path.endsWith(".mp4")) return "video/mp4";
	if (path.endsWith(".webm")) return "video/webm";
    if (path.endsWith(".pdf")) return "application/pdf";
	if (path.endsWith(".manifest")) return "text/cache-manifest";
    return "application/octet-stream";
}

void deleteRecursive(const String& path) {
    File dir = SD.open(path);
    if (!dir) return;
    
    if (dir.isDirectory()) {
        File entry = dir.openNextFile();
        while (entry) {
            String entryPath = path + "/" + String(entry.name());
            if (entry.isDirectory()) {
                entry.close();
                deleteRecursive(entryPath); 
            } else {
                entry.close();
                SD.remove(entryPath);
            }
            entry = dir.openNextFile();
        }
        dir.close();
        SD.rmdir(path);
    } else {
        dir.close();
        SD.remove(path);
    }
}

void sendError(HTTPResponse* res, int code, const String& msg) {
    res->setStatusCode(code);
    res->setHeader("Content-Type", "text/html; charset=utf-8");
    res->println("<!DOCTYPE html><html><head><meta charset='UTF-8'>");
    res->println("<meta name='viewport' content='width=device-width'>");
    res->println("<title>Error " + String(code) + "</title>");
    res->println("<style>" + String(CSS) + "</style></head><body>");
    res->println("<h1>Error " + String(code) + "</h1>");
    res->println("<p>" + msg + "</p>");
    res->println("</body></html>");
}

String jsEncode(const String& Value) {
    String s = Value;
    s.replace("\\", "\\\\");
    s.replace("\"", "\\\"");
    s.replace("\n", "\\n");
    return s;
}

String getFileIcon(const String& path) {
    String lower = path;
    lower.toLowerCase();

    if (lower.endsWith(".html") || lower.endsWith(".htm")) return "🌐";
    if (lower.endsWith(".css")) return "🎨";
    if (lower.endsWith(".js")) return "🟨";
    if (lower.endsWith(".json")) return "📄";
    if (lower.endsWith(".xml")) return "📄";
    if (lower.endsWith(".txt") || lower.endsWith(".ini")) return "📄";
    if (lower.endsWith(".jpg") || lower.endsWith(".jpeg") || lower.endsWith(".png") || lower.endsWith(".gif") || lower.endsWith(".webp")) return "🖼️";
    if (lower.endsWith(".ico") || lower.endsWith(".svg")) return "🔖";
    if (lower.endsWith(".mp3") || lower.endsWith(".wav") || lower.endsWith(".ogg")) return "🎵";
    if (lower.endsWith(".mp4") || lower.endsWith(".webm")) return "🎬";
    if (lower.endsWith(".pdf")) return "📕";
    if (lower.endsWith(".manifest")) return "📄";
    return "📄";
}

void sendDirListing(HTTPResponse* res, const String& fsPath, const String& urlPath) {
    File dir = SD.open(fsPath);
    if (!dir || !dir.isDirectory()) {
        sendError(res, 404, "Directory not found");
        return;
    }

    res->setStatusCode(200);
    res->setHeader("Content-Type", "text/html; charset=utf-8");
    
    res->println("<!DOCTYPE html><html><head><meta charset='UTF-8'>");
    res->println("<meta name='viewport' content='width=device-width'>");
    res->println("<title>Index of " + urlPath + "</title>");
    res->println("<style>" + String(CSS) + "</style></head><body>");
    res->println("<h2>Index of " + urlPath + "</h2>");

    // Control buttons / Кнопки управления
    res->println("<div class='controls'>");
	res->println("<button onclick='history.back()'><</button>");
    res->print("<button onclick=\"var n=prompt('Folder name:');");
    res->print("if(n)location='/?mkdir=" + urlEncode(fsPath) + "&name='+encodeURIComponent(n)");
    res->println("+'&back=" + urlEncode(urlPath) + "';\">New Folder</button>");
    
    res->print("<form method='POST' enctype='multipart/form-data' ");
    res->print("action='/upload?path=" + urlEncode(fsPath));
    res->println("&back=" + urlEncode(urlPath) + "' style='display:inline'>");
    res->println("<input type='file' name='file' id='f' style='display:none' onchange='this.form.submit()'>");
    res->println("<button type='button' onclick='document.getElementById(\"f\").click()'>Upload</button>");
    res->println("</form></div>");

    // List of files / Список файлов
    
    
    File entry = dir.openNextFile();
	if (entry) {
		res->println("<div class='table'>");
		while (entry) {
			String name = String(entry.name());
			bool isDir = entry.isDirectory();
			
			String linkUrl = urlPath;
			if (!linkUrl.endsWith("/")) linkUrl += "/";
			linkUrl += urlEncode(name);
			if (isDir) linkUrl += "/";

			String delPath = fsPath;
			if (!delPath.endsWith("/")) delPath += "/";
			delPath += name;

			res->println("<div class='row'>");
			res->print("<div class='name'><a href='" + linkUrl + "'>");
			res->print(isDir ? "📁 " : getFileIcon(name) + " ");
			res->println(name + "</a></div>");
			res->println("<div class='size'>" + (isDir ? "" : formatBytes(entry.size())) + "</div>");
			res->print("<div class='actions'><a href='/?delete=" + urlEncode(delPath));
			res->print("&back=" + urlEncode(urlPath));
			res->print("' onclick='return confirm(\"Delete \"" + jsEncode(name) + "\"?\")'>❌</a></div>");
			res->println("</div>");
			
			entry = dir.openNextFile();
		}
		res->println("</div>");
	}
	res->println("</body></html>");
    dir.close();
}

void sendFile(HTTPResponse* res, const String& path) {
    File file = SD.open(path);
    if (!file) {
        sendError(res, 404, "File not found");
        return;
    }

    String mime = getMimeType(path);
    res->setStatusCode(200);
    res->setHeader("Content-Type", mime.c_str());

    byte buf[1024];
    while (file.available()) {
        size_t len = file.read(buf, sizeof(buf));
        res->write(buf, len);
    }
    file.close();
}

void sendDebugPage(HTTPRequest* req, HTTPResponse* res, bool secure) {
    res->setStatusCode(200);
    res->setHeader("Content-Type", "text/html; charset=utf-8");
    
    res->println("<!DOCTYPE html><html><head><meta charset='UTF-8'>");
    res->println("<meta name='viewport' content='width=device-width'>");
    res->println("<title>ESP32 Server</title>");
    res->println("<style>" + String(CSS) + "</style></head><body>");
    
    res->println("<h2>Hosts</h2><div class='table'>");
    for (auto& host : hostRoutes) {
        res->println("<div class='row'><div class='name'><a href='" + 
                    String(secure ? "https://" : "http://") + host.domain + "/'>" + 
                    host.domain + "</a></div><div>" + host.directory + "</div></div>");
    }
    res->println("</div>");

    res->println("<h2>Emulation</h2><div class='table'>");
    for (auto& pageEmu : pageEmulations) {
        res->println("<div class='row'><div class='name'><a href=\"" + String(secure ? "https://" : "http://") + pageEmu.url + "\">" + pageEmu.url + "</a></div>" +
					 "<div class='name'>" + pageEmu.response + "</div>" + 
					 "<div onclick='alert(\"" + "Address: " + pageEmu.url + "\\n" +
												"Content type: " + pageEmu.contentType + "\\n" +
												"Status code: " + pageEmu.statusCode + "\\n" +
												"Response: " + jsEncode(pageEmu.response) + "\")'>📝</div></div>");
    }
    res->println("</div>");

    res->println("<a id='system'><h2>System</h2>");
    res->println("<p>🌐 Protocol: " + String(secure ? "HTTPS" : "HTTP"));
    res->println(" | <a href='" + String(secure ? "http://" : "https://") + WiFi.softAPIP().toString() + "'>Switch</a>");
    res->println(" | <a href='" + String(secure ? "https://" : "http://") + String(req->getHeader("Host").c_str()) + "'>Refresh</a>");
    res->println("</p>");
    res->println("<p>🖥️ HOST: " + String(req->getHeader("Host").c_str()) + ", IP: " + WiFi.softAPIP().toString() + "</p>");
    res->println("<p>🧠 CPU: " + String(ESP.getCpuFreqMHz()) + " MHz</p>");
    res->println("<p>🗄️ RAM: " + formatBytes(ESP.getFreeHeap()) + " / " + formatBytes(ESP.getHeapSize()) + " (" + formatBytes(ESP.getHeapSize() - ESP.getFreeHeap()) + " free)</p></p>");
    res->println("<p>💾 ROM: " + formatBytes(ESP.getSketchSize()) + " / " + formatBytes(ESP.getSketchSize() + ESP.getFreeSketchSpace()) + " (" + formatBytes(ESP.getFreeSketchSpace()) + " free)</p>");
    
    if (sdReady)
        res->println("<p>💾 SD: " + formatBytes(SD.usedBytes()) + " / " + formatBytes(SD.totalBytes()) + " (" + formatBytes(SD.totalBytes() - SD.usedBytes()) + " free)</p>");
	
	float temp = temperatureRead();
	res->println("<p>🌡️ Temperature: " + String(temp, 1) + " °C</p>");
    
    res->println("</body></html>");
}

bool handleMkdir(HTTPRequest* req, HTTPResponse* res, const String& query) {
    if (query.indexOf("?mkdir=") == -1) return false;

    // Parsing parameters / Парсим параметры
    int pathStart = query.indexOf("?mkdir=") + 7;
    int pathEnd = query.indexOf("&name=");
    if (pathEnd == -1) return false;
    
    String basePath = urlDecode(query.substring(pathStart, pathEnd));
    
    int nameStart = pathEnd + 6;
    int nameEnd = query.indexOf("&back=", nameStart);
    if (nameEnd == -1) nameEnd = query.length();
    
    String folderName = urlDecode(query.substring(nameStart, nameEnd));
    
    int backStart = query.indexOf("&back=");
    String backUrl = backStart != -1 ? urlDecode(query.substring(backStart + 6)) : "/";

    // Make dir / Создаем папку
    String newPath = basePath;
    if (!newPath.endsWith("/")) newPath += "/";
    newPath += folderName;
    
    if (isSafePath(newPath)) {
        SD.mkdir(newPath);
    }

    // Redirect / Редирект
    res->setStatusCode(303);
    res->setHeader("Location", backUrl.c_str());
    return true;
}

bool handleDelete(HTTPRequest* req, HTTPResponse* res, const String& query) {
    if (query.indexOf("?delete=") == -1) return false;

    int pathStart = query.indexOf("?delete=") + 8;
    int pathEnd = query.indexOf("&back=");
    if (pathEnd == -1) pathEnd = query.length();
    
    String delPath = urlDecode(query.substring(pathStart, pathEnd));
    
    int backStart = query.indexOf("&back=");
    String backUrl = backStart != -1 ? urlDecode(query.substring(backStart + 6)) : "/";

    // Remove / Удаляем
    delPath = normalizePath(delPath);
    if (isSafePath(delPath) && delPath != "/") {
        deleteRecursive(delPath);
    }

    // Redirect / Редирект
    res->setStatusCode(303);
    res->setHeader("Location", backUrl.c_str());
    return true;
}

bool handleUpload(HTTPRequest* req, HTTPResponse* res, const String& query) {
    if (req->getMethod() != "POST") return false;
    if (query.indexOf("/upload?") == -1) return false;

    // Parsing parameters / Парсим параметры
    int pathStart = query.indexOf("path=") + 5;
    int pathEnd = query.indexOf("&back=");
    if (pathEnd == -1) pathEnd = query.length();
    
    String uploadPath = urlDecode(query.substring(pathStart, pathEnd));
    if (!uploadPath.endsWith("/")) uploadPath += "/";
    
    int backStart = query.indexOf("&back=");
    String backUrl = backStart != -1 ? urlDecode(query.substring(backStart + 6)) : "/";

    // Upload file / Загружаем файл
    HTTPMultipartBodyParser parser(req);
    while (parser.nextField()) {
        String filename = String(parser.getFieldFilename().c_str());
        if (filename.length() == 0) continue;

        // Clearing the file name / Очищаем имя файла
        filename.replace("\\", "/");
        int lastSlash = filename.lastIndexOf('/');
        if (lastSlash != -1) {
            filename = filename.substring(lastSlash + 1);
        }

        if (!isSafePath(filename)) continue;

        String fullPath = uploadPath + filename;
        File file = SD.open(fullPath, FILE_WRITE);
        if (!file) continue;

        byte buf[512];
        while (!parser.endOfField()) {
            size_t len = parser.read(buf, sizeof(buf));
            file.write(buf, len);
        }
        file.close();
    }

    // Redirect / Редирект
    res->setStatusCode(303);
    res->setHeader("Location", backUrl.c_str());
    return true;
}

void handleRequest(HTTPRequest* req, HTTPResponse* res) {
    if (!sdReady) {
        sendError(res, 503, "SD card not available");
        return;
    }

    // Receiving request data / Получаем данные запроса
    bool secure = req->isSecure();
    String method = String(req->getMethod().c_str());
    String host = String(req->getHeader("Host").c_str());
    String path = String(req->getRequestString().c_str());
    String userAgent = String(req->getHeader("User-Agent").c_str());

    // Cleaning the host / Очищаем хост
    int colon = host.indexOf(':');
    if (colon != -1) host = host.substring(0, colon);
    host.trim();
    if (host.endsWith(".")) host.remove(host.length() - 1);

    // Reading the body for POST / Читаем body для POST
    String body = "";
    if (method == "POST" || method == "PUT") {
        int len = req->getContentLength();
        if (len > 0 && len < 8192) {
            byte* buf = new byte[len + 1];
            if (buf) {
                int read = req->readBytes(buf, len);
                buf[read] = 0;
                body = String((char*)buf);
                delete[] buf;
            }
        }
    }

	if (config.logsEnabled)
		logRequest(secure ? "https://" : "http://", host, path, method, userAgent, body);

    // Processing actions / Обработка действий
    if (handleMkdir(req, res, path)) return;
    if (handleDelete(req, res, path)) return;
    if (handleUpload(req, res, path)) return;

    // We decode the path only for routing / Декодируем путь только для роутинга
    String decodedPath = path;
    int questionMark = decodedPath.indexOf('?');
    if (questionMark != -1) {
        decodedPath = decodedPath.substring(0, questionMark);
    }
    decodedPath = urlDecode(decodedPath);
    decodedPath = normalizePath(decodedPath);

    // Page emulation
    String fullUrl = host + decodedPath;
    for (auto& e : pageEmulations) {
        if (e.url == fullUrl || (decodedPath == "/" && e.url == host)) {
            if (e.response == "-") {
                res->setStatusCode(e.statusCode);
                return;
				
            } else if (e.response.startsWith("/")) {
				res->setStatusCode(e.statusCode);
				res->setHeader("Content-Type", e.contentType.c_str());
				sendFile(res, e.response);
				return;
				
			} else {
				String content = e.response;
				if (content.startsWith("\"") && content.endsWith("\"")) {
					content = content.substring(1, content.length() - 1);
					content.replace("\\n", "\n");
				}
				
				res->setStatusCode(e.statusCode);
				res->setHeader("Content-Type", e.contentType.c_str());
				res->println(content);
				return;
			}
        }
    }

    // Information page / Информационная страница
    if (host == WiFi.softAPIP().toString() || host == "i.me") {
        sendDebugPage(req, res, secure);
        return;
    }

    // Looking for a directory for the host / Ищем директорию для хоста
    String hostDir = "";
    for (auto& r : hostRoutes) {
        if (host.equalsIgnoreCase(r.domain)) {
            hostDir = r.directory;
            break;
        }
    }

    if (hostDir.length() == 0) {
        sendError(res, 404, "Host not found: " + host);
        return;
    }

    // Full path to the file / Полный путь к файлу
    String fsPath = hostDir;
    if (!fsPath.endsWith("/")) fsPath += "/";
    
    if (decodedPath != "/" && decodedPath.length() > 0) {
        String p = decodedPath;
        if (p.startsWith("/")) p = p.substring(1);
        fsPath += p;
    }

    fsPath = normalizePath(fsPath);

    if (!isSafePath(fsPath)) {
        sendError(res, 400, "Invalid path");
        return;
    }

    if (!SD.exists(fsPath)) {
        sendError(res, 404, "Not found: " + decodedPath);
        return;
    }

    File file = SD.open(fsPath);
    if (!file) {
        sendError(res, 500, "Cannot open");
        return;
    }

    // If it's a folder, look for index.html or show the listing / Если папка - ищем index.html или показываем листинг
    if (file.isDirectory()) {
        file.close();
        
        String indexPath = fsPath;
        if (!indexPath.endsWith("/")) indexPath += "/";
        indexPath += "index.html";
        
        if (hostDir != "/" && SD.exists(indexPath)) {
            sendFile(res, indexPath);
        } else {
            sendDirListing(res, fsPath, decodedPath);
        }
        return;
    }

    // If it's a file, give it / Если файл - отдаем
    file.close();
    sendFile(res, fsPath);
}

bool initSD() {
    SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
    delay(500);
    
    for (int i = 0; i < 3; i++) {
        if (SD.begin(PIN_SD_CS)) return true;
        delay(500);
    }
    return false;
}

/*SSLCert* loadCertFromSD() {
    if (SD.exists("/server.crt") && SD.exists("/server.key")) {
        File crtFile = SD.open("/server.crt");
        File keyFile = SD.open("/server.key");

        if (crtFile && keyFile) {
            size_t crtSize = crtFile.size();
            size_t keySize = keyFile.size();

            if (crtSize > 0 && keySize > 0 && crtSize <= 8192 && keySize <= 8192) {
                uint8_t* crtBuf = new uint8_t[crtSize];
                uint8_t* keyBuf = new uint8_t[keySize];

                if (crtBuf && keyBuf) {
                    crtFile.read(crtBuf, crtSize);
                    keyFile.read(keyBuf, keySize);

                    crtFile.close();
                    keyFile.close();

                    SSLCert* cert = new SSLCert(crtBuf, crtSize, keyBuf, keySize);

                    delete[] crtBuf;
                    delete[] keyBuf;

                    return cert;
                }

                delete[] crtBuf;
                delete[] keyBuf;
            }

            crtFile.close();
            keyFile.close();
        }
    }

    // Fallback to the embedded certificate / Фоллбек на встроенный сертификат
    return new SSLCert(crt_DER, crt_DER_len, key_DER, key_DER_len);
}*/

void setup() {
    delay(1000);

    sdReady = initSD();
    
    if (sdReady) {
        loadConfig();
        loadHosts();
        loadEmulation();
    }

    // WiFi
    WiFi.persistent(false);
    WiFi.setSleep(false);
    WiFi.disconnect(true, true);
    delay(100);
    WiFi.softAP(config.ssid.c_str(), config.password.c_str());

    // DNS
    dnsServer.start(53, "*", WiFi.softAPIP());
	
	// HTTP / HTTPS
    ResourceNode* handler = new ResourceNode("", "ANY", &handleRequest);
    httpServer.setDefaultNode(handler);
    httpServer.start();
	
	ResourceNode* handler2 = new ResourceNode("", "ANY", &handleRequest);
	httpsServer.setDefaultNode(handler2);
	httpsServer.start();
    
	// Cert files
    /*if (sdReady) {
        //SSLCert* cert = loadCertFromSD();
        //if (cert) {
            httpsServer = new HTTPSServer(cert);
            httpsServer->setDefaultNode(handler);
            httpsServer->start();
        //}
    //}*/
}

void loop() {
    dnsServer.processNextRequest();
    httpServer.loop();
    httpsServer.loop();
    delay(1);
}