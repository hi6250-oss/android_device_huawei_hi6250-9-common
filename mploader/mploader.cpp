#define LOG_TAG "mploader"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>
#include <android-base/properties.h>

#define DEFAULD_ID "0X00000000"
#define SYS_PROP_READY "sys.rilprops_ready"

#define CMDLINE "/proc/cmdline"
#define PHONE_PROP "/vendor/phone.prop"

#include <string>
#include <vector>
#include <fstream>

std::string ReadProductId() {
    std::string prid = DEFAULD_ID;
    std::string cmdline;

    if (android::base::ReadFileToString(CMDLINE, &cmdline)) {
        for (const auto& i : android::base::Split(android::base::Trim(cmdline), " ")) {
            std::vector<std::string> parts = android::base::Split(i, "=");
            if (parts.size() == 2 && parts.at(0) == "productid") {
                prid = parts.at(1);
                std::transform(prid.begin(), prid.end(), prid.begin(),
                   [](unsigned char c){ return toupper(c); });
            }
        }
    }

    return prid;
}

int LoadPhoneProperties(std::string prid) {
    int ret = -1;
    std::string line;
    std::ifstream file(PHONE_PROP);

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (ret == 0 && line.length() == 0) break;
            if (line.find(prid) != std::string::npos) ret = 0;

            if (ret == 0) {
                std::vector<std::string> parts = android::base::Split(line, "=");
                if (parts.size() == 2) {
                    LOG(INFO) << "Setting property: " << parts.at(0);
                    android::base::SetProperty(parts.at(0), parts.at(1));
                }
            }
        }
    }

    return ret;
}

int main() {
    int ret = -1;
    std::string productId = ReadProductId();

    if (productId != DEFAULD_ID) {
        if ((ret = LoadPhoneProperties(productId)) == 0) {
            LOG(INFO) << "Successfully loaded phone properties for " << productId;
            android::base::SetProperty(SYS_PROP_READY, "1");
        }
    }

    return ret;
}
